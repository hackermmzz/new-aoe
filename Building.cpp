#include "Building.h"

std::list<ImageResource>* Building::build[4];
std::list<ImageResource>* Building::built[3][7];

std::string Building::Buildingname[4]={"Small_Foundation","Foundation","Big_Foundation","Building_House1"};
std::string Building::Builtname[3][7]={
                                        {},
                                       {"House1","Granary","Center1","Stock","Farm","Market","ArrowTower"},
                                       {"House2","Granary","Center2","Stock","Farm","Market","ArrowTower"}
                                      };
std::string Building::BuildDisplayName[7]={"房屋","谷仓","市镇中心","仓库","农场","市场","箭塔"};

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
        type_Missile = Missile_Arrow;
    }
    else
    {
        Foundation=FOUNDATION_MIDDLE;
    }

    if(Foundation==FOUNDATION_SMALL)
    {
        BlockSizeLen = SIZELEN_SMALL;
    }
    else if(Foundation==FOUNDATION_MIDDLE)
    {
        BlockSizeLen = SIZELEN_MIDDLE;
    }
    else if(Foundation==FOUNDATION_BIG)
    {
        BlockSizeLen = SIZELEN_BIG;
    }

    setDetailPointAttrb_FormBlock();


    this->Percent=Percent;
    setNowRes();

    this->imageX=this->nowres->pix.width()/2.0;
    this->imageY=this->nowres->pix.width()/4.0;
    this->globalNum=10000+g_globalNum;
    g_Object.insert({this->globalNum,this});
    g_globalNum++;
}

int Building::getSort()
{
    if(Num!=BUILDING_FARM)
    return SORT_BUILDING;
    else return SORT_FARM;
}

void Building::setAction( int actNum)
{
    this->actNum = actNum;
//    this->actName =

    actPercent = 0;
    ratio_Action = get_retio_Action();
}

void Building::update_Build()
{
    double ratio = get_retio_Build();
    if(Percent<100) Percent+=ratio;
    Blood+=ratio/100;

    if(Blood>1) Blood = 1;
}

void Building::setNowRes()
{
    std::list<ImageResource>* tempNowlist = NULL;
    if(Percent<100) tempNowlist = build[Foundation];
    else tempNowlist = built[civ][Num];

    if(tempNowlist != nowlist)
    {
        nowlist = tempNowlist;
        nowres = nowlist->begin();
    }
}

void Building::nextframe()
{

    setNowRes();
    if(Percent<100) advance(nowres , Percent/25);
    else
    {
        nowres++;
        if(nowres==nowlist->end())  nowres=nowlist->begin(); //读到最后回到最初
    }

    this->imageX=this->nowres->pix.width()/2.0;
    this->imageY=this->nowres->pix.width()/4.0;
}
