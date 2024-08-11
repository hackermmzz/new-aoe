#include "Building.h"

/********************静态资源**************************/
std::list<ImageResource>* Building::build[4];
std::list<ImageResource>* Building::built[3][10];
std::list<ImageResource>* Building::buildFire[3];

std::string Building::Buildingname[4]={"Small_Foundation","Foundation","Big_Foundation","Building_House1"};
std::string Building::Builtname[3][10]={{},
                                       {"House1","Granary","Center1","Stock","Farm","Market","ArrowTower","ArmyCamp","Stable","Range"},
                                       {"House2","Granary","Center2","Stock","Farm","Market","ArrowTower","ArmyCamp","Stable","Range"}
                                      };
std::string Building::BuildDisplayName[10]={"房屋","谷仓","市镇中心","仓库","农场","市场","箭塔","兵营","马厩","靶场"};

std::string Building::BuildFireName[3] = { "S_Fire", "M_Fire", "B_Fire"};

std::string Building::sound_click[10] = {\
    "Click_House","Click_Granary","Click_Center","Click_Stock","Click_Farm","Click_Market","Villager_ArrowTower","Click_ArmyCamp","Click_Stable","Click_Range"\
};

int Building::actNames[BUILDING_TYPE_MAXNUM][ACT_WINDOW_NUM_FREE] = {ACT_NULL};

int Building::BuildingMaxBlood[10] = { BLOOD_BUILD_HOUSE,  BLOOD_BUILD_GRANARY, BLOOD_BUILD_CENTER, BLOOD_BUILD_STOCK, BLOOD_BUILD_FARM,\
                                     BLOOD_BUILD_MARKET, BLOOD_BUILD_ARROWTOWER, BLOOD_BUILD_ARMYCAMP, BLOOD_BUILD_STABLE, BLOOD_BUILD_RANGE};

int Building::BuildingFundation[10] = { FOUNDATION_SMALL, FOUNDATION_MIDDLE, FOUNDATION_MIDDLE, FOUNDATION_MIDDLE, FOUNDATION_MIDDLE,\
                                      FOUNDATION_MIDDLE, FOUNDATION_SMALL, FOUNDATION_MIDDLE, FOUNDATION_MIDDLE, FOUNDATION_MIDDLE,};

int Building::BuildingVision[10] = { VISION_HOME, VISION_GRANARY, VISION_CENTER, VISION_STOCK, VISION_FARM,\
                                   VISION_MARKET, VISION_ARROWTOWER, VISION_ARMYCAMP, VISION_STABLE, VISION_RANGE};

/********************静态资源**************************/
bool is_cheatAction = false;
extern Score usrScore;


/********************构造与析构**************************/
Building::Building(int Num, int BlockDR, int BlockUR, int civ, Development* playerScience, int playerRepresent, int Percent)
{
    this->playerScience = playerScience;
    this->playerRepresent = playerRepresent;

    this->Num=Num;
    setBlockDRUR(BlockDR, BlockUR);
    this->civ=civ;
    this->visible=1;
    this->imageH=(BlockDR-BlockUR)*BLOCKSIDELENGTH;
    this->Percent=Percent;
    if(isFinish()) recordConstruct();
    setAttribute();
    init_Blood();
    setFundation();

    setDetailPointAttrb_FormBlock();
    setNowRes();

    updateImageXYByNowRes();
    this->globalNum=10000*SORT_BUILDING+g_globalNum;
    g_Object.insert({this->globalNum,this});
    g_globalNum++;
}


/********************虚函数**************************/
/***************状态与图像显示****************/
void Building::nextframe()
{
    setNowRes();
    if(Percent<100)
    {
        nowres = nowlist->begin();
        advance(nowres, (int)(Percent/25));
    }
    else
    {
        nowres++;
        if(nowres==nowlist->end())
        {
            nowres=nowlist->begin(); //读到最后回到最初
            initAttack_perCircle();
        }

        if(fireNowList != NULL)
        {
            fireNowRes++;

            if(fireNowRes == fireNowList->end())
                fireNowRes = fireNowList->begin();

            this->fireImageX = fireNowRes->pix.width()/2.0;
            this->fireImageY = fireNowRes->pix.width()/4.0;
        }

        if(defencing)
            missionThrowTimer = missionThrowTimer == missionThrowStep ? 0 : missionThrowTimer+1;
    }

    updateImageXYByNowRes();
}

void Building::setNowRes()
{
    std::list<ImageResource>* tempNowlist = NULL;
    if(Percent<100) tempNowlist = Building::build[Foundation];
    else
    {
        tempNowlist = Building::built[get_civilization()][Num];
        setFireNowRes();
    }

    if(tempNowlist != nowlist)
    {
        nowlist = tempNowlist;
        nowres = nowlist->begin();
    }
}


/*******状态与属性设置、获取*******/
void Building::setAttribute()
{
    /*********特殊设定**********/
    switch (Num) {
    case BUILDING_CENTER:
    case BUILDING_HOME:
    case BUILDING_STOCK:
    case BUILDING_GRANARY:
    case BUILDING_ARMYCAMP:
    case BUILDING_MARKET:
    case BUILDING_RANGE:
    case BUILDING_STABLE:
    case BUILDING_WALL:
        break;
    case BUILDING_ARROWTOWER:
        atk = ATK_BUILD_ARROWTOWER;
        defence_shoot = DEFSHOOT_BUILD_ARROWTOWER;
        dis_Attack = DIS_ARROWTOWER;
        attackType = ATTACKTYPE_SHOOT;
        type_Missile = Missile_Arrow;
        missionThrowStep = THROWMISSION_ARROWTOWN_TIMER;

        isAttackable = true;
        break;
    default:
        incorrectNum = true;
        Foundation=FOUNDATION_MIDDLE;
        break;
    }

    if(incorrectNum)
        return;

    MaxBlood = BuildingMaxBlood[Num];
    Foundation = BuildingFundation[Num];
    vision = BuildingVision[Num];
}

