#ifndef CORE_CONDIFUNC_H
#define CORE_CONDIFUNC_H

#include <GlobalVariate.h>
#include <Map.h>
#include <Player.h>

struct relation_Object
{
    bool isExist , isUseAlterGoal=false;
    Coordinate* goalObject;     //目标对象
    Coordinate* alterOb;
    int sort;   //记录goalObject的Sort
    int resourceBuildingType;
    int relationAct;    //记录当前行动的种类
    int nowPhaseNum = 0;    //记录当前行动所属的detail阶段
    double DR_goal,UR_goal , DR_alter , UR_alter;   //移动等目标位置，alter为暂时更改的目标的位置
    double distance_AllowWork , dis_AllowWork_alter;  //若goalObject为可工作对象，human对其的可工作距离（游戏中具体距离）
    double distance_Record; //游戏中的距离的记录,为曼哈顿距离，用于更新relation
    double disAttack = 0;

    bool needResourceBuilding = false;
    bool respondConduct = true;

    relation_Object()
    {
        isExist = false;
        goalObject = NULL;
    }

    relation_Object( Coordinate* goal , int eventClass)
    {
        isExist = true;
        goalObject = goal;
        relationAct = eventClass;
        sort = goalObject->getSort();
        update_GoalPoint();
        init_AlterOb();
    }

    relation_Object(double DR_goal , double UR_goal , int eventClass )
    {
        isExist = true;
        goalObject = NULL;
        this->DR_goal = DR_goal;
        this->UR_goal = UR_goal;
        relationAct = eventClass;
        init_AlterOb();
    }

    void set_distance_AllowWork(){ distance_AllowWork = goalObject->getSideLength()/2.0 + 0.2*BLOCKSIDELENGTH; }

    void set_dis_AllowWork_alter(){ dis_AllowWork_alter = alterOb->getSideLength()/2.0 + 0.2*BLOCKSIDELENGTH; }

    void set_ResourceBuildingType()
    {
        Resource* resource = NULL;
        goalObject->printer_ToResource((void**)&resource);

        if( resource ==NULL )  resourceBuildingType = BUILDING_CENTER;
        else resourceBuildingType = resource->get_ReturnBuildingType();
    }

    void set_AlterOb(Coordinate* AlterObject , double dis_record)
    {
        alterOb = AlterObject;
        distance_Record = dis_record;
        update_Attrib_alter();
    }

    void update_GoalPoint()
    {
        if(goalObject!= NULL)
        {
            DR_goal = goalObject->getDR();
            UR_goal = goalObject->getUR();
            set_distance_AllowWork();
        }
    }

    void update_Attrib_alter()
    {
        if(alterOb!= NULL)
        {
            DR_alter = alterOb->getDR();
            UR_alter = alterOb->getUR();
            set_dis_AllowWork_alter();
        }
    }

    void init_AlterOb()
    {
        alterOb = NULL;
        dis_AllowWork_alter = 1e6;
        distance_Record = 1e6;
    }

    void init_AttackAb( Coordinate* object1 )
    {
        BloodHaver* attacker = NULL;
        object1->printer_ToBloodHaver((void**)&attacker);
        if(attacker!=NULL)
        {
             attacker->setAttackObject(goalObject); //攻击者记录攻击目标, 用于对于army会计算特攻,farmer计算距离
             disAttack = attacker->getDis_attack();
        }
    }
};

struct conditionF
{
    int variableArgu;
    bool isNegation;
    bool (*condition)( Coordinate* , relation_Object & , int& , bool);

    conditionF()
    {

    }

    conditionF( bool (*func)(Coordinate* , relation_Object & , int& ,bool))
    {
        condition = func;
        variableArgu = OPERATECON_DEFAULT;
        isNegation = false;
    }

    conditionF( bool (*func)(Coordinate* , relation_Object & , int& , bool) , int variableArgu)
    {
        condition = func;
        this->variableArgu = variableArgu;
        isNegation = false;
    }

    conditionF( bool (*func)(Coordinate* , relation_Object & , int& , bool) , int variableArgu , bool isNegation)
    {
        condition = func;
        this->variableArgu = variableArgu;
        this->isNegation = isNegation;
    }
};

