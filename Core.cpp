#include "Core.h"

//****************************************************************************************
//关系函数定义
//永否
bool condition_AllFalse( Coordinate* object1, relation_Object& relation , int operate , bool isNegation){return false;}
//永真
bool condition_AllTrue( Coordinate* object1, relation_Object& relation , int operate , bool isNegation){return true;}
//单一Object死亡
bool condition_UniObjectDie( Coordinate* , relation_Object& , int , bool);
//单一object被攻击中
bool condition_UniObjectUnderAttack( Coordinate* , relation_Object& , int ,bool);
//工作者背包容量满
bool condition_Object1_FullBackpack( Coordinate* , relation_Object& , int ,bool);
//位置距离接近
bool condition_ObjectNearby( Coordinate* , relation_Object& , int ,bool);
//object目标能被采集
bool condition_Object2CanbeGather(Coordinate* , relation_Object& , int ,bool);

Core::Core()
{
    int *phaseList;
    conditionF* conditionList;
    list<conditionF>forcedInterrupCondition;
    list<conditionF>overCondition;

    //添加静态表
    //行动：只移动
    phaseList = new int(CoreDetail_Move);
    conditionList = new ( conditionF )( conditionF(condition_ObjectNearby) );
    forcedInterrupCondition.push_back(conditionF(condition_UniObjectDie , OPERATECON_OBJECT1));
    forcedInterrupCondition.push_back(conditionF(condition_UniObjectUnderAttack , OPERATECON_OBJECT1));

    relation_Event_static[CoreEven_JustMoveTo] = detail_EventPhase( 1 ,  phaseList, conditionList , forcedInterrupCondition );
    relation_Event_static[CoreEven_JustMoveTo].setEnd_Absolute();
    delete phaseList;
    delete conditionList;
    forcedInterrupCondition.clear();

    //行动: 攻击
    phaseList = new int[2]{ CoreDetail_Move , CoreDetail_Attack };
    conditionList = new conditionF[2]{ conditionF(condition_ObjectNearby , OPERATECON_NEAR_ATTACK) , conditionF( conditionF(condition_ObjectNearby,OPERATECON_NEAR_ATTACK,true) )};
    forcedInterrupCondition.push_back(conditionF(condition_UniObjectDie,OPERATECON_OBJECT1));

    relation_Event_static[CoreEven_Attacking] = detail_EventPhase(2 , phaseList , conditionList , forcedInterrupCondition );
    overCondition.push_back(conditionF( condition_UniObjectDie,OPERATECON_OBJECT2 ));
    relation_Event_static[CoreEven_Attacking].setLoop(0,1,overCondition);
    delete phaseList;
    delete conditionList;
    forcedInterrupCondition.clear();
    overCondition.clear();

    //行动：采集_不需攻击


}

void Core::gameUpdate(Map* theMap, Player* player[], int** memorymap, MouseEvent *mouseEvent)
{
    this->theMap = theMap;
    this->player = player;
    this->memorymap = memorymap;
    this->mouseEvent = mouseEvent;

    std::list<Human *>::iterator humaniter=player[0]->human.begin();
    while(humaniter!=player[0]->human.end())
    {
        if((*humaniter)->getSort() == SORT_FARMER)
        {
            Farmer *farmer=(Farmer *)(*humaniter);
            if(farmer->needTranState())
            {
                farmer->setNowState(farmer->getPreState());
                farmer->setPreStateIsIdle();
                farmer->setNowRes();
            }
        }
        humaniter++;
    }

    std::list<Animal*>::iterator animaliter=theMap->animal.begin();
    while(animaliter!=theMap->animal.end())
    {
        (*animaliter)->nextframe();
        animaliter++;
    }
    std::list<StaticRes*>::iterator SRiter=theMap->staticres.begin();
    while(SRiter!=theMap->staticres.end())
    {
        (*SRiter)->nextframe();
        SRiter++;
    }
    std::list<Human *>::iterator hiter=player[0]->human.begin();
    while(hiter!=player[0]->human.end())
    {
        (*hiter)->nextframe();
        (*hiter)->updateLU();
        hiter++;
    }

    if(mouseEvent->mouseEventType!=NULL_MOUSEEVENT) manageMouseEvent();
    manageOrder();
    manageRelationList();
}

