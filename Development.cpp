#include "Development.h"

Development::Development()
{
    init_DevelopLab();
}

double Development::get_rate_Move(int sort , int type)
{
    double rate = 1;
    if(sort == SORT_FARMER) rate += rate_FarmerMove;

    return rate;
}

/***************************************************************/

double Development::get_rate_Blood(int sort , int type)
{
    double rate = 1;
    if(sort == SORT_FARMER) rate+= rate_FarmerBlood;


    return rate;
}

int Development::get_addition_Blood( int sort , int type )
{
    int addition = 0;

    return addition;
}

/***************************************************************/
//攻击倍率加成
double Development::get_rate_Attack( int sort , int type , int armyClass , int attackType )
{
    double rate = 1;

    return rate;
}

//攻击力加成
int Development::get_addition_Attack( int sort , int type , int armyClass , int attackType )
{
    int level = 0;
    int addition = 0;
    if(attackType == ATTACKTYPE_CLOSE)
    {
        level = getActLevel(BUILDING_STOCK,BUILDING_STOCK_UPGRADE_USETOOL);
        switch (level) {
        case 1:
            addition+=BUILDING_STOCK_UPGRADE_USETOOL_ADDITION_ATKCLOSE;
        default:
            break;
        }
    }
    if(sort == SORT_ARMY && type == AT_SLINGER)
    {
        level = getActLevel(BUILDING_MARKET,BUILDING_MARKET_STONE_UPGRADE);
        switch (level) {
        case 1:
            addition+=BUILDING_MARKET_STONE_UPGRADE_ADDITION_SILNGERATK;
        default:
            break;
        }

    }

    return addition;
}


/***************************************************************/
//攻击距离
int Development::get_addition_DisAttack( int sort, int type , int armyClass , int attackType )
{
    int addition = 0;
    int level = 0;

    if(sort == SORT_ARMY && type == AT_SLINGER) //slinger不吃远程攻击距离加成
    {
        level = getActLevel(BUILDING_MARKET,BUILDING_MARKET_STONE_UPGRADE);
        switch (level) {
        case 1:
            addition+=BUILDING_MARKET_STONE_UPGRADE_ADDITION_SILNGERDIS;
        default:
            break;
        }
    }else if(attackType == ATTACKTYPE_SHOOT)
    {
        level = getActLevel(BUILDING_MARKET,BUILDING_MARKET_WOOD_UPGRADE);

        switch (level) {
        case 1:
            addition+=BUILDING_MARKET_WOOD_UPGRADE_ADDITION_DISSHOOT;
        default:
            break;
        }
    }



    return addition;
}

/***************************************************************/
//防御减伤倍率
double Development::get_rate_Defence( int sort , int type , int armyClass , int attackType_got )
{
    double rate = 1;

    return rate;
}


//防御减伤加成
int Development::get_addition_Defence( int sort , int type , int armyClass , int attackType_got )
{
    double addition = 0;
    int level = 0;

    if( sort == SORT_ARMY )
    {
        if(attackType_got == ATTACKTYPE_ANIMAL || attackType_got == ATTACKTYPE_CLOSE)
        {
            //步兵护甲加成
            if(armyClass == ARMY_INFANTRY)
            {
                level = getActLevel(BUILDING_STOCK , BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY);
                switch (level) {
                case 1:
                    addition+=BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY_ADDITION_DEFENSE_INFANTRY;
                default:
                    break;
                }
            }
            //弓兵护甲加成
            if(armyClass == ARMY_ARCHER)
            {
                level = getActLevel(BUILDING_STOCK , BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER);
                switch (level) {
                case 1:
                    addition+=BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER_ADDITION_DEFENSE_ARCHER;
                default:
                    break;
                }
            }
            //骑兵护甲加成
            if(armyClass == ARMY_RIDER)
            {
                level = getActLevel(BUILDING_STOCK , BUILDING_STOCK_UPGRADE_DEFENSE_RIDER);
                switch (level) {
                case 1:
                    addition+=BUILDING_STOCK_UPGRADE_DEFENSE_RIDER_ADDITION_DEFENSE_RIDER;
                default:
                    break;
                }
            }
        }
        else if(attackType_got == ATTACKTYPE_SHOOT)
        {
            //铜器时代再编写

        }
    }
    return addition;
}

