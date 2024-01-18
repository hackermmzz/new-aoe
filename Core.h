#ifndef CORE_H
#define CORE_H

#include <GlobalVariate.h>
#include <Map.h>
#include <Player.h>

class Core
{
public:
    Core();
    void gameUpdate(Map* map, Player* player[], int** memorymap,MouseEvent *mouseEvent);

    bool isValidPoint(const int (&map)[MAP_L][MAP_U], const Point& p);
    vector<Point> getAdjacentPoints(const int (&map)[MAP_L][MAP_U], const Point& p);
    stack<Point> findPath(const int (&findPathMap)[MAP_L][MAP_U],Map *map, const Point& start, const Point& destination);
    stack<Point> findPathAlternative(const int (&map)[MAP_L][MAP_U], const Point& start, const Point& destination);
};

#endif // CORE_H