bool Core::isValidPoint(const int (&map)[MAP_L][MAP_U], const Point &p)
{
    int rows = MAP_L;
    int cols = MAP_U;
    return (p.x >= 0 && p.x < rows && p.y >= 0 && p.y < cols);
}

vector<Point> Core::getAdjacentPoints(const int (&map)[MAP_L][MAP_U], const Point &p)
{
    vector<Point> adjacentPoints;

    // 八个相邻正方向的偏移量
    int dx[] = { -1, 1, 0, 0, -1, -1, 1, 1 };
    int dy[] = { 0, 0, -1, 1, -1, 1, -1, 1 };

    for (int i = 0; i < 8; i++) {
        int newX = p.x + dx[i];
        int newY = p.y + dy[i];
        Point newPoint = { newX, newY };
        if (isValidPoint(map, newPoint) && map[newX][newY] != 1) {
            if(abs(dx[i])+abs(dy[i])==2)
            {
                if(map[newX][p.y]!=1&&map[p.x][newY]!=1)
                {
                    adjacentPoints.push_back(newPoint);
                }
            }
            else adjacentPoints.push_back(newPoint);
        }
    }
    return adjacentPoints;
}

stack<Point> Core::findPath(const int (&findPathMap)[MAP_L][MAP_U], Map *map, const Point &start, const Point &destination)
{
    int rows = 72;
    int cols = 72;

    // 记录已访问的点
    vector<vector<bool>> visited(rows, vector<bool>(cols, false));

    // 使用map记录每个点的前驱点，以便回溯路径
    vector<vector<Point>> prev(rows, vector<Point>(cols));

    // 使用queue保存路径
    queue<Point> q;

    // 广度优先搜索
    q.push(start);
    visited[start.x][start.y] = true;

    while (!q.empty()) {
        Point current = q.front();
        q.pop();

        // 找到目标点，回溯路径
        if (current.x == destination.x && current.y == destination.y) {
            stack<Point> pathStack;
            while (!(current.x == start.x && current.y == start.y)) {
                pathStack.push(current);
                current = prev[current.x][current.y];
            }
            return pathStack;
        }

        vector<Point> adjacentPoints = getAdjacentPoints(findPathMap, current);
        for (const Point& next : adjacentPoints) {
            if (!visited[next.x][next.y]) {
                visited[next.x][next.y] = true;
                q.push(next);
                prev[next.x][next.y] = current;
            }
        }
    }

    return findPathAlternative(map->intmap,start,destination);
}

stack<Point> Core::findPathAlternative(const int (&map)[MAP_L][MAP_U], const Point &start, const Point &destination)
{
    int rows = 72;
    int cols = 72;

    // 记录已访问的点
    vector<vector<bool>> visited(rows, vector<bool>(cols, false));

    // 使用map记录每个点的前驱点，以便回溯路径
    vector<vector<Point>> prev(rows, vector<Point>(cols));

    // 使用queue保存路径
    queue<Point> q;

    // 广度优先搜索
    q.push(start);
    visited[start.x][start.y] = true;

    while (!q.empty()) {
        Point current = q.front();
        q.pop();

        // 找到目标点，回溯路径
        if (current.x == destination.x && current.y == destination.y) {
            stack<Point> pathStack;
            while (!(current.x == start.x && current.y == start.y)) {
                pathStack.push(current);
                current = prev[current.x][current.y];
            }
            return pathStack;
        }

        vector<Point> adjacentPoints = getAdjacentPoints(map, current);
        for (const Point& next : adjacentPoints) {
            if (!visited[next.x][next.y]) {
                visited[next.x][next.y] = true;
                q.push(next);
                prev[next.x][next.y] = current;
            }
        }
    }

    return stack<Point>();
}

