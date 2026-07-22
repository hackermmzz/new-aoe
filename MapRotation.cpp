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

int objectFootprint(const QString& topKey, const QJsonObject& obj)
{
    if (isBuildingEntry(topKey, obj)) {
        return buildingFootprint(obj);
    }

    if (topKey.startsWith(QStringLiteral("StaticRes_"))) {
        switch (obj.value(QStringLiteral("Num")).toInt()) {
        case NUM_STATICRES_Stone:
        case NUM_STATICRES_GoldOre:
        case NUM_STATICRES_Fish:
            return SIZELEN_SMALL;
        case NUM_STATICRES_Bush:
        default:
            return SIZELEN_SINGEL;
        }
    }

    return SIZELEN_SINGEL;
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

int terrainHeightAt(const std::vector<std::vector<int>>& heights, int x, int y)
{
    const int width = static_cast<int>(heights.size());
    const int height = width > 0 ? static_cast<int>(heights[0].size()) : 0;
    if (x < 0 || y < 0 || x >= width || y >= height) {
        return MAPHEIGHT_FLAT;
    }
    return heights[x][y];
}

bool hasNearbyOcean(const std::vector<std::vector<int>>& heights, int x, int y)
{
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (terrainHeightAt(heights, x + dx, y + dy) == MAPHEIGHT_OCEAN) {
                return true;
            }
        }
    }
    return false;
}

int calculateHeightTerrainType(const std::vector<std::vector<int>>& heights, int x, int y)
{
    const int height = terrainHeightAt(heights, x, y);
    if (height == MAPHEIGHT_OCEAN || hasNearbyOcean(heights, x, y)) {
        return MAPTYPE_OCEAN;
    }

    const auto heightDiff = [&](int dx, int dy) {
        return terrainHeightAt(heights, x + dx, y + dy) - height;
    };
    const int cornerDiff = std::abs(heightDiff(-1, -1))
        + std::abs(heightDiff(1, -1))
        + std::abs(heightDiff(-1, 1))
        + std::abs(heightDiff(1, 1));
    const int edgeDiff = std::abs(heightDiff(-1, 0))
        + std::abs(heightDiff(1, 0))
        + std::abs(heightDiff(0, -1))
        + std::abs(heightDiff(0, 1));

    int type = MAPTYPE_FLAT;
    if (cornerDiff == 1) {
        if (heightDiff(1, 1) == 1) {
            type = MAPTYPE_A3_UPTOR;
        } else if (heightDiff(1, -1) == 1) {
            type = MAPTYPE_A2_DOWNTOU;
        } else if (heightDiff(-1, 1) == 1) {
            type = MAPTYPE_A2_UPTOU;
        } else if (heightDiff(-1, -1) == 1) {
            type = MAPTYPE_A1_UPTOL;
        }
    }

    if (edgeDiff == 2) {
        if (heightDiff(1, 0) == 1 && heightDiff(0, 1) == 1) {
            type = MAPTYPE_A1_DOWNTOL;
        } else if (heightDiff(-1, 0) == 1 && heightDiff(0, -1) == 1) {
            type = MAPTYPE_A3_DOWNTOR;
        } else if ((heightDiff(-1, 0) == 1 && heightDiff(0, 1) == 1)
            || (heightDiff(1, 0) == 1 && heightDiff(0, -1) == 1)) {
            type = MAPTYPE_A0_DOWNTOD;
        }
    } else if (edgeDiff == 1) {
        if (heightDiff(1, 0) == 1) {
            type = MAPTYPE_L3_UPTORD;
        } else if (heightDiff(-1, 0) == 1) {
            type = MAPTYPE_L1_UPTOLU;
        } else if (heightDiff(0, 1) == 1) {
            type = MAPTYPE_L2_UPTORU;
        } else if (heightDiff(0, -1) == 1) {
            type = MAPTYPE_L0_UPTOLD;
        }
    }
    return type;
}

