#include "Building.h"
#include "Player.h"
#include "RuntimeConfig.h"
/********************静态资源**************************/
std::list<ImageResource>* Building::build[4];
std::list<ImageResource>* Building::built[4][2][BUILDING_TYPE_MAXNUM];
std::list<ImageResource>* Building::buildFire[3];

array<std::string,4> Building::Buildingname;
array<array<array<std::string,BUILDING_TYPE_MAXNUM>,2>,4> Building::Builtname;
array<std::string,BUILDING_TYPE_MAXNUM>  Building::BuildDisplayName;
array<std::string,3> Building::BuildFireName;

array<string,BUILDING_TYPE_MAXNUM> Building::sound_click;

array<array<int,ACT_WINDOW_NUM_FREE>,BUILDING_TYPE_MAXNUM> Building::actNames;
array<int,BUILDING_TYPE_MAXNUM> Building::BuildingMaxBlood;

array<int,BUILDING_TYPE_MAXNUM>Building::BuildingFundation;

array<int,BUILDING_TYPE_MAXNUM> Building::BuildingVision;

std::list<ImageResource>* Building::builtArrowTowerUpgraded[4][2];

std::string Building::getArrowTowerUpgradedResourceName(int age, int isEnemy)
{
    (void)age;
    // 与资源文件名一致：ArrowTower2_Egypt_###.png（我方）、ArrowTower2_Daiwa_###.png（敌方）
    // resMap 键为最后一个 '_' 前的前缀（见 InitImageResMap）
    return isEnemy ? std::string("ArrowTower2_Daiwa") : std::string("ArrowTower2_Egypt");
}

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
    if (Num == BUILDING_ARROWTOWER && getArrowTowerUpgradeLevel() >= 1)
        missionThrowStep = THROWMISSION_ARROWTOWER_UPGRADED;
    else if (Num == BUILDING_ARROWTOWER)
        missionThrowStep = THROWMISSION_ARROWTOWN_TIMER;

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
        // 判断敌我: 如果 playerRepresent != NOWPLAYERREPRESENT，则为敌方
        int isEnemy = (playerRepresent != NOWPLAYERREPRESENT) ? 1 : 0;
        int civ = get_civilization();
        //转化后的建筑保持转化时刻的时代贴图不降级(取较高者)
        if (levelFrozen && frozenCiv > civ) civ = frozenCiv;
        if (Num == BUILDING_ARROWTOWER && getArrowTowerUpgradeLevel() >= 1
            && civ >= 1 && civ <= 3
            && builtArrowTowerUpgraded[civ][isEnemy] != NULL
            && !builtArrowTowerUpgraded[civ][isEnemy]->empty())
            tempNowlist = builtArrowTowerUpgraded[civ][isEnemy];
        else
            tempNowlist = Building::built[civ][isEnemy][Num];
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
    case BUILDING_DOCK:
    case BUILDING_SIEGE:
    case BUILDING_COLLAGE:
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
        return vision + getArrowTowerRangeAddition();
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
        else if(actNum == BUILDING_CENTER_UPGRADE) {
            // 与市场木材/工艺、仓库工具/金属加工相同：用 getActLevel 区分同 actCon 链上的阶段
            if(playerScience != NULL && playerScience->getActLevel(BUILDING_CENTER, BUILDING_CENTER_UPGRADE) >= 1)
                actName = ACT_UPGRADE_BRONZEAGE;
            else
                actName = ACT_UPGRADE_AGE;
        }
    }
    else if( Num == BUILDING_GRANARY)
    {
        if(actNum == BUILDING_GRANARY_ARROWTOWER) actName = ACT_UPGRADE_TOWERBUILD;
        else if(actNum == BUILDING_GRANARY_ARROWTOWE_UPGRADE) actName = ACT_UPGRADE_ARROWTOWER;
    }
    else if(Num == BUILDING_STOCK)
    {
        if(actNum == BUILDING_STOCK_UPGRADE_USETOOL) {
            // 根据当前节点的时代返回不同的动作ID
            if(playerScience != NULL && playerScience->getActLevel(BUILDING_STOCK, BUILDING_STOCK_UPGRADE_USETOOL) >= 1)
                actName = ACT_STOCK_UPGRADE_METALWORKING;
            else
                actName = ACT_STOCK_UPGRADE_USETOOL;
        }
        else if(actNum == BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY) {
            if(playerScience != NULL && playerScience->getActLevel(BUILDING_STOCK, BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY) >= 1)
                actName = ACT_STOCK_UPGRADE_DEFENSE_INFANTRY_SCALE;
            else
                actName = ACT_STOCK_UPGRADE_DEFENSE_INFANTRY;
        }
        else if(actNum == BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER) {
            if(playerScience != NULL && playerScience->getActLevel(BUILDING_STOCK, BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER) >= 1)
                actName = ACT_STOCK_UPGRADE_DEFENSE_ARCHER_SCALE;
            else
                actName = ACT_STOCK_UPGRADE_DEFENSE_ARCHER;
        }
        else if(actNum == BUILDING_STOCK_UPGRADE_DEFENSE_RIDER) {
            if(playerScience != NULL && playerScience->getActLevel(BUILDING_STOCK, BUILDING_STOCK_UPGRADE_DEFENSE_RIDER) >= 1)
                actName = ACT_STOCK_UPGRADE_DEFENSE_RIDER_SCALE;
            else
                actName = ACT_STOCK_UPGRADE_DEFENSE_RIDER;
        }
        else if(actNum == BUILDING_STOCK_UPGRADE_MISSILE_DEFENSE_INFANTRY) actName = ACT_STOCK_UPGRADE_MISSILE_DEFENSE_INFANTRY;
    }
    else if(Num == BUILDING_ARMYCAMP)
    {
        if(actNum == BUILDING_ARMYCAMP_CREATE_CLUBMAN) actName = ACT_ARMYCAMP_CREATE_CLUBMAN;
        else if(actNum == BUILDING_ARMYCAMP_UPGRADE_CLUBMAN) actName = ACT_ARMYCAMP_UPGRADE_CLUBMAN;
        else if(actNum == BUILDING_ARMYCAMP_CREATE_SLINGER) actName = ACT_ARMYCAMP_CREATE_SLINGER;
        else if(actNum == BUILDING_ARMYCAMP_CREATE_BROADSWORD) actName = ACT_ARMYCAMP_CREATE_BROADSWORD;
        else if(actNum == BUILDING_ARMYCAMP_UPGRADE_BROADSWORD) actName = ACT_ARMYCAMP_UPGRADE_BROADSWORD;
        else if(actNum == BUILDING_ARMYCAMP_RESEARCH_LOGISTICS) actName = ACT_ARMYCAMP_RESEARCH_LOGISTICS;
    }
    else if(Num == BUILDING_MARKET)
    {
        if(actNum == BUILDING_MARKET_WOOD_UPGRADE) {
            // 根据当前节点的时代返回不同的动作ID
            if(playerScience != NULL && playerScience->getActLevel(BUILDING_MARKET, BUILDING_MARKET_WOOD_UPGRADE) >= 1)
                actName = ACT_UPGRADE_CRAFT;
            else
                actName = ACT_UPGRADE_WOOD;
        }
        else if(actNum == BUILDING_MARKET_STONE_UPGRADE) actName = ACT_UPGRADE_STONE;
        else if(actNum == BUILDING_MARKET_FARM_UPGRADE) {
            // 根据当前节点的时代返回不同的动作ID
            if(playerScience != NULL && playerScience->getActLevel(BUILDING_MARKET, BUILDING_MARKET_FARM_UPGRADE) >= 1)
                actName = ACT_UPGRADE_PLOW;
            else
                actName = ACT_UPGRADE_FARM;
        }
        else if(actNum == BUILDING_MARKET_GOLD_UPGRADE) actName = ACT_UPGRADE_GOLD;
        else if(actNum == BUILDING_MARKET_WHEEL_UPGRADE) actName = ACT_UPGRADE_WHEEL;
       }
    else if( Num == BUILDING_RANGE)
    {
        if(actNum == BUILDING_RANGE_CREATE_BOWMAN) actName = ACT_RANGE_CREATE_BOWMAN;
        else if(actNum == BUILDING_RANGE_CREATE_CHARIOT_ARCHER) actName = ACT_RANGE_CREATE_CHARIOT_ARCHER;
        else if(actNum == BUILDING_RANGE_CREATE_COMPOSITE_BOWMAN) actName = ACT_RANGE_CREATE_COMPOSITE_BOWMAN;
        else if(actNum == BUILDING_RANGE_UPGRADE_COMPOSITE_BOW) actName = ACT_RANGE_UPGRADE_COMPOSITE_BOW;
    }
    else if(Num == BUILDING_STABLE)
    {
        if(actNum == BUILDING_STABLE_CREATE_SCOUT) actName =ACT_STABLE_CREATE_SCOUT;
        else if(actNum == BUILDING_STABLE_CREATE_CHARIOT) actName = ACT_STABLE_CREATE_CHARIOT;
        else if(actNum == BUILDING_STABLE_CREATE_CAVALRY) actName = ACT_STABLE_CREATE_CAVALRY;
    }
    else if(Num == BUILDING_DOCK)
    {
        if(actNum == BUILDING_DOCK_CREATE_SAILING) actName =ACT_DOCK_CREATE_SAILING;
        else if(actNum == BUILDING_DOCK_CREATE_WOOD_BOAT) actName = ACT_DOCK_CREATE_WOOD_BOAT;
        else if(actNum == BUILDING_DOCK_CREATE_SHIP) actName = ACT_DOCK_CREATE_SHIP;
    }
    else if(Num == BUILDING_SIEGE)
    {
        if(actNum == BUILDING_SIEGE_CREATE_STONE_THROWER) actName =ACT_SIEGE_CREATE_STONE_THROWER;
    }
    else if(Num == BUILDING_COLLAGE)
    {
        if(actNum == BUILDING_COLLAGE_CREATE_HOPLITE) actName = ACT_COLLAGE_CREATE_HOPLITE;
    }
}