//处理鼠标事件
void Core::manageMouseEvent()
{
    if(mouseEvent->mouseEventType==LEFT_PRESS)
    {
        nowobject=g_Object[memorymap[mouseEvent->memoryMapX][mouseEvent->memoryMapY]];

        mouseEvent->mouseEventType=NULL_MOUSEEVENT;
    }
    if(mouseEvent->mouseEventType==RIGHT_PRESS&&nowobject!=NULL)
    {
        if(nowobject->getSort()==SORT_FARMER)
        {
            if( g_Object[memorymap[mouseEvent->memoryMapX][mouseEvent->memoryMapY]] == NULL )
            {
                suspendRelation(nowobject);
                addRelation(nowobject , mouseEvent->DR,mouseEvent->UR , CoreEven_JustMoveTo);
            }

            mouseEvent->mouseEventType=NULL_MOUSEEVENT;
        }
    }
}
//后续编写，用于处理AI指令
void Core::manageOrder()
{

}

//****************************************************************************************
//
//对人，命令不必手动中止，直接变更；对建筑，命令变更将被无视，必须手动中止当前命令后再下达新命令。
bool Core::addRelation( Coordinate * object1, Coordinate * object2, int eventType)
{
    if(! relate_AllObject[object1].isExist )
    {
        relate_AllObject[object1] = relation_Object(object2 , eventType);
        relate_AllObject[object1].DR_goal = object2->getDR();
        relate_AllObject[object1].UR_goal = object2->getUR();
        relate_AllObject[object1].sort = object2->getSort();
        return true;
    }

    return false;
}

bool Core::addRelation( Coordinate * object1, double DR , double UR, int eventType)
{
    if(! relate_AllObject[object1].isExist )
    {
        relate_AllObject[object1] = relation_Object(DR , UR , eventType);
        relate_AllObject[object1].sort = SORT_COORDINATE;
        return true;
    }

    return false;
}

void Core::suspendRelation(Coordinate * object)
{
    if(relate_AllObject[object].isExist) relate_AllObject.erase(object);
}

void Core::manageRelationList()
{
    Coordinate* object1;
    Coordinate* object2;
    list<conditionF> forcedInterrupCondition , overCondition;

    for( map<Coordinate* , relation_Object>::iterator iter = relate_AllObject.begin() ; iter!=relate_AllObject.end() ;)
    {
        object1 = iter->first;
        relation_Object& thisRelation = iter->second;
        if( thisRelation.isExist )
        {
            int& nowPhaseNum = thisRelation.nowPhaseNum;
            detail_EventPhase& thisDetailEven = relation_Event_static[nowPhaseNum];
            object2 = thisRelation.goalObject;
            conditionF* recordCondition;

            //该部分为强制中止行动部分代码
            forcedInterrupCondition = thisDetailEven.forcedInterrupCondition;
            for(list<conditionF>::iterator iter_list = forcedInterrupCondition.begin();iter_list!=forcedInterrupCondition.end();iter_list++)
            {
                if( iter_list->condition( object1,thisRelation,iter_list->variableArgu,iter_list->isNegation ) )
                {
                    thisRelation.nowPhaseNum = thisDetailEven.phaseInterrup;
                    break;
                }
            }

            //判断是否在loop中，是否需要中止
            if( thisDetailEven.isLoop(nowPhaseNum) )
            {
                overCondition = thisDetailEven.loopOverCondition[nowPhaseNum];
                for(list<conditionF>::iterator iter_list = overCondition.begin(); iter_list!= overCondition.end();iter_list++)
                {
                    if( iter_list->condition( object1 , thisRelation , iter_list->variableArgu,iter_list->isNegation ))
                    {
                        nowPhaseNum ++;
                        break;
                    }
                }
            }

            recordCondition = &thisDetailEven.chageCondition[nowPhaseNum];
            //判断是否需要切换当前阶段
            if( nowPhaseNum < thisDetailEven.phaseAmount && recordCondition->condition(object1,thisRelation , recordCondition->variableArgu,recordCondition->isNegation))
                nowPhaseNum = thisDetailEven.changeLinkedList[nowPhaseNum];


            //实际执行行动
            switch (thisDetailEven.phaseList[nowPhaseNum])
            {
            case CoreDetail_Move:
                if(thisRelation.isUseAlterGoal)object_Move(object1 , thisRelation.DR_alter , thisRelation.UR_alter);
                else object_Move(object1 , thisRelation.DR_goal , thisRelation.UR_goal);
                break;
            case CoreDetail_Attack:
                object_Attack(object1,object2);
                break;
            case CoreDetail_Gather:
                break;
            case CoreDetail_ResourceIn:
                break;
            case CoreDetail_NormalEnd:
                iter = object_FinishAction(iter);
                continue;
            case CoreDetail_AbsoluteEnd:
                iter = object_FinishAction_Absolute(iter);
                continue;
            default:
                break;
            }
            iter++;
        }
    }
}

