#ifndef BUILDING_RESOURCE_H
#define BUILDING_RESOURCE_H

#include "Building.h"
#include "Resource.h"

class Building_Resource : public Building,public Resource
{
public:
    Building_Resource(){}
    Building_Resource(int Num, int BlockDR, int BlockUR, int civ = CIVILIZATION_IRONAGE , Development* playerScience = NULL, int playerRepresent = MAXPLAYER ,int Percent=100)\
                    :Building(Num,BlockDR,BlockUR,civ,playerScience,playerRepresent,Percent){
        setAttribute();
        init_Blood();
        setFundation();
        setDetailPointAttrb_FormBlock();
        setNowRes();
    }

  /**********************虚函数**************************/
    int getSort(){ return SORT_Building_Resource; }

    void nextframe();
    void setAttribute();

    bool isFarmerGatherable( void* farmer ){ return isGathererAsLandlord((Coordinate*)farmer); }
    /***************指针强制转化****************/
    //若要将Building类指针转化为父类指针,务必用以下函数!
    void printer_ToBuilding_Resource(void **ptr){ *ptr = this; }
    void printer_ToBloodHaver(void** ptr){ *ptr = dynamic_cast<BloodHaver*>(this); }    //传入ptr为BloodHaver类指针的地址
    void printer_ToResource(void** ptr){ *ptr = dynamic_cast<Resource*>(this); }    //传入ptr为Resource类指针的地址
    /*************以上指针强制转化****************/
  /********************以上虚函数**************************/
    bool isGathererAsLandlord(Coordinate* gatherer);

private:
    void setMaxCnt();
    void initGatherer(){ gatherer = NULL; }
    void setGatherer( Coordinate* gatherer){ this->gatherer = gatherer; }

    Coordinate* gatherer = NULL;    
};

#endif // BUILDING_RESOURCE_H
