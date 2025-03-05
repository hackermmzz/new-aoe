#include "Core_CondiFunc.h"

//****************************************************************************************
//寻路用结构体
Point pathNode::goalPoint = Point(0,0);
int pathNode::cost_stra = 10,pathNode::cost_bias = 14;
std::map<Point , int> pathNode::costLab = {\
    {Point(0,1),pathNode::cost_stra} , {Point(1,0),pathNode::cost_stra} , {Point(-1,0),pathNode::cost_stra} , {Point(0,-1),pathNode::cost_stra},\
    {Point(1,1),pathNode::cost_bias} , {Point(1,-1),pathNode::cost_bias} , {Point(-1,-1),pathNode::cost_bias} , {Point(-1,1),pathNode::cost_bias},\
};

pathNode::pathNode( Point newPoint )
{
    /**
     要求： 创建新结点前，必须保证已使用静态函数setGoal，设置了全局目标点
    */
    position.x = newPoint.x;
    position.y = newPoint.y;
    calCost_predict();
}

//添加后续点
pathNode* pathNode::pushNode( pathNode* nextNode )
{
    nextNode->preNode = this;
    nextNode->cost_total = cost_total;
    nextNode->cost_total += costLab[ nextNode->position - this->position ];
    nextNode->pathLength += this->pathLength;

    return nextNode;
}

//小根堆
lessHeap::~lessHeap()
{
    if(cacheNode!=NULL)
    {
        delete cacheNode;
        cacheNode = NULL;
    }

    clear();
}

void lessHeap::clear()
{
    if(head == NULL) return;
    stack<treeNode*> dire;
    treeNode* dele;
    dire.push( head );

    while(dire.size())
    {
        dele = dire.top();
        dire.pop();
        if(dele->lchild) dire.push(dele->lchild);
        if(dele->rchild) dire.push(dele->rchild);
        delete dele;
        nodeNum--;
    }
}

void lessHeap::pop()
{
    if(head == NULL) return;
    if(nodeNum == 1)
    {
        delete head;
        head = NULL;
        nodeNum = 0;
        return;
    }
    treeNode* iter = head , *endPosi = head;
    stack<bool> dire;   //0表左儿子，1表右儿子
    int deep = nodeNum-1;
//    int number = nodeNum;

    while(deep)
    {
        dire.push((bool)((deep-1)%2));
        deep=(deep-1)/2;
    }
    while(dire.size()>0)
    {
        if(dire.top()) endPosi = endPosi->rchild;
        else endPosi = endPosi->lchild;
        dire.pop();
    }
    //节点关系变更
    nodeChange(endPosi,iter);

    if(endPosi->father->lchild == endPosi) endPosi->father->lchild = NULL;
    else if(endPosi->father->rchild == endPosi) endPosi->father->rchild = NULL;
    //删除原头节点
    delete endPosi;
    nodeNum--;

    downChange();
    return;
}

void lessHeap::addNode( pathNode* newNode )
{
    cacheNode = new treeNode(newNode);
    push_back();
    floatUp(cacheNode);
    cacheNode = NULL;
}

void lessHeap::floatUp( treeNode* obNode )
{
    //上浮操作
    if(obNode)
    {
        while(obNode->father)
        {
            if(!(*obNode<*(obNode->father)))break;
            nodeChange(obNode->father,obNode);
            obNode = obNode->father;
        }
    }
}

void lessHeap::downChange()
{
    //下沉操作
    treeNode* obNode = head , *minNode;
    if(obNode == NULL) return;

    while(obNode->lchild || obNode->rchild)
    {
        minNode = obNode;
        if( obNode->lchild && *(obNode->lchild)<*minNode)minNode = obNode->lchild;
        if( obNode->rchild && *(obNode->rchild)<*minNode)minNode = obNode->rchild;

        if(obNode == minNode) break;

        nodeChange(obNode,minNode);
        obNode = minNode;
    }
    return;
}

void lessHeap::push_back()
{
    //将cacheNode加入树的尾部
    if(head == NULL)
    {
        head = cacheNode;
        nodeNum = 1;
    }
    else
    {
        stack<bool> dire , lan;   //0表左儿子，1表右儿子
        treeNode* addPosi = head;
        int deep = nodeNum;

        while(deep)
        {
            dire.push((bool)((deep-1)%2));
            deep = (deep-1)/2;
        }

        lan = dire;
        while(dire.size()>1)
        {
            if(dire.top()) addPosi = addPosi->rchild;
            else addPosi = addPosi->lchild;
            dire.pop();
        }
        cacheNode->father = addPosi;    //设置父结点
        //设置左右孩子结点
        if(dire.top()) addPosi->rchild = cacheNode;
        else addPosi->lchild = cacheNode;
        nodeNum++;
    }
}

