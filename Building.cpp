#include "Building.h"

/********************静态资源**************************/
std::list<ImageResource>* Building::build[4];
std::list<ImageResource>* Building::built[3][10];

std::string Building::Buildingname[4]={"Small_Foundation","Foundation","Big_Foundation","Building_House1"};
std::string Building::Builtname[3][10]={{},
                                       {"House1","Granary","Center1","Stock","Farm","Market","ArrowTower","ArmyCamp","Stable","Range"},
                                       {"House2","Granary","Center2","Stock","Farm","Market","ArrowTower","ArmyCamp","Stable","Range"}
                                      };
std::string Building::BuildDisplayName[10]={"房屋","谷仓","市镇中心","仓库","农场","市场","箭塔","兵营","马厩","靶场"};

int Building::actNames[BUILDING_TYPE_MAXNUM][ACT_WINDOW_NUM_FREE] = {ACT_NULL};

/********************静态资源**************************/


/********************构造与析构**************************/
Building::Building()
{

}

Building::Building(int Num, int BlockDR, int BlockUR,int civ ,Development* playerScience, int playerRepresent,int Percent)
{
    this->playerScience = playerScience;
    this->playerRepresent = playerRepresent;

    this->Num=Num;
    this->BlockDR=BlockDR;
    this->BlockUR=BlockUR;
    this->civ=civ;
    this->visible=1;
    this->imageH=(BlockDR-BlockUR)*BLOCKSIDELENGTH;
    this->Percent=Percent;
    setAttribute();
    init_Blood();
    setFundation();

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
        else if(actNum == BUILDING_CENTER_UPGRADE) actName = ACT_UPGRADE_AGE;
    }
    else if( Num == BUILDING_GRANARY)
    {
        if(actNum == BUILDING_GRANARY_ARROWTOWER) actName = ACT_UPGRADE_TOWERBUILD;
    }
    else if(Num == BUILDING_STOCK)
    {
        if(actNum == BUILDING_STOCK_UPGRADE_USETOOL) actName = ACT_STOCK_UPGRADE_USETOOL;
        else if(actNum == BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY) actName = ACT_STOCK_UPGRADE_DEFENSE_INFANTRY;
        else if(actNum == BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER) actName = ACT_STOCK_UPGRADE_DEFENSE_ARCHER;
        else if(actNum == BUILDING_STOCK_UPGRADE_DEFENSE_RIDER) actName = ACT_STOCK_UPGRADE_DEFENSE_RIDER;
    }
    else if(Num == BUILDING_ARMYCAMP)
    {
        if(actNum == BUILDING_ARMYCAMP_CREATE_CLUBMAN) actName = ACT_ARMYCAMP_CREATE_CLUBMAN;
        else if(actNum == BUILDING_ARMYCAMP_UPGRADE_CLUBMAN) actName = ACT_ARMYCAMP_UPGRADE_CLUBMAN;
        else if(actNum == BUILDING_ARMYCAMP_CREATE_SLINGER) actName = ACT_ARMYCAMP_CREATE_SLINGER;
    }
    else if(Num == BUILDING_MARKET)
    {
        if(actNum == BUILDING_MARKET_WOOD_UPGRADE) actName = ACT_UPGRADE_WOOD;
        else if(actNum == BUILDING_MARKET_STONE_UPGRADE) actName = ACT_UPGRADE_STONE;
        else if(actNum == BUILDING_MARKET_FARM_UPGRADE) actName = ACT_UPGRADE_FARM;
        else if(actNum == BUILDING_MARKET_GOLD_UPGRADE) actName = ACT_UPGRADE_GOLD;
    }
    else if( Num == BUILDING_RANGE)
    {
        if(actNum == BUILDING_RANGE_CREATE_BOWMAN) actName = ACT_RANGE_CREATE_BOWMAN;
    }
    else if(Num == BUILDING_STABLE)
    {
        if(actNum == BUILDING_STABLE_CREATE_SCOUT) actName =ACT_STABLE_CREATE_SCOUT;
    }
}

void Building::setNowRes()
{
    std::list<ImageResource>* tempNowlist = NULL;
    if(Percent<100) tempNowlist = Building::build[Foundation];
    else tempNowlist = Building::built[get_civilization()][Num];

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
    case BUILDING_CENTER:
        MaxBlood = BLOOD_BUILD_CENTER;
        Foundation = FOUNDATION_MIDDLE;
        break;
    case BUILDING_HOME:
        MaxBlood = BLOOD_BUILD_HOUSE;
        Foundation=FOUNDATION_SMALL;
        break;
    case BUILDING_STOCK:
        MaxBlood = BLOOD_BUILD_STOCK;
        Foundation = FOUNDATION_MIDDLE;
        break;
    case BUILDING_GRANARY:
        MaxBlood = BLOOD_BUILD_GRANARY;
        Foundation = FOUNDATION_MIDDLE;
        break;
    case BUILDING_ARMYCAMP:
        MaxBlood = BLOOD_BUILD_ARMYCAMP;
        Foundation = FOUNDATION_MIDDLE;
        break;
    case BUILDING_MARKET:
        MaxBlood = BLOOD_BUILD_MARKET;
        Foundation = FOUNDATION_MIDDLE;
        break;
    case BUILDING_RANGE:
        MaxBlood = BLOOD_BUILD_RANGE;
        Foundation = FOUNDATION_MIDDLE;
        break;
    case BUILDING_STABLE:
        MaxBlood = BLOOD_BUILD_STABLE;
        Foundation = FOUNDATION_MIDDLE;
        break;
    case BUILDING_ARROWTOWER:
        MaxBlood = BLOOD_BUILD_ARROWTOWER;
        Foundation=FOUNDATION_SMALL;
        atk = ATK_BUILD_ARROWTOWER;
        defence_shoot = DEFSHOOT_BUILD_ARROWTOWER;
        attackType = ATTACKTYPE_SHOOT;
        type_Missile = Missile_Arrow;
        missionThrowStep = THROWMISSION_ARROWTOWN_TIMER;
        dis_Attack = DIS_ARROWTOWER;

        isAttackable = true;
        break;

    case BUILDING_WALL:
        MaxBlood = BLOOD_BUILD_WALL;
        Foundation = FOUNDATION_SMALL;
        defence_shoot = DEFSHOOT_BUILD_WALL;
        break;
    default:
        incorrectNum = true;
        Foundation=FOUNDATION_MIDDLE;
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
        if(nowres==nowlist->end())
        {
            nowres=nowlist->begin(); //读到最后回到最初
            initAttack_perCircle();
        }

        if(defencing) missionThrowTimer = missionThrowTimer == missionThrowStep? 0: missionThrowTimer+1;
    }

    this->imageX=this->nowres->pix.width()/2.0;
    this->imageY=this->nowres->pix.width()/4.0;
}

void Building::init_Blood()
{
    if(Percent == 100) Blood = 1;
    else Blood = 1.0/(double)getMaxBlood();
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

void Building::setFundation()
{
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


void Building::setActStatus(int wood , int food , int stone , int gold)
{
    int actionName, actionNumber;

    for(int position = 0 ; position<ACT_WINDOW_NUM_FREE;position++)
    {
        actionName = getActNames(position);
        actionNumber = ActNameToActNum(actionName);

        if(actionNumber>-1 && !playerScience->get_isBuildActionAble(Num , actionNumber , get_civilization(),wood,food,stone,gold))
            actStatus[position] = ACT_STATUS_DISABLED;
        else actStatus[position] = ACT_STATUS_ENABLED;
    }
}
