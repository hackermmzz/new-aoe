#include "Core_list.h"

Core_List::Core_List(Map* theMap, Player* player[])
{
    this->theMap = theMap;
    this->player = player;
    initDetailList();
}

//****************************************************************************************
//控制动态表
//对人，命令不必手动中止，直接变更；当建筑正进行升级、造兵等行动，命令变更将被无视，必须手动中止当前命令后再下达新命令。
bool Core_List::addRelation( Coordinate * object1, Coordinate * object2, int eventType , bool respond)
{
    if(object1 == NULL) return false;

    if(relate_AllObject[object1].isExist && relate_AllObject[object1].respondConduct) suspendRelation(object1);

    if(! relate_AllObject[object1].isExist )
    {
        object1->set_interAct(object2->getSort() , object2->getNum());
        relate_AllObject[object1] = relation_Object(object2 , eventType);
        relate_AllObject[object1].respondConduct = respond;

        switch (eventType)
        {
            case CoreEven_Gather:
                relate_AllObject[object1].update_GoalPoint();
                relate_AllObject[object1].set_ResourceBuildingType();
            case CoreEven_Attacking:
                relate_AllObject[object1].init_AttackAb(object1);
                break;
            default:
                break;
        }

        return true;
    }
    return false;
}

bool Core_List::addRelation( Coordinate * object1, double DR , double UR, int eventType , bool respond , int type)
{
    if(object1 == NULL) return false;

    if(relate_AllObject[object1].isExist && relate_AllObject[object1].respondConduct) suspendRelation(object1);

    if(! relate_AllObject[object1].isExist )
    {
        if(eventType == CoreEven_JustMoveTo)
        {
            if(DR<0) DR = 0;
            if(UR<0) UR = 0;
            if(DR>71.9*BLOCKSIDELENGTH) DR = 71.9*BLOCKSIDELENGTH;
            if(UR>71.9*BLOCKSIDELENGTH) UR = 71.9*BLOCKSIDELENGTH;
            relate_AllObject[object1] = relation_Object(DR , UR , eventType);
            relate_AllObject[object1].respondConduct = respond;
            relate_AllObject[object1].sort = SORT_COORDINATE;
            return true;
        }
        //判断行动为CoreEven_CreatBuilding，紧接着判断地图上建筑范围内是否有障碍物，最后判断player是否有足够资源并进行资源扣除
        else if(eventType == CoreEven_CreatBuilding && \
                is_BuildingCanBuild(type , tranBlockDR(DR) , tranBlockUR(UR)) && player[((Farmer*)object1)->getPlayerRepresent()]->get_isBuildingAble(type) )
        {
             player[((Farmer*)object1)->getPlayerRepresent()]->changeResource_byBuild(type);
             return addRelation(object1 , player[((Farmer*)object1)->getPlayerRepresent()]->addBuilding( type,tranBlockDR(DR) , tranBlockUR(UR)) , CoreEven_FixBuilding);
        }
    }

    return false;
}

bool Core_List::addRelation( Coordinate* object1, int BlockDR , int BlockUR, int eventType , bool respond , int type)
{
    if(object1 == NULL) return false;

    if(relate_AllObject[object1].isExist && relate_AllObject[object1].respondConduct) suspendRelation(object1);

    if(! relate_AllObject[object1].isExist )
    {
        //判断行动为CoreEven_CreatBuilding，紧接着判断地图上建筑范围内是否有障碍物，最后判断player是否有足够资源并进行资源扣除
        if(eventType == CoreEven_CreatBuilding && \
                is_BuildingCanBuild(type , BlockDR, BlockUR) && player[((Farmer*)object1)->getPlayerRepresent()]->get_isBuildingAble(type) )
        {
            player[((Farmer*)object1)->getPlayerRepresent()]->changeResource_byBuild(type);
            return addRelation(object1 , player[((Farmer*)object1)->getPlayerRepresent()]->addBuilding( type, BlockDR , BlockUR) , CoreEven_FixBuilding);
        }

    }

    return false;
}