//****************************************************************************************
//通用的控制对象行动函数
void Core::object_Move(Coordinate * object , double DR , double UR)
{
    MoveObject* moveObject = (MoveObject*)object;
    if((moveObject->getDR0()!=DR || moveObject->getUR0()!= UR))
    {
        Point start,destination;
        if( object->getSort() == SORT_FARMER )
        {
            if(!moveObject->isWalking()) moveObject->setPreWalk();
            moveObject->setdestination(mouseEvent->DR, mouseEvent->UR);
            start.x = tranBlockDR(moveObject->getDR());
            start.y = tranBlockUR(moveObject->getUR());
            destination.x = tranBlockDR(moveObject->getDR0());
            destination.y = tranBlockUR(moveObject->getUR0());
            theMap->loadfindPathMap();
            moveObject->setPath(findPath(theMap->findPathMap , theMap , start , destination));
        }
    }
}

void Core::object_Attack(Coordinate* object1 ,Coordinate* object2)
{
    int damage; //记录伤害
    bool normalAttack = true;   //用于判断是否是祭司
    BloodHaver* attacker = NULL;    //攻击者
    BloodHaver* attackee = NULL;    //受攻击者

    //攻击者指针赋值(object1强制转换)
    switch(object1->getSort())
    {
    case SORT_BUILDING:
        ((Building*)object1)->printer_ToBloodHaver(&attacker);
        break;
    case SORT_ANIMAL:
        ((Animal*)object1)->printer_ToBloodHaver(&attacker);
        break;
    case SORT_FARMER:
        ((Farmer*)object1)->printer_ToBloodHaver(&attacker);
        break;
    case SORT_ARMY:
        ((Army*)object1)->printer_ToBloodHaver(&attacker);
        if(((Army*)object1)->getArmyClass() == ARMY_FLAMEN) normalAttack = false;
        break;
    default:
        break;
    }
    //受攻击者指针赋值(object2强制转换)
    switch(object2->getSort())
    {
    case SORT_BUILDING:
        ((Building*)object2)->printer_ToBloodHaver(&attackee);
        break;
    case SORT_ANIMAL:
        ((Animal*)object2)->printer_ToBloodHaver(&attackee);
        break;
    case SORT_FARMER:
        ((Farmer*)object2)->printer_ToBloodHaver(&attackee);
        break;
    case SORT_ARMY:
        ((Army*)object2)->printer_ToBloodHaver(&attackee);
        break;
    default:
        break;
    }

    if(attackee != NULL && attacker!=NULL)  //若指针均非空
    {
        if(normalAttack)
        {
            //非祭司,是普通的伤害计算公式
            /** 后续版本若有投石车等喷溅伤害,判断还需细化*/
            qDebug()<<"缺少攻击的硬直,远程攻击需要投掷物命中再伤害计算";

            attacker->setAttackObject(object2); //攻击者记录攻击目标, 用于对于army会计算特攻等
            damage = attacker->getATK()-attackee->getDEF(attacker->get_AttackType());   //统一伤害计算公式
            if(damage<0) damage = 0;
            attackee->updateBlood(damage);  //damage反映到受攻击者血量减少
        }
    }
}