/*******战斗相关*******/
double Building::getDis_attack()
{
    if(getNum() == BUILDING_ARROWTOWER)
        return ( dis_Attack + getArrowTowerRangeAddition() )*BLOCKSIDELENGTH;
    else return 0;
}

int Building::get_add_specialAttack()
{
    if (Num == BUILDING_ARROWTOWER)
    {
        int addition = (playerScience != NULL) ? playerScience->get_addition_Attack(SORT_BUILDING, BUILDING_ARROWTOWER, 0, get_AttackType()) : 0;
        //转化后的箭塔保持转化时刻的攻击科技加成不降级(取较高者)
        if (levelFrozen && frozenAtkAddition > addition) addition = frozenAtkAddition;
        return addition;
    }
    return 0;
}

int Building::showATK_Basic()
{
    if (Num == BUILDING_ARROWTOWER)
        return atk;
    return BloodHaver::showATK_Basic();
}

int Building::showATK_Addition()
{
    if (Num == BUILDING_ARROWTOWER)
        return get_add_specialAttack();
    return BloodHaver::showATK_Addition();
}

int Building::showArrowTowerRangeBaseBlocks() const
{
    if (Num != BUILDING_ARROWTOWER)
        return 0;
    return (int)dis_Attack;
}

int Building::showArrowTowerRangeBonusBlocks() const
{
    if (Num != BUILDING_ARROWTOWER)
        return 0;
    return getArrowTowerRangeAddition();
}

