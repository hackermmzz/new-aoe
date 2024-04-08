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
    int addition = 0;
    if(attackType == ATTACKTYPE_CLOSE) addition += attack_close;

    return addition;
}


/***************************************************************/
//攻击距离
int Development::get_addition_DisAttack( int sort, int type , int armyClass , int attackType )
{
    int addition = 0;
    if(sort == SORT_ARMY)
    {
        if(attackType == ATTACKTYPE_SHOOT) addition+= dis_shoot;

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

    if( sort == SORT_ARMY )
    {
        if(attackType_got == ATTACKTYPE_ANIMAL||ATTACKTYPE_CLOSE)
        {

            if(armyClass == ARMY_INFANTRY) addition+= defence_infantry;
            else if(armyClass == ARMY_ARCHER) addition+=defence_archer;
            else if(armyClass == ARMY_RIDER) addition+= defence_rider;
        }
        else if(attackType_got == ATTACKTYPE_SHOOT)
        {
            if(armyClass == ARMY_INFANTRY) addition+= specialDefence_toShoot;

        }
    }
    return addition;
}

/***************************************************************/
int Development::get_level( int sort, int type )
{
    if( sort == SORT_ARMY )
    {
        switch(type)
        {
        case AT_CLUBMAN:    return level_Clubman;

        case AT_SWORDSMAN:  return level_ShortSwordsman;

        default:
            break;
        }
    }
    else if( sort == SORT_BUILDING )
    {
        if(type == BUILDING_ARROWTOWER) return level_ArrowTower;
    }

    return 0;
}


int Development::get_addition_ResourceSort( int resourceSort )
{
    int addition = 0;
    if(resourceSort == HUMAN_WOOD) addition = capacity_wood;
    else if(resourceSort == HUMAN_STONE) addition = capacity_stone;
    else if(resourceSort == HUMAN_GOLD) addition = capacity_gold;

    return addition;
}


void Development::init_DevelopLab()
{
    conditionDevelop* newNode;

    //市镇中心
    {
        developLab[BUILDING_CENTER].buildCon = new conditionDevelop( CIVILIZATION_IRONAGE , BUILDING_CENTER , TIME_BUILD_CENTER );
        //new分配空间在结构体内析构
        //造村民
        newNode = new conditionDevelop(CIVILIZATION_STONEAGE , BUILDING_CENTER , TIME_BUILDING_CENTER_CREATEFARMER,\
                                       0 ,BUILDING_CENTER_CREATEFARMER_FOOD );
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
    }


    //谷仓
    {
         developLab[BUILDING_GRANARY].buildCon = new conditionDevelop( CIVILIZATION_STONEAGE , BUILDING_GRANARY , TIME_BUILD_GRANARY , BUILD_GRANARY_WOOD );

         //研发、升级箭塔
         newNode = new conditionDevelop(CIVILIZATION_TOOLAGE , BUILDING_GRANARY , TIME_BUILDING_GRANARY_RESEARCH_ARROWTOWER , 0 , BUILDING_GRANARY_ARROWTOWER_FOOD );
         developLab[BUILDING_GRANARY].actCon[BUILDING_GRANARY_ARROWTOWER].setHead(newNode);
    }

    //兵营

    //市场
    {
        developLab[BUILDING_MARKET].buildCon = new conditionDevelop( CIVILIZATION_TOOLAGE , BUILDING_MARKET , TIME_BUILD_MARKET , BUILD_MARKET_WOOD );

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


        //升级农田
        {
            newNode = new conditionDevelop( CIVILIZATION_TOOLAGE , BUILDING_MARKET , TIME_BUILDING_MARKET_UPGRADE_FARM ,\
                                            BUILDING_MARKET_FARM_UPGRADE_WOOD , BUILDING_MARKET_FARM_UPGRADE_FOOD);
            developLab[BUILDING_MARKET].actCon[BUILDING_MARKET_FARM_UPGRADE].setHead(newNode);
        }
    }

    //马厩

    //靶场

    //箭塔
    developLab[BUILDING_ARROWTOWER].buildCon = new conditionDevelop( CIVILIZATION_TOOLAGE , BUILDING_ARROWTOWER , TIME_BUILD_ARROWTOWER ,0 ,0 , BUILD_ARROWTOWER_STONE );
    developLab[BUILDING_ARROWTOWER].buildCon->addPreCondition(developLab[BUILDING_GRANARY].actCon[BUILDING_GRANARY_ARROWTOWER].headAct);

    //城墙
}