bool Core_List::addRelation( Coordinate* object1, int evenType , int actNum )
{
    if(object1 == NULL) return false;

    if( object1->getSort() == SORT_BUILDING && !relate_AllObject[object1].isExist)
    {
        Building* buildOb = NULL;
        object1->printer_ToBuilding((void**)&buildOb);
        if(player[buildOb->getPlayerRepresent()]->get_isBuildActionAble(buildOb,actNum))
        {
            player[buildOb->getPlayerRepresent()]->changeResource_byBuildAction(buildOb,actNum);
            buildOb->setAction(actNum);
            relate_AllObject[object1] = relation_Object(evenType);
            return true;
        }
    }
    return false;
}

void Core_List::suspendRelation(Coordinate * object)
{
    if(relate_AllObject[object].isExist)
    {
        Building* buildOb = NULL;
        object->printer_ToBuilding((void**)&buildOb);
        if(buildOb!=NULL) player[buildOb->getPlayerRepresent()]->back_Resource_TS(buildOb); //返还资源
        object->initAction();  //行动全部重置

        relate_AllObject[object].isExist = false;
    }
}

void Core_List::manageRelationList()
{
    Coordinate* object1;
    Coordinate* object2;
//    list<conditionF> forcedInterrupCondition;


    map<Coordinate* , relation_Object>::iterator iter = relate_AllObject.begin();

    while( iter!= relate_AllObject.end() )
    {
        object1 = iter->first;
        relation_Object& thisRelation = iter->second;
        if( thisRelation.isExist )
        {
            manageRelation_updateMassage(object1);

            int& nowPhaseNum = thisRelation.nowPhaseNum;
            int exePhaseNum = nowPhaseNum;
            detail_EventPhase& thisDetailEven = relation_Event_static[thisRelation.relationAct];
            object2 = thisRelation.goalObject;
            conditionF* recordCondition;

            //该部分为强制中止行动部分代码
//            forcedInterrupCondition = thisDetailEven.forcedInterrupCondition;
//            for(list<conditionF>::iterator iter_list = forcedInterrupCondition.begin();iter_list!=forcedInterrupCondition.end();iter_list++)
//            {
//                if( iter_list->condition( object1,thisRelation,iter_list->variableArgu,iter_list->isNegation ) )
//                {
//                    thisRelation.nowPhaseNum = thisDetailEven.phaseInterrup;
//                    break;
//                }
//            }

            //判断是否在loop中，是否需要中止
            if( thisDetailEven.isLoop(nowPhaseNum) )
            {
                list<conditionF> &overCondition = thisDetailEven.loopOverCondition[nowPhaseNum];
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
            if(nowPhaseNum == exePhaseNum)  //nowphase变化后，防止提前行动。变化后的行动在下一帧开始做
            {
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
                        object_Gather(object1,object2);
                        break;
                    case CoreDetail_ResourceIn:
                        object_ResourceChange(object1 , thisRelation);
                        break;
                    case CoreDetail_UpdateRatio:
                        object_RatioChange(object1 , thisRelation);
                        break;
                    case CoreDetail_NormalEnd:
                        object_FinishAction(object1);
                        break;
                    case CoreDetail_AbsoluteEnd:
                        object_FinishAction_Absolute(object1);
                        break;
                    default:
                        break;
                }
            }
            else    //nowphase变化产生的1帧停顿，进行需要的相关初始值设置
            {
                switch (thisDetailEven.phaseList[nowPhaseNum])
                {
                    case CoreDetail_Move:
                        break;
                    case CoreDetail_Attack:
                        if(thisRelation.relationAct == CoreEven_MissileAttack) thisRelation.set_ExecutionTime(1);
                        break;
                    case CoreDetail_Gather:
                        thisRelation.needResourceBuilding = true;
                        break;
                    case CoreDetail_ResourceIn:
                        break;
                    default:
                        break;
                }
            }
            iter++;

        }
        else iter = relate_AllObject.erase(iter);
    }

}

void Core_List::manageRelation_deleteGoalOb( Coordinate* goalObject )
{
    map<Coordinate* , relation_Object>::iterator iterNow = relate_AllObject.begin() , iterEnd = relate_AllObject.end();

    //遍历整个动态表，删除goalObject
    while(iterNow != iterEnd)
    {
        if(iterNow->second.goalObject == goalObject)
        {
            iterNow->second.update_GoalPoint();
            iterNow->second.goalObject = NULL;
        }

        if(iterNow->second.alterOb == goalObject)
        {
            iterNow->second.update_Attrib_alter();
            iterNow->second.init_AlterOb();
        }

        iterNow++;
    }
}

