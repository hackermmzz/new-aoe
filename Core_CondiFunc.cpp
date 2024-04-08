#include "Core_CondiFunc.h"

//****************************************************************************************
//relation_Object结构体 内部函数

relation_Object::relation_Object( Coordinate* goal , int eventClass)
{
    isExist = true;
    goalObject = goal;
    relationAct = eventClass;
    sort = goalObject->getSort();
    update_GoalPoint();
    init_AlterOb();
}

relation_Object::relation_Object(double DR_goal , double UR_goal , int eventClass )
{
    isExist = true;
    goalObject = NULL;
    this->DR_goal = DR_goal;
    this->UR_goal = UR_goal;
    relationAct = eventClass;
    init_AlterOb();
}

void relation_Object::set_ResourceBuildingType()
{
    Resource* resource = NULL;
    goalObject->printer_ToResource((void**)&resource);

    if( resource ==NULL )  resourceBuildingType = BUILDING_CENTER;
    else resourceBuildingType = resource->get_ReturnBuildingType();
}

void relation_Object::set_AlterOb(Coordinate* AlterObject , double dis_record)
{
    alterOb = AlterObject;
    distance_Record = dis_record;
    update_Attrib_alter();
}

void relation_Object::update_GoalPoint()
{
    if(goalObject!= NULL)
    {
        DR_goal = goalObject->getDR();
        UR_goal = goalObject->getUR();
        set_distance_AllowWork();
    }
}

void relation_Object::update_Attrib_alter()
{
    if(alterOb!= NULL)
    {
        DR_alter = alterOb->getDR();
        UR_alter = alterOb->getUR();
        set_dis_AllowWork_alter();
    }
}

void relation_Object::init_AlterOb()
{
    alterOb = NULL;
    dis_AllowWork_alter = 1e6;
    distance_Record = 1e6;
}

void relation_Object::init_AttackAb( Coordinate* object1 )
{
    BloodHaver* attacker = NULL;
    object1->printer_ToBloodHaver((void**)&attacker);
    if(attacker!=NULL)
    {
         attacker->setAttackObject(goalObject); //攻击者记录攻击目标, 用于对于army会计算特攻,farmer计算距离
         disAttack = attacker->getDis_attack();
    }
}

//****************************************************************************************
//conditionF结构体 构造函数
conditionF::conditionF( bool (*func)(Coordinate* , relation_Object & , int& ,bool))
{
    condition = func;
    variableArgu = OPERATECON_DEFAULT;
    isNegation = false;
}

conditionF::conditionF( bool (*func)(Coordinate* , relation_Object & , int& , bool) , int variableArgu)
{
    condition = func;
    this->variableArgu = variableArgu;
    isNegation = false;
}

conditionF::conditionF( bool (*func)(Coordinate* , relation_Object & , int& , bool) , int variableArgu , bool isNegation)
{
    condition = func;
    this->variableArgu = variableArgu;
    this->isNegation = isNegation;
}

//****************************************************************************************
// detail_EventPhase结构体 内部函数
detail_EventPhase::detail_EventPhase()
{
    phaseList[0] = CoreDetail_NormalEnd;
    phaseList[phaseInterrup] = CoreDetail_AbsoluteEnd;
}

detail_EventPhase::detail_EventPhase( int phaseAmount , int* theList, conditionF* conditionList/* , list< conditionF >forcedInterrupCondition*/ )
{
    /** ********************************************************
     *构造函数
     *传入：    阶段总数 ， 阶段表 ， 各个阶段的切换条件表 ， 强制中止条件
     */
    this->phaseAmount = phaseAmount;
    for(int i = 0; i<phaseAmount; i++)
    {
        phaseList[i] = theList[i];
        chageCondition[i] = conditionList[i];
        changeLinkedList[i] = i+1;
    }
//    this->forcedInterrupCondition = forcedInterrupCondition;

    phaseList[phaseAmount] = CoreDetail_NormalEnd;//中止细节操作
    phaseList[phaseInterrup] = CoreDetail_AbsoluteEnd;  //强制中止
}

