#include "Building.h"

std::list<ImageResource>* Building::build[4];
std::list<ImageResource>* Building::built[3][7];

std::string Building::Buildingname[4]={"Small_Foundation","Foundation","Big_Foundation","Building_House1"};
std::string Building::Builtname[3][7]={
                                        {},
                                       {"House1","Granary","Center1","Stock","Farm","Market","ArrowTower"},
                                       {"House2","Granary","Center2","Stock","Farm","Market","ArrowTower"}
                                      };

Building::Building()
{

}

Building::Building(int Num, int BlockDR, int BlockUR,int civ,int Percent)
{
    this->Num=Num;
    this->BlockDR=BlockDR;
    this->BlockUR=BlockUR;
    this->civ=civ;
    this->visible=1;
    this->imageH=(BlockDR-BlockUR)*BLOCKSIDELENGTH;
    if(Num==BUILDING_HOME)
    {
        Foundation=FOUNDATION_SMALL;
    }
    else if(Num==BUILDING_ARROWTOWER)
    {
        Foundation=FOUNDATION_SMALL;
    }
    else
    {
        Foundation=FOUNDATION_MIDDLE;
    }
    if(Foundation==FOUNDATION_SMALL)
    {
        this->DR=(BlockDR+1)*BLOCKSIDELENGTH;
        this->UR=(BlockUR+1)*BLOCKSIDELENGTH;
        this->SideLength=2*BLOCKSIDELENGTH;
    }
    if(Foundation==FOUNDATION_MIDDLE)
    {
        this->DR=(BlockDR+1.5)*BLOCKSIDELENGTH;
        this->UR=(BlockUR+1.5)*BLOCKSIDELENGTH;
        this->SideLength=3*BLOCKSIDELENGTH;
    }
    if(Foundation==FOUNDATION_BIG)
    {
        this->DR=(BlockDR+2)*BLOCKSIDELENGTH;
        this->UR=(BlockUR+2)*BLOCKSIDELENGTH;
        this->SideLength=4*BLOCKSIDELENGTH;
    }
    this->Percent=Percent;
    if(Percent!=100)
    {
        this->nowres=build[this->Foundation]->begin();
    }
    else this->nowres=built[civ][this->Num]->begin();
    this->imageX=this->nowres->pix.width()/2.0;
    this->imageY=this->nowres->pix.width()/4.0;
    this->globalNum=10000+g_globalNum;
    g_Object.insert({this->globalNum,this});
    g_globalNum++;
}
