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

    /***************指针强制转化****************/
    //若要将StaticRes类指针转化为父类指针,务必用以下函数!
    void printer_ToCoordinate(Coordinate** ptr){ *ptr = this; }   //传入ptr为Coordinatel类指针的地址
    void printer_ToResource(Resource** ptr){ *ptr = dynamic_cast<Resource*>(this); }    //传入ptr为Resource类指针的地址

    /*************以上指针强制转化****************/

private:

};

#endif // STATICRES_H