int Building::getVision()
{
    if(getNum() == BUILDING_ARROWTOWER)
        return vision + playerScience->get_addition_DisAttack(getSort(), Num, 0, get_AttackType());
    else
        return vision;
}

bool Building::isMonitorObject(Coordinate* judOb)
{
    if(Num == BUILDING_ARROWTOWER)
        return judOb->isPlayerControl() && judOb->getPlayerRepresent() != getPlayerRepresent();

    return false;
}

int Building::get_civilization()
{
    if(playerScience == NULL)
        return CIVILIZATION_STONEAGE;
    else
        return playerScience->get_civilization();
}

void Building::init_Blood()
{
    if(Percent == 100) Blood = 1;
    else Blood = 1.0/(double)getMaxBlood();
}


/*******行动相关*******/
void Building::setAction( int actNum)
{
    this->actNum = actNum;

    ActNumToActName();
    initActionPersent();
    playerScience->BuildingActionExecuting(Num, actNum);
    actSpeed= get_retio_Action();
}

void Building::initAction()
{
    if(actSpeed != 0 && actNum != ACT_NULL)
        playerScience->BuildingActionOverExecuting(Num, actNum);

    actName = ACT_NULL;
    actNum = ACT_NULL;
    actSpeed = 0;
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


/*******战斗相关*******/
double Building::getDis_attack()
{
    if(getNum() == BUILDING_ARROWTOWER)
        return ( dis_Attack + playerScience->get_addition_DisAttack(getSort(),Num,0,get_AttackType()) )*BLOCKSIDELENGTH;
    else return 0;
}

/********************虚函数**************************/


/********************静态函数**************************/
void Building::deallocatebuild(int i)
{
    delete build[i];
    build[i] = nullptr;
}

void Building::deallocatebuilt(int i,int j)
{
    delete built[i][j];
    built[i][j] = nullptr;
}

void Building::deallocatebuildFire(int type)
{
    if(buildFire[type]!=nullptr)
    {
        delete buildFire[type];
        buildFire[type] = nullptr;
    }
}

/********************静态函数**************************/


/********************************************/
/*****************act相关***************/
void Building::update_Build()
{
    double ratio = get_retio_Build();

    if(!constructed)
    {
        Percent+=ratio;
        if(Percent>100) Percent = 100;
    }

    Blood+=ratio/100;

    if(Blood>1) Blood = 1;
}

void Building::update_Action(){
    actPercent += actSpeed;
    if(actPercent > 100) actPercent = 100;
}

void Building::setActStatus(int wood , int food , int stone , int gold)
{
    int actionName, actionNumber;

    for(int position = 0; position<ACT_WINDOW_NUM_FREE; position++)
    {
        actionName = getActNames(position);
        actionNumber = ActNameToActNum(actionName);

        if(actionNumber>-1 && !playerScience->get_isBuildActionAble(Num, actionNumber, get_civilization(), wood, food, stone, gold))
            actStatus[position] = ACT_STATUS_DISABLED;
        else actStatus[position] = ACT_STATUS_ENABLED;
    }
}

double Building::get_retio_Build()
{
    if(is_cheatAction) return 100.0;
    else return 100.0/playerScience->get_buildTime(Num)/FRAMES_PER_SECOND;
}

double Building::get_retio_Action()
{
    if(is_cheatAction) return 100.0;
    else return 100.0/playerScience->get_actTime(Num, actNum)/FRAMES_PER_SECOND;
}


/*******状态与属性设置、获取*******/
bool Building::isMatchResourceType(int resourceType)
{
    if(Num == BUILDING_CENTER)
        return true;

    if(Num == BUILDING_STOCK && ( resourceType == HUMAN_WOOD || resourceType == HUMAN_GOLD || resourceType == HUMAN_STONE || resourceType == HUMAN_STOCKFOOD ))
        return true;

    if(Num == BUILDING_GRANARY &&  resourceType == HUMAN_GRANARYFOOD )
        return true;

    return false;
}

//依据fundation设置数据
void Building::setFundation()
{
    //设置地基大小
    switch (Foundation) {
    case FOUNDATION_SMALL:
        BlockSizeLen = SIZELEN_SMALL;
        crashLength = CRASHBOX_SMALL;
        break;
    case FOUNDATION_MIDDLE:
        BlockSizeLen = SIZELEN_MIDDLE;
        crashLength = CRASHBOX_MIDDLE;
        break;
    case FOUNDATION_BIG:
        BlockSizeLen = SIZELEN_BIG;
        crashLength = CRASHBOX_BIG;
        break;
    default:
        break;
    }
}


/***************状态与图像显示****************/
void Building::setFireNowRes()
{
    std::list<ImageResource>* tempNowlist = NULL;

    if(Blood <= BUILDING_BLOOD_FIRE_BIG)
        tempNowlist = buildFire[BUILDING_FIRE_BIG];
    else if(Blood <= BUILDING_BLOOD_FIRE_MIDDLE)
        tempNowlist = buildFire[BUILDING_FIRE_MIDDLE];
    else if(Blood <= BUILDING_BLOOD_FIRE_SMALL)
        tempNowlist = buildFire[BUILDING_FIRE_SMALL];

    if(fireNowList != tempNowlist)
    {
        fireNowList = tempNowlist;
        if(fireNowList != NULL)
            fireNowRes = fireNowList->begin();
    }

    return;
}