//实时更新关系表内部信息
void Core_List::manageRelation_updateMassage( Coordinate* object1 )
{
    relation_Object& relation = relate_AllObject[object1];

    if(relation.alterOb == NULL)
    {
        if(relation.needResourceBuilding) findResourceBuiding( relation ,  player[object1->getPlayerRepresent()]->build);
    }

    relation.update_GoalPoint();
    relation.update_Attrib_alter();
}

//查询最近的符合要求的建筑，并设置alterOB
void Core_List::findResourceBuiding( relation_Object& relation , list<Building*>& building)
{
    int type = relation.resourceBuildingType;
    double dis, dis_opti = relation.distance_Record , dr = relation.DR_goal , ur =relation.UR_goal;
    Building* judeBuild, *optimum = NULL;

    list<Building*>::iterator iterNow = building.begin() , iterEnd = building.end();

    while( iterNow != iterEnd )
    {
        judeBuild = *iterNow;
        if( judeBuild->isFinish() &&(judeBuild->getNum() == BUILDING_CENTER || judeBuild->getNum() == type))
        {
            dis = calculateManhattanDistance(dr,ur,judeBuild->getDR(),judeBuild->getUR());
            if(dis < dis_opti)
            {
                optimum = judeBuild;
                dis_opti =dis;
            }
        }
        iterNow++;
    }
    if(optimum!=NULL) relation.set_AlterOb(optimum , dis_opti);
}

void Core_List::eraseObject(Coordinate* eraseOb)
{
    eraseRelation(eraseOb);
    manageRelation_deleteGoalOb(eraseOb);
}

int Core_List::getNowPhaseNum(Coordinate* object){
    ///获取当前object的行动阶段，用于将信息传递给AIGame
    relation_Object& thisRelation=relate_AllObject[object];
    if(!thisRelation.isExist||thisRelation.relationAct==CoreEven_JustMoveTo){
        return HUMAN_STATE_IDLE;
    }
    int& nowPhaseNum = thisRelation.nowPhaseNum;
    //detail_EventPhase& thisDetailEven = relation_Event_static[thisRelation.relationAct];
    Coordinate* obj=relate_AllObject[object].goalObject;
    if(nowPhaseNum==0){
        if(obj->getSort()==SORT_ANIMAL&&obj->getNum()!=ANIMAL_TREE&&obj->getNum()!=ANIMAL_FOREST){
            return HUMAN_STATE_GOTO_ATTACK;
        }else{
            return HUMAN_STATE_GOTO_OBJECT;
        }
    }else if(nowPhaseNum==1||nowPhaseNum==2){
        if(obj->getSort()==SORT_ANIMAL&&obj->getNum()!=ANIMAL_TREE&&obj->getNum()!=ANIMAL_FOREST){
            return HUMAN_STATE_ATTACKING;
        }else{
            return HUMAN_STATE_CUTTING;
        }
    }else if(nowPhaseNum==3||nowPhaseNum==4||nowPhaseNum==8||nowPhaseNum==9){
        return HUMAN_STATE_GOTO_RESOURCE;
    }else if(nowPhaseNum==5||nowPhaseNum==10||nowPhaseNum==11){
        return HUMAN_STATE_GOTO_OBJECT;
    }else if(nowPhaseNum==6||nowPhaseNum==7){
        ///资源采集
        if(obj->getSort()==SORT_STATICRES){
            if(obj->getNum()==0){
                return HUMAN_STATE_GATHERING;
            }else if(obj->getNum()==1){
                return HUMAN_STATE_DIGGING_STONE;
            }else if(obj->getNum()==2){
                return HUMAN_STATE_DIGGING_GOLD;
            }
        }else if(obj->getSort()==SORT_ANIMAL){
            if(obj->getNum()==ANIMAL_TREE||obj->getNum()==ANIMAL_FOREST){
                return HUMAN_STATE_CUTTING;
            }else{
                return HUMAN_STATE_BUTCHERING;
            }
        }
    }
    return -1;
}

