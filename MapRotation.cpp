#include "MapRotation.h"
#include "config.h"

#include <QJsonArray>
#include <QJsonValue>
#include <QSet>
#include <algorithm>
#include <cmath>
#include <map>
#include <utility>
#include <vector>

namespace {

const std::map<int, int> kBuildingBlockSize = {
    {BUILDING_HOME, 2},
    {BUILDING_GRANARY, 3},
    {BUILDING_CENTER, 3},
    {BUILDING_STOCK, 3},
    {BUILDING_FARM, 3},
    {BUILDING_MARKET, 3},
    {BUILDING_ARROWTOWER, 2},
    {BUILDING_ARMYCAMP, 3},
    {BUILDING_STABLE, 3},
    {BUILDING_RANGE, 3},
    {BUILDING_DOCK, 2},
    {BUILDING_SIEGE, 3},
    {BUILDING_COLLAGE, 3},
    {BUILDING_TEMPLE, 3},
};

const std::map<int, int> kOceanCodeToNum = {
    {0b00000001, 37},
    {0b00000010, 29},
    {0b00000100, 38},
    {0b00001000, 39},
    {0b00000011, 30},
    {0b00000110, 32},
    {0b00001001, 40},
    {0b00001100, 31},
    {0b00010000, 34},
    {0b00100000, 36},
    {0b01000000, 35},
    {0b10000000, 33},
};

const std::map<int, int> kSandCodeToNum = {
    {0b00000001, 8},
    {0b00000010, 15},
    {0b00000100, 2},
    {0b00001000, 6},
    {0b00000011, 10},
    {0b00000110, 5},
    {0b00001001, 4},
    {0b00001100, 13},
    {0b00010000, 14},
    {0b00100000, 12},
    {0b01000000, 13},
    {0b10000000, 4},
};

const QSet<int> kShoreNums = [] {
    QSet<int> nums;
    for (const auto& item : kOceanCodeToNum) nums.insert(item.second);
    for (const auto& item : kSandCodeToNum) nums.insert(item.second);
    return nums;
}();

bool isBuildingEntry(const QString& topKey, const QJsonObject& obj)
{
    return topKey.startsWith(QStringLiteral("Building_"))
        && obj.contains(QStringLiteral("BlockDR"))
        && obj.contains(QStringLiteral("BlockUR"))
        && obj.contains(QStringLiteral("Num"));
}

int buildingFootprint(const QJsonObject& obj)
{
    const int buildingType = obj.value(QStringLiteral("Num")).toInt();
    const auto it = kBuildingBlockSize.find(buildingType);
    if (it != kBuildingBlockSize.end()) {
        return it->second;
    }
    return 3;
}

std::pair<int, int> rotateBlockAnchor(
    int blockDR,
    int blockUR,
    int width,
    int height,
    int degrees,
    int footprint = 1
)
{
    if (degrees == 90) {
        return {height - blockUR - footprint, blockDR};
    }
    if (degrees == 180) {
        return {width - blockDR - footprint, height - blockUR - footprint};
    }
    if (degrees == 270) {
        return {blockUR, width - blockDR - footprint};
    }
    return {blockDR, blockUR};
}

double clampDetail(double value, double upper)
{
    if (value < 0.0) {
        return 0.0;
    }
    if (value >= upper) {
        return std::nextafter(upper, 0.0);
    }
    return value;
}

std::pair<double, double> rotateDetailPoint(
    double dr,
    double ur,
    int width,
    int height,
    double blockSize,
    int degrees
)
{
    const double widthPx = width * blockSize;
    const double heightPx = height * blockSize;

    double newDR = dr;
    double newUR = ur;
    double newWidthPx = widthPx;
    double newHeightPx = heightPx;

    if (degrees == 90) {
        newDR = heightPx - ur;
        newUR = dr;
        newWidthPx = heightPx;
        newHeightPx = widthPx;
    } else if (degrees == 180) {
        newDR = widthPx - dr;
        newUR = heightPx - ur;
    } else if (degrees == 270) {
        newDR = ur;
        newUR = widthPx - dr;
        newWidthPx = heightPx;
        newHeightPx = widthPx;
    }

    return {
        clampDetail(newDR, newWidthPx),
        clampDetail(newUR, newHeightPx),
    };
}

struct RotatedRect {
    double dr;
    double ur;
    double width;
    double height;
};

RotatedRect rotateDetailAnchor(
    double dr,
    double ur,
    double widthValue,
    double heightValue,
    int mapWidth,
    int mapHeight,
    double blockSize,
    int degrees
)
{
    const double mapWidthPx = mapWidth * blockSize;
    const double mapHeightPx = mapHeight * blockSize;

    if (degrees == 90) {
        return {mapHeightPx - ur - heightValue, dr, heightValue, widthValue};
    }
    if (degrees == 180) {
        return {mapWidthPx - dr - widthValue, mapHeightPx - ur - heightValue, widthValue, heightValue};
    }
    if (degrees == 270) {
        return {ur, mapWidthPx - dr - widthValue, heightValue, widthValue};
    }
    return {dr, ur, widthValue, heightValue};
}

bool transformAreaObject(
    const QJsonObject& obj,
    int width,
    int height,
    double blockSize,
    int degrees,
    QJsonObject* out
)
{
    const QString areaType = obj.value(QStringLiteral("Type")).toString();
    if (areaType != QStringLiteral("Line")
        && areaType != QStringLiteral("Circle")
        && areaType != QStringLiteral("Rect")) {
        return false;
    }

    *out = obj;

    if (areaType == QStringLiteral("Line")) {
        QJsonArray points = out->value(QStringLiteral("Point")).toArray();
        QJsonArray newPoints;
        for (const QJsonValue& pointValue : points) {
            QJsonArray point = pointValue.toArray();
            if (point.size() >= 2) {
                const auto rotated = rotateDetailPoint(
                    point.at(0).toDouble(),
                    point.at(1).toDouble(),
                    width,
                    height,
                    blockSize,
                    degrees
                );
                point.replace(0, rotated.first);
                point.replace(1, rotated.second);
            }
            newPoints.append(point);
        }
        out->insert(QStringLiteral("Point"), newPoints);
        return true;
    }

    if (areaType == QStringLiteral("Circle")) {
        if (out->contains(QStringLiteral("DR")) && out->contains(QStringLiteral("UR"))) {
            const auto rotated = rotateDetailPoint(
                out->value(QStringLiteral("DR")).toDouble(),
                out->value(QStringLiteral("UR")).toDouble(),
                width,
                height,
                blockSize,
                degrees
            );
            out->insert(QStringLiteral("DR"), rotated.first);
            out->insert(QStringLiteral("UR"), rotated.second);
        }
        return true;
    }

    if (areaType == QStringLiteral("Rect")) {
        if (out->contains(QStringLiteral("DR"))
            && out->contains(QStringLiteral("UR"))
            && out->contains(QStringLiteral("W"))
            && out->contains(QStringLiteral("H"))) {
            const RotatedRect rotated = rotateDetailAnchor(
                out->value(QStringLiteral("DR")).toDouble(),
                out->value(QStringLiteral("UR")).toDouble(),
                out->value(QStringLiteral("W")).toDouble(),
                out->value(QStringLiteral("H")).toDouble(),
                width,
                height,
                blockSize,
                degrees
            );
            out->insert(QStringLiteral("DR"), rotated.dr);
            out->insert(QStringLiteral("UR"), rotated.ur);
            out->insert(QStringLiteral("W"), rotated.width);
            out->insert(QStringLiteral("H"), rotated.height);
        }
        return true;
    }

    return true;
}

QJsonValue transformValue(
    const QJsonValue& value,
    int width,
    int height,
    double blockSize,
    int degrees,
    const QString& topKey
);

QJsonObject transformObject(
    const QJsonObject& obj,
    int width,
    int height,
    double blockSize,
    int degrees,
    const QString& topKey
)
{
    QJsonObject areaObject;
    if (transformAreaObject(obj, width, height, blockSize, degrees, &areaObject)) {
        return areaObject;
    }

    QJsonObject out = obj;

    if (out.contains(QStringLiteral("BlockDR")) && out.contains(QStringLiteral("BlockUR"))) {
        int footprint = 1;
        if (isBuildingEntry(topKey, out)) {
            footprint = buildingFootprint(out);
        }

        const auto rotated = rotateBlockAnchor(
            out.value(QStringLiteral("BlockDR")).toInt(),
            out.value(QStringLiteral("BlockUR")).toInt(),
            width,
            height,
            degrees,
            footprint
        );
        out.insert(QStringLiteral("BlockDR"), rotated.first);
        out.insert(QStringLiteral("BlockUR"), rotated.second);
    }

    if (out.contains(QStringLiteral("DR")) && out.contains(QStringLiteral("UR"))) {
        const auto rotated = rotateDetailPoint(
            out.value(QStringLiteral("DR")).toDouble(),
            out.value(QStringLiteral("UR")).toDouble(),
            width,
            height,
            blockSize,
            degrees
        );
        out.insert(QStringLiteral("DR"), rotated.first);
        out.insert(QStringLiteral("UR"), rotated.second);
    }

    for (auto it = out.begin(); it != out.end(); ++it) {
        if (it.value().isObject() || it.value().isArray()) {
            it.value() = transformValue(it.value(), width, height, blockSize, degrees, topKey);
        }
    }

    return out;
}

QJsonValue transformValue(
    const QJsonValue& value,
    int width,
    int height,
    double blockSize,
    int degrees,
    const QString& topKey
)
{
    if (value.isArray()) {
        QJsonArray array = value.toArray();
        QJsonArray out;
        for (const QJsonValue& item : array) {
            out.append(transformValue(item, width, height, blockSize, degrees, topKey));
        }
        return out;
    }

    if (value.isObject()) {
        return transformObject(value.toObject(), width, height, blockSize, degrees, topKey);
    }

    return value;
}

std::pair<int, int> rotatedMapSize(int width, int height, int degrees)
{
    if (degrees == 90 || degrees == 270) {
        return {height, width};
    }
    return {width, height};
}

int directionCode(const std::vector<std::vector<int>>& tempMap, int x, int y, int targetType)
{
    const int width = static_cast<int>(tempMap.size());
    const int height = width > 0 ? static_cast<int>(tempMap[0].size()) : 0;

    const int left = (x > 0 && tempMap[x - 1][y] == targetType) ? 1 : 0;
    const int up = (y + 1 < height && tempMap[x][y + 1] == targetType) ? 1 : 0;
    const int right = (x + 1 < width && tempMap[x + 1][y] == targetType) ? 1 : 0;
    const int down = (y > 0 && tempMap[x][y - 1] == targetType) ? 1 : 0;

    const int mainCode = (left << 0) | (up << 1) | (right << 2) | (down << 3);
    if (mainCode) {
        return mainCode;
    }

    const int upLeft = (x > 0 && y + 1 < height && tempMap[x - 1][y + 1] == targetType) ? 1 : 0;
    const int upRight = (x + 1 < width && y + 1 < height && tempMap[x + 1][y + 1] == targetType) ? 1 : 0;
    const int downRight = (x + 1 < width && y > 0 && tempMap[x + 1][y - 1] == targetType) ? 1 : 0;
    const int downLeft = (x > 0 && y > 0 && tempMap[x - 1][y - 1] == targetType) ? 1 : 0;

    int cornerCode = 0;
    if (upLeft && !up && !left) cornerCode |= 0b00010000;
    if (upRight && !up && !right) cornerCode |= 0b00100000;
    if (downRight && !down && !right) cornerCode |= 0b01000000;
    if (downLeft && !down && !left) cornerCode |= 0b10000000;

    return cornerCode;
}

int rebuildShores(QJsonObject* data, int width, int height)
{
    std::map<std::pair<int, int>, QString> cellsByPos;
    for (auto it = data->begin(); it != data->end(); ++it) {
        if (!it.value().isObject()) {
            continue;
        }
        const QJsonObject obj = it.value().toObject();
        if (!obj.contains(QStringLiteral("BlockDR"))
            || !obj.contains(QStringLiteral("BlockUR"))
            || !obj.contains(QStringLiteral("Type"))
            || !obj.contains(QStringLiteral("Num"))) {
            continue;
        }

        const int x = obj.value(QStringLiteral("BlockDR")).toInt();
        const int y = obj.value(QStringLiteral("BlockUR")).toInt();
        if (x >= 0 && x < width && y >= 0 && y < height) {
            cellsByPos[{x, y}] = it.key();
        }
    }

    if (cellsByPos.empty()) {
        return 0;
    }

    std::vector<std::vector<int>> tempMap(width, std::vector<int>(height, 0));
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            const auto it = cellsByPos.find({x, y});
            if (it == cellsByPos.end()) {
                continue;
            }
            const QJsonObject cell = data->value(it->second).toObject();
            tempMap[x][y] = cell.value(QStringLiteral("Type")).toInt() == MAPTYPE_OCEAN ? 1 : 0;
        }
    }

    std::map<std::pair<int, int>, int> newNums;
    std::vector<std::vector<int>> firstPassMap = tempMap;

    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            if (firstPassMap[x][y] != 0) {
                continue;
            }

            const int code = directionCode(firstPassMap, x, y, 1);
            const auto it = kOceanCodeToNum.find(code);
            if (it != kOceanCodeToNum.end()) {
                newNums[{x, y}] = it->second;
                firstPassMap[x][y] = 2;
            }
        }
    }

    std::vector<std::vector<int>> secondPassMap = firstPassMap;
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            if (secondPassMap[x][y] != 0) {
                continue;
            }

            const int code = directionCode(secondPassMap, x, y, 2);
            const auto it = kSandCodeToNum.find(code);
            if (it != kSandCodeToNum.end()) {
                newNums[{x, y}] = it->second;
                secondPassMap[x][y] = 3;
            }
        }
    }

    int changed = 0;
    for (const auto& item : cellsByPos) {
        const int x = item.first.first;
        const int y = item.first.second;
        const QString& key = item.second;
        QJsonObject cell = data->value(key).toObject();
        const int oldNum = cell.value(QStringLiteral("Num")).toInt();
        int newNum = oldNum;

        if (cell.value(QStringLiteral("Type")).toInt() == MAPTYPE_OCEAN) {
            newNum = 0;
        } else {
            const auto it = newNums.find({x, y});
            if (it != newNums.end()) {
                newNum = it->second;
            } else if (kShoreNums.contains(oldNum)) {
                newNum = 0;
            }
        }

        if (newNum != oldNum) {
            cell.insert(QStringLiteral("Num"), newNum);
            data->insert(key, cell);
            ++changed;
        }
    }

    return changed;
}

