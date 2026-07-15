#include "Development.h"

Development::Development()
{
    init_DevelopLab();
}
Development::Development(int represent)
{
    playerRepresent = represent;
    init_DevelopLab();
}

double Development::get_rate_Move(int sort, int type)
{
    double rate = 1;
    if (sort == SORT_FARMER) rate += rate_FarmerMove;

    return rate;
}

/***************************************************************/

double Development::get_rate_Blood(int sort, int type)
{
    double rate = 1;
    if (sort == SORT_FARMER) rate += rate_FarmerBlood;


    return rate;
}

int Development::get_addition_Blood(int sort, int type)
{
    int addition = 0;

    return addition;
}

/***************************************************************/
//攻击倍率加成
double Development::get_rate_Attack(int sort, int type, int armyClass, int attackType, int interSort, int interNum)
{
    double rate = 1;

    if (interSort != -1 && interNum != -1)
    {
        if (interSort == SORT_BUILDING && sort == SORT_ARMY && (type == AT_SWORDSMAN || type == AT_CAVALRY || type == AT_IMPROVED))
            rate = 2;
    }

    return rate;
}

//攻击力加成
int Development::get_addition_Attack(int sort, int type, int armyClass, int attackType)
{
    int level = 0;
    int addition = 0;
    if (attackType == ATTACKTYPE_CLOSE && sort != SORT_FARMER)
    {
        level = getActLevel(BUILDING_STOCK, BUILDING_STOCK_UPGRADE_USETOOL);
        switch (level) {
        case 2:
            addition +=BUILDING_STOCK_UPGRADE_CLOSER_ATTACK_2_ADDITION_ATTACK;
        case 1:
            addition += BUILDING_STOCK_UPGRADE_CLOSER_ATTACK_ADDITION_ATTACK;
        default:
            break;
        }
    }
    if (sort == SORT_ARMY && type == AT_SLINGER)
    {
        level = getActLevel(BUILDING_MARKET, BUILDING_MARKET_STONE_UPGRADE);
        switch (level) {
        case 1:
            addition += BUILDING_MARKET_STONE_UPGRADE_ADDITION_SILNGERATK;
        default:
            break;
        }

    }
    if (sort == SORT_BUILDING && type == BUILDING_ARROWTOWER
        && getActLevel(BUILDING_GRANARY, BUILDING_GRANARY_ARROWTOWE_UPGRADE) >= 1)
        addition += BUILDING_GRANARY_UPGRADE_ARROWTOWER_ADDITION_ATK;
    return addition;
}


/***************************************************************/
//攻击距离
int Development::get_addition_DisAttack(int sort, int type, int armyClass, int attackType)
{
    int addition = 0;
    int level = 0;

    if (sort == SORT_ARMY && type == AT_SLINGER) //slinger不吃远程攻击距离加成
    {
        level = getActLevel(BUILDING_MARKET, BUILDING_MARKET_STONE_UPGRADE);
        switch (level) {
        case 1:
            addition += BUILDING_MARKET_STONE_UPGRADE_ADDITION_SILNGERDIS;
        default:
            break;
        }
    }
    else if (attackType == ATTACKTYPE_SHOOT)
    {
        if (sort == SORT_BUILDING && type == BUILDING_ARROWTOWER
            && getActLevel(BUILDING_GRANARY, BUILDING_GRANARY_ARROWTOWE_UPGRADE) >= 1)
            addition += BUILDING_GRANARY_UPGRADE_ARROWTOWER_ADDITION_DIS;
        // 检查木材加工科技（工具时代）
        if (getActLevel(BUILDING_MARKET, BUILDING_MARKET_WOOD_UPGRADE) >= 1)
        {
            addition += BUILDING_MARKET_WOOD_UPGRADE_ADDITION_DISSHOOT;
        }
        // 检查工艺科技（铜器时代，在木材加工基础上再增加）
        if (getActLevel(BUILDING_MARKET, BUILDING_MARKET_WOOD_UPGRADE) >= 2)
        {
            addition += BUILDING_MARKET_CRAFT_UPGRADE_ADDITION_DISSHOOT;
        }
    }



    return addition;
}

/***************************************************************/
//防御减伤倍率
double Development::get_rate_Defence(int sort, int type, int armyClass, int attackType_got)
{
    double rate = 1;

    return rate;
}


//防御减伤加成
int Development::get_addition_Defence(int sort, int type, int armyClass, int attackType_got)
{
    double addition = 0;
    int level = 0;

    if (sort == SORT_ARMY)
    {
        if (attackType_got == ATTACKTYPE_ANIMAL || attackType_got == ATTACKTYPE_CLOSE)
        {
            //步兵护甲加成
            if (armyClass == ARMY_INFANTRY)
            {
                level = getActLevel(BUILDING_STOCK, BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY);
                switch (level) {
                case 2:
                    addition+=BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY_2_ADDITION_DEFENSE_INFANTRY;
                case 1:
                    addition += BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY_ADDITION_DEFENSE_INFANTRY;
                default:
                    break;
                }
            }
            //弓兵护甲加成
            if (armyClass == ARMY_ARCHER)
            {
                level = getActLevel(BUILDING_STOCK, BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER);
                switch (level) {
                case 2:
                    addition += BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER_2_ADDITION_DEFENSE_ARCHER;
                case 1:
                    addition += BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER_ADDITION_DEFENSE_ARCHER;
                default:
                    break;
                }
            }
            //骑兵护甲加成
            if (armyClass == ARMY_RIDER)
            {
                level = getActLevel(BUILDING_STOCK, BUILDING_STOCK_UPGRADE_DEFENSE_RIDER);
                switch (level) {
                case 2:
                     addition += BUILDING_STOCK_UPGRADE_DEFENSE_RIDER_2_ADDITION_DEFENSE_RIDER;
                case 1:
                    addition += BUILDING_STOCK_UPGRADE_DEFENSE_RIDER_ADDITION_DEFENSE_RIDER;
                default:
                    break;
                }
            }
        }
        else if (attackType_got == ATTACKTYPE_SHOOT)
        {
            //步兵对投射武器防御力加成（青铜盾科技）
            if (armyClass == ARMY_INFANTRY)
            {
                if (getActLevel(BUILDING_STOCK, BUILDING_STOCK_UPGRADE_MISSILE_DEFENSE_INFANTRY) >= 1)
                {
                    addition += BUILDING_STOCK_UPGRADE_MISSILE_DEFENSE_INFANTRY_ADDITION_DEFENSE_INFANTRY;
                }
            }
        }
    }
    return addition;
}

