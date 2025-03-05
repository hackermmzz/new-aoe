#ifndef STATICRES_H
#define STATICRES_H

#include "Coordinate.h"
#include "Resource.h"

class StaticRes:public Coordinate,public Resource
{
public:
    /** **********************
    *   Num 0：浆果丛
    *   Num 1: 石头
    *   Num 2: 金矿
    */


    StaticRes(){}
    StaticRes(int Num,double DR,double UR);
    StaticRes(int Num, int BlockDR, int BlockUR);

  /**********************虚函数**************************/
    void nextframe();
    int getSort(){return SORT_STATICRES;}

    QString getChineseName(){ return QString::fromStdString(getStaticResDisplayName(Num)); }

    void setAttribute();
    void setNowRes();
    /***************指针强制转化****************/
    //若要将StaticRes类指针转化为父类指针,务必用以下函数!
    void printer_ToResource(void** ptr){ *ptr = dynamic_cast<Resource*>(this); }    //传入ptr为Resource类指针的地址
    void printer_ToStaticRes(void**ptr){ *ptr = this; }

    /*************以上指针强制转化****************/
  /********************以上虚函数**************************/



  /**********************静态函数**************************/
    //获取资源在资源文件中的名称
    static std::string getStaticResName(int num) {return StaticResname[num];}
    static std::string getStaticResDisplayName(int num){return StaticResDisplayName[num];}

    static std::list<ImageResource>* getStaticResource(int num) { return staticResource[num]; }

    static void allocateStaticResource(int num){staticResource[num]=new std::list<ImageResource>;}

    static void deallocateStaticResource(int num) {delete staticResource[num]; staticResource[num] = nullptr;}

  /**********************以上静态函数**************************/

private:
    static std::string StaticResDisplayName[10];
    static std::string StaticResname[10];
    static std::list<ImageResource> *staticResource[10]; //存储image资源的链表
};

#endif // STATICRES_H