int Core_List::getObjectSN(Coordinate* object){
    relation_Object& thisRelation=relate_AllObject[object];
    if(thisRelation.isExist){
        return thisRelation.goalObject->getglobalNum();
    }
}
//****************************************************************************************
//通用的控制对象行动函数
void Core_List::object_Move(Coordinate * object , double DR , double UR)
{
    MoveObject* moveObject = (MoveObject*)object;
    if((moveObject->getDR0()!=DR || moveObject->getUR0()!= UR))
    {
//        qDebug()<<object->getSort();
        Point start,destination;
        if(!moveObject->isWalking()) moveObject->setPreWalk();
        moveObject->setdestination(DR, UR);
        start.x = tranBlockDR(moveObject->getDR());
        start.y = tranBlockUR(moveObject->getUR());
        destination.x = tranBlockDR(moveObject->getDR0());
        destination.y = tranBlockUR(moveObject->getUR0());
        theMap->loadfindPathMap();
        moveObject->setPath(findPath(theMap->findPathMap , theMap , start , destination));
    }

    if(!moveObject->isWalking()) moveObject->setPreWalk();
}

void Core_List::object_Attack(Coordinate* object1 ,Coordinate* object2)
{
    bool calculateDamage = false;
    int damage; //记录伤害
    BloodHaver* attacker = NULL;    //攻击者
    BloodHaver* attackee = NULL;    //受攻击者
    Missile* missile = NULL;

    //攻击者指针赋值(object1强制转换)
    object1->printer_ToBloodHaver((void**)&attacker);
    //受攻击者指针赋值(object2强制转换)
    object2->printer_ToBloodHaver((void**)&attackee);
    //判断obect1是否为投射物
    object1->printer_ToMissile((void**)&missile);

    if(attackee != NULL && attacker!=NULL)  //若指针均非空
    {
        if(!attacker->isAttacking()) attacker->setPreAttack();
        //非祭司,是普通的伤害计算公式
        /** 后续版本若有投石车等喷溅伤害,判断还需细化*/

        if(attacker->is_missileAttack())
        {
            if(object1->get_isActionImageToPhaseFromEnd(PhaseFromEnd_Attack_ThrowMissile))
                addRelation( creatMissile(object1 , object2) , object2 , CoreEven_MissileAttack , false);
        }
        else if(object1->get_isActionEnd())
        {
            calculateDamage = true;
            if(!attackee->isGotAttack()) attackee->setAvangeObject(object1);
        }
    }
    else if(missile!=NULL && missile->is_HitTarget() && attackee!=NULL )
    {
        calculateDamage = true;
        attacker = missile->getAttackAponsor();
        if(!attackee->isGotAttack())
        {
            if(missile->isAttackerHaveDie())
            {
                double DR,UR;
                missile->get_AttackSponsor_Position(DR , UR);
                attackee->setAvangeObject(DR,UR);
            }
            else attackee->setAvangeObject(missile->getAttacker());
        }
    }

    if(calculateDamage)
    {
        qDebug()<<(attacker->getATK())<<(attackee->getDEF(attacker->get_AttackType()));
        damage = attacker->getATK()-attackee->getDEF(attacker->get_AttackType());   //统一伤害计算公式
        if(damage<0) damage = 0;
        attackee->updateBlood(damage);  //damage反映到受攻击者血量减少
        qDebug()<<damage;
    }
}

void Core_List::object_Gather(Coordinate* object1 , Coordinate* object2)
{
    Farmer* gatherer = (Farmer*) object1;
    Resource* res = NULL;
    object2->printer_ToResource((void**)&res);

    if(res!= NULL)
    {
        if(!gatherer->isWorking()) gatherer->setPreWork();
        else
        {
            if(gatherer->getResourceSort() != res->get_ResourceSort())
            {
                gatherer->set_ResourceSort(res->get_ResourceSort());
                gatherer->update_resourceClear();
            }

            if(gatherer->get_isActionEnd())
            {
                res->updateCnt_byGather(gatherer->get_quantityGather());
                gatherer->update_addResource();
            }
        }
    }
}

