#include "Development.h"

Development::Development()
{

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


