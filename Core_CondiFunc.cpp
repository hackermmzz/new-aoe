#include "Core_CondiFunc.h"

//通用的关系函数

//****************************************************************************************
//永否
bool condition_AllFalse( Coordinate* object1, relation_Object& relation , int& operate , bool isNegation){return false;}


//****************************************************************************************
//永真
bool condition_AllTrue( Coordinate* object1, relation_Object& relation , int& operate , bool isNegation){return true;}


//****************************************************************************************
//times次假
bool condition_TimesFalse( Coordinate* object1, relation_Object& relation , int& operate , bool isNegation)
{
    if(operate/OPERATECHANGE_TIME == 0) return isNegation^true;
    else {operate --; return isNegation^false;}
}


//****************************************************************************************
//单一Object死亡
bool condition_UniObjectDie( Coordinate* object1, relation_Object& relation , int& operate , bool isNegation)
{
    BloodHaver* object_judget = NULL;
    if(operate == OPERATECON_OBJECT1&& object1!=NULL) object1->printer_ToBloodHaver((void**)&object_judget);
    else if(operate == OPERATECON_OBJECT2 && relation.goalObject!=NULL) relation.goalObject->printer_ToBloodHaver((void**)&object_judget);

    if(object_judget == NULL) return isNegation ^ true;
    else return isNegation ^ object_judget->isDie();
}


//****************************************************************************************
//单一object被攻击中
bool condition_UniObjectUnderAttack( Coordinate* object1, relation_Object& relation, int& operate, bool isNegation )
{
    BloodHaver* object_judget = NULL;
    if(operate == OPERATECON_OBJECT1&& object1!=NULL) object1->printer_ToBloodHaver((void**)&object_judget);
    else if(operate == OPERATECON_OBJECT2 && relation.goalObject!=NULL) relation.goalObject->printer_ToBloodHaver((void**)&object_judget);

    if(object_judget == NULL) return isNegation ^ true;
    else return isNegation ^ (object_judget->getAvangeObject() != NULL);
}


//****************************************************************************************
//单一object为NULL
bool condition_UniObjectNULL( Coordinate* object1, relation_Object& relation, int& operate, bool isNegation )
{
    Coordinate* judge = NULL;
    if(operate == OPERATECON_OBJECT1) judge = object1;
    else if(operate == OPERATECON_OBJECT2) judge = relation.goalObject;

    if(judge == NULL) return isNegation^true;
    else return isNegation^false;
}


//****************************************************************************************
//工作者背包空
bool condition_Object1_EmptyBackpack( Coordinate* object1 , relation_Object& relation, int& operate, bool isNegation)
{
    Farmer* worker = (Farmer*)object1;
    return isNegation^worker->get_isEmptyBackpack();
}


//****************************************************************************************
//工作者背包容量满
bool condition_Object1_FullBackpack( Coordinate* object1 , relation_Object& relation, int& operate, bool isNegation )
{
    Farmer* worker = (Farmer*)object1;
    Resource* collectible = NULL;

    if(relation.goalObject != NULL) relation.goalObject->printer_ToResource((void**)&collectible);

    if( collectible == NULL || worker->get_isFullBackpack() )
        return isNegation^true;
    else return isNegation^false;
}


//****************************************************************************************
//位置距离接近
bool condition_ObjectNearby( Coordinate* object1, relation_Object& relation, int& operate , bool isNegation )
{
    double dr1 = object1->getDR() , ur1 = object1->getUR() ,dr2 , ur2;
    double dis = 1e6;

    if(operate/OPERATECHANGE == OPERATECON_MOVEALTER)
    {
        if(!relation.isUseAlterGoal) relation.isUseAlterGoal = true;
        switch( operate )
        {
            case OPERATECON_NEARALTER_ABSOLUTE:
                dis = DISTANCE_Manhattan_MoveEndNEAR;
                break;
            case OPERATECON_NEARALTER_WORK:
                dis = relation.dis_AllowWork_alter;
                break;
            default:
                break;
        }

        dr2 = relation.DR_alter;
        ur2 = relation.UR_alter;
    }
    else
    {
        switch (operate)
        {
            case OPERATECON_NEAR_ABSOLUTE:
                dis = DISTANCE_Manhattan_MoveEndNEAR;
                break;
            case OPERATECON_NEAR_ATTACK:
                dis = relation.disAttack;
                break;
            case OPERATECON_NEAR_WORK:
                dis = relation.distance_AllowWork;
                break;
            default:
                break;
        }
        dr2 = relation.DR_goal;
        ur2 = relation.UR_goal;
    }

    if(isNear_Manhattan(dr1 ,ur1 , dr2 , ur2 , dis))
    {
        if(relation.isUseAlterGoal) relation.isUseAlterGoal = false;
        return isNegation^true;
    }
    return isNegation ^ false;
}


//****************************************************************************************
//object目标能被采集
bool condition_Object2CanbeGather(Coordinate* object1, relation_Object& relation, int& operate , bool isNegation)
{

    if(relation.goalObject != NULL)
    {
        //object2强制转化为Resource指针，若转化后仍为NULL，说明对象不包含Resource类的属性
        Resource* object_gathered = NULL ;
        relation.goalObject->printer_ToResource((void**)&object_gathered);

        //判断是否可采集
        if(object_gathered != NULL) return isNegation^(object_gathered->get_Gatherable() && object_gathered->is_Surplus());
    }

    return isNegation^false;
}