int Building::getArrowTowerUpgradeLevel() const
{
    int level = (playerScience != NULL) ? playerScience->getActLevel(BUILDING_GRANARY, BUILDING_GRANARY_ARROWTOWE_UPGRADE) : 0;
    //转化后的箭塔保持转化时刻的强化等级不降级(取较高者)
    if (levelFrozen && frozenArrowTowerLevel > level) level = frozenArrowTowerLevel;
    return level;
}

int Building::getArrowTowerRangeAddition() const
{
    int addition = (playerScience != NULL) ? playerScience->get_addition_DisAttack(SORT_BUILDING, BUILDING_ARROWTOWER, 0, ATTACKTYPE_SHOOT) : 0;
    //转化后的箭塔保持转化时刻的射程/视野科技加成不降级(取较高者)
    if (levelFrozen && frozenRangeAddition > addition) addition = frozenRangeAddition;
    return addition;
}

//转化等级冻结（建筑）：快照转化时刻(原主人科技下的)时代与箭塔科技等级。
//与单位的 freezeStats 不同，此处不做永久锁定，而是此后取"快照值"与"当前主人科技实时值"
//中的较高者，保证转化后不降级，且新主人后续研究更高科技时仍能生效（与原版一致：
//建筑升级科技作用于所有己方建筑）。血量与血量上限不在快照范围内，保持原有机制不变
void Building::freezeLevel()
{
    if (levelFrozen) return;    //已冻结则保持首次快照

    frozenCiv = get_civilization();
    if (Num == BUILDING_ARROWTOWER && playerScience != NULL)
    {
        frozenArrowTowerLevel = playerScience->getActLevel(BUILDING_GRANARY, BUILDING_GRANARY_ARROWTOWE_UPGRADE);
        frozenAtkAddition = playerScience->get_addition_Attack(SORT_BUILDING, BUILDING_ARROWTOWER, 0, ATTACKTYPE_SHOOT);
        frozenRangeAddition = playerScience->get_addition_DisAttack(SORT_BUILDING, BUILDING_ARROWTOWER, 0, ATTACKTYPE_SHOOT);
    }

    levelFrozen = true;
}