QSet<QString> findScreenSpaceTerrainCorrections(
    const QJsonObject& root,
    int width,
    int height
)
{
    std::vector<std::vector<int>> heights(width, std::vector<int>(height, MAPHEIGHT_FLAT));
    for (auto it = root.begin(); it != root.end(); ++it) {
        if (!it.key().startsWith(QStringLiteral("Cell_")) || !it.value().isObject()) {
            continue;
        }
        const QJsonObject cell = it.value().toObject();
        const int x = cell.value(QStringLiteral("BlockDR")).toInt();
        const int y = cell.value(QStringLiteral("BlockUR")).toInt();
        if (x >= 0 && y >= 0 && x < width && y < height) {
            heights[x][y] = cell.value(QStringLiteral("Height")).toInt(MAPHEIGHT_FLAT);
        }
    }

    QSet<QString> corrections;
    for (auto it = root.begin(); it != root.end(); ++it) {
        if (!it.key().startsWith(QStringLiteral("Cell_")) || !it.value().isObject()) {
            continue;
        }
        const QJsonObject cell = it.value().toObject();
        const int type = cell.value(QStringLiteral("Type")).toInt(MAPTYPE_FLAT);
        if (type == MAPTYPE_FLAT || type == MAPTYPE_OCEAN) {
            continue;
        }
        const int x = cell.value(QStringLiteral("BlockDR")).toInt();
        const int y = cell.value(QStringLiteral("BlockUR")).toInt();
        if (x >= 0 && y >= 0 && x < width && y < height
            && calculateHeightTerrainType(heights, x, y) == MAPTYPE_FLAT) {
            corrections.insert(it.key());
        }
    }
    return corrections;
}

bool isTerrainCellCoveredByObject(
    const QJsonObject& root,
    int cellX,
    int cellY
)
{
    for (auto it = root.begin(); it != root.end(); ++it) {
        if (it.key().startsWith(QStringLiteral("Cell_")) || !it.value().isObject()) {
            continue;
        }
        const QJsonObject obj = it.value().toObject();
        if (!obj.contains(QStringLiteral("BlockDR"))
            || !obj.contains(QStringLiteral("BlockUR"))) {
            continue;
        }
        const int objectX = obj.value(QStringLiteral("BlockDR")).toInt();
        const int objectY = obj.value(QStringLiteral("BlockUR")).toInt();
        const int footprint = objectFootprint(it.key(), obj);
        if (cellX >= objectX && cellX < objectX + footprint
            && cellY >= objectY && cellY < objectY + footprint) {
            return true;
        }
    }
    return false;
}

int rotateDirectionalTerrainType90(int type)
{
    switch (type) {
    case MAPTYPE_A2_UPTOU: return MAPTYPE_A1_UPTOL;
    case MAPTYPE_A1_UPTOL: return MAPTYPE_A2_DOWNTOU;
    case MAPTYPE_A2_DOWNTOU: return MAPTYPE_A3_UPTOR;
    case MAPTYPE_A3_UPTOR: return MAPTYPE_A2_UPTOU;

    case MAPTYPE_A0_DOWNTOD: return MAPTYPE_A3_DOWNTOR;
    case MAPTYPE_A3_DOWNTOR: return MAPTYPE_A0_UPTOD;
    case MAPTYPE_A0_UPTOD: return MAPTYPE_A1_DOWNTOL;
    case MAPTYPE_A1_DOWNTOL: return MAPTYPE_A0_DOWNTOD;

    case MAPTYPE_L1_UPTOLU: return MAPTYPE_L0_UPTOLD;
    case MAPTYPE_L0_UPTOLD: return MAPTYPE_L3_UPTORD;
    case MAPTYPE_L3_UPTORD: return MAPTYPE_L2_UPTORU;
    case MAPTYPE_L2_UPTORU: return MAPTYPE_L1_UPTOLU;
    default: return type;
    }
}

