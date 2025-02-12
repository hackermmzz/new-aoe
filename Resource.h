#ifndef RESOURCE_H
#define RESOURCE_H

#include "config.h"

class Resource
{
public:
    Resource();

   /****************虚函数********************/
    virtual void printer_ToResource(void** ptr){ *ptr = this; }
    virtual void printer_ToAnimal(void** ptr){ *ptr = NULL; }
    virtual void printer_ToBuilding_Resource(void** ptr){ *ptr = NULL; }

    virtual bool isFarmerGatherable( void* farmer ){return true;}
   /****************虚函数********************/
    bool get_Gatherable(){return gatherable;}
    int get_ResourceSort(){ return resourceSort; }
    int get_Cnt(){ return (int)Cnt; }
    int get_ReturnBuildingType();

    void updateCnt_byGather( double gather){  Cnt -= gather ;}
    void updateCnt_byDecay(){ Cnt*=(1-DecayRate); }
    bool is_Surplus(){ return Cnt>0; }

protected:
    double Cnt; //Cnt表示当前剩余的实际资源量

    int MaxCnt;

    bool gatherable = true;
    //false为不可采集 true为可以采集且不再具有其他功能

    double DecayRate=0;
    //腐烂速度

    int resourceSort;
    //指示资源的类型
    //1指代木头 2指代肉 3指代石头 4指代金子
    //eg:    HUMAN_WOOD

    //设置资源为可采集
    void changeToGatherAble(){ gatherable = true; }
};

#endif // RESOURCE_H