/***************************************************************/
//采集携带量加成
int Development::get_addition_ResourceSort(int resourceSort)
{
    int addition = 0;
    int level = 0;
    if (resourceSort == HUMAN_WOOD)  //对搬运wood加成
    {
        level = getActLevel(BUILDING_MARKET, BUILDING_MARKET_WOOD_UPGRADE);
        switch (level) {
        case 1:
            addition += BUILDING_MARKET_WOOD_UPGRADE_ADDITION_CARRY;
        default:
            break;
        }
    }
    else if (resourceSort == HUMAN_STONE)    //对搬运stone加成
    {
        level = getActLevel(BUILDING_MARKET, BUILDING_MARKET_STONE_UPGRADE);
        switch (level) {
        case 1:
            addition += BUILDING_MARKET_STONE_UPGRADE_ADDITION_CARRY;
        default:
            break;
        }
    }
    else if (resourceSort == HUMAN_GOLD)     //对搬运gold加成
    {
        level = getActLevel(BUILDING_MARKET, BUILDING_MARKET_GOLD_UPGRADE);
        switch (level) {
        case 1:
            addition += BUILDING_MARKET_GOLD_UPGRADE_ADDITION_CARRY;
        default:
            break;
        }
    }


    return addition;
}

double Development::get_rate_ResorceGather(int resourceSort)
{
    int rate = 1;

    int level = 0;
    if (resourceSort == HUMAN_WOOD)  //对搬运wood加成
    {
        // 检查木材加工科技（工具时代）
        if (getActLevel(BUILDING_MARKET, BUILDING_MARKET_WOOD_UPGRADE) >= 1)
        {
            rate += BUILDING_MARKET_WOOD_UPGRADE_ADDITION_GATHERRATE;
        }
        // 检查工艺科技（铜器时代，在木材加工基础上再增加）
        if (getActLevel(BUILDING_MARKET, BUILDING_MARKET_WOOD_UPGRADE) >= 2)
        {
            rate += BUILDING_MARKET_CRAFT_UPGRADE_ADDITION_GATHERRATE;
        }
    }
    else if (resourceSort == HUMAN_STONE)    //对搬运stone加成
    {
        level = getActLevel(BUILDING_MARKET, BUILDING_MARKET_STONE_UPGRADE);
        switch (level) {
        case 1:
            rate += BUILDING_MARKET_STONE_UPGRADE_ADDITION_GATHERRATE;
        default:
            break;
        }
    }
    else if (resourceSort == HUMAN_GOLD)     //对搬运gold加成
    {
        level = getActLevel(BUILDING_MARKET, BUILDING_MARKET_GOLD_UPGRADE);
        switch (level) {
        case 1:
            rate += BUILDING_MARKET_GOLD_UPGRADE_ADDITION_GATHERRATE;
        default:
            break;
        }
    }

    return rate;
}


int Development::get_addition_MaxCnt(int sort, int type)
{
    int addition = 0, level = 0;

    if (sort == SORT_Building_Resource)
    {
        if (type == BUILDING_FARM) {
            // 检查驯养动物科技（工具时代）
            if (getActLevel(BUILDING_MARKET, BUILDING_MARKET_FARM_UPGRADE) >= 1)
            {
                addition += BUILDING_MARKET_FARM_UPGRADE_ADDITION_FOOD;
            }
            // 检查犁科技（铜器时代，在驯养动物基础上再增加）
            if (getActLevel(BUILDING_MARKET, BUILDING_MARKET_FARM_UPGRADE) >= 2)
            {
                addition += BUILDING_MARKET_PLOW_UPGRADE_ADDITION_FOOD;
            }
        }
    }

    return addition;
}


//int Development::get_civiBuild_Times( int civilization )
//{
//    int times = 0;

//    for(map< int , st_buildAction >::iterator iter = developLab.begin(); iter!= developLab.end() ; iter++)
//        if(iter->second.buildCon->civilization == civilization)
//            times+=iter->second.buildCon->getActTimes();

//    return times;
//}

void Development::finishAction(int buildingType, int buildact)
{
    if (buildingType == BUILDING_CENTER && buildact == BUILDING_CENTER_UPGRADE) civiChange();

    // 升级车轮：村民移速提升30%
    if (buildingType == BUILDING_MARKET && buildact == BUILDING_MARKET_WHEEL_UPGRADE)
    {
        rate_FarmerMove = 0.3;  // 村民移速提升30%
    }

    developLab[buildingType].finishAction(buildact);
}

bool Development::isLogisticsResearched()
{
    return getActLevel(BUILDING_ARMYCAMP, BUILDING_ARMYCAMP_RESEARCH_LOGISTICS) > 0;
}

int Development::getPopulationHalfSlots(int sourceBuilding, int objectSort)
{
    if (objectSort == SORT_ARMY &&
        sourceBuilding == BUILDING_ARMYCAMP &&
        isLogisticsResearched())
    {
        return 1;
    }

    return 2;
}