void lessHeap::nodeChange( treeNode* __x , treeNode* __y )
{
    swap( __x->value,__y->value );
}


//****************************************************************************************
//relation_Object结构体 内部函数
relation_Object::relation_Object( int evenClass )
{
    isExist = true;
    goalObject = NULL;
    relationAct = evenClass;
    sort = -1;
    init_AlterOb();
}

relation_Object::relation_Object( Coordinate* goal , int eventClass)
{
    isExist = true;
    goalObject = goal;
    relationAct = eventClass;
    if(goalObject!=NULL)
        sort = goalObject->getSort();
    update_GoalPoint();
    init_AlterOb();
}

relation_Object::relation_Object(double DR_goal , double UR_goal , int eventClass )
{
    isExist = true;
    goalObject = NULL;

    set_goalPoint(DR_goal, UR_goal);
    relationAct = eventClass;
    init_AlterOb();
}

void relation_Object::set_ResourceBuildingType()
{
    Resource* resource = NULL;
    if(goalObject != NULL)
        goalObject->printer_ToResource((void**)&resource);

    if( resource == NULL )  resourceBuildingType = BUILDING_CENTER;
    else resourceBuildingType = resource->get_ReturnBuildingType();
}

void relation_Object::set_AlterOb(Coordinate* AlterObject , double dis_record)
{
    alterOb = AlterObject;
    distance_Record = dis_record;
    update_Attrib_alter();
}

void relation_Object::reset_Object1Predicted(Coordinate* object1)
{
    MoveObject* moveOb = NULL;
    object1->printer_ToMoveObject((void**)&moveOb);
    if(moveOb)
    {
        DR_Predicted = moveOb->get_PredictedDR();
        UR_Predicted = moveOb->get_PredictedUR();
    }
    else
    {
        DR_Predicted = object1->getDR();
        UR_Predicted = object1->getUR();
    }
}

void relation_Object::update_GoalPoint()
{
    if(goalObject!= NULL)
    {
        set_goalPoint(goalObject->getDR(), goalObject->getUR());
        crashLength_goal = goalObject->getCrashLength();
        set_distance_AllowWork();
    }
}

