#include "Core_list.h"
extern Score usrScore;
extern Score enemyScore;
Core_List::Core_List(Map* theMap, Player* player[])
{
    this->theMap = theMap;
    this->player = player;
    initDetailList();
}
void Core_List::update()
{
    //对正在监视的Object，进行行动处理
    manageMontorAct();
    theMap->loadBarrierMap();//更新寻路用障碍表
    jud_resetResBuild();

    manageRelationList();

    init_resetResBuild();
}

//****************************************************************************************
//控制动态表
//对人，命令不必手动中止，直接变更；当建筑正进行升级、造兵等行动，命令变更将被无视，必须手动中止当前命令后再下达新命令。
int Core_List::addRelation( Coordinate * object1, Coordinate * object2, int eventType , bool respond)
{
    if(object1 == NULL) return ACTION_INVALID_NULLWORKER;

    if(relate_AllObject[object1].isExist && relate_AllObject[object1].respondConduct) suspendRelation(object1);

    if(! relate_AllObject[object1].isExist )
    {
        BloodHaver* bloodOb = NULL;
        Building* buildOb = NULL;
        object1->printer_ToBloodHaver((void**)&bloodOb);
        object1->printer_ToBuilding((void**)&buildOb);
        if( bloodOb != NULL && bloodOb->isDie()) return ACTION_INVALID_SN;
        if( buildOb != NULL && !(buildOb->isConstructed() && buildOb->isFinish())) return ACTION_INVALID_BUILDACT_NEEDBUILT;

        //为工作者设置交互对象类别属性，主要用于farmer的status判断/Attack...
        bool isSameReprensent;
        if(object1->isPlayerControl() && object2->isPlayerControl()) isSameReprensent = object1->getPlayerRepresent() == object2->getPlayerRepresent();
        else isSameReprensent = false;

        if(object2->getSort() == SORT_BUILDING || object2->getSort() == SORT_Building_Resource)
            object1->set_interAct(object2->getSort() , object2->getNum(), isSameReprensent , ((Building*)object2)->isFinish());
        else
            object1->set_interAct(object2->getSort() , object2->getNum(), isSameReprensent);

        //加入行动交互表
        relate_AllObject[object1] = relation_Object(object2 , eventType);
        relate_AllObject[object1].respondConduct = respond; //是否可由addrelation更改行动

        //根据行动类型，设置需要的额外属性
        switch (eventType)
        {
            case CoreEven_Gather:
                relate_AllObject[object1].update_GoalPoint();
                relate_AllObject[object1].set_ResourceBuildingType();
            case CoreEven_Attacking:
                relate_AllObject[object1].init_AttackAb(object1);
                ((MoveObject*)object1)->beginRun();
                break;
            default:
                break;
        }

        requestSound_Action(object1, eventType, object2);
        return ACTION_SUCCESS;
    }
    return ACTION_INVALID_ISNTFREE;
}

int Core_List::addRelation( Coordinate * object1, double DR , double UR, int eventType , bool respond , int type)
{
    if(object1 == NULL) return ACTION_INVALID_NULLWORKER;

    if(relate_AllObject[object1].isExist && relate_AllObject[object1].respondConduct) suspendRelation(object1);

    if(! relate_AllObject[object1].isExist )
    {
        BloodHaver* bloodOb = NULL;
        object1->printer_ToBloodHaver((void**)&bloodOb);
        if( bloodOb!=NULL && bloodOb->isDie()) return ACTION_INVALID_SN;

        if(eventType == CoreEven_JustMoveTo)
        {
            if(DR<0) DR = 0;
            if(UR<0) UR = 0;
            if(DR>71.9*BLOCKSIDELENGTH) DR = 71.9*BLOCKSIDELENGTH;
            if(UR>71.9*BLOCKSIDELENGTH) UR = 71.9*BLOCKSIDELENGTH;
            relate_AllObject[object1] = relation_Object(DR , UR , eventType);
            relate_AllObject[object1].respondConduct = respond;
            relate_AllObject[object1].sort = SORT_COORDINATE;

            requestSound_Action(object1, CoreEven_JustMoveTo);
            return ACTION_SUCCESS;
        }
        //判断行动为CoreEven_CreatBuilding，紧接着判断地图上建筑范围内是否有障碍物，最后判断player是否有足够资源并进行资源扣除
        else if(eventType == CoreEven_CreatBuilding)
        {
            int wrongCode;
            wrongCode = is_BuildingCanBuild(type , tranBlockDR(DR) , tranBlockUR(UR),((Farmer*)object1)->getPlayerRepresent());
            if(wrongCode<0) return wrongCode;

            if(!player[((Farmer*)object1)->getPlayerRepresent()]->get_isBuildingShowAble(type))
                return ACTION_INVALID_HUMANBUILD_LOCK;

            if(!player[((Farmer*)object1)->getPlayerRepresent()]->get_isBuildingAble(type))
                return ACTION_INVALID_RESOURCE;

            player[((Farmer*)object1)->getPlayerRepresent()]->changeResource_byBuild(type);
            return addRelation(object1 , player[((Farmer*)object1)->getPlayerRepresent()]->addBuilding( type,tranBlockDR(DR) , tranBlockUR(UR)) , CoreEven_FixBuilding);
        }
    }

    return ACTION_INVALID_ISNTFREE;
}

//建造
int Core_List::addRelation( Coordinate* object1, int BlockDR , int BlockUR, int eventType , bool respond , int type)
{
    if(object1 == NULL) return ACTION_INVALID_NULLWORKER;

    if(relate_AllObject[object1].isExist && relate_AllObject[object1].respondConduct) suspendRelation(object1);

    if(! relate_AllObject[object1].isExist )
    {
        BloodHaver* bloodOb = NULL;
        object1->printer_ToBloodHaver((void**)&bloodOb);
        if( bloodOb != NULL && bloodOb->isDie()) return ACTION_INVALID_SN;

        //判断行动为CoreEven_CreatBuilding，紧接着判断地图上建筑范围内是否有障碍物，最后判断player是否有足够资源并进行资源扣除
        if(eventType == CoreEven_CreatBuilding)
        {
            int wrongCode;
            wrongCode = is_BuildingCanBuild(type , BlockDR, BlockUR,((Farmer*)object1)->getPlayerRepresent());
            if(wrongCode<0) return wrongCode;

            if(!player[((Farmer*)object1)->getPlayerRepresent()]->get_isBuildingShowAble(type))
                return ACTION_INVALID_HUMANBUILD_LOCK;

            if(!player[((Farmer*)object1)->getPlayerRepresent()]->get_isBuildingAble(type))
                return ACTION_INVALID_RESOURCE;

            player[((Farmer*)object1)->getPlayerRepresent()]->changeResource_byBuild(type);
            return addRelation(object1 , player[((Farmer*)object1)->getPlayerRepresent()]->addBuilding( type, BlockDR , BlockUR) , CoreEven_FixBuilding);
        }
    }

    return ACTION_INVALID_ISNTFREE;
}