void Core_List::object_ResourceChange( Coordinate* object1, relation_Object& relation)
{
    if(relation.relationAct == CoreEven_Gather)
    {
        if(object1->getSort() == SORT_FARMER)
        {
            Farmer* worker = (Farmer*) object1;
            if(relation.alterOb == NULL) worker->setPreStand();
            else
            {
                player[worker->getPlayerRepresent()]->changeResource(worker->getResourceSort() , worker->getResourceNowHave());
                worker->update_resourceClear();
            }
        }
    }
}

void Core_List::object_RatioChange( Coordinate* object1, relation_Object& relation)
{
    if(relation.relationAct == CoreEven_FixBuilding)
    {
        if(!((Farmer*)object1)->isWorking()) ((Farmer*)object1)->setPreWork();
        ((Building*)relation.goalObject)->update_Build();
    }
    else if(relation.relationAct == CoreEven_BuildingAct)
        ((Building*)object1)->update_Action();
}

void Core_List::object_FinishAction_Absolute(Coordinate* object1)
{
    MoveObject* moOb = NULL;
    object1->printer_ToMoveObject((void**)&moOb);
    if(moOb!=NULL) moOb->setPreStand();

    suspendRelation(object1);
}

void Core_List::object_FinishAction(Coordinate* object1)
{
//    if(relate_AllObject[object1].relationAct == CoreEven_FixBuilding && 是农田)
    if(relate_AllObject[object1].relationAct == CoreEven_BuildingAct)
        player[((Building*)object1)->getPlayerRepresent()]->enforcementAction((Building*)object1);  //进行建筑行动的结果处理
    else if(relate_AllObject[object1].relationAct == CoreEven_FixBuilding && relate_AllObject[object1].goalObject!=NULL )
    {
        player[((Human*)object1)->getPlayerRepresent()]->finishBuild((Building*)relate_AllObject[object1].goalObject);
        relate_AllObject[object1].goalObject->initAction();
    }
    else if(relate_AllObject[object1].relationAct == CoreEven_MissileAttack)
    {
        Missile* misOb = NULL;
        object1->printer_ToMissile((void**)&misOb);
        if(misOb!=NULL) misOb->needDelete();
    }

    object_FinishAction_Absolute(object1);
}

//****************************************************************************************
//处理受到攻击的诱发行动
void Core_List::conduct_Attacked(Coordinate* object)
{
    double dr,ur;
    Coordinate* attacker = NULL;
    BloodHaver* attackee = NULL;
    object->printer_ToBloodHaver((void**)&attackee);

    if(  !attackee->isDie() && (!relate_AllObject[object].isExist || relate_AllObject[object].respondConduct) )
    {
        if(attackee != NULL && attackee->isGotAttack())
        {
            //判断攻击者是否为空指针
            attacker=attackee->getAvangeObject();
            if(attacker!=NULL) attackee->updateAvangeObjectPosition();
            //设置攻击者坐标
            attackee->get_AvangeObject_Position(dr,ur);

            if(object->getSort() == SORT_ANIMAL)
            {
                switch (((Animal*)object)->get_Friendly())
                {
                    case FRIENDLY_FRI:
                        calMirrorPoint(dr,ur,object->getDR(),object->getUR(),3.5*BLOCKSIDELENGTH);
                        suspendRelation(object);
                        if(addRelation(object,dr,ur,CoreEven_JustMoveTo,false)) ((MoveObject*)object)->beginRun();
                        break;
                    case FRIENDLY_ENEMY:
                    case FRIENDLY_FENCY:
                        if(attacker!=NULL) addRelation(object,attacker,CoreEven_Attacking,false);
                        break;
                    default:
                        break;
                }
            }
            else if(object->getSort() == SORT_FARMER)
            {
//                if(attacker!=NULL)
//                {
//                    if(attacker->getSort() == SORT_ANIMAL)  addRelation(object,attacker,CoreEven_Attacking,false);
//                    else
//                    {
//                        calMirrorPoint(dr,ur,object->getDR(),object->getUR(),3.5*BLOCKSIDELENGTH);
//                        suspendRelation(object);
//                        addRelation(object,dr,ur,CoreEven_JustMoveTo,false);
//                    }
//                }
            }
        }
    }

    attackee->initAvengeObject();
}