/********************虚函数**************************/


/********************静态函数**************************/
void Building::deallocatebuild(int i)
{
    delete build[i];
    build[i] = nullptr;
}

void Building::deallocatebuilt(int age, int isEnemy, int buildType)
{
    delete built[age][isEnemy][buildType];
    built[age][isEnemy][buildType] = nullptr;
}

void Building::deallocateBuiltArrowTowerUpgraded(int age, int isEnemy)
{
    if (builtArrowTowerUpgraded[age][isEnemy] != nullptr)
    {
        delete builtArrowTowerUpgraded[age][isEnemy];
        builtArrowTowerUpgraded[age][isEnemy] = nullptr;
    }
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
bool Building::tryDeductRepairHpCost(double hpFractionRepaired, Player* owner)
{
    if (owner == NULL || hpFractionRepaired <= 0.0 || !isConstructed() || playerScience == NULL)
        return true;

    int wood = 0, food = 0, stone = 0, gold = 0;
    playerScience->get_Resource_Consume(Num, wood, food, stone, gold);
    const double mult = REPAIR_COST_RATIO * hpFractionRepaired;

    repairResDebtWood += wood * mult;
    repairResDebtFood += food * mult;
    repairResDebtStone += stone * mult;
    repairResDebtGold += gold * mult;

    const int costWood = static_cast<int>(repairResDebtWood);
    const int costFood = static_cast<int>(repairResDebtFood);
    const int costStone = static_cast<int>(repairResDebtStone);
    const int costGold = static_cast<int>(repairResDebtGold);

    repairResDebtWood -= costWood;
    repairResDebtFood -= costFood;
    repairResDebtStone -= costStone;
    repairResDebtGold -= costGold;

    if (costWood == 0 && costFood == 0 && costStone == 0 && costGold == 0)
        return true;

    if (owner->getWood() < costWood || owner->getFood() < costFood
        || owner->getStone() < costStone || owner->getGold() < costGold)
    {
        repairResDebtWood += costWood;
        repairResDebtFood += costFood;
        repairResDebtStone += costStone;
        repairResDebtGold += costGold;
        return false;
    }

    owner->changeResource(costWood, costFood, costStone, costGold, true);
    return true;
}

void Building::update_Build()
{
    double ratio = get_retio_Build();

    if(!constructed)
    {
        Percent+=ratio;
        if(Percent>100) Percent = 100;
    }

    if(!isDie())
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
    else return 100.0/playerScience->get_buildTime(Num)/TimePerFrame;
}

double Building::get_retio_Action()
{
    if(is_cheatAction) return 100.0;
    else return 100.0/playerScience->get_actTime(Num, actNum)/TimePerFrame;
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
