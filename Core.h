#ifndef CORE_H
#define CORE_H

#include <GlobalVariate.h>
#include <Map.h>
#include <Player.h>

class Core
{
public:
    Core();
    void gameUpdate(Map* map, Player* player[], int** memorymap);
};

#endif // CORE_H
