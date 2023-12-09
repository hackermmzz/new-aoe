#ifndef BLOCKRES_H
#define BLOCKRES_H

#include <Coordinate.h>
#include <Resource.h>

class Blockres:public Coordinate,public Resource
{
public:
    Blockres();
};

#endif // BLOCKRES_H
