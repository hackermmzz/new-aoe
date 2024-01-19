#ifndef STATICRES_H
#define STATICRES_H

#include <Coordinate.h>
#include <Resource.h>

class StaticRes:public Coordinate,public Resource
{
public:
    StaticRes();
    StaticRes(int Num,double DR,double UR);
    StaticRes(int Num, int BlockDR, int BlockUR);
    void nextframe();
    int getSort();

private:

};

#endif // STATICRES_H