int Development::getActionPopulationHalfSlots(int buildingNum, int actNum)
{
    conditionDevelop* node = developLab[buildingNum].actCon[actNum].nowExecuteNode;
    if (node == NULL) return 0;

    int createSort = -1;
    int createNum = -1;
    if (!node->isNeedCreatObject(createSort, createNum)) return 0;

    return getPopulationHalfSlots(buildingNum, createSort);
}

bool Development::get_isBuildActionAble(int buildingNum, int actNum, int civilization, int wood, int food, int stone, int gold, int* oper)
{
    //如果需要创建人口，按照该行动的实际人口权重判断容量。
    int requiredHalfSlots = getActionPopulationHalfSlots(buildingNum, actNum);
    if (requiredHalfSlots > 0 && !get_isHumanHaveSpace(requiredHalfSlots))
    {
        if (oper != NULL) *oper = 1;
        return false;
    }
    return developLab[buildingNum].actCon[actNum].executable(civilization, wood, food, stone, gold);
}

void Development::set_civilization(int civ)
{
    civilization = civ;
    // 市镇中心升时代链末尾经 endNodeAsOver() 后 endNode->nextDevAction == endNode（自环）。
    // 若目标时代高于链上最后一个节点的 civilization，原先只判断「当前 < civ」会 shift 到自身并死循环（例如 cap=3 时末节点为工具时代）。
    st_upgradeLab& centerUp = developLab[BUILDING_CENTER].actCon[BUILDING_CENTER_UPGRADE];
    while (centerUp.nowExecuteNode != NULL && centerUp.nowExecuteNode->civilization < civilization) {
        if (centerUp.nowExecuteNode == centerUp.endNode && centerUp.nowExecuteNode->nextDevAction == centerUp.endNode)
            break;
        centerUp.shift();
    }
}

/*****************游戏进程信息*******************/
//时代升级，进入下一时代
void Development::civiChange()
{
    civilization++;
    if (playerRepresent == NOWPLAYERREPRESENT)
        soundQueue.push("Age_Level_Up");
}

/***************************************************************/
void Development::all_technology_tree()
{
    map< int, st_buildAction >::iterator iter = developLab.begin(), itere = developLab.end();
    map<int, st_upgradeLab>::iterator iter1, iter1e;

    while (iter != itere)
    {
        for (iter1 = iter->second.actCon.begin(), iter1e = iter->second.actCon.end(); iter1 != iter1e; iter1++)
        {
            while (iter1->second.nowExecuteNode != NULL)
            {
                if (iter1->second.nowExecuteNode == iter1->second.endNode && iter1->second.nowExecuteNode->nextDevAction == iter1->second.endNode)
                    break;

                iter1->second.shift();
            }
        }

        iter++;
    }
}

void Development::technology_tree_up_to(int max_civilization)
{
    set_civilization(max_civilization);

    map< int, st_buildAction >::iterator iter = developLab.begin(), itere = developLab.end();
    map<int, st_upgradeLab>::iterator iter1, iter1e;

    while (iter != itere)
    {
        for (iter1 = iter->second.actCon.begin(), iter1e = iter->second.actCon.end(); iter1 != iter1e; iter1++)
        {
            // 市镇中心「升级时代」链已由 set_civilization 对齐到上限，此处若再 shift 会越过设定时代（例如停在工具时代却仍执行升铜器）。
            if (iter->first == BUILDING_CENTER && iter1->first == BUILDING_CENTER_UPGRADE)
                continue;

            while (iter1->second.nowExecuteNode != NULL)
            {
                if (iter1->second.nowExecuteNode == iter1->second.endNode && iter1->second.nowExecuteNode->nextDevAction == iter1->second.endNode)
                    break;

                if (iter1->second.nowExecuteNode->civilization > max_civilization)
                    break;

                iter1->second.shift();
            }
        }

        iter++;
    }
}