//**************************************************************
//寻路相关
bool Core_List::isValidPoint(const int (&map)[MAP_L][MAP_U], const Point &p)
{
    int rows = MAP_L;
    int cols = MAP_U;
    return (p.x >= 0 && p.x < rows && p.y >= 0 && p.y < cols);
}

vector<Point> Core_List::getAdjacentPoints(const int (&map)[MAP_L][MAP_U], const Point &p)
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

stack<Point> Core_List::findPath(const int (&findPathMap)[MAP_L][MAP_U], Map *map, const Point &start, const Point &destination)
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

stack<Point> Core_List::findPathAlternative(const int (&map)[MAP_L][MAP_U], const Point &start, const Point &destination)
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


//*************************************************************
//行动预备处理
//创建投掷物
Missile* Core_List::creatMissile(Coordinate* attacker ,Coordinate* attackee)
{
    int playerRepresent = MAXPLAYER;
    Human* judHuman = NULL;
    Building* judBuild = NULL;

    attacker->printer_ToBuilding((void**)&judBuild);
    attacker->printer_ToHuman((void**)&judHuman);

    if(judHuman!=NULL) playerRepresent = judHuman->getPlayerRepresent();
    if(judBuild!= NULL) playerRepresent = judBuild->getPlayerRepresent();

    if(playerRepresent<MAXPLAYER) return player[playerRepresent]->addMissile(attacker , attackee);
    else return NULL;
}


