#include "Building.h"

/********************静态资源**************************/
std::list<ImageResource>* Building::build[4];
std::list<ImageResource>* Building::built[3][7];

std::string Building::Buildingname[4]={"Small_Foundation","Foundation","Big_Foundation","Building_House1"};
std::string Building::Builtname[3][7]={
                                        {},
                                       {"House1","Granary","Center1","Stock","Farm","Market","ArrowTower"},
                                       {"House2","Granary","Center2","Stock","Farm","Market","ArrowTower"}
                                      };
std::string Building::BuildDisplayName[7]={"房屋","谷仓","市镇中心","仓库","农场","市场","箭塔"};

/********************静态资源**************************/


/********************构造与析构**************************/
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
    this->Percent=Percent;
    init_Blood();

    setAttribute();
    setDetailPointAttrb_FormBlock();
    setNowRes();

    this->imageX=this->nowres->pix.width()/2.0;
    this->imageY=this->nowres->pix.width()/4.0;
    this->globalNum=10000+g_globalNum;
    g_Object.insert({this->globalNum,this});
    g_globalNum++;
}
/********************构造与析构**************************/


/********************虚函数**************************/
/**********获取*************/
int Building::getSort()
{
    if(Num!=BUILDING_FARM)
    return SORT_BUILDING;
    else return SORT_FARM;
}


/**********设置*************/
void Building::setAction( int actNum)
{
    this->actNum = actNum;

    ActNumToActName();
    actPercent = 0;
    actSpeed= get_retio_Action();
}

void Building::ActNumToActName()
{
    if(Num == BUILDING_CENTER)
    {
        if(actNum == BUILDING_CENTER_CREATEFARMER) actName = ACT_CREATEFARMER;

    }
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

void Building::setAttribute()
{
    //根据房屋种类设置相关信息
    switch (Num) {
    case BUILDING_HOME:
        Foundation=FOUNDATION_SMALL;
        break;
    case BUILDING_ARROWTOWER:
        Foundation=FOUNDATION_SMALL;
        type_Missile = Missile_Arrow;
        break;
    default:
        Foundation=FOUNDATION_MIDDLE;
        break;
    }

    //设置地基大小
    switch (Foundation) {
    case FOUNDATION_SMALL:
        BlockSizeLen = SIZELEN_SMALL;
        break;
    case FOUNDATION_MIDDLE:
        BlockSizeLen = SIZELEN_MIDDLE;
        break;
    case FOUNDATION_BIG:
        BlockSizeLen = SIZELEN_BIG;
        break;
    default:
        break;
    }
}

/**********更新*************/
void Building::nextframe()
{
    setNowRes();
    if(Percent<100)
    {
        nowres = nowlist->begin();
        advance(nowres , (int)(Percent/25));
    }
    else
    {
        nowres++;
        if(nowres==nowlist->end())  nowres=nowlist->begin(); //读到最后回到最初
    }

    this->imageX=this->nowres->pix.width()/2.0;
    this->imageY=this->nowres->pix.width()/4.0;
}

void Building::init_Blood()
{
//    MaxBlood = 600;
    if(Percent == 100) Blood = 1;
    else Blood = 1.0/(double)MaxBlood;
}

/********************虚函数**************************/

void Building::update_Build()
{
    double ratio = get_retio_Build();
    if(Percent<100) Percent+=ratio;
    if(Percent>100) Percent = 100;
    Blood+=ratio/100;

    if(Blood>1) Blood = 1;
}