void Core::object_Gather(Coordinate* object1 , Coordinate* object2)
{

}

map<Coordinate* , relation_Object>::iterator Core::object_FinishAction_Absolute(map<Coordinate* , relation_Object>::iterator iter)
{
    MoveObject* thisObject = (MoveObject*)iter->first;
    thisObject->setPreStand();
    return relate_AllObject.erase(iter);
}

map<Coordinate* , relation_Object>::iterator Core::object_FinishAction(map<Coordinate* , relation_Object>::iterator iter)
{
    if(false)   //添代码，找寻相同类型的目标，自动更改目标对象；使得行动有自动性、连续性
    {

        return ++iter;
    }
    else return object_FinishAction_Absolute(iter);

}

//****************************************************************************************
//通用的关系函数
//
bool condition_UniObjectDie( Coordinate* object1, relation_Object& relation , int operate , bool isNegation)
{
    BloodHaver* object_judget = NULL;
    if(operate == OPERATECON_OBJECT1&& object1!=NULL)
    {
        switch(object1->getSort())
        {
        case SORT_BUILDING:
            ((Building*)object1)->printer_ToBloodHaver(&object_judget);
            break;
        case SORT_ANIMAL:
            ((Animal*)object1)->printer_ToBloodHaver(&object_judget);
            break;
        case SORT_FARMER:
            ((Farmer*)object1)->printer_ToBloodHaver(&object_judget);
            break;
        case SORT_ARMY:
            ((Army*)object1)->printer_ToBloodHaver(&object_judget);
            break;
        default:
            break;
        }
    }
    else if(operate == OPERATECON_OBJECT2 && relation.goalObject!=NULL)
    {
        Coordinate* object2 = relation.goalObject;
        switch(object2->getSort())
        {
        case SORT_BUILDING:
            ((Building*)object2)->printer_ToBloodHaver(&object_judget);
            break;
        case SORT_ANIMAL:
            ((Animal*)object2)->printer_ToBloodHaver(&object_judget);
            break;
        case SORT_FARMER:
            ((Farmer*)object2)->printer_ToBloodHaver(&object_judget);
            break;
        case SORT_ARMY:
            ((Army*)object2)->printer_ToBloodHaver(&object_judget);
            break;
        default:
            break;
        }
    }

    if(object_judget != NULL) return isNegation ^ object_judget->isDie();
    else return isNegation ^ true;
}

bool condition_UniObjectUnderAttack( Coordinate* object1, relation_Object& relation, int operate, bool isNegation )
{
    BloodHaver* object_judget = NULL;
    if(operate == OPERATECON_OBJECT1&& object1!=NULL)
    {
        switch(object1->getSort())
        {
        case SORT_BUILDING:
            ((Building*)object1)->printer_ToBloodHaver(&object_judget);
            break;
        case SORT_ANIMAL:
            ((Animal*)object1)->printer_ToBloodHaver(&object_judget);
            break;
        case SORT_FARMER:
            ((Farmer*)object1)->printer_ToBloodHaver(&object_judget);
            break;
        case SORT_ARMY:
            ((Army*)object1)->printer_ToBloodHaver(&object_judget);
            break;
        default:
            break;
        }
    }
    else if(operate == OPERATECON_OBJECT2 && relation.goalObject!=NULL)
    {
        Coordinate* object2 = relation.goalObject;
        switch(object2->getSort())
        {
        case SORT_BUILDING:
            ((Building*)object2)->printer_ToBloodHaver(&object_judget);
            break;
        case SORT_ANIMAL:
            ((Animal*)object2)->printer_ToBloodHaver(&object_judget);
            break;
        case SORT_FARMER:
            ((Farmer*)object2)->printer_ToBloodHaver(&object_judget);
            break;
        case SORT_ARMY:
            ((Army*)object2)->printer_ToBloodHaver(&object_judget);
            break;
        default:
            break;
        }
    }

    if(object_judget != NULL) return isNegation ^ (object_judget->getAvangeObject() != NULL);
    else return isNegation ^ true;
}