void relation_Object::update_Attrib_alter()
{
    if(alterOb!= NULL)
    {
        set_AlterPoint(alterOb->getDR(), alterOb->getUR());
        crashLength_alter = alterOb->getCrashLength();
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
    if(object1!=NULL)
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

detail_EventPhase::detail_EventPhase( int phaseAmount , int* theList, conditionF* conditionList , list< conditionF >forcedInterrupCondition )
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
    Building* buildOb = NULL;
    int relationAct = relation.relationAct;
    if(operate == OPERATECON_OBJECT1) judge = object1;
    else if(operate == OPERATECON_OBJECT2) judge = relation.goalObject;

    if(judge == NULL) return isNegation^true;

    judge->printer_ToBuilding((void**)&buildOb);
    if(buildOb != NULL)
    {
        if(relationAct == CoreEven_FixBuilding)
            return isNegation^(buildOb->isFullHp()&& buildOb->isFinish());
        else if(relationAct == CoreEven_BuildingAct)
        {
            int creatSort = -1, creatNum = -1;
            bool needCreatOb = buildOb->isActionNeedCreatObject(creatSort, creatNum);

            if(needCreatOb && (creatSort == SORT_ARMY || creatSort == SORT_FARMER))
                return isNegation^(buildOb->is_ActionFinish() && buildOb->isRepresentHumanHaveSpace());
            else
                return isNegation^(buildOb->is_ActionFinish());
        }
    }

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
//    double predr = relation.DR_Predicted,preur = relation.UR_Predicted;
//    double dis_r = object1->getCrashLength();
    double dis = 1e6;
    BloodHaver* attacker = NULL;
    int heightAdd = 0;

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
//      dis_r+=relation.crashLength_alter;
    }
    else
    {
        switch (operate)
        {
            case OPERATECON_NEAR_UNLOAD:
                dis=DISTANCE_Manhattan_Unload;
                break;
            case OPERATECON_NEAR_ABSOLUTE:
                dis = DISTANCE_Manhattan_MoveEndNEAR;
                break;
            case OPERATECON_NEAR_ATTACK:
                dis = relation.disAttack;
                object1->printer_ToBloodHaver((void**)&attacker);
                if(attacker && attacker->is_missileAttack())
                {
                    heightAdd = relation.height_Object - relation.height_GoalObject;
                    if(heightAdd > 0)
                        dis = min( dis + heightAdd*BLOCKSIDELENGTH , object1->getVision() * BLOCKSIDELENGTH );
                }
                break;
            case OPERATECON_NEAR_ATTACK_MOVE:
                dis = relation.disAttack;   //暂且修改为disAttack，理论应略小于disAttack
                object1->printer_ToBloodHaver((void**)&attacker);
                if(attacker && attacker->is_missileAttack())
                {
                    heightAdd = relation.height_Object - relation.height_GoalObject;
                    if(heightAdd > 0)
                        dis = min( dis + heightAdd*BLOCKSIDELENGTH , object1->getVision() * BLOCKSIDELENGTH );
                }
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
//        dis_r+=relation.crashLength_goal;
    }

    //对飞行物的距离判定进行特判
    if(operate == OPERATECON_NEAR_MISSILE)
    {
        if( !((Missile*)object1)->is_haveToArrive() && relation.goalObject!=NULL && countdistance(dr1 ,ur1 , dr2 , ur2 ) <= dis+relation.goalObject->getCrashLength()/2.0 )
            ((Missile*)object1)->hitTarget();
        return isNegation^((Missile*)object1)->isMissileFinishTask();
    }
    //对渔场资源进行特判（考虑到建在岸边的渔场农民采集会出现问题，直接在这里对这种情况特判即可）
    //
    //一般性距离判断（使用曼哈顿距离）
    if(isNear_Manhattan(dr1 ,ur1 , dr2 , ur2 , dis) /*|| isNear_Manhattan(predr,preur,dr2,ur2,dis_r)*/)
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

//取消判断无效的行动
bool condition_UselessAction(Coordinate* object1, relation_Object& relation, int& operate , bool isNegation)
{
    return (relation.useless_norm>operate)^isNegation;
}


bool condition_Object1_AttackingEnd(Coordinate* object1, relation_Object& relation, int& operate , bool isNegation)
{
    if(operate == OPERATECON_NEAR_ATTACK)
    {
        BloodHaver* bloodOb;
        MoveObject* moveOb = NULL;
        Building* buildOb = NULL;
        object1->printer_ToBloodHaver((void**)&bloodOb);
        object1->printer_ToMoveObject((void**)&moveOb);
        object1->printer_ToBuilding((void**)&buildOb);

        if(moveOb!=NULL)
            return isNegation^( (!bloodOb->isAttacking()|| moveOb->isAction_ResBegin()) && condition_ObjectNearby(object1,relation,operate,true));
        else if(buildOb!=NULL)
            return isNegation^( (!bloodOb->isAttacking()|| buildOb->isAttackBegin()) && condition_ObjectNearby(object1,relation,operate,true));
    }

    return true;
}

bool condition_Object2_Transported(Coordinate*object1,relation_Object&relation,int&operate,bool isNegation){
    Human*human1=(Human*)object1;
    return isNegation^(human1->getTransported());
}

bool condition_Object1_Unload(Coordinate *object1, relation_Object &relation, int &operate, bool isNegation)
{
    extern Map*GlobalMap;
    Farmer*ship=(Farmer*)object1;
    return ship->getHumanTransport().size()==0;
    //寻找最近的陆地区块
    static const int off[][2]={{-1,0},{-1,1},{-1,-1},{1,0},{1,-1},{1,1},{0,1},{0,-1}};
    for(auto*o:off){
        int L=o[0]+ship->getBlockDR(),U=o[1]+ship->getBlockUR();
        if(L>=0&&L<MAP_L&&U>=0&&U<MAP_U){
            Block&block=GlobalMap->cell[L][U];

            if(block.getMapType()!=MAPTYPE_OCEAN&&GlobalMap->map_Object[L][U].empty()){//如果不为海洋，那就是陆地，并且无障碍物
                double dr=ship->getDR()-block.getDR()-BLOCKSIDELENGTH/2,ur=ship->getUR()-block.getUR()-BLOCKSIDELENGTH/2;
                if(dr*dr+ur*ur<=SHIP_ACT_MAX_DISTANCE)return true;
            }
        }
    }
    return false;
}