bool detail_EventPhase::setLoop( int loopBeginPhase , int loopEndPhase , list<conditionF>overCondition )
{
    if( loopBeginPhase<loopEndPhase && loopBeginPhase>-1&& loopEndPhase <phaseAmount )
    {
        changeLinkedList[loopEndPhase] = loopBeginPhase;
        loopOverCondition[loopEndPhase] = overCondition;
        return true;
    }
    else return false;
}

//****************************************************************************************
//通用的关系函数
//永真
bool condition_AllTrue( Coordinate* object1, relation_Object& relation , int& operate , bool isNegation){return isNegation^true;}

//times次假
bool condition_TimesFalse( Coordinate* object1, relation_Object& relation , int& operate , bool isNegation)
{
    if(relation.is_ExecutionOver()) return isNegation^true;

    relation.excutionOnce();
    return isNegation^false;
}

//单一Object死亡
bool condition_UniObjectDie( Coordinate* object1, relation_Object& relation , int& operate , bool isNegation)
{
    BloodHaver* object_judget = NULL;
    if(operate == OPERATECON_OBJECT1&& object1!=NULL) object1->printer_ToBloodHaver((void**)&object_judget);
    else if(operate == OPERATECON_OBJECT2 && relation.goalObject!=NULL) relation.goalObject->printer_ToBloodHaver((void**)&object_judget);

    if(object_judget == NULL) return isNegation ^ true;
    else return isNegation ^ object_judget->isDie();
}

//单一object被攻击中
bool condition_UniObjectUnderAttack( Coordinate* object1, relation_Object& relation, int& operate, bool isNegation )
{
    BloodHaver* object_judget = NULL;
    if(operate == OPERATECON_OBJECT1&& object1!=NULL) object1->printer_ToBloodHaver((void**)&object_judget);
    else if(operate == OPERATECON_OBJECT2 && relation.goalObject!=NULL) relation.goalObject->printer_ToBloodHaver((void**)&object_judget);

    if(object_judget == NULL) return isNegation ^ true;
    else return isNegation ^ object_judget->isGotAttack();
}

//单一object为NULL
bool condition_UniObjectNULL( Coordinate* object1, relation_Object& relation, int& operate, bool isNegation )
{
    Coordinate* judge = NULL;
    if(operate == OPERATECON_OBJECT1) judge = object1;
    else if(operate == OPERATECON_OBJECT2) judge = relation.goalObject;

    if(judge == NULL) return isNegation^true;
    else return isNegation^false;
}

//单一object的行动完成度百分比
bool condition_UniObjectPercent(  Coordinate* object1, relation_Object& relation, int& operate, bool isNegation )
{
    Coordinate* judge = NULL;
    int relationAct = relation.relationAct;
    if(operate == OPERATECON_OBJECT1) judge = object1;
    else if(operate == OPERATECON_OBJECT2) judge = relation.goalObject;

    if(relationAct == CoreEven_FixBuilding) return isNegation^(((Building*)judge)->isFullHp()&&((Building*)judge)->isFinish());
    else if(relationAct == CoreEven_BuildingAct) return isNegation^((Building*)judge)->is_ActionFinish();

    return isNegation^true;
}

//工作者背包空
bool condition_Object1_EmptyBackpack( Coordinate* object1 , relation_Object& relation, int& operate, bool isNegation)
{
    Farmer* worker = (Farmer*)object1;
    return isNegation^worker->get_isEmptyBackpack();
}

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
            case OPERATECON_NEAR_MISSILE:
                dis = ((Missile*)object1)->get_Distance_hitTarget();

            default:
                break;
        }
        dr2 = relation.DR_goal;
        ur2 = relation.UR_goal;
    }

    //对飞行物的距离判定进行特判
    if(operate == OPERATECON_NEAR_MISSILE)
    {
        if( !((Missile*)object1)->is_haveToArrive() && relation.goalObject!=NULL && countdistance(dr1 ,ur1 , dr2 , ur2 )<=dis )
            ((Missile*)object1)->hitTarget();
        return isNegation^((Missile*)object1)->isMissileFinishTask();
    }

    //一般性距离判断（使用曼哈顿距离）
    if(isNear_Manhattan(dr1 ,ur1 , dr2 , ur2 , dis))
    {
        if(relation.isUseAlterGoal) relation.isUseAlterGoal = false;
        return isNegation^true;
    }
    return isNegation ^ false;
}

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
