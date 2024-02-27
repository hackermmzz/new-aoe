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

  /**********************虚函数**************************/
    void nextframe();
    int getSort();

    /***************指针强制转化****************/
    //若要将StaticRes类指针转化为父类指针,务必用以下函数!
    void printer_ToResource(void** ptr){ *ptr = dynamic_cast<Resource*>(this); }    //传入ptr为Resource类指针的地址

    /*************以上指针强制转化****************/
  /********************以上虚函数**************************/
    static std::string getStaticResDisplayName(int num)
    {
        return StaticResDisplayName[num];
    }
    static std::string getStaticResName(int num)
    {
        return StaticResname[num];
    }
    double getCnt()
    {
        return this->Cnt;
    }
private:
    void setAttribute();
    static std::string StaticResDisplayName[3];
    static std::string StaticResname[3];
    double Cnt;
};

#endif // STATICRES_H