//初始化develop科技树
void Development::init_DevelopLab()
{
    conditionDevelop* newNode;

    //市镇中心
    {
        developLab[BUILDING_CENTER].buildCon = new conditionDevelop(CIVILIZATION_IRONAGE, BUILDING_CENTER, TIME_BUILD_CENTER, BUILD_CENTER_WOOD);
        //new分配空间在结构体内析构
        //造村民
        newNode = new conditionDevelop(CIVILIZATION_STONEAGE, BUILDING_CENTER, TIME_BUILDING_CENTER_CREATEFARMER, \
            0, BUILDING_CENTER_CREATEFARMER_FOOD);
        newNode->setCreatObjectAfterAction(SORT_FARMER, FARMERTYPE_FARMER);
        developLab[BUILDING_CENTER].actCon[BUILDING_CENTER_CREATEFARMER].setHead(newNode);
        developLab[BUILDING_CENTER].actCon[BUILDING_CENTER_CREATEFARMER].endNodeAsOver();

        //升级至工具时代
        newNode = new conditionDevelop(CIVILIZATION_STONEAGE, BUILDING_CENTER, TIME_BUILDING_CENTER_UPGRADE,
            0, BUILDING_CENTER_UPGRADE_TOOLAGE_FOOD);
        developLab[BUILDING_CENTER].actCon[BUILDING_CENTER_UPGRADE].setHead(newNode);
        //升级至铜器时代
        newNode = new conditionDevelop(CIVILIZATION_TOOLAGE,BUILDING_CENTER,TIME_BUILDING_CENTER_UPGRADE,
                                       0,BUILDING_CENTER_UPGRADE_BRONZEAGE_FOOD,0,BUILDING_CENTER_UPGRADE_BRONZEAGE_GOLD);
        developLab[BUILDING_CENTER].actCon[BUILDING_CENTER_UPGRADE].push_back(newNode);
        /** 缺少石器时代两个建筑的前置条件*/
        developLab[BUILDING_CENTER].actCon[BUILDING_CENTER_UPGRADE].endNodeAsOver();
    }


    //房屋
    developLab[BUILDING_HOME].buildCon = new conditionDevelop(CIVILIZATION_STONEAGE, BUILDING_HOME, TIME_BUILD_HOME, BUILD_HOUSE_WOOD);

    //仓库
    {
        developLab[BUILDING_STOCK].buildCon = new conditionDevelop(CIVILIZATION_STONEAGE, BUILDING_STOCK, TIME_BUILD_STOCK, BUILD_STOCK_WOOD);

        //升级/近战部队攻击力
        newNode = new conditionDevelop(CIVILIZATION_TOOLAGE, BUILDING_STOCK, TIME_BUILDING_STOCK_UPGRADE_CLOSER_ATTACK, \
            0, BUILDING_STOCK_UPGRADE_CLOSER_ATTACK_FOOD);
        developLab[BUILDING_STOCK].actCon[BUILDING_STOCK_UPGRADE_USETOOL].setHead(newNode);
        //升级步兵护甲
        newNode = new conditionDevelop(CIVILIZATION_TOOLAGE, BUILDING_STOCK, TIME_BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY, \
            0, BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY_FOOD);
        developLab[BUILDING_STOCK].actCon[BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY].setHead(newNode);
        //升级弓兵护甲
        newNode = new conditionDevelop(CIVILIZATION_TOOLAGE, BUILDING_STOCK, TIME_BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER, \
            0, BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER_FOOD);
        developLab[BUILDING_STOCK].actCon[BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER].setHead(newNode);
        //升级骑兵护甲
        newNode = new conditionDevelop(CIVILIZATION_TOOLAGE, BUILDING_STOCK, TIME_BUILDING_STOCK_UPGRADE_DEFENSE_RIDER, \
            0, BUILDING_STOCK_UPGRADE_DEFENSE_RIDER_FOOD);
        developLab[BUILDING_STOCK].actCon[BUILDING_STOCK_UPGRADE_DEFENSE_RIDER].setHead(newNode);
        //铜器时代
        //近战部队攻击力+2
        newNode = new conditionDevelop(CIVILIZATION_BRONZEAGE,BUILDING_STOCK,TIME_BUILDING_STOCK_UPGRADE_CLOSER_ATTACK_2,0,BUILDING_STOCK_UPGRADE_CLOSER_ATTACK_2_FOOD,0,BUILDING_STOCK_UPGRADE_CLOSER_ATTACK_2_GOLD);
        developLab[BUILDING_STOCK].actCon[BUILDING_STOCK_UPGRADE_USETOOL].push_back(newNode);
        //步兵护甲+2
        newNode = new conditionDevelop(CIVILIZATION_BRONZEAGE,BUILDING_STOCK,TIME_BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY_2,0,BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY_2_FOOD,0,BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY_2_GOLD);
        developLab[BUILDING_STOCK].actCon[BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY].push_back(newNode);
        //弓兵护甲+2
        newNode = new conditionDevelop(CIVILIZATION_BRONZEAGE,BUILDING_STOCK,TIME_BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER_2,0,BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER_2_FOOD,0,BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER_2_GOLD);
        developLab[BUILDING_STOCK].actCon[BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER].push_back(newNode);
        //骑兵护甲+2
        newNode = new conditionDevelop(CIVILIZATION_BRONZEAGE,BUILDING_STOCK,TIME_BUILDING_STOCK_UPGRADE_DEFENSE_RIDER_2,0,BUILDING_STOCK_UPGRADE_DEFENSE_RIDER_2_FOOD,0,BUILDING_STOCK_UPGRADE_DEFENSE_RIDER_2_GOLD);
        developLab[BUILDING_STOCK].actCon[BUILDING_STOCK_UPGRADE_DEFENSE_RIDER].push_back(newNode);
        //步兵对投射武器防御力+1
        //newNode = new conditionDevelop(CIVILIZATION_BRONZEAGE,BUILDING_STOCK,TIME_BUILDING_STOCK_UPGRADE_MISSILE_DEFENSE_INFANTRY,0,BUILDING_STOCK_UPGRADE_MISSILE_DEFENSE_INFANTRY_FOOD,0,BUILDING_STOCK_UPGRADE_MISSILE_DEFENSE_INFANTRY_GOLD);
        //developLab[BUILDING_STOCK].actCon[BUILDING_STOCK_UPGRADE_MISSILE_DEFENSE_INFANTRY].setHead(newNode);
        //步兵对投射武器防御力+1（研究青铜盾）
        newNode = new conditionDevelop(CIVILIZATION_BRONZEAGE,BUILDING_STOCK,TIME_BUILDING_STOCK_UPGRADE_MISSILE_DEFENSE_INFANTRY,0,BUILDING_STOCK_UPGRADE_MISSILE_DEFENSE_INFANTRY_FOOD,0,BUILDING_STOCK_UPGRADE_MISSILE_DEFENSE_INFANTRY_GOLD);
        developLab[BUILDING_STOCK].actCon[BUILDING_STOCK_UPGRADE_MISSILE_DEFENSE_INFANTRY].setHead(newNode);
       // developLab[BUILDING_STOCK].actCon[BUILDING_STOCK_UPGRADE_MISSILE_DEFENSE_INFANTRY].endNodeAsOver();
    }


    //谷仓
    {
        developLab[BUILDING_GRANARY].buildCon = new conditionDevelop(CIVILIZATION_STONEAGE, BUILDING_GRANARY, TIME_BUILD_GRANARY, BUILD_GRANARY_WOOD);

        //研发、升级箭塔
        newNode = new conditionDevelop(CIVILIZATION_TOOLAGE, BUILDING_GRANARY, TIME_BUILDING_GRANARY_RESEARCH_ARROWTOWER, 0, BUILDING_GRANARY_ARROWTOWER_FOOD);
        developLab[BUILDING_GRANARY].actCon[BUILDING_GRANARY_ARROWTOWER].setHead(newNode);
        //铜器时代：强化箭塔（前置：谷仓内“建造箭塔”研发已完成）
        newNode = new conditionDevelop(CIVILIZATION_BRONZEAGE, BUILDING_GRANARY, TIME_BUILDING_GRANARY_UPGRADE_ARROWTOWER,
                                       0, BUILDING_GRANARY_UPGRADE_ARROWTOWER_FOOD, BUILDING_GRANARY_UPGRADE_ARROWTOWER_STONE, 0);
        newNode->addPreCondition(developLab[BUILDING_GRANARY].actCon[BUILDING_GRANARY_ARROWTOWER].headAct);
        developLab[BUILDING_GRANARY].actCon[BUILDING_GRANARY_ARROWTOWE_UPGRADE].setHead(newNode);
        //研发城墙
//         newNode = new conditionDevelop(CIVILIZATION_TOOLAGE , BUILDING_GRANARY , TIME_BUILDING_GRANARY_RESEARCH_WALL , 0 , BUILDING_GRANARY_RESEARCH_WALL_FOOD);
//         developLab[BUILDING_GRANARY].actCon[BUILDING_GRANARY_WALL].setHead(newNode);
    }

    //兵营
    {
        developLab[BUILDING_ARMYCAMP].buildCon = new conditionDevelop(CIVILIZATION_STONEAGE, BUILDING_ARMYCAMP, TIME_BUILD_ARMYCAMP, BUILD_ARMYCAMP_WOOD);

        //造棍棒兵
        newNode = new conditionDevelop(CIVILIZATION_STONEAGE, BUILDING_ARMYCAMP, TIME_BUILDING_ARMYCAMP_CREATE_CLUBMAN, 0, BUILDING_ARMYCAMP_CREATE_CLUBMAN_FOOD);
        newNode->setCreatObjectAfterAction(SORT_ARMY, AT_CLUBMAN);
        developLab[BUILDING_ARMYCAMP].actCon[BUILDING_ARMYCAMP_CREATE_CLUBMAN].setHead(newNode);
        developLab[BUILDING_ARMYCAMP].actCon[BUILDING_ARMYCAMP_CREATE_CLUBMAN].endNodeAsOver();
        //升级棍棒兵为斧头兵
        newNode = new conditionDevelop(CIVILIZATION_TOOLAGE, BUILDING_ARMYCAMP, TIME_BUILDING_ARMYCAMP_UPGRADE_CLUBMAN, 0, BUILDING_ARMYCAMP_UPGRADE_CLUBMAN_FOOD);
        developLab[BUILDING_ARMYCAMP].actCon[BUILDING_ARMYCAMP_UPGRADE_CLUBMAN].setHead(newNode);
        //造投石兵
        newNode = new conditionDevelop(CIVILIZATION_TOOLAGE, BUILDING_ARMYCAMP, TIME_BUILDING_ARMYCAMP_CREATE_SLINGER, \
            0, BUILDING_ARMYCAMP_CREATE_SLINGER_FOOD, BUILDING_ARMYCAMP_CREATE_SLINGER_STONE);
        newNode->setCreatObjectAfterAction(SORT_ARMY, AT_SLINGER);
        developLab[BUILDING_ARMYCAMP].actCon[BUILDING_ARMYCAMP_CREATE_SLINGER].setHead(newNode);
        developLab[BUILDING_ARMYCAMP].actCon[BUILDING_ARMYCAMP_CREATE_SLINGER].endNodeAsOver();

        //升级阔剑兵科技
        newNode = new conditionDevelop(CIVILIZATION_BRONZEAGE, BUILDING_ARMYCAMP, TIME_BUILDING_ARMYCAMP_UPGRADE_BROADSWORD,
                                      0, BUILDING_ARMYCAMP_UPGRADE_BROADSWORD_FOOD, BUILDING_ARMYCAMP_UPGRADE_BROADSWORD_GOLD);
        developLab[BUILDING_ARMYCAMP].actCon[BUILDING_ARMYCAMP_UPGRADE_BROADSWORD].setHead(newNode);

        //训练阔剑兵（需要阔剑科技）
        newNode = new conditionDevelop(CIVILIZATION_BRONZEAGE, BUILDING_ARMYCAMP, TIME_BUILDING_ARMYCAMP_CREATE_BROADSWORD,
                                      0, BUILDING_ARMYCAMP_CREATE_BROADSWORD_FOOD, BUILDING_ARMYCAMP_CREATE_BROADSWORD_GOLD);
        // 添加阔剑科技作为前置条件
        newNode->addPreCondition(developLab[BUILDING_ARMYCAMP].actCon[BUILDING_ARMYCAMP_UPGRADE_BROADSWORD].headAct);
        newNode->setCreatObjectAfterAction(SORT_ARMY, AT_BROADSWORDSMAN);
        developLab[BUILDING_ARMYCAMP].actCon[BUILDING_ARMYCAMP_CREATE_BROADSWORD].setHead(newNode);
        developLab[BUILDING_ARMYCAMP].actCon[BUILDING_ARMYCAMP_CREATE_BROADSWORD].endNodeAsOver();

        //研究后勤：铜器时代，兵营单位人口占用减半（一次性科技）
        newNode = new conditionDevelop(CIVILIZATION_BRONZEAGE, BUILDING_ARMYCAMP,
                                       TIME_BUILDING_ARMYCAMP_RESEARCH_LOGISTICS,
                                       0, BUILDING_ARMYCAMP_RESEARCH_LOGISTICS_FOOD,
                                       0, BUILDING_ARMYCAMP_RESEARCH_LOGISTICS_GOLD);
        developLab[BUILDING_ARMYCAMP].actCon[BUILDING_ARMYCAMP_RESEARCH_LOGISTICS].setHead(newNode);

    }

    //市场
    {
        developLab[BUILDING_MARKET].buildCon = new conditionDevelop(CIVILIZATION_TOOLAGE, BUILDING_MARKET, TIME_BUILD_MARKET, BUILD_MARKET_WOOD);
        developLab[BUILDING_MARKET].buildCon->addPreCondition(developLab[BUILDING_GRANARY].buildCon);

        //升级伐木
        {
            newNode = new conditionDevelop(CIVILIZATION_TOOLAGE, BUILDING_MARKET, TIME_BUILDING_MARKET_UPGRADE_CUTTING, \
                BUILDING_MARKET_WOOD_UPGRADE_WOOD, BUILDING_MARKET_WOOD_UPGRADE_FOOD);
            developLab[BUILDING_MARKET].actCon[BUILDING_MARKET_WOOD_UPGRADE].setHead(newNode);
        }


        //升级挖石头
        {
            newNode = new conditionDevelop(CIVILIZATION_TOOLAGE, BUILDING_MARKET, TIME_BUILDING_MARKET_UPGRADE_DIGGINGSOTNE, \
                0, BUILDING_MARKET_STONE_UPGRADE_FOOD, BUILDING_MARKET_STONE_UPGRADE_STONE);
            developLab[BUILDING_MARKET].actCon[BUILDING_MARKET_STONE_UPGRADE].setHead(newNode);
        }

        //升级金矿采集
        {
            newNode = new conditionDevelop(CIVILIZATION_TOOLAGE, BUILDING_MARKET, TIME_BUILDING_MARKET_UPGRADE_GOLD, \
                BUILDING_MARKET_GOLD_UPGRADE_WOOD, BUILDING_MARKET_GOLD_UPGRADE_FOOD);
            developLab[BUILDING_MARKET].actCon[BUILDING_MARKET_GOLD_UPGRADE].setHead(newNode);
        }

        //升级农田
        {
            newNode = new conditionDevelop(CIVILIZATION_TOOLAGE, BUILDING_MARKET, TIME_BUILDING_MARKET_UPGRADE_FARM, \
                BUILDING_MARKET_FARM_UPGRADE_WOOD, BUILDING_MARKET_FARM_UPGRADE_FOOD);
            developLab[BUILDING_MARKET].actCon[BUILDING_MARKET_FARM_UPGRADE].setHead(newNode);
        }
         //升级车轮（解锁战车，村民移速+30%）
        {
             newNode = new conditionDevelop(CIVILIZATION_BRONZEAGE, BUILDING_MARKET, TIME_BUILDING_MARKET_WHEEL_UPGRADE,
                BUILDING_MARKET_WHEEL_UPGRADE_WOOD, BUILDING_MARKET_WHEEL_UPGRADE_FOOD);
             developLab[BUILDING_MARKET].actCon[BUILDING_MARKET_WHEEL_UPGRADE].setHead(newNode);
             //developLab[BUILDING_MARKET].actCon[BUILDING_MARKET_WHEEL_UPGRADE].endNodeAsOver();
         }
        //铜器时代（前置条件：工具时代的对应升级）
        //研发工艺（木材加工升级版）- 需要木材加工作为前置条件
        {

            //研发工艺 - 木材加工升级版（前置：研发木材加工）
            newNode = new conditionDevelop(CIVILIZATION_BRONZEAGE, BUILDING_MARKET, TIME_BUILDING_MARKET_CRAFT_UPGRADE,
                                          BUILDING_MARKET_CRAFT_UPGRADE_WOOD, BUILDING_MARKET_CRAFT_UPGRADE_FOOD);
            newNode->addPreCondition(developLab[BUILDING_MARKET].actCon[BUILDING_MARKET_WOOD_UPGRADE].headAct);
            developLab[BUILDING_MARKET].actCon[BUILDING_MARKET_WOOD_UPGRADE].push_back(newNode);
        }


        {
            //研发犁 - 驯养动物升级版（前置：研发驯养动物）
            newNode = new conditionDevelop(CIVILIZATION_BRONZEAGE, BUILDING_MARKET, TIME_BUILDING_MARKET_PLOW_UPGRADE,
                                          BUILDING_MARKET_PLOW_UPGRADE_WOOD, BUILDING_MARKET_PLOW_UPGRADE_FOOD);
            newNode->addPreCondition(developLab[BUILDING_MARKET].actCon[BUILDING_MARKET_FARM_UPGRADE].headAct);
            developLab[BUILDING_MARKET].actCon[BUILDING_MARKET_FARM_UPGRADE].push_back(newNode);
        }

    }

    //马厩
    {
        developLab[BUILDING_STABLE].buildCon = new conditionDevelop(CIVILIZATION_TOOLAGE, BUILDING_STABLE, TIME_BUILD_STABLE, BUILD_STABLE_WOOD);
        developLab[BUILDING_STABLE].buildCon->addPreCondition(developLab[BUILDING_ARMYCAMP].buildCon);

        //造侦察骑兵
        newNode = new conditionDevelop(CIVILIZATION_TOOLAGE, BUILDING_STABLE, TIME_BUILDING_STABLE_CREATE_SCOUT, 0, BUILDING_STABLE_CREATE_SCOUT_FOOD);
        newNode->setCreatObjectAfterAction(SORT_ARMY, AT_SCOUT);
        developLab[BUILDING_STABLE].actCon[BUILDING_STABLE_CREATE_SCOUT].setHead(newNode);
        developLab[BUILDING_STABLE].actCon[BUILDING_STABLE_CREATE_SCOUT].endNodeAsOver();

        //训练战车（需要车轮科技）
        newNode = new conditionDevelop(CIVILIZATION_BRONZEAGE, BUILDING_STABLE, TIME_BUILDING_STABLE_CREATE_CHARIOT,
                                      BUILDING_STABLE_CREATE_CHARIOT_WOOD, BUILDING_STABLE_CREATE_CHARIOT_FOOD);
        // 添加车轮升级作为前置条件
        newNode->addPreCondition(developLab[BUILDING_MARKET].actCon[BUILDING_MARKET_WHEEL_UPGRADE].headAct);
        newNode->setCreatObjectAfterAction(SORT_ARMY, AT_CHARIOT);
        developLab[BUILDING_STABLE].actCon[BUILDING_STABLE_CREATE_CHARIOT].setHead(newNode);
        developLab[BUILDING_STABLE].actCon[BUILDING_STABLE_CREATE_CHARIOT].endNodeAsOver();

        //训练骑兵
          newNode = new conditionDevelop(CIVILIZATION_BRONZEAGE, BUILDING_STABLE, TIME_BUILDING_STABLE_CREATE_CAVALRY,
                                        0, BUILDING_STABLE_CREATE_CAVALRY_FOOD, 0, BUILDING_STABLE_CREATE_CAVALRY_GOLD);
          newNode->setCreatObjectAfterAction(SORT_ARMY, AT_CAVALRY);
          developLab[BUILDING_STABLE].actCon[BUILDING_STABLE_CREATE_CAVALRY].setHead(newNode);
          developLab[BUILDING_STABLE].actCon[BUILDING_STABLE_CREATE_CAVALRY].endNodeAsOver();

    }


    //靶场
    {
        developLab[BUILDING_RANGE].buildCon = new conditionDevelop(CIVILIZATION_TOOLAGE, BUILDING_RANGE, TIME_BUILD_RANGE, BUILD_RANGE_WOOD);
        developLab[BUILDING_RANGE].buildCon->addPreCondition(developLab[BUILDING_ARMYCAMP].buildCon);


        //造弓箭手
        newNode = new conditionDevelop(CIVILIZATION_TOOLAGE, BUILDING_RANGE, TIME_BUILDING_RANGE_CREATE_BOWMAN, \
            BUILDING_RANGE_CREATE_BOWMAN_WOOD, BUILDING_RANGE_CREATE_BOWMAN_FOOD);
        newNode->setCreatObjectAfterAction(SORT_ARMY, AT_BOWMAN);
        developLab[BUILDING_RANGE].actCon[BUILDING_RANGE_CREATE_BOWMAN].setHead(newNode);
        developLab[BUILDING_RANGE].actCon[BUILDING_RANGE_CREATE_BOWMAN].endNodeAsOver();

        //训练战车弓箭手（需要车轮科技）
        newNode = new conditionDevelop(CIVILIZATION_BRONZEAGE, BUILDING_RANGE, TIME_BUILDING_RANGE_CREATE_CHARIOT_ARCHER,
                                      BUILDING_RANGE_CREATE_CHARIOT_ARCHER_WOOD, BUILDING_RANGE_CREATE_CHARIOT_ARCHER_FOOD);
        // 添加车轮升级作为前置条件
        newNode->addPreCondition(developLab[BUILDING_MARKET].actCon[BUILDING_MARKET_WHEEL_UPGRADE].headAct);
        newNode->setCreatObjectAfterAction(SORT_ARMY, AT_CHARIOT_ARCHER);
        developLab[BUILDING_RANGE].actCon[BUILDING_RANGE_CREATE_CHARIOT_ARCHER].setHead(newNode);
        developLab[BUILDING_RANGE].actCon[BUILDING_RANGE_CREATE_CHARIOT_ARCHER].endNodeAsOver();

        //升级复合弓科技（初始科技，没有强弓兵）
        newNode = new conditionDevelop(CIVILIZATION_BRONZEAGE, BUILDING_RANGE, TIME_BUILDING_RANGE_UPGRADE_COMPOSITE_BOW,
                                      BUILDING_RANGE_UPGRADE_COMPOSITE_BOW_WOOD, BUILDING_RANGE_UPGRADE_COMPOSITE_BOW_FOOD);
        developLab[BUILDING_RANGE].actCon[BUILDING_RANGE_UPGRADE_COMPOSITE_BOW].setHead(newNode);

        //训练复合弓兵（需要复合弓科技）
        newNode = new conditionDevelop(CIVILIZATION_BRONZEAGE, BUILDING_RANGE, TIME_BUILDING_RANGE_CREATE_COMPOSITE_BOWMAN,
                                      0, BUILDING_RANGE_CREATE_COMPOSITE_BOWMAN_FOOD, BUILDING_RANGE_CREATE_COMPOSITE_BOWMAN_GOLD);
        // 添加复合弓科技作为前置条件
        newNode->addPreCondition(developLab[BUILDING_RANGE].actCon[BUILDING_RANGE_UPGRADE_COMPOSITE_BOW].headAct);
        newNode->setCreatObjectAfterAction(SORT_ARMY, AT_COMPOSITE_BOWMAN);
        developLab[BUILDING_RANGE].actCon[BUILDING_RANGE_CREATE_COMPOSITE_BOWMAN].setHead(newNode);
        developLab[BUILDING_RANGE].actCon[BUILDING_RANGE_CREATE_COMPOSITE_BOWMAN].endNodeAsOver();

    }


    //农田
    developLab[BUILDING_FARM].buildCon = new conditionDevelop(CIVILIZATION_TOOLAGE, BUILDING_FARM, TIME_BUILD_FARM, BUILD_FARM_WOOD);
    developLab[BUILDING_FARM].buildCon->addPreCondition(developLab[BUILDING_MARKET].buildCon);

    //箭塔
    developLab[BUILDING_ARROWTOWER].buildCon = new conditionDevelop(CIVILIZATION_TOOLAGE, BUILDING_ARROWTOWER, TIME_BUILD_ARROWTOWER, 0, 0, BUILD_ARROWTOWER_STONE);
    developLab[BUILDING_ARROWTOWER].buildCon->addPreCondition(developLab[BUILDING_GRANARY].actCon[BUILDING_GRANARY_ARROWTOWER].headAct);

    //城墙
//    developLab[BUILDING_WALL].buildCon = new conditionDevelop(CIVILIZATION_TOOLAGE , BUILDING_WALL, TIME_BUILD_WALL , 0 , 0 , BUILD_WALL_STONE);
//    developLab[BUILDING_WALL].buildCon->addPreCondition(developLab[BUILDING_GRANARY].actCon[BUILDING_GRANARY_WALL].headAct);
    //船坞
    {
        developLab[BUILDING_DOCK].buildCon = new conditionDevelop(CIVILIZATION_STONEAGE, BUILDING_DOCK, TIME_BUILD_DOCK, BUILD_DOCK_WOOD);
        newNode = new conditionDevelop(CIVILIZATION_STONEAGE, BUILDING_DOCK, TIME_BUILDING_DOCK_CREATE_SAILING, BUILDING_DOCK_CREATE_SAILING_WOOD);
        newNode->setCreatObjectAfterAction(SORT_FARMER, FARMERTYPE_SAILING);
        developLab[BUILDING_DOCK].actCon[BUILDING_DOCK_CREATE_SAILING].setHead(newNode);
        developLab[BUILDING_DOCK].actCon[BUILDING_DOCK_CREATE_SAILING].endNodeAsOver();

        newNode = new conditionDevelop(CIVILIZATION_TOOLAGE, BUILDING_DOCK, TIME_BUILDING_DOCK_CREATE_WOOD_BOAT, BUILDING_DOCK_CREATE_WOOD_BOAT_WOOD);
        newNode->setCreatObjectAfterAction(SORT_FARMER, FARMERTYPE_WOOD_BOAT);
        developLab[BUILDING_DOCK].actCon[BUILDING_DOCK_CREATE_WOOD_BOAT].setHead(newNode);
        developLab[BUILDING_DOCK].actCon[BUILDING_DOCK_CREATE_WOOD_BOAT].endNodeAsOver();

        newNode = new conditionDevelop(CIVILIZATION_TOOLAGE, BUILDING_DOCK, TIME_BUILDING_DOCK_CREATE_SHIP, BUILDING_DOCK_CREATE_SHIP_WOOD);
        newNode->setCreatObjectAfterAction(SORT_ARMY, AT_SHIP);
        developLab[BUILDING_DOCK].actCon[BUILDING_DOCK_CREATE_SHIP].setHead(newNode);
        developLab[BUILDING_DOCK].actCon[BUILDING_DOCK_CREATE_SHIP].endNodeAsOver();
    }
    //攻城武器厂
    {
        developLab[BUILDING_SIEGE].buildCon=new conditionDevelop(CIVILIZATION_BRONZEAGE,BUILDING_SIEGE,TIME_BUILD_SIEGE,BUILD_SIEGE_WOOD,0,0,0);
        newNode=new conditionDevelop(CIVILIZATION_BRONZEAGE,BUILDING_SIEGE,TIME_BUILDING_SIEGE_CREATE_STONE_THROWER,BUILDING_SIEGE_CREATE_STONE_THROWER_WOOD,0,0,BUILDING_SIEGE_CREATE_STONE_THROWER_GOLD);
        newNode->setCreatObjectAfterAction(SORT_ARMY,AT_STONE_THROWER);
        developLab[BUILDING_SIEGE].actCon[BUILDING_SIEGE_CREATE_STONE_THROWER].setHead(newNode);
        developLab[BUILDING_SIEGE].actCon[BUILDING_SIEGE_CREATE_STONE_THROWER].endNodeAsOver();
    }
    //学院
    {
        developLab[BUILDING_COLLAGE].buildCon=new conditionDevelop(CIVILIZATION_BRONZEAGE,BUILDING_COLLAGE,TIME_BUILD_COLLAGE,BUILD_COLLAGE_WOOD,0,0,0);

        // 训练方阵兵
               newNode = new conditionDevelop(CIVILIZATION_BRONZEAGE, BUILDING_COLLAGE, TIME_BUILDING_COLLAGE_CREATE_HOPLITE,
                                               0, BUILDING_COLLAGE_CREATE_HOPLITE_FOOD, 0, BUILDING_COLLAGE_CREATE_HOPLITE_GOLD);
               newNode->setCreatObjectAfterAction(SORT_ARMY, AT_HOPLITE);
               developLab[BUILDING_COLLAGE].actCon[BUILDING_COLLAGE_CREATE_HOPLITE].setHead(newNode);
               developLab[BUILDING_COLLAGE].actCon[BUILDING_COLLAGE_CREATE_HOPLITE].endNodeAsOver();
    }
    //wlh友情提示：存在内存泄漏
}