int countDirectionalCellTypes(const QJsonObject& data)
{
    int count = 0;
    for (auto it = data.begin(); it != data.end(); ++it) {
        if (!it.key().startsWith(QStringLiteral("Cell_")) || !it.value().isObject()) {
            continue;
        }

        const int cellType = it.value().toObject().value(QStringLiteral("Type")).toInt();
        if (cellType != MAPTYPE_FLAT && cellType != MAPTYPE_OCEAN) {
            ++count;
        }
    }
    return count;
}

void validateValue(
    const QJsonValue& value,
    const QString& path,
    int width,
    int height,
    double blockSize,
    QStringList* errors
)
{
    if (value.isArray()) {
        const QJsonArray array = value.toArray();
        for (int i = 0; i < array.size(); ++i) {
            validateValue(array.at(i), QStringLiteral("%1[%2]").arg(path).arg(i), width, height, blockSize, errors);
        }
        return;
    }

    if (!value.isObject()) {
        return;
    }

    const QJsonObject obj = value.toObject();
    const QString topKey = path.section('.', 0, 0).section('[', 0, 0);

    if (obj.contains(QStringLiteral("BlockDR")) && obj.contains(QStringLiteral("BlockUR"))) {
        int footprint = 1;
        if (isBuildingEntry(topKey, obj)) {
            footprint = buildingFootprint(obj);
        }

        const int blockDR = obj.value(QStringLiteral("BlockDR")).toInt();
        const int blockUR = obj.value(QStringLiteral("BlockUR")).toInt();
        if (blockDR < 0 || blockUR < 0 || blockDR + footprint > width || blockUR + footprint > height) {
            errors->append(
                QStringLiteral("%1: BlockDR/BlockUR out of bounds after rotation: (%2, %3), footprint=%4, map=(%5, %6)")
                    .arg(path)
                    .arg(blockDR)
                    .arg(blockUR)
                    .arg(footprint)
                    .arg(width)
                    .arg(height)
            );
        }
    }

    if (obj.contains(QStringLiteral("DR")) && obj.contains(QStringLiteral("UR"))) {
        const double dr = obj.value(QStringLiteral("DR")).toDouble();
        const double ur = obj.value(QStringLiteral("UR")).toDouble();
        const double widthPx = width * blockSize;
        const double heightPx = height * blockSize;
        if (dr < 0.0 || ur < 0.0 || dr >= widthPx || ur >= heightPx) {
            errors->append(
                QStringLiteral("%1: DR/UR out of bounds after rotation: (%2, %3), detail_map=(%4, %5)")
                    .arg(path)
                    .arg(dr)
                    .arg(ur)
                    .arg(widthPx)
                    .arg(heightPx)
            );
        }
    }

    for (auto it = obj.begin(); it != obj.end(); ++it) {
        if (it.value().isObject() || it.value().isArray()) {
            const QString childPath = path.isEmpty() ? it.key() : QStringLiteral("%1.%2").arg(path, it.key());
            validateValue(it.value(), childPath, width, height, blockSize, errors);
        }
    }
}

