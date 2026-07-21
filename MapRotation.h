#ifndef MAPROTATION_H
#define MAPROTATION_H

#include <QJsonObject>
#include <QStringList>

namespace MapRotation {

struct Result {
    QJsonObject root;
    QStringList warnings;
    QStringList errors;
    int shoreChanges = 0;
    int directionalCellTypes = 0;
    int outputWidth = 0;
    int outputHeight = 0;
};

bool isSupportedDegrees(int degrees);

Result rotateNjustMapRoot(
    const QJsonObject& root,
    int degrees,
    int mapWidth,
    int mapHeight,
    double blockSize
);

}

#endif // MAPROTATION_H