/***************************************************************/
//采集携带量加成
int Development::get_addition_ResourceSort( int resourceSort )
{
    int addition = 0;
    int level = 0;
    if(resourceSort == HUMAN_WOOD)  //对搬运wood加成
    {
        level = getActLevel(BUILDING_MARKET , BUILDING_MARKET_WOOD_UPGRADE);
        switch (level) {
        case 1:
            addition+=BUILDING_MARKET_WOOD_UPGRADE_ADDITION_CARRY;
        default:
            break;
        }
    }
    else if(resourceSort == HUMAN_STONE)    //对搬运stone加成
    {
        level = getActLevel(BUILDING_MARKET , BUILDING_MARKET_STONE_UPGRADE);
        switch (level) {
        case 1:
            addition+=BUILDING_MARKET_STONE_UPGRADE_ADDITION_CARRY;
        default:
            break;
        }
    }
    else if(resourceSort == HUMAN_GOLD)     //对搬运gold加成
    {
        level = getActLevel(BUILDING_MARKET , BUILDING_MARKET_GOLD_UPGRADE);
        switch (level) {
        case 1:
            addition+=BUILDING_MARKET_GOLD_UPGRADE_ADDITION_CARRY;
        default:
            break;
        }
    }

    return addition;
}

int Development::get_addition_MaxCnt( int sort , int type )
{
    int addition = 0 , level = 0;

    if(sort == SORT_Building_Resource && type == BUILDING_FARM)
    {
        level = getActLevel(BUILDING_MARKET , BUILDING_MARKET_FARM_UPGRADE);

        switch (level) {
        case 1:
            addition+=BUILDING_MARKET_FARM_UPGRADE_ADDITION_FOOD;
        default:
            break;
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

void Development::finishAction(int buildingType , int buildact)
{
    if(buildingType == BUILDING_CENTER && buildact == BUILDING_CENTER_UPGRADE) civiChange();

    developLab[buildingType].finishAction(buildact);
}

bool Development::get_isBuildActionAble( int buildingNum, int actNum, int civilization ,int wood, int food , int stone, int gold , int* oper )
{
    //如果需要创建人口，判断是否有容量添加人
    if(developLab[buildingNum].actCon[actNum].isNeedCreatObject() && !get_isHumanHaveSpace())
    {
        if(oper !=NULL) *oper = 1;
        return false;
    }
    return developLab[buildingNum].actCon[actNum].executable(civilization,wood,food,stone,gold);
}

void Development::set_civilization( int civ )
{
    civilization = civ;
    while(developLab[BUILDING_CENTER].actCon[BUILDING_CENTER_UPGRADE].nowExecuteNode!=NULL && developLab[BUILDING_CENTER].actCon[BUILDING_CENTER_UPGRADE].nowExecuteNode->civilization<civilization)
    {
        developLab[BUILDING_CENTER].actCon[BUILDING_CENTER_UPGRADE].shift();
    }
}
/***************************************************************/
//初始化develop科技树
void Development::init_DevelopLab()
{
    conditionDevelop* newNode;

    //市镇中心
    {
        developLab[BUILDING_CENTER].buildCon = new conditionDevelop( CIVILIZATION_IRONAGE , BUILDING_CENTER , TIME_BUILD_CENTER, BUILD_CENTER_WOOD );
        //new分配空间在结构体内析构
        //造村民
        newNode = new conditionDevelop(CIVILIZATION_STONEAGE , BUILDING_CENTER , TIME_BUILDING_CENTER_CREATEFARMER,\
                                       0 ,BUILDING_CENTER_CREATEFARMER_FOOD );
        newNode->setCreatObjectAfterAction(SORT_FARMER );
        developLab[BUILDING_CENTER].actCon[BUILDING_CENTER_CREATEFARMER].setHead(newNode);
        developLab[BUILDING_CENTER].actCon[BUILDING_CENTER_CREATEFARMER].endNodeAsOver();

        //升级时代
        newNode = new conditionDevelop(CIVILIZATION_STONEAGE , BUILDING_CENTER , TIME_BUILDING_CENTER_UPGRADE,\
                                       0, BUILDING_CENTER_UPGRADE_FOOD);
        developLab[BUILDING_CENTER].actCon[BUILDING_CENTER_UPGRADE].setHead(newNode);
        /** 缺少石器时代两个建筑的前置条件*/
    }


    //房屋
    developLab[BUILDING_HOME].buildCon =new conditionDevelop( CIVILIZATION_STONEAGE , BUILDING_HOME , TIME_BUILD_HOME , BUILD_HOUSE_WOOD );

    //仓库
    {
        developLab[BUILDING_STOCK].buildCon = new conditionDevelop( CIVILIZATION_STONEAGE , BUILDING_STOCK , TIME_BUILD_STOCK , BUILD_STOCK_WOOD );

        //升级工具利用
        newNode = new conditionDevelop( CIVILIZATION_TOOLAGE , BUILDING_STOCK , TIME_BUILDING_STOCK_UPGRADE_USETOOL , \
                                        0 ,BUILDING_STOCK_UPGRADE_USETOOL_FOOD);
        developLab[BUILDING_STOCK].actCon[BUILDING_STOCK_UPGRADE_USETOOL].setHead(newNode);
        //升级步兵护甲
        newNode = new conditionDevelop(CIVILIZATION_TOOLAGE  , BUILDING_STOCK , TIME_BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY,\
                                       0, BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY_FOOD);
        developLab[BUILDING_STOCK].actCon[BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY].setHead(newNode);
        //升级弓兵护甲
        newNode = new conditionDevelop(CIVILIZATION_TOOLAGE , BUILDING_STOCK , TIME_BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER , \
                                       0, BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER_FOOD);
        developLab[BUILDING_STOCK].actCon[BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER].setHead(newNode);
        //升级骑兵护甲
        newNode = new conditionDevelop(CIVILIZATION_TOOLAGE , BUILDING_STOCK , TIME_BUILDING_STOCK_UPGRADE_DEFENSE_RIDER , \
                                       0 , BUILDING_STOCK_UPGRADE_DEFENSE_RIDER_FOOD);
        developLab[BUILDING_STOCK].actCon[BUILDING_STOCK_UPGRADE_DEFENSE_RIDER].setHead(newNode);
    }


    //谷仓
    {
         developLab[BUILDING_GRANARY].buildCon = new conditionDevelop( CIVILIZATION_STONEAGE , BUILDING_GRANARY , TIME_BUILD_GRANARY , BUILD_GRANARY_WOOD );

         //研发、升级箭塔
         newNode = new conditionDevelop(CIVILIZATION_TOOLAGE , BUILDING_GRANARY , TIME_BUILDING_GRANARY_RESEARCH_ARROWTOWER , 0 , BUILDING_GRANARY_ARROWTOWER_FOOD );
         developLab[BUILDING_GRANARY].actCon[BUILDING_GRANARY_ARROWTOWER].setHead(newNode);
         //研发城墙
//         newNode = new conditionDevelop(CIVILIZATION_TOOLAGE , BUILDING_GRANARY , TIME_BUILDING_GRANARY_RESEARCH_WALL , 0 , BUILDING_GRANARY_RESEARCH_WALL_FOOD);
//         developLab[BUILDING_GRANARY].actCon[BUILDING_GRANARY_WALL].setHead(newNode);
    }

    //兵营
    {
        developLab[BUILDING_ARMYCAMP].buildCon = new conditionDevelop( CIVILIZATION_STONEAGE , BUILDING_ARMYCAMP , TIME_BUILD_ARMYCAMP , BUILD_ARMYCAMP_WOOD );

        //造棍棒兵
        newNode = new conditionDevelop(CIVILIZATION_STONEAGE , BUILDING_ARMYCAMP , TIME_BUILDING_ARMYCAMP_CREATE_CLUBMAN , 0 , BUILDING_ARMYCAMP_CREATE_CLUBMAN_FOOD);
        newNode->setCreatObjectAfterAction(SORT_ARMY , AT_CLUBMAN);
        developLab[BUILDING_ARMYCAMP].actCon[BUILDING_ARMYCAMP_CREATE_CLUBMAN].setHead(newNode);
        developLab[BUILDING_ARMYCAMP].actCon[BUILDING_ARMYCAMP_CREATE_CLUBMAN].endNodeAsOver();
        //升级棍棒兵为斧头兵
        newNode = new conditionDevelop(CIVILIZATION_TOOLAGE , BUILDING_ARMYCAMP , TIME_BUILDING_ARMYCAMP_UPGRADE_CLUBMAN , 0 , BUILDING_ARMYCAMP_UPGRADE_CLUBMAN_FOOD);
        developLab[BUILDING_ARMYCAMP].actCon[BUILDING_ARMYCAMP_UPGRADE_CLUBMAN].setHead(newNode);
        //造投石兵
        newNode = new conditionDevelop(CIVILIZATION_TOOLAGE , BUILDING_ARMYCAMP , TIME_BUILDING_ARMYCAMP_CREATE_SLINGER ,\
                                       0 , BUILDING_ARMYCAMP_CREATE_SLINGER_FOOD , BUILDING_ARMYCAMP_CREATE_SLINGER_STONE);
        newNode->setCreatObjectAfterAction(SORT_ARMY,AT_SLINGER);
        developLab[BUILDING_ARMYCAMP].actCon[BUILDING_ARMYCAMP_CREATE_SLINGER].setHead(newNode);
        developLab[BUILDING_ARMYCAMP].actCon[BUILDING_ARMYCAMP_CREATE_SLINGER].endNodeAsOver();
    }

    //市场
    {
        developLab[BUILDING_MARKET].buildCon = new conditionDevelop( CIVILIZATION_TOOLAGE , BUILDING_MARKET , TIME_BUILD_MARKET , BUILD_MARKET_WOOD );
        developLab[BUILDING_MARKET].buildCon->addPreCondition(developLab[BUILDING_GRANARY].buildCon);

        //升级伐木
        {
            newNode = new conditionDevelop( CIVILIZATION_TOOLAGE , BUILDING_MARKET , TIME_BUILDING_MARKET_UPGRADE_CUTTING ,\
                                            BUILDING_MARKET_WOOD_UPGRADE_WOOD , BUILDING_MARKET_WOOD_UPGRADE_FOOD);
            developLab[BUILDING_MARKET].actCon[BUILDING_MARKET_WOOD_UPGRADE].setHead(newNode);
        }


        //升级挖石头
        {
            newNode = new conditionDevelop( CIVILIZATION_TOOLAGE , BUILDING_MARKET , TIME_BUILDING_MARKET_UPGRADE_DIGGINGSOTNE ,\
                                            0, BUILDING_MARKET_STONE_UPGRADE_FOOD , BUILDING_MARKET_STONE_UPGRADE_STONE);
            developLab[BUILDING_MARKET].actCon[BUILDING_MARKET_STONE_UPGRADE].setHead(newNode);
        }

        //升级金矿采集
//        {
//            newNode = new conditionDevelop(CIVILIZATION_TOOLAGE , BUILDING_MARKET , TIME_BUILDING_MARKET_UPGRADE_GOLD, \
//        }
        //                                           BUILDING_MARKET_GOLD_UPGRADE_WOOD , BUILDING_MARKET_GOLD_UPGRADE_FOOD);
        //            developLab[BUILDING_MARKET].actCon[BUILDING_MARKET_GOLD_UPGRADE].setHead(newNode);


        //升级农田
        {
            newNode = new conditionDevelop( CIVILIZATION_TOOLAGE , BUILDING_MARKET , TIME_BUILDING_MARKET_UPGRADE_FARM ,\
                                            BUILDING_MARKET_FARM_UPGRADE_WOOD , BUILDING_MARKET_FARM_UPGRADE_FOOD);
            developLab[BUILDING_MARKET].actCon[BUILDING_MARKET_FARM_UPGRADE].setHead(newNode);
        }
    }

    //马厩
    {
        developLab[BUILDING_STABLE].buildCon = new conditionDevelop( CIVILIZATION_TOOLAGE , BUILDING_STABLE , TIME_BUILD_STABLE , BUILD_STABLE_WOOD );
        developLab[BUILDING_STABLE].buildCon->addPreCondition(developLab[BUILDING_ARMYCAMP].buildCon);

        //造侦察骑兵
        newNode = new conditionDevelop( CIVILIZATION_TOOLAGE , BUILDING_STABLE , TIME_BUILDING_STABLE_CREATE_SCOUT , 0 ,BUILDING_STABLE_CREATE_SCOUT_FOOD );
        newNode->setCreatObjectAfterAction(SORT_ARMY , AT_SCOUT);
        developLab[BUILDING_STABLE].actCon[BUILDING_STABLE_CREATE_SCOUT].setHead(newNode);
        developLab[BUILDING_STABLE].actCon[BUILDING_STABLE_CREATE_SCOUT].endNodeAsOver();
    }

    //靶场
    {
        developLab[BUILDING_RANGE].buildCon = new conditionDevelop(CIVILIZATION_TOOLAGE , BUILDING_RANGE , TIME_BUILD_RANGE , BUILD_RANGE_WOOD);
        developLab[BUILDING_RANGE].buildCon->addPreCondition(developLab[BUILDING_ARMYCAMP].buildCon);


        //造弓箭手
        newNode = new conditionDevelop(CIVILIZATION_TOOLAGE , BUILDING_RANGE , TIME_BUILDING_RANGE_CREATE_BOWMAN ,\
                                       BUILDING_RANGE_CREATE_BOWMAN_WOOD , BUILDING_RANGE_CREATE_BOWMAN_FOOD);
        newNode->setCreatObjectAfterAction(SORT_ARMY,AT_BOWMAN);
        developLab[BUILDING_RANGE].actCon[BUILDING_RANGE_CREATE_BOWMAN].setHead(newNode);
        developLab[BUILDING_RANGE].actCon[BUILDING_RANGE_CREATE_BOWMAN].endNodeAsOver();
    }

    //农田
    developLab[BUILDING_FARM].buildCon = new conditionDevelop(CIVILIZATION_TOOLAGE , BUILDING_FARM , TIME_BUILD_FARM , BUILD_FARM_WOOD);
    developLab[BUILDING_FARM].buildCon->addPreCondition(developLab[BUILDING_MARKET].buildCon);

    //箭塔
    developLab[BUILDING_ARROWTOWER].buildCon = new conditionDevelop( CIVILIZATION_TOOLAGE , BUILDING_ARROWTOWER , TIME_BUILD_ARROWTOWER ,0 ,0 , BUILD_ARROWTOWER_STONE );
    developLab[BUILDING_ARROWTOWER].buildCon->addPreCondition(developLab[BUILDING_GRANARY].actCon[BUILDING_GRANARY_ARROWTOWER].headAct);

    //城墙
//    developLab[BUILDING_WALL].buildCon = new conditionDevelop(CIVILIZATION_TOOLAGE , BUILDING_WALL, TIME_BUILD_WALL , 0 , 0 , BUILD_WALL_STONE);
//    developLab[BUILDING_WALL].buildCon->addPreCondition(developLab[BUILDING_GRANARY].actCon[BUILDING_GRANARY_WALL].headAct);
}

void Development::all_technology_tree()
{
    map< int , st_buildAction >::iterator iter = developLab.begin(),itere = developLab.end();
    map<int , st_upgradeLab>::iterator iter1,iter1e;

    while(iter != itere)
    {
        for(iter1 = iter->second.actCon.begin(),iter1e = iter->second.actCon.end(); iter1 != iter1e; iter1++)
        {
            while(iter1->second.nowExecuteNode != NULL)
            {
                if(iter1->second.nowExecuteNode == iter1->second.endNode && iter1->second.nowExecuteNode->nextDevAction == iter1->second.endNode )
                    break;

                iter1->second.shift();
            }
        }

        iter++;
    }
}