struct detail_EventPhase
{
    /*关于条件
    * 对所有条件：true 代表执行切换阶段操作
    * 所有条件list中条件均为“或”连接
    */
    /*一定需要：phaseAmount<= CoreDetailLinkMaxNum-2*/

    int phaseAmount = 0;    //阶段总数
    int phaseInterrup = CoreDetailLinkMaxNum-1;   //预设阶段 ， list中该位置存储强制中止行动的命令
    int phaseList[CoreDetailLinkMaxNum];      //core中，由此值指示进入函数
    conditionF chageCondition[CoreDetailLinkMaxNum-2];   //切换phase的判断条件
    map<int,int> changeLinkedList;  //关系链，
    list<conditionF>forcedInterrupCondition;//细节控制的强制中止条件

    map< int , list<conditionF> > loopOverCondition;  //loop的中止条件
    /**
    * 第一个键值：    loopEndPhase 为loop中阶段编号最大的阶段，    如：loop:1->2->3->1，loopEndPhase为3
    * 第二个值：      loop的中止条件。在loopEndPhase进行判断，条件链表中有一个为真即中止
    *               loop中止后将进入loop后的一个阶段。   如：loop:1->2->3->1，  中止条件在3进行判断，判断中止loop后，进入阶段4
    */


    detail_EventPhase()
    {
        phaseList[0] = CoreDetail_NormalEnd;
        phaseList[phaseInterrup] = CoreDetail_AbsoluteEnd;
    }

    detail_EventPhase( int phaseAmount , int* theList, conditionF* conditionList , list< conditionF >forcedInterrupCondition )
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
        this->forcedInterrupCondition = forcedInterrupCondition;

        phaseList[phaseAmount] = CoreDetail_NormalEnd;//中止细节操作
        phaseList[phaseInterrup] = CoreDetail_AbsoluteEnd;  //强制中止
    }

    bool setLoop( int loopBeginPhase , int loopEndPhase , list<conditionF>overCondition )
    {
        if( loopBeginPhase<loopEndPhase && loopBeginPhase>-1&& loopEndPhase <phaseAmount )
        {
            changeLinkedList[loopEndPhase] = loopBeginPhase;
            loopOverCondition[loopEndPhase] = overCondition;
            return true;
        }
        else return false;
    }

    void setJump( int beginPhase, int endPhase ){ changeLinkedList[beginPhase] = endPhase; }

    bool isLoop( int phase){ return phase<phaseAmount && phase>changeLinkedList[phase]; }
    //判断phase是否是loopEndPhase，是否需要loop中止判断
    //由于map对未在表内的键值自己设初值，可能会造成bug

    void setEnd_Absolute(){ phaseList[phaseAmount] = CoreDetail_AbsoluteEnd; }
};

//****************************************************************************************
//关系函数定义
/** 参数表模板
 （ Coordinate* object1第一个对象，
    relation_Object& 对象的关系（内含目标对象） ，
    int& 操作控制符 ，
    bool 是否取反（true表取反，flase按原真值输出） ）
*/

//永否
bool condition_AllFalse( Coordinate*, relation_Object& , int& , bool);
//永真
bool condition_AllTrue( Coordinate*, relation_Object&, int& , bool);
//times次假
bool condition_TimesFalse( Coordinate*, relation_Object& , int&, bool);
//单一Object死亡
bool condition_UniObjectDie( Coordinate* , relation_Object& , int& , bool);
//单一object被攻击中
bool condition_UniObjectUnderAttack( Coordinate* , relation_Object& , int& ,bool);
//单一object为NULL
bool condition_UniObjectNULL( Coordinate* , relation_Object& , int& ,bool);
//工作者背包空
bool condition_Object1_EmptyBackpack( Coordinate* , relation_Object& , int& ,bool);
//工作者背包容量满
bool condition_Object1_FullBackpack( Coordinate* , relation_Object& , int& ,bool);
//位置距离接近
bool condition_ObjectNearby( Coordinate* , relation_Object& , int& ,bool);
//object目标能被采集
bool condition_Object2CanbeGather(Coordinate* , relation_Object& , int& ,bool);
//****************************************************************************************


#endif // CORE_CONDIFUNC_H