int rotateDirectionalTerrainType(int type, int degrees)
{
    for (int step = 0; step < degrees / 90; ++step) {
        type = rotateDirectionalTerrainType90(type);
    }
    return type;
}

void setTerrainVisual(QJsonObject* cell, int type)
{
    if (type == MAPTYPE_FLAT || type == MAPTYPE_OCEAN) {
        return;
    }
    const int height = cell->value(QStringLiteral("Height")).toInt(MAPHEIGHT_FLAT);
    int offsetX = 0;
    int offsetY = height > MAPHEIGHT_FLAT ? DRAW_OFFSET * height : 0;

    if (type == MAPTYPE_A2_UPTOU || type == MAPTYPE_A0_DOWNTOD
        || type == MAPTYPE_L1_UPTOLU || type == MAPTYPE_L2_UPTORU
        || type == MAPTYPE_A1_DOWNTOL || type == MAPTYPE_A3_DOWNTOR) {
        offsetY = DRAW_OFFSET;
    } else if (type == MAPTYPE_L0_UPTOLD) {
        offsetX = -1;
    } else if (type == MAPTYPE_L3_UPTORD) {
        offsetX = 1;
    } else if (type == MAPTYPE_A0_UPTOD) {
        offsetY = 1;
    }

    cell->insert(QStringLiteral("Type"), type);
    cell->insert(QStringLiteral("Pattern"), MAPPATTERN_GRASS);
    cell->insert(QStringLiteral("Num"), 15 + type);
    cell->insert(QStringLiteral("OffsetX"), offsetX);
    cell->insert(QStringLiteral("OffsetY"), offsetY);
}

void rotateTerrainVisual(QJsonObject* cell, int degrees)
{
    const int oldType = cell->value(QStringLiteral("Type")).toInt(MAPTYPE_FLAT);
    setTerrainVisual(cell, rotateDirectionalTerrainType(oldType, degrees));
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
        const int footprint = objectFootprint(topKey, out);

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

    if (topKey.startsWith(QStringLiteral("Cell_"))) {
        rotateTerrainVisual(&out, degrees);
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
        if (!it.key().startsWith(QStringLiteral("Cell_"))) {
            continue;
        }
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
        const int footprint = objectFootprint(topKey, obj);

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

    const QSet<QString> screenSpaceTerrainCorrections =
        findScreenSpaceTerrainCorrections(root, mapWidth, mapHeight);

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

    for (const QString& key : screenSpaceTerrainCorrections) {
        if (!rotated.value(key).isObject() || !root.value(key).isObject()) {
            continue;
        }
        QJsonObject cell = rotated.value(key).toObject();
        const QJsonObject originalCell = root.value(key).toObject();
        const int originalX = originalCell.value(QStringLiteral("BlockDR")).toInt();
        const int originalY = originalCell.value(QStringLiteral("BlockUR")).toInt();
        if (isTerrainCellCoveredByObject(root, originalX, originalY)) {
            // Hidden orphan slopes can contain black mask pixels that the original
            // object sprite happened to cover. Object sprites are not image-rotated,
            // so use the opaque downward seam cover after rotating the map.
            setTerrainVisual(&cell, MAPTYPE_A0_DOWNTOD);
        } else {
            // Uncovered orphan slopes are intentional screen-space seam covers.
            const int originalType = originalCell.value(QStringLiteral("Type"))
                .toInt(MAPTYPE_FLAT);
            setTerrainVisual(&cell, originalType);
        }
        rotated.insert(key, cell);
    }

    result.directionalCellTypes = countDirectionalCellTypes(rotated);

    result.shoreChanges = rebuildShores(&rotated, result.outputWidth, result.outputHeight);
    result.errors = validate(rotated, result.outputWidth, result.outputHeight, blockSize);
    result.root = rotated;
    return result;
}

} // namespace MapRotation