QStringList validate(const QJsonObject& data, int width, int height, double blockSize)
{
    QStringList errors;
    for (auto it = data.begin(); it != data.end(); ++it) {
        validateValue(it.value(), it.key(), width, height, blockSize, &errors);
    }
    return errors;
}

} // namespace

namespace MapRotation {

bool isSupportedDegrees(int degrees)
{
    return degrees == 90 || degrees == 180 || degrees == 270;
}

Result rotateNjustMapRoot(
    const QJsonObject& root,
    int degrees,
    int mapWidth,
    int mapHeight,
    double blockSize
)
{
    Result result;
    result.root = root;
    result.outputWidth = mapWidth;
    result.outputHeight = mapHeight;

    if (!isSupportedDegrees(degrees)) {
        result.errors.append(QStringLiteral("unsupported rotation degrees: %1").arg(degrees));
        return result;
    }

    QJsonObject rotated;
    for (auto it = root.begin(); it != root.end(); ++it) {
        rotated.insert(
            it.key(),
            transformValue(it.value(), mapWidth, mapHeight, blockSize, degrees, it.key())
        );
    }

    const auto outputSize = rotatedMapSize(mapWidth, mapHeight, degrees);
    result.outputWidth = outputSize.first;
    result.outputHeight = outputSize.second;

    result.directionalCellTypes = countDirectionalCellTypes(rotated);
    if (result.directionalCellTypes > 0) {
        result.warnings.append(
            QStringLiteral("%1 cells use directional/non-flat terrain Type values; positions were rotated but slope Type direction codes were not remapped.")
                .arg(result.directionalCellTypes)
        );
    }

    result.shoreChanges = rebuildShores(&rotated, result.outputWidth, result.outputHeight);
    result.errors = validate(rotated, result.outputWidth, result.outputHeight, blockSize);
    result.root = rotated;
    return result;
}

} // namespace MapRotation