/**************************************************************/
//建立行动细节的静态表
void Core_List::initDetailList()
{
    int *phaseList;
    conditionF* conditionList;
//    list<conditionF>forcedInterrupCondition;
    list<conditionF>overCondition;

    //添加静态表
    //行动：只移动*************************************
    {
        phaseList = new int(CoreDetail_Move);
        conditionList = new ( conditionF )( conditionF(condition_ObjectNearby) );
//        forcedInterrupCondition.push_back(conditionF(condition_UniObjectDie , OPERATECON_OBJECT1));
//        forcedInterrupCondition.push_back(conditionF(condition_UniObjectUnderAttack , OPERATECON_OBJECT1));

        relation_Event_static[CoreEven_JustMoveTo] = detail_EventPhase( 1 ,  phaseList, conditionList/* , forcedInterrupCondition*/ );
        relation_Event_static[CoreEven_JustMoveTo].setEnd_Absolute();
        delete phaseList;
        delete conditionList;
//        forcedInterrupCondition.clear();
    }


    //行动: 攻击**************************************
    {
        phaseList = new int[2]{ CoreDetail_Move , CoreDetail_Attack };
        conditionList = new conditionF[2]{ conditionF(condition_ObjectNearby , OPERATECON_NEAR_ATTACK) ,  conditionF(condition_ObjectNearby,OPERATECON_NEAR_ATTACK,true)};
//        forcedInterrupCondition.push_back(conditionF(condition_UniObjectDie,OPERATECON_OBJECT1));

        relation_Event_static[CoreEven_Attacking] = detail_EventPhase(2 , phaseList , conditionList /*, forcedInterrupCondition*/ );
        overCondition.push_back(conditionF( condition_UniObjectDie, OPERATECON_OBJECT2 ));
        relation_Event_static[CoreEven_Attacking].setLoop(0,1,overCondition);   //向前跳转使用setLoop

        delete phaseList;
        delete conditionList;
//        forcedInterrupCondition.clear();
        overCondition.clear();
    }


    //行动：采集**************************************
    {
        phaseList = new int[12]{ /*0前往攻击目标*/CoreDetail_Move ,/*1攻击*/CoreDetail_Attack ,       /*2*/CoreDetail_JumpPhase ,\
                               /*3前往资源建筑*/CoreDetail_Move ,  /*4资源放置*/CoreDetail_ResourceIn, /*5前往资源*/CoreDetail_Move,\
                               /*6采集*/CoreDetail_Gather, /*资源被采集完毕，需要判断村民携带资源*/  /*7*/CoreDetail_JumpPhase ,\
                               /*8前往资源建筑*/CoreDetail_Move ,  /*9资源放置*/CoreDetail_ResourceIn,/*10前往资源原位置*/CoreDetail_Move ,\
                               /*11*/ CoreDetail_JumpPhase};
        conditionList = new conditionF[12]{ \
                /*0*/conditionF(condition_ObjectNearby,OPERATECON_NEAR_ATTACK) ,    /*1*/conditionF(condition_ObjectNearby,OPERATECON_NEAR_ATTACK,true),\
                /*2*/conditionF(condition_AllTrue) ,                                /*3*/conditionF(condition_ObjectNearby,OPERATECON_NEARALTER_WORK) , \
                /*4*/conditionF(condition_Object1_EmptyBackpack),                   /*5*/conditionF(condition_ObjectNearby,OPERATECON_NEAR_WORK),\
                /*6*/conditionF(condition_Object1_FullBackpack) , \
                /*7*/conditionF(condition_AllTrue),                                 /*8*/conditionF(condition_ObjectNearby,OPERATECON_NEARALTER_WORK),\
                /*9*/conditionF(condition_Object1_EmptyBackpack),                   /*10*/conditionF(condition_ObjectNearby,OPERATECON_NEAR_WORK),\
                /*11*/conditionF(condition_AllTrue)};

//        forcedInterrupCondition.push_back(conditionF(condition_UniObjectDie , OPERATECON_OBJECT1));
//        forcedInterrupCondition.push_back(conditionF(condition_UniObjectUnderAttack , OPERATECON_OBJECT1));

        relation_Event_static[CoreEven_Gather] = detail_EventPhase(12 , phaseList, conditionList/*,forcedInterrupCondition*/);
        //设置循环，0->1，攻击猎物直至可采集
        overCondition.push_back(conditionF(condition_Object2CanbeGather));
        relation_Event_static[CoreEven_Gather].setLoop(0,1,overCondition);
        overCondition.clear();
        //设置循环，3->6，持续采集，目标无资源不可采集
        overCondition.push_back(conditionF(condition_Object2CanbeGather,OPERATECON_OBJECT2,true));
        relation_Event_static[CoreEven_Gather].setLoop(3,6,overCondition);
        overCondition.clear();

        //设置循环，8->11，用于最后一次返回资源建筑，若身上无资源，则直接停止
        overCondition.push_back(conditionF(condition_Object1_EmptyBackpack));
        relation_Event_static[CoreEven_Gather].setLoop(8,11,overCondition);
        overCondition.clear();

        //猎物可采集后，跳转至前往资源
        relation_Event_static[CoreEven_Gather].setJump(2,5);
        //资源被采集完毕后，若身上无资源，则直接停止
        relation_Event_static[CoreEven_Gather].setJump(7 , 11);

        delete phaseList;
        delete conditionList;
//        forcedInterrupCondition.clear();
        overCondition.clear();
    }


    //行动：修建建筑***********************************
    {
        phaseList = new int[2]{CoreDetail_Move , CoreDetail_UpdateRatio };
        conditionList = new conditionF[2]{ conditionF( condition_ObjectNearby , OPERATECON_NEAR_WORK ) , conditionF( condition_UniObjectPercent , OPERATECON_OBJECT2 ) };

        relation_Event_static[CoreEven_FixBuilding] = detail_EventPhase(2 , phaseList , conditionList);

        delete phaseList;
        delete conditionList;
    }


    //行动：建筑工作***********************************
    {
        phaseList = new int(CoreDetail_UpdateRatio);
        conditionList = new (conditionF)( conditionF(condition_UniObjectPercent , OPERATECON_OBJECT1));

        relation_Event_static[CoreEven_BuildingAct] = detail_EventPhase(1 , phaseList , conditionList);

        delete phaseList;
        delete conditionList;
    }

    //行动：飞行物攻击*********************************
    {
        phaseList = new int[2]{ CoreDetail_Move , CoreDetail_Attack };
        conditionList = new conditionF[2]{ conditionF( condition_ObjectNearby, OPERATECON_NEAR_MISSILE ) , conditionF( condition_TimesFalse ) };

        relation_Event_static[CoreEven_MissileAttack] = detail_EventPhase(2 , phaseList , conditionList);

        delete phaseList;
        delete conditionList;
    }
}