bool condition_Object1_FullBackpack( Coordinate* object1 , relation_Object& relation, int operate, bool isNegation = false)
{
    Farmer* worker = (Farmer*)object1;
    Resource* collectible = NULL;

    switch (relation.goalObject->getSort())
    {
    case SORT_ANIMAL:
        ((Animal*)relation.goalObject)->printer_ToResource(&collectible);
        break;
    case SORT_STATICRES:
        ((StaticRes*)relation.goalObject)->printer_ToResource(&collectible);
    default:
        break;
    }

    if( collectible != NULL && (worker->getResourceSort()!= collectible->get_ResourceSort() || worker->getResourceNowHave()<worker->getResourceHave_Max()) )
        return isNegation^false;
    else
    {
        relation.isUseAlterGoal = true;
        return isNegation^true;
    }

}

bool condition_ObjectNearby( Coordinate* object1, relation_Object& relation, int operate = OPERATECON_NEAR_ABSOLUTE , bool isNegation = false)
{
    if(relation.goalObject!= NULL)
    {
        relation.DR_goal = relation.goalObject->getDR();
        relation.UR_goal = relation.goalObject->getUR();
    }

    if(operate == OPERATECON_NEAR_ABSOLUTE)
    {
        if(relation.isUseAlterGoal)
        {
            if(fabs(object1->getDR()-relation.DR_alter)<DISTANCE_Manhattan_MoveEndNEAR&&fabs(object1->getUR()-relation.UR_alter)<DISTANCE_Manhattan_MoveEndNEAR)
            {
                relation.isUseAlterGoal = false;
                return isNegation^true;
            }
        }
        else return isNegation ^ (fabs(object1->getDR()-relation.DR_goal)<DISTANCE_Manhattan_MoveEndNEAR && fabs(object1->getUR()-relation.UR_goal)<DISTANCE_Manhattan_MoveEndNEAR);

    }
    else if( operate == OPERATECON_NEAR_ATTACK )
    {
        BloodHaver* attacker;
        switch (object1->getSort())
        {
        case SORT_FARMER:
           ((Farmer*)object1)->printer_ToBloodHaver(&attacker);
           attacker->setAttackObject(relation.goalObject); //farmer攻击对动物/军队会有不同的距离
           break;
        case SORT_ANIMAL:
           ((Animal*)object1)->printer_ToBloodHaver(&attacker);
           break;
        case SORT_ARMY:
           ((Army*)object1)->printer_ToBloodHaver(&attacker);
           break;
        default:
           break;
        }
        return isNegation ^ (countdistance(object1->getDR(),object1->getUR(),relation.DR_goal,relation.UR_goal)<= attacker->getDis_attack());
    }

    return isNegation ^ false;
}

bool condition_Object2CanbeGather(Coordinate* object1, relation_Object& relation, int operate , bool isNegation)
{
    Resource* object_gathered ;
    if(relation.goalObject->getSort() == SORT_ANIMAL) ((Animal*)relation.goalObject)->printer_ToResource(&object_gathered);
    else if(relation.goalObject->getSort() == SORT_STATICRES) ((StaticRes*)relation.goalObject)->printer_ToResource(&object_gathered);

    if(object_gathered->get_Gatherable()) return true;
    else    //除了animal是需要猎杀后再采集,其他的可以直接采集
    {
        if( ((Animal*)relation.goalObject)->isDie() )
        {
            object_gathered->set_Gatherable(true);
            return isNegation ^ true;
        }
        else return isNegation^false;
    }
}