//添加building行动
int Core_List::addRelation( Coordinate* object1, int evenType , int actNum )
{
    if(object1 == NULL) return ACTION_INVALID_NULLWORKER;

    if( object1->getSort() == SORT_BUILDING && !relate_AllObject[object1].isExist)
    {
        BloodHaver* bloodOb = NULL;
        Building* buildOb = NULL;
        int oper = 0;
        object1->printer_ToBuilding((void**)&buildOb);
        object1->printer_ToBloodHaver((void**)&bloodOb);
        if( !buildOb || !bloodOb || bloodOb->isDie()) return ACTION_INVALID_SN;

        if(!(buildOb->isConstructed() && buildOb->isFinish())) return ACTION_INVALID_BUILDACT_NEEDBUILT;

        if(!player[buildOb->getPlayerRepresent()]->get_isBuildActionShowAble(buildOb->getNum(),actNum))
            return ACTION_INVALID_BUILDACT_LOCK;

        //判断行动是否可进行，是否可进行主要受资源数量限制，对于产人行动，还受当前人口数量限制。
        if(!player[buildOb->getPlayerRepresent()]->get_isBuildActionAble(buildOb,actNum,&oper))
        {
            /** oper == 1 说明是由于人口上限
             *  oper == 0 说明是由于资源数量
            */
            if(oper == 1) return ACTION_INVALID_BUILDACT_MAXHUMAN;
            else if(oper == 0) return ACTION_INVALID_RESOURCE;
        }

        player[buildOb->getPlayerRepresent()]->changeResource_byBuildAction(buildOb,actNum);
        buildOb->setAction(actNum);
        relate_AllObject[object1] = relation_Object(evenType);
        return ACTION_SUCCESS;
    }
    return ACTION_INVALID_ISNTFREE;
}

void Core_List::suspendRelation(Coordinate * object)
{
    if(relate_AllObject[object].isExist)
    {
        Building* buildOb = NULL;
        MoveObject* moveOb = NULL;
        object->printer_ToBuilding((void**)&buildOb);
        object->printer_ToMoveObject((void**)&moveOb);
        if(buildOb!=NULL) player[buildOb->getPlayerRepresent()]->back_Resource_TS(buildOb); //返还资源
        if(moveOb!= NULL)   //清空路径
        {
            moveOb->setPreStand();
            moveOb->setPath(stack<Point>(),object->getDR(),object->getUR());
        }
        object->initAction();  //行动全部重置

        relate_AllObject[object].isExist = false;
    }
}

void Core_List::manageRelationList()
{
    Coordinate* object1;
    Coordinate* object2;
    list<conditionF> forcedInterrupCondition;

    map<Coordinate* , relation_Object>::iterator iter = relate_AllObject.begin();

    while( iter!= relate_AllObject.end() )
    {
        object1 = iter->first;
        relation_Object& thisRelation = iter->second;
        if( thisRelation.isExist )
        {
            //更新object1对应表内信息
            manageRelation_updateMassage(object1);

            int& nowPhaseNum = thisRelation.nowPhaseNum;
            int exePhaseNum = nowPhaseNum;
            detail_EventPhase& thisDetailEven = relation_Event_static[thisRelation.relationAct];
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
        else iter = relate_AllObject.erase(iter);   //删表
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

    if(relation.alterOb == NULL || needReset_resBuild)
    {
        if(relation.needResourceBuilding) findResourceBuiding( relation ,  player[object1->getPlayerRepresent()]->build);
    }

    relation.reset_Object1Predicted(object1);
    relation.height_Object = theMap->get_MapHeight(object1->getBlockDR() , object1->getBlockUR());
    relation.update_GoalPoint();
    if(relation.goalObject != NULL)
        relation.height_GoalObject = theMap->get_MapHeight(relation.goalObject->getBlockDR() , relation.goalObject->getBlockUR());
    relation.update_Attrib_alter();
    relation.waiting();
}

void Core_List::eraseObject(Coordinate* eraseOb)
{
    eraseRelation(eraseOb);
    manageRelation_deleteGoalOb(eraseOb);
}

//****************************************************************************************
//辅助动态表处理交互
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

//判断是否可以建造建筑
int Core_List::is_BuildingCanBuild(int buildtype , int BlockDR , int BlockUR ,int playerID)
{
    int answer = ACTION_SUCCESS;
    int DRL = -1,DRR = -1,URD = -1, URU = -1;    //记录边界
    Building* tempBuild = NULL;

    //预创建
    if(buildtype == BUILDING_FARM)
    {
        Building_Resource* tempBuild_resource = new Building_Resource(buildtype,BlockDR,BlockUR);
        tempBuild = tempBuild_resource;
    }
    else tempBuild = new Building(buildtype,BlockDR,BlockUR);

    DRL = tempBuild->getBlockDR();
    DRR = tempBuild->getBlockDR()+tempBuild->get_BlockSizeLen();
    URD = tempBuild->getBlockUR();
    URU = tempBuild->getBlockUR()+tempBuild->get_BlockSizeLen();

    if(DRL<0 || DRR>71 || URD<0 || URU>71)
    {
        call_debugText("red"," 在("+QString::number(BlockDR)+","+QString::number(BlockUR)+")建造"+tempBuild->getChineseName()+" 建造失败,选中位置越界",playerID);
        answer = ACTION_INVALID_HUMANBUILD_OVERBORDER;
    }
    else if( !theMap->cell[DRL][URD].Explored )
    {
        call_debugText("red"," 在("+QString::number(BlockDR)+","+QString::number(BlockUR)+")建造"+tempBuild->getChineseName()+" 建造失败,选中位置未被探索",playerID);
        answer = ACTION_INVALID_HUMANBUILD_UNEXPLORE;
    }
    else if(answer == 0) //如果以上限制均未触发，判断是否有重叠
    {
        int bDR_ba,bUR_ba;
        if(theMap->isHaveObject(tempBuild->getBlockDR(),tempBuild->getBlockUR(),bDR_ba,bUR_ba , tempBuild->get_BlockSizeLen()))
        {
            call_debugText("red"," 在("+QString::number(BlockDR)+","+QString::number(BlockUR)+")建造"+tempBuild->getChineseName()+" 建造失败:放置位置非空地，在("+ QString::number(bDR_ba)+","+QString::number(bUR_ba)+")处与其他物体重叠",playerID);
            answer = ACTION_INVALID_HUMANBUILD_OVERLAP;
        }
        else if(!theMap->isFlat(tempBuild))
        {
            call_debugText("red"," 在("+QString::number(BlockDR)+","+QString::number(BlockUR)+")建造"+tempBuild->getChineseName()+" 建造失败:放置位置存在高度差或斜坡",playerID);
            answer = ACTION_INVALID_HUMANBUILD_DIFFERENTHIGH;
        }
    }

    //*************结束******************
    delete tempBuild;

    return answer;
}

int Core_List::getObjectSN(Coordinate* object){
    relation_Object& thisRelation=relate_AllObject[object];
    if(thisRelation.isExist&&thisRelation.goalObject!=nullptr){
        return thisRelation.goalObject->getglobalNum();
    }else{
        return -1;
    }
}

//音效
void Core_List::requestSound_Action( Coordinate* object, int actionType, Coordinate* goalObject)
{
    if(object->getPlayerRepresent() != NOWPLAYERREPRESENT) return;

    Animal* aniOb = NULL;
    StaticRes* staticOb = NULL;

    if(object->getSort() == SORT_FARMER)
    {
        switch (actionType) {
        case CoreEven_FixBuilding:
            soundQueue.push("Villager_Builder");
            break;
        case CoreEven_JustMoveTo:
            soundQueue.push("Villager_Move");
            break;
        case CoreEven_Gather:

            goalObject->printer_ToAnimal((void**)&aniOb);
            goalObject->printer_ToStaticRes((void**)&staticOb);

            if(aniOb!=NULL)
            {
                if(aniOb->isTree()) soundQueue.push("Villager_Lumber");
                else soundQueue.push("Villager_Hunter");
            }
            else if(staticOb!=NULL)
            {
                if(staticOb->getNum() == NUM_STATICRES_Bush)
                    soundQueue.push("Villager_Gather");
                else
                    soundQueue.push("Villager_Miner");
            }
            else if(goalObject->getSort() == SORT_Building_Resource)
                soundQueue.push("Villager_Gather");

            break;
        default:
            break;
        }
    }
    else if(object->getSort() == SORT_ARMY)
    {
        soundQueue.push("Army_Walk");
    }
}


//****************************************************************************************
//通用的控制对象行动函数
void Core_List::object_Move(Coordinate * object , double DR , double UR)
{
    MoveObject* moveObject = NULL;
    Coordinate* goalOb;
    object->printer_ToMoveObject((void**)&moveObject);
    //如果有目标点，设置目标点
    if(moveObject) moveObject->stateCrash=false;//重置碰撞状态（tagGame）

    if(relate_AllObject[object].isUseAlterGoal)
        goalOb = relate_AllObject[object].alterOb;
    else
        goalOb = relate_AllObject[object].goalObject;

    if(moveObject)
    {
        if(!relate_AllObject[object].isWaiting())
        {
            if((moveObject->getDR0()!=DR || moveObject->getUR0()!= UR))
            {
                //为moveObject设置路径
                setPath(moveObject, goalOb, DR, UR);
            }
            else
            {
                if(!moveObject->isWalking()) moveObject->setPreWalk();
                else if(moveObject->getCrashOb()!=NULL)
                {
                    //处理碰撞
                    crashHandle(moveObject);
                }
                else if(relate_AllObject[object].crash_DealPhase)
                {
                    //碰撞后的调整
                    work_CrashPhase(moveObject);
                }
                else relate_AllObject[object].useOnce();
            }
        }

        moveObject->initCrash();
    }
    else
    {
        relate_AllObject[object].useless();
        if(moveObject) moveObject->stateCrash = true;
    }

}

void Core_List::object_Attack(Coordinate* object1 ,Coordinate* object2)
{
    bool calculateDamage = false;
    int damage; //记录伤害
    int extra_damage = 0;
    BloodHaver* attacker = NULL;    //攻击者
    BloodHaver* attackee = NULL;    //受攻击者
    MoveObject* moveOb = NULL;
    Missile* missile = NULL;

    object1->printer_ToBloodHaver((void**)&attacker);   //攻击者指针赋值(object1强制转换)
    if(object2) object2->printer_ToBloodHaver((void**)&attackee);   //受攻击者指针赋值(object2强制转换)
    object1->printer_ToMissile((void**)&missile);   //判断obect1是否为投射物

    if(attackee != NULL && attacker!=NULL && attacker->canAttack())  //若指针均非空
    {        
        if(!attacker->isAttacking()) attacker->setPreAttack();
        else
        {
            object1->printer_ToMoveObject((void**)&moveOb);
            if(moveOb!=NULL) moveOb->adjustAngle(object2->getDR(),object2->getUR());
            //非祭司,是普通的伤害计算公式
            /** 后续版本若有投石车等喷溅伤害,判断还需细化*/
            if( attacker->is_missileAttack())
            {
                if( attacker->is_missileThrow() )
                {
                    addRelation( creatMissile(object1 , object2) , object2 , CoreEven_MissileAttack , false);
                    attacker->haveAttack();
                }
            }
            else if( attacker->is_attackHit())
            {
                calculateDamage = true;
                attacker->haveAttack();
                if(attacker->get_isRangeAttack()) deal_RangeAttack(object1 , object2);
                if(!attackee->isGotAttack())
                {
                    attackee->setAvangeObject(object1);
                    if(object1->getPlayerRepresent() != 0) object1->visibleSomeTimes();
                }
            }
        }
    }
    else if(missile!=NULL && missile->is_HitTarget() && attackee!=NULL )
    {
        calculateDamage = true;
        attacker = missile->getAttackAponsor();

        extra_damage += missile->get_AttackAddition_Height(theMap->get_MapHeight(object2->getBlockDR() ,object2->getBlockUR()));
        if(!attackee->isGotAttack())
        {
            if(missile->isAttackerHaveDie())
            {
                double DR,UR;
                missile->get_AttackSponsor_Position(DR , UR);
                attackee->setAvangeObject(DR,UR);
            }
            else
            {
                attackee->setAvangeObject(missile->getAttacker());

                if(missile->getAttacker()->getPlayerRepresent() != 0) missile->getAttacker()->visibleSomeTimes();
            }
        }
    }

    if(calculateDamage)
    {
        bool isDead=attackee->isDie();
        damage = attacker->getATK()-attackee->getDEF(attacker->get_AttackType()) + extra_damage;   //统一伤害计算公式
        if(damage<0) damage = 0;
        attackee->updateBlood(damage);  //damage反映到受攻击者血量减少

        //更新得分
        if(!isDead&&attackee->isDie()&&object2->getPlayerRepresent()==1&&object2->getSort()==SORT_ARMY&&object1->getPlayerRepresent()==0){
            if(object2->getNum()>3){
                usrScore.update(_KILL10);
            }else{
                usrScore.update(_KILL2);
            }
        }
        // if(object2->getSort()==SORT_BUILDING&&attackee->isDie()){
        //     qDebug()<<!isDead<<attackee->isDie()<<object2->getPlayerRepresent()<<object1->getPlayerRepresent();
        // }
        if(!isDead&&attackee->isDie()&&object2->getPlayerRepresent()==0&&object1->getPlayerRepresent()==1){
            if(object2->getSort()==SORT_BUILDING){
                switch(object2->getNum()){
                case BUILDING_HOME:
                case BUILDING_FARM:
                    enemyScore.update(_DESTORY2);
                    break;
                case BUILDING_ARROWTOWER:
                    enemyScore.update(_DESTORY5);
                    break;
                case BUILDING_CENTER:
                    enemyScore.update(_DESTORY10);
                    break;
                default:
                    enemyScore.update(_DESTORY4);
                }
            }else{
                enemyScore.update(_KILL2);
            }
        }

    }
}

void Core_List::object_Gather(Coordinate* object1 , Coordinate* object2)
{
    Farmer* gatherer = (Farmer*) object1;
    Resource* res = NULL;

    StaticRes* s_res=NULL;
    object2->printer_ToResource((void**)&res);

    object2->printer_ToStaticRes((void**)&s_res);
    if(res!= NULL)
    {
        if(!gatherer->isWorking()) gatherer->setPreWork();
        else
        {
            gatherer->adjustAngle(object2->getDR(),object2->getUR());
            if(gatherer->getResourceSort() != res->get_ResourceSort())
            {
                gatherer->set_ResourceSort(res->get_ResourceSort());
                gatherer->update_resourceClear();
            }

            if( res->isFarmerGatherable(gatherer) && gatherer->get_isActionEnd())
            {
                res->updateCnt_byGather(gatherer->get_quantityGather());
                //更新首次收集得分
                if(object2->getSort()==SORT_STATICRES&&s_res->getNum()==0){
                    usrScore.update(_BERRY);
                }else if(object2->getSort()==SORT_ANIMAL){
                    Animal* animal=NULL;
                    object2->printer_ToAnimal((void**)&animal);
                    if(animal->getNum()==ANIMAL_GAZELLE){
                        usrScore.update(_GAZELLE);
                    }else if(animal->getNum()==ANIMAL_ELEPHANT){
                        usrScore.update(_ELEPHANT);
                    }
                }else if(object2->getSort()==SORT_Building_Resource){
                    usrScore.update(_FARM);
                }
                if(res->get_ResourceSort()==HUMAN_WOOD){
                    usrScore.update(_ISWOOD);
                }else if(res->get_ResourceSort()==HUMAN_STONE){
                    usrScore.update(_ISSTONE);
                }
                gatherer->update_addResource();
            }
        }
    }
}

void Core_List::object_ResourceChange( Coordinate* object1, relation_Object& relation)
{
    if(relation.relationAct == CoreEven_Gather && object1->getSort() == SORT_FARMER)
    {
        Farmer* worker = (Farmer*) object1;
        if(relation.alterOb == NULL) worker->setPreStand();
        else
        {
            Farmer* worker = (Farmer*) object1;
            if(relation.alterOb == NULL) worker->setPreStand();
            else
            {
                player[worker->getPlayerRepresent()]->changeResource(worker->getResourceSort() , worker->getResourceNowHave());
                //更新累计收集得分
                switch(worker->getResourceSort()){
                case HUMAN_WOOD:
                    usrScore.update(_WOOD,worker->getResourceNowHave());
                    break;
                case HUMAN_STONE:
                    usrScore.update(_STONE,worker->getResourceNowHave());
                    break;
                case HUMAN_GRANARYFOOD:
                case HUMAN_STOCKFOOD:
                    usrScore.update(_MEAT,worker->getResourceNowHave());
                    break;
                }
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
    Building* buiOb = NULL;
    object1->printer_ToMoveObject((void**)&moOb);
    object1->printer_ToBuilding((void**)&buiOb);
    object1->resetCoreAttribute();
    if(moOb!=NULL) moOb->setPreStand();
    if(buiOb!=NULL) buiOb->init_BuildAttackAct();

    suspendRelation(object1);
}

void Core_List::object_FinishAction(Coordinate* object1)
{
    Missile* misOb = NULL;
    vector<Point> Block_Free;
    switch(relate_AllObject[object1].relationAct){
    case CoreEven_FixBuilding:
        if( relate_AllObject[object1].goalObject!=NULL )
        {
            if(!((Building*)relate_AllObject[object1].goalObject)->isConstructed())
            {
                std::string clickSound;

                player[((Human*)object1)->getPlayerRepresent()]->finishBuild((Building*)relate_AllObject[object1].goalObject);
                if(relate_AllObject[object1].goalObject->getNum() == BUILDING_STOCK || relate_AllObject[object1].goalObject->getNum() == BUILDING_GRANARY)
                    resourceBuildingChange = true;

                clickSound = relate_AllObject[object1].goalObject->getSound_Click();

                if(!clickSound.empty()) //建筑建造完成时，出发一次点击音效
                    soundQueue.push(clickSound);
            }
            relate_AllObject[object1].goalObject->initAction();
            if(relate_AllObject[object1].goalObject->getSort() == SORT_Building_Resource) //是农田
            {
                 object1->initAction();
                 addRelation(object1 , relate_AllObject[object1].goalObject ,CoreEven_Gather);
                 return;
            }
        }
        break;
    case CoreEven_BuildingAct:
        Block_Free = theMap->findBlock_Free(object1);
        player[((Building*)object1)->getPlayerRepresent()]->enforcementAction((Building*)object1,Block_Free);  //进行建筑行动的结果处理
        break;
    case CoreEven_MissileAttack:
        object1->printer_ToMissile((void**)&misOb);
        if(misOb!=NULL) misOb->needDelete();
        break;
    default:
        break;
    }

    object1->initAction();
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

    if(attackee == NULL || !attackee->isGotAttack())
        return;

    if(object->getPlayerRepresent() <MAXPLAYER)
        player[object->getPlayerRepresent()]->beginAttack();

    if(!attackee->isDie())
    {

        //判断攻击者是否为空指针
        attacker=attackee->getAvangeObject();
        if(attacker!=NULL)
        {
            /*call_debugText("red"," "+object->getChineseName()+"(编号:" + QString::number(object->getglobalNum()) + \
                           ")被"+attacker->getChineseName()+"(编号："+QString::number(attacker->getglobalNum())+")攻击",object->getPlayerRepresent());*/

             attackee->updateAvangeObjectPosition();
        }
        //设置攻击者坐标
        attackee->get_AvangeObject_Position(dr,ur);

        //根据受攻击者的类别分别处理
        if( !relate_AllObject[object].isExist || relate_AllObject[object].respondConduct )
        {
            if(object->getSort() == SORT_ANIMAL)
            {
                switch (((Animal*)object)->get_Friendly())
                {
                    case FRIENDLY_FRI:
                        calMirrorPoint(dr,ur,object->getDR(),object->getUR(),3.5*BLOCKSIDELENGTH);
                        suspendRelation(object);
                        if(addRelation(object,dr,ur,CoreEven_JustMoveTo,false) == ACTION_SUCCESS) ((MoveObject*)object)->beginRun();
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
                if(object->getPlayerRepresent() == NOWPLAYERREPRESENT)
                    soundQueue.push("Villager_BeAttacked");

                if(attacker!=NULL && (isObject_Free(object) || relate_AllObject[object].relationAct != CoreEven_Attacking \
                                      && relate_AllObject[object].relationAct != CoreEven_JustMoveTo ))
                {
                    if(attacker->getSort() == SORT_ANIMAL)  addRelation(object,attacker,CoreEven_Attacking);
                    else
                    {
                        //受到敌对ai的攻击，逃命
                        calMirrorPoint(dr,ur,object->getDR(),object->getUR(),3.5*BLOCKSIDELENGTH);
                        suspendRelation(object);
                        addRelation(object,dr,ur,CoreEven_JustMoveTo);
                    }
                }
            }
        }

    }

    attackee->initAvengeObject();
}

void Core_List::manageMontorAct()
{
    //满足添加进入relation的对象表。第一个键值为正在监视的object，第二个键值为离其最近的监视中的object
    map< Coordinate* , Coordinate* >pendingLab;
    Coordinate* ob_m,*ob_ed;
    map< Coordinate* , Coordinate* >::iterator iter , itere;
    double dr,ur;

    int size_ob,size_vision;

    for(int x = 0; x<MAP_L;x++)
        for(int y = 0 ; y<MAP_U;y++)
        {
            size_ob = theMap->map_Object[x][y].size();
            size_vision = theMap->map_Vision[x][y].size();
            if(!size_vision) continue;
            if(!size_ob) continue;

            for(int i = 0 ; i<size_vision;i++)
            {
                for(int j = 0 ;j<size_ob;j++)
                {
                    ob_m = theMap->map_Vision[x][y][i];
                    ob_ed = theMap->map_Object[x][y][j];
                    if(ob_m == ob_ed) continue;

                    if(ob_m->isMonitorObject(ob_ed))
                    {
                        if(pendingLab[ob_m] == NULL) pendingLab[ob_m] = ob_ed;
                        else if( ob_m->getDis_E_Detail(ob_ed) < ob_m->getDis_E_Detail(pendingLab[ob_m]) )
                            pendingLab[ob_m] = ob_ed;
                    }
                }
            }
        }

    iter = pendingLab.begin();
    itere = pendingLab.end();

    while(iter!=itere)
    {
        ob_m = iter->first;
        ob_ed = iter->second;

        if(ob_ed)
        {
            if(ob_m->getSort() == SORT_ANIMAL && ob_m->getNum() == ANIMAL_GAZELLE)
            {
                dr = ob_ed->getDR();
                ur = ob_ed->getUR();
                calMirrorPoint(dr,ur,ob_m->getDR(),ob_m->getUR(),3.5*BLOCKSIDELENGTH);
                suspendRelation(ob_m);
                if(addRelation(ob_m,dr,ur,CoreEven_JustMoveTo,false) == ACTION_SUCCESS) ((MoveObject*)ob_m)->beginRun();
            }
            else addRelation(ob_m , ob_ed , CoreEven_Attacking);
        }
        iter++;
    }

    return;
}

void Core_List::deal_RangeAttack( Coordinate* attacker , Coordinate* attackee )
{
    int bx,by;
    int size , damage;
    BloodHaver* blooder , *bloodee;
    Coordinate* judOb;
    attacker->printer_ToBloodHaver((void**)&blooder);
    for(int x = -1 ; x<2; x++)
    {
        for(int y = -1; y<2 ; y++)
        {
            bx = x+attacker->getBlockDR();
            by = x+attacker->getBlockUR();
            if(bx < 0 || by < 0 || bx>=MAP_L|| by >= MAP_U) continue;
            size = theMap->map_Object[bx][by].size();
            bloodee = NULL;
            for(int i = 0 ; i<size ; i++)
            {
                judOb = theMap->map_Object[bx][by][i];
                if(judOb == attackee) continue;
                if(judOb->getSort() == SORT_ANIMAL && ((Animal*)judOb)->isTree()) continue;
                if(attacker->getPlayerRepresent() == judOb->getPlayerRepresent()) continue;

                judOb->printer_ToBloodHaver((void**)&bloodee);
                if(bloodee && countdistance(attacker->getDR() , attacker->getUR() , judOb->getDR() , judOb->getUR()) <= blooder->getDis_attack())
                {
                    damage = blooder->getATK() - bloodee->getDEF(blooder->get_AttackType());   //统一伤害计算公式
                    if(damage<0) damage = 0;
                    bloodee->updateBlood(damage);  //damage反映到受攻击者血量减少
                }
            }
        }
    }

}

//**************************************************************
//寻路相关
void Core_List::setPath(MoveObject* moveOb, Coordinate* goalOb, double DR0, double UR0)
{
    //设置起点
    Point start = Point(tranBlockDR(moveOb->getDR()),tranBlockUR(moveOb->getUR()));
    Point destination =  Point(tranBlockDR(DR0),tranBlockUR(UR0));
    Point crashBlockPoint;
    stack<Point> path;

    theMap->loadfindPathMap(moveOb);
    theMap->findPathMap[destination.x][destination.y] = 0;
    if(relate_AllObject[moveOb].crashPointLab.size())   //如果之前经历了碰撞，将碰撞处设为障碍物
    {
        crashBlockPoint = relate_AllObject[moveOb].crashPointLab.top();
        relate_AllObject[moveOb].crashPointLab.pop();
        theMap->findPathMap[crashBlockPoint.x][crashBlockPoint.y] = 1;
    }

    if(moveOb->getSort() == SORT_MISSILE) path.push(destination);
    else path = findPath(theMap->findPathMap , theMap , start , destination , goalOb);

    relate_AllObject[moveOb].nullPath = path.empty();
    moveOb->setPath(path,DR0,UR0);
}

void Core_List::crashHandle(MoveObject* moveOb)
{
    Point PreviousPoint, nextBlockPoint;
    Point probePoint;
    vector<Point> blockLab_canMove;

    //处理碰撞
    moveOb->GoBackLU();
    if(/*relate_AllObject[object].nullPath || */relate_AllObject[moveOb].relationAct == CoreEven_JustMoveTo && moveOb->getPath_size() == 0)
    {

        /*call_debugText("red", moveOb->getChineseName()+ "(编号:" + QString::number(moveOb->getglobalNum()) + "当前位置为 ("+\
                       QString::number(moveOb->getDR()) +"," + QString::number(moveOb->getUR())+") , 目标点 ("+\
                       QString::number(moveOb->getDR0()) + "," + QString::number(moveOb->getUR0()) + ") 附近不可抵达",moveOb->getPlayerRepresent());*/
        relate_AllObject[moveOb].wait(100);

        moveOb->stateCrash=true;
        if(!moveOb->isStand()) moveOb->setPreStand();
    }
    else
    {
        relate_AllObject[moveOb].crash_DealPhase = true;
        nextBlockPoint = moveOb->get_NextBlockPoint();
        PreviousPoint = moveOb->get_PreviousBlock();

        if(moveOb->is_MoveFirstStep())
        {
            probePoint = moveOb->getBlockPosition() - moveOb->getMoveDire();
            if(!theMap->isBarrier(probePoint))
                PreviousPoint = probePoint;
            else
            {
                blockLab_canMove = theMap->findBlock_Free(moveOb,1,false);

                if(blockLab_canMove.size()) PreviousPoint = blockLab_canMove[rand()%blockLab_canMove.size()];
            }
        }
        moveOb->pathOptimize(PreviousPoint);

        //碰撞后，离开碰撞所在格子
        relate_AllObject[moveOb].crashMove_Point = PreviousPoint;
        relate_AllObject[moveOb].crashPointLab.push(nextBlockPoint);
        relate_AllObject[moveOb].crashRepresent = moveOb->getCrashOb()->getPlayerRepresent();
    }
}

void Core_List::work_CrashPhase(MoveObject* moveOb)
{
    if(!(moveOb->get_NextBlockPoint() == relate_AllObject[moveOb].crashMove_Point))
    {
        relate_AllObject[moveOb].crash_DealPhase = false;
        moveOb->stateCrash=true;
        if(moveOb->getPlayerRepresent() == relate_AllObject[moveOb].crashRepresent)
            relate_AllObject[moveOb].wait(rand()%50);
        else relate_AllObject[moveOb].wait(rand()%20);

        if(rand() % 8 > 0) moveOb->setPath(stack<Point>(),moveOb->getDR(),moveOb->getUR());
        if(!moveOb->isStand()) moveOb->setPreStand();
    }
}

stack<Point> Core_List::findPath(const int (&findPathMap)[MAP_L][MAP_U], Map *map, const Point &start, const Point &destination , Coordinate* goalOb)
{
    //8个移动方向 <4为斜线方向，>=4为水平竖直方向
    static Point dire[8] = { Point(1,1) , Point(1,-1) ,Point(-1,-1),Point(-1,1),\
                             Point(1,0),Point(-1,0) , Point(0,1), Point(0,-1)};

    bool goalMap[MAP_L][MAP_U];
    stack<Point> path;
    vector<Point> objectBlock;
    vector<Point>::iterator oBiter,oBitere;
    Point nextPoint, nowPoint;
    pathNode* newPathNode = NULL , *findPathNode = NULL, *nowBestPathNode;
    stack<pathNode*> lab_needDele;
    lessHeap* nodeQue = new lessHeap();
    bool meetGoal = false;

    initMap_HaveJud();
    pathNode::setGoalPoint(destination.x,destination.y);    //设置寻路目标点

    //起始点标记
    haveJud_Map_Move(start);

    memset(goalMap , 0 , sizeof(goalMap));
    //标记终点
    if(goalOb != NULL)
    {
        objectBlock = map->get_ObjectBlock(goalOb);
        oBiter = objectBlock.begin();
        oBitere = objectBlock.end();
        while(oBiter != oBitere)
        {
            goalMap[(*oBiter).x][(*oBiter).y] = true;
            oBiter++;
        }
    }
    goalMap[destination.x][destination.y] = true;

    newPathNode = new pathNode(start);
    lab_needDele.push(newPathNode);
    nodeQue->addNode(newPathNode);

    if( goalMap[start.x][start.y] )
    {
        meetGoal = true;
        findPathNode = newPathNode;
    }

    while( nodeQue->top() && !meetGoal)    //nowBestPathNode记录当前节点
    {
        nowBestPathNode = nodeQue->top()->value;
        nodeQue->pop();
        nowPoint = nowBestPathNode->position;    //当前nowBestPathNode记录点的坐标

        for(int i = 0; i<8 && !meetGoal; i++)
        {
            nextPoint = nowPoint + dire[i];
            if(nextPoint.x<= 0 || nextPoint.y <= 0 || nextPoint.x >= MAP_L || nextPoint.y >= MAP_U) continue;

            //判断格子是否可走、未走过
            //斜线方向需要多判断马脚操作
            if( !(isHaveJud(nextPoint) || findPathMap[nextPoint.x][nextPoint.y] && !goalMap[nextPoint.x][nextPoint.y] ||\
                  i<4 && (findPathMap[nextPoint.x][nowPoint.y]||findPathMap[nowPoint.x][nextPoint.y])) )
            {
                //创建新节点
                newPathNode = new pathNode( nextPoint );
                haveJud_Map_Move(nextPoint);
                lab_needDele.push(newPathNode);

                //为新节点增加前驱点信息,然后添加新节点到小根堆
                nodeQue->addNode(nowBestPathNode->pushNode(newPathNode));

                if( goalMap[nextPoint.x][nextPoint.y] )
                {
                    meetGoal = true;
                    findPathNode = newPathNode;
                }
            }
        }
    }

    if(meetGoal)
    {
        while(findPathNode!=NULL)
        {
            path.push(findPathNode->position);
            findPathNode = findPathNode->preNode;
        }
        //当前所在格子不加入栈
        path.pop();
    }

    //释放动态空间
    while(lab_needDele.size())
    {
        delete lab_needDele.top();
        lab_needDele.pop();
    }
    delete nodeQue;

    return path;
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

    if(playerRepresent<MAXPLAYER)
        return player[playerRepresent]->addMissile(attacker , attackee , theMap->get_MapHeight(attacker->getBlockDR() , attacker->getBlockUR()));
    else return NULL;
}

/**************************************************************/
//建立行动细节的静态表
void Core_List::initDetailList()
{
    int *phaseList;
    conditionF* conditionList;
    list<conditionF>forcedInterrupCondition;
    list<conditionF>overCondition;

    //添加静态表
    //行动：只移动*************************************
    {
        phaseList = new int(CoreDetail_Move);
        conditionList = new ( conditionF )( conditionF(condition_ObjectNearby) );
//        forcedInterrupCondition.push_back(conditionF(condition_UniObjectDie , OPERATECON_OBJECT1));
//        forcedInterrupCondition.push_back(conditionF(condition_UniObjectUnderAttack , OPERATECON_OBJECT1));
        forcedInterrupCondition.push_back(conditionF(condition_UselessAction,OPERATECON_TIMES_USELESSACT_MOVE));

        relation_Event_static[CoreEven_JustMoveTo] = detail_EventPhase( 1 ,  phaseList, conditionList , forcedInterrupCondition );
        relation_Event_static[CoreEven_JustMoveTo].setEnd_Absolute();
        delete phaseList;
        delete conditionList;
        forcedInterrupCondition.clear();
    }


    //行动: 攻击**************************************
    {
        phaseList = new int[2]{ CoreDetail_Move , CoreDetail_Attack };
        conditionList = new conditionF[2]{ conditionF(condition_ObjectNearby , OPERATECON_NEAR_ATTACK_MOVE) ,  conditionF(condition_Object1_AttackingEnd,OPERATECON_NEAR_ATTACK)};
//        forcedInterrupCondition.push_back(conditionF(condition_UniObjectDie,OPERATECON_OBJECT1));
        forcedInterrupCondition.push_back(conditionF(condition_UselessAction,OPERATECON_TIMES_USELESSACT_MOVE));

        relation_Event_static[CoreEven_Attacking] = detail_EventPhase(2 , phaseList , conditionList , forcedInterrupCondition );
        overCondition.push_back(conditionF( condition_UniObjectDie, OPERATECON_OBJECT2 ));
        relation_Event_static[CoreEven_Attacking].setLoop(0,1,overCondition);   //向前跳转使用setLoop

        delete phaseList;
        delete conditionList;
        forcedInterrupCondition.clear();
        overCondition.clear();
    }


    //行动：采集**************************************
    {
        phaseList = new int[13]{/*0判断是否需要攻击*/ CoreDetail_JumpPhase, \
                               /*1前往攻击目标*/CoreDetail_Move ,/*2攻击*/CoreDetail_Attack , /*3*/CoreDetail_JumpPhase ,\
                               /*4前往资源建筑*/CoreDetail_Move ,  /*5资源放置*/CoreDetail_ResourceIn, /*6前往资源*/CoreDetail_Move,\
                               /*7采集*/CoreDetail_Gather, /*资源被采集完毕，需要判断村民携带资源*/  /*8*/CoreDetail_JumpPhase ,\
                               /*9前往资源建筑*/CoreDetail_Move ,  /*10资源放置*/CoreDetail_ResourceIn,/*11前往资源原位置*/CoreDetail_Move ,\
                               /*12*/ CoreDetail_JumpPhase};
        conditionList = new conditionF[13]{ conditionF( condition_AllTrue), \
                /*1*/conditionF(condition_ObjectNearby,OPERATECON_NEAR_ATTACK) ,    /*2*/conditionF(condition_Object1_AttackingEnd,OPERATECON_NEAR_ATTACK),\
                /*3*/conditionF(condition_AllTrue) ,                                /*4*/conditionF(condition_ObjectNearby,OPERATECON_NEARALTER_WORK) , \
                /*5*/conditionF(condition_Object1_EmptyBackpack),                   /*6*/conditionF(condition_ObjectNearby,OPERATECON_NEAR_WORK),\
                /*7*/conditionF(condition_Object1_FullBackpack) , \
                /*8*/conditionF(condition_AllTrue),                                 /*9*/conditionF(condition_ObjectNearby,OPERATECON_NEARALTER_WORK),\
                /*10*/conditionF(condition_Object1_EmptyBackpack),                   /*11*/conditionF(condition_ObjectNearby,OPERATECON_NEAR_WORK),\
                /*12*/conditionF(condition_AllTrue)};

//        forcedInterrupCondition.push_back(conditionF(condition_UniObjectDie , OPERATECON_OBJECT1));
//        forcedInterrupCondition.push_back(conditionF(condition_UniObjectUnderAttack , OPERATECON_OBJECT1));
        forcedInterrupCondition.push_back(conditionF(condition_UselessAction,OPERATECON_TIMES_USELESSACT_MOVE));

        relation_Event_static[CoreEven_Gather] = detail_EventPhase(13 , phaseList, conditionList,forcedInterrupCondition);
        //设置循环，1->2，攻击猎物直至可采集
        overCondition.push_back(conditionF(condition_Object2CanbeGather));
        relation_Event_static[CoreEven_Gather].setLoop(1,2,overCondition);
        overCondition.clear();
        //设置循环，4->7，持续采集，目标无资源不可采集
        overCondition.push_back(conditionF(condition_Object2CanbeGather,OPERATECON_OBJECT2,true));
        relation_Event_static[CoreEven_Gather].setLoop(4,7,overCondition);
        overCondition.clear();

        //设置循环，9->12，用于最后一次返回资源建筑，若身上无资源，则直接停止
        overCondition.push_back(conditionF(condition_Object1_EmptyBackpack));
        relation_Event_static[CoreEven_Gather].setLoop(9,12,overCondition);
        overCondition.clear();

        //行动起始，判断是否可直接采集
        relation_Event_static[CoreEven_Gather].setJump(0,2);
        //猎物可采集后，跳转至前往资源
        relation_Event_static[CoreEven_Gather].setJump(3,6);
        //资源被采集完毕后，若身上无资源，则直接停止
        relation_Event_static[CoreEven_Gather].setJump(8 , 12);

        delete phaseList;
        delete conditionList;
        forcedInterrupCondition.clear();
        overCondition.clear();
    }


    //行动：修建建筑***********************************
    {
        phaseList = new int[2]{CoreDetail_Move , CoreDetail_UpdateRatio };
        conditionList = new conditionF[2]{ conditionF( condition_ObjectNearby , OPERATECON_NEAR_WORK ) , conditionF( condition_UniObjectPercent , OPERATECON_OBJECT2 ) };
        forcedInterrupCondition.push_back(conditionF(condition_UselessAction,OPERATECON_TIMES_USELESSACT_MOVE));

        relation_Event_static[CoreEven_FixBuilding] = detail_EventPhase(2 , phaseList , conditionList,forcedInterrupCondition);

        delete phaseList;
        delete conditionList;
        forcedInterrupCondition.clear();
    }


    //行动：建筑工作***********************************
    {
        phaseList = new int(CoreDetail_UpdateRatio);
        conditionList = new (conditionF)( conditionF(condition_UniObjectPercent , OPERATECON_OBJECT1));

        relation_Event_static[CoreEven_BuildingAct] = detail_EventPhase(1 , phaseList , conditionList,forcedInterrupCondition);

        delete phaseList;
        delete conditionList;
    }

    //行动：飞行物攻击*********************************
    {
        phaseList = new int[2]{ CoreDetail_Move , CoreDetail_Attack };
        conditionList = new conditionF[2]{ conditionF( condition_ObjectNearby, OPERATECON_NEAR_MISSILE ) , conditionF( condition_TimesFalse ) };

        relation_Event_static[CoreEven_MissileAttack] = detail_EventPhase(2 , phaseList , conditionList,forcedInterrupCondition);

        delete phaseList;
        delete conditionList;
    }
}

int STATE_JustMoveTo[1]={HUMAN_STATE_JUSTWALKING};
int STATE_Attacking[2]={HUMAN_STATE_GOTO_ATTACK,HUMAN_STATE_ATTACKING};
int STATE_Gather[13]={/*0判断是否需要攻击*/ HUMAN_STATE_GOTO_OBJECT, \
                      /*1前往攻击目标*/HUMAN_STATE_GOTO_ATTACK ,/*2攻击*/HUMAN_STATE_ATTACKING , /*3*/HUMAN_STATE_GOTO_OBJECT ,\
                      /*4前往资源建筑*/HUMAN_STATE_GOTO_RESOURCE ,  /*5资源放置*/HUMAN_STATE_GOTO_OBJECT, /*6前往资源*/HUMAN_STATE_GOTO_OBJECT,\
                      /*7采集*/HUMAN_STATE_GATHERING, /*资源被采集完毕，需要判断村民携带资源*/  /*8*/HUMAN_STATE_GOTO_OBJECT ,\
                      /*9前往资源建筑*/HUMAN_STATE_GOTO_RESOURCE ,  /*10资源放置*/HUMAN_STATE_GOTO_OBJECT,/*11前往资源原位置*/HUMAN_STATE_JUSTWALKING ,\
                      /*12*/ HUMAN_STATE_JUSTWALKING};
int STATE_Gather_Static[13]={/*0判断是否需要攻击*/ HUMAN_STATE_GOTO_RESOURCE, \
                      /*1前往攻击目标*/HUMAN_STATE_GOTO_RESOURCE ,/*2攻击*/HUMAN_STATE_GATHERING , /*3*/HUMAN_STATE_GOTO_OBJECT ,\
                      /*4前往资源建筑*/HUMAN_STATE_GOTO_RESOURCE ,  /*5资源放置*/HUMAN_STATE_GOTO_OBJECT, /*6前往资源*/HUMAN_STATE_GOTO_OBJECT,\
                      /*7采集*/HUMAN_STATE_GATHERING, /*资源被采集完毕，需要判断村民携带资源*/  /*8*/HUMAN_STATE_GOTO_OBJECT ,\
                      /*9前往资源建筑*/HUMAN_STATE_GOTO_RESOURCE ,  /*10资源放置*/HUMAN_STATE_GOTO_OBJECT,/*11前往资源原位置*/HUMAN_STATE_JUSTWALKING ,\
                      /*12*/ HUMAN_STATE_JUSTWALKING};

int STATE_FixBuilding[2]={HUMAN_STATE_GOTO_OBJECT,HUMAN_STATE_FIXING};
int STATE_CreateBuilding[2]={HUMAN_STATE_GOTO_OBJECT,HUMAN_STATE_BUILDING};

int Core_List::getNowPhaseNum(Coordinate* object)
{
    ///获取当前object的行动阶段，用于将信息传递给AIGame
    relation_Object& thisRelation=relate_AllObject[object];
    MoveObject* move=dynamic_cast<MoveObject*>(object);
    Coordinate* obj=relate_AllObject[object].goalObject;
    if(move->stateCrash){
        return HUMAN_STATE_STOP;
    }
    if(!thisRelation.isExist){
        return HUMAN_STATE_IDLE;
    }
    int& nowPhaseNum = thisRelation.nowPhaseNum;

    if(thisRelation.relationAct==CoreEven_JustMoveTo||obj==NULL){
        return HUMAN_STATE_JUSTWALKING;
    }else if(thisRelation.relationAct==CoreEven_Attacking){
        return STATE_Attacking[nowPhaseNum];
    }else if(thisRelation.relationAct==CoreEven_Gather){
        if(obj->getSort()==SORT_ANIMAL&&obj->getNum()!=ANIMAL_TREE&&obj->getNum()!=ANIMAL_FOREST){
            //对动物
            Animal* animal=dynamic_cast<Animal*>(obj);
            if(STATE_Gather_Static[nowPhaseNum]==HUMAN_STATE_GATHERING&&animal->getBlood()<=0){
                return HUMAN_STATE_BUTCHERING;
            }else if(STATE_Gather_Static[nowPhaseNum]==HUMAN_STATE_GATHERING&&animal->getBlood()>0){
                return HUMAN_STATE_ATTACKING;
            }
            return STATE_Gather[nowPhaseNum];
        }else if(obj->getSort()==SORT_ANIMAL&&(obj->getNum()!=ANIMAL_TREE||obj->getNum()!=ANIMAL_FOREST)){
            //对树
            if(STATE_Gather_Static[nowPhaseNum]==HUMAN_STATE_GATHERING){
                return HUMAN_STATE_CUTTING;
            }
            return STATE_Gather_Static[nowPhaseNum];

        }else if(obj->getSort()==SORT_STATICRES&&obj->getNum()==0){
            //对浆果
            return STATE_Gather_Static[nowPhaseNum];
        }else if(obj->getSort()==SORT_STATICRES&&obj->getNum()==1){
            //对石头
            if(STATE_Gather_Static[nowPhaseNum]==HUMAN_STATE_GATHERING){
                return HUMAN_STATE_DIGGING_STONE;
            }
            return STATE_Gather_Static[nowPhaseNum];
        }else if(obj->getSort()==SORT_STATICRES&&obj->getNum()==2){
            //对金子
            if(STATE_Gather_Static[nowPhaseNum]==HUMAN_STATE_GATHERING){
                return HUMAN_STATE_DIGGING_GOLD;
            }
            return STATE_Gather_Static[nowPhaseNum];
        }
    }else if(thisRelation.relationAct==CoreEven_FixBuilding){
        Building* building=dynamic_cast<Building*>(obj);
        if(building->getPercent()<100){
            return STATE_CreateBuilding[nowPhaseNum];
        }else{
            return STATE_FixBuilding[nowPhaseNum];
        }
    }

    return -1;
}
