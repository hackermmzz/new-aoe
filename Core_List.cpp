#include "Core_list.h"
//int timerStand = 0;
Core_List::Core_List(Map* theMap, Player* player[])
{
    this->theMap = theMap;
    this->player = player;
    initDetailList();
}


/** ********
 * 函数：Core_List::update
 * 参数：无；
 * 内容：动态关系表更新，并更新相关辅助数据；
 * 返回值：空。
 */
void Core_List::update()
{
    //对正在监视的Object，进行行动处理
    manageMontorAct();

    jud_resetResBuild();

    //    QElapsedTimer timer4;
    //    timer4.start();

    manageRelationList();
    //    //若想知道微秒级的运行时间
    //    qint64 elapsedNanoseconds = timer4.nsecsElapsed();
    //    timerStand = elapsedNanoseconds/1000;
    //    qDebug() <<g_frame<< "manageRelationList:" << elapsedNanoseconds/1000<<"(ns)";


    init_resetResBuild();
}


//****************************************************************************************
//控制动态表
/** ********
 * 函数：Core_List::addRelation
 * 参数：object1: 行动的主体
 *      object2: 行动的目标
 *      envenType:  行动的类型，见常量表 “关系事件名称”
 *      respond: true表示，本addRelation添加的行动不会被后续addRelation覆盖，除非在调用addRelation前调用suspendRelation取消object1的行动
 *
 * 内容：添加有两个对象（一个主体，一个目标）的行动到动态关系表中，并返回该操作的结果（成功或相关错误）
 *      对人，命令不必手动中止，直接变更；当建筑正进行升级、造兵等行动，命令变更将被无视，必须手动中止当前命令后再下达新命令。
 * 返回值：动作返回编号及action错误码，详见常量表
 */
int Core_List::addRelation(Coordinate* object1, Coordinate* object2, int eventType, bool respond)
{
    if (object1 == NULL) return ACTION_INVALID_NULLWORKER;

    if (relate_AllObject[object1].isExist && relate_AllObject[object1].respondConduct) suspendRelation(object1);

    if (!relate_AllObject[object1].isExist)
    {
        BloodHaver* bloodOb = NULL;
        Building* buildOb = NULL;
        Building* buildGoalOb = NULL;
        BloodHaver* bloodGoalOb = NULL;
        MoveObject* moveOb = NULL;
        object1->printer_ToBloodHaver((void**)&bloodOb);
        object1->printer_ToBuilding((void**)&buildOb);
        object1->printer_ToMoveObject((void**)&moveOb);
        object2->printer_ToBuilding((void**)&buildGoalOb);
        object2->printer_ToBloodHaver((void**)&bloodGoalOb);
        if (bloodOb != NULL && bloodOb->isDie()) return ACTION_INVALID_SN;
        if (buildOb != NULL && !buildOb->isConstructed()) return ACTION_INVALID_BUILDACT_NEEDBUILT;

        if (eventType == CoreEven_FixBuilding && bloodGoalOb != NULL && bloodGoalOb->isFullHp()) //建筑不需要修理
            return ACTION_INVALID_HUMANACTION_BUILDNOTNEEDFIX;

        if (eventType == CoreEven_Gather && object1->getSort() == SORT_FARMER && object2->getSort() == SORT_BUILDING\
            && buildGoalOb != NULL && !buildGoalOb->isMatchResourceType(((Farmer*)object1)->getResourceSort()))
            return ACTION_INVALID_HUMANACTION_BUILD2RESOURCENOMATCH;
        if (eventType == CoreEven_Transport) {//运输船运输人必须保持距离合适
            Farmer* f0 = (Farmer*)object2;
            Human* f1 = (Human*)object1;
            //如果满载,返回错误码
            if (f0->getResourceNowHave() >= 5)return ACTION_INVALID_FULLY_LOAD;
            //如果两个相距太远，返回错误码
            /*
            double dr0=f0->getDR(),ur0=f0->getUR(),dr1=f1->getDR(),ur1=f1->getUR();
            double dr=dr1-dr0,ur=ur1-ur0;
            if(dr*dr+ur*ur>=SHIP_ACT_MAX_DISTANCE){
                return ACTION_INVALID_DISTANCE_FAR;
            }
            */
        }
        //为工作者设置交互对象类别属性，主要用于farmer的status判断/Attack...
        bool isSameReprensent;
        if (object1->isPlayerControl() && object2->isPlayerControl())
            isSameReprensent = object1->getPlayerRepresent() == object2->getPlayerRepresent();
        else
            isSameReprensent = false;

        if (buildGoalOb != NULL)
            object1->set_interAct(object2->getSort(), object2->getNum(), isSameReprensent, buildGoalOb->isConstructed());
        else
            object1->set_interAct(object2->getSort(), object2->getNum(), isSameReprensent);

        //加入行动交互表
        if (eventType == CoreEven_Gather && object2->getSort() == SORT_BUILDING)
        {
            relate_AllObject[object1] = relation_Object(NULL, eventType);

            relate_AllObject[object1].set_goalPoint(object1->getDR(), object1->getUR());
            relate_AllObject[object1].distance_AllowWork = 1e6;
            relate_AllObject[object1].alterOb = object2;
            relate_AllObject[object1].update_Attrib_alter();
            relate_AllObject[object1].distance_Record = 0;
        }
        else {
            relate_AllObject[object1] = relation_Object(object2, eventType);
        }

        relate_AllObject[object1].respondConduct = respond; //是否可由addrelation更改行动

        //根据行动类型，设置需要的额外属性
        switch (eventType)
        {
        case CoreEven_Gather:
            relate_AllObject[object1].set_ResourceBuildingType();
        case CoreEven_Attacking:
            relate_AllObject[object1].init_AttackAb(object1);
            if (moveOb != NULL) moveOb->beginRun();
            break;
        default:
            break;
        }
        requestSound_Action(object1, eventType, object2);
        return ACTION_SUCCESS;
    }
    return ACTION_INVALID_ISNTFREE;
}

/** ********
 * 函数：Core_List::addRelation
 * 参数：object1: 行动的主体
 *      DR UR: 目标位置的细节坐标
 *      envenType:  行动的类型，见常量表 “关系事件名称”，该函数保证eventType为CoreEven_JustMoveTo
 *      respond: true表示，本addRelation添加的行动不会被后续addRelation覆盖，除非在调用addRelation前调用suspendRelation取消object1的行动
 *
 * 内容：添加moveobject对象进行移动的行动到动态关系表中，并返回该操作的结果（成功或相关错误）
 * 返回值：动作返回编号及action错误码，详见常量表
 */
int Core_List::addRelation(Coordinate* object1, double DR, double UR, int eventType, bool respond)
{
    if (object1 == NULL) return ACTION_INVALID_NULLWORKER;

    if (relate_AllObject[object1].isExist && relate_AllObject[object1].respondConduct) suspendRelation(object1);

    if (!relate_AllObject[object1].isExist)
    {
        BloodHaver* bloodOb = NULL;
        object1->printer_ToBloodHaver((void**)&bloodOb);
        if (bloodOb != NULL && bloodOb->isDie()) return ACTION_INVALID_SN;

        if (eventType == CoreEven_JustMoveTo)
        {
            if (DR < 0) DR = 0;
            if (UR < 0) UR = 0;
            if (DR > MAP_L * BLOCKSIDELENGTH) DR = MAP_L * BLOCKSIDELENGTH;
            if (UR > MAP_U * BLOCKSIDELENGTH) UR = MAP_U * BLOCKSIDELENGTH;
            //如果是运输船,并且指定的位置是岸边,那么执行卸货动作(就算木有人也执行卸货,反正就多一个细节罢了)
            if (object1->getSort() == SORT_FARMER && ((Farmer*)object1)->get_farmerType() == FARMERTYPE_WOOD_BOAT) {
                Farmer* obj = (Farmer*)object1;
                {
                    int blockDR = DR / BLOCKSIDELENGTH;
                    int blockUR = UR / BLOCKSIDELENGTH;
                    //如果是岸边,执行卸货的行动
                    if (blockDR < MAP_L && blockUR < MAP_U && theMap->cell[blockDR][blockUR].getMapType() != MAPTYPE_OCEAN) {
                        //寻找直线上的岸边,作为目的地(这个功能先不实现，因为有bug)
                        /*
                        Point target=GetCoastInLine({blockDR,blockUR},{object1->getBlockDR(),object1->getBlockUR()});
                        DR=(target.x+0.5)*BLOCKSIDELENGTH;
                        UR=(target.y+0.5)*BLOCKSIDELENGTH;
                        */
                        //
                        eventType = CoreEven_UnLoad;
                    }
                }
            }
            //
            relate_AllObject[object1] = relation_Object(DR, UR, eventType);
            relate_AllObject[object1].respondConduct = respond;
            relate_AllObject[object1].sort = SORT_COORDINATE;

            requestSound_Action(object1, CoreEven_JustMoveTo);
            return ACTION_SUCCESS;
        }
    }

    return ACTION_INVALID_ISNTFREE;
}

/** ********
 * 函数：Core_List::addRelation
 * 参数：object1: 行动的主体
 *      BlockDR BlockUR: 目标位置的块坐标
 *      envenType:  行动的类型，见常量表 “关系事件名称”，该函数保证eventType为CoreEven_CreatBuilding
 *      respond: true表示，本addRelation添加的行动不会被后续addRelation覆盖，除非在调用addRelation前调用suspendRelation取消object1的行动
 *
 * 内容：进行建造建筑地基，如果建造地基成功，则调用addrelation重载添加object对地基对象进行修建的行动到动态关系表中，并返回该操作的结果（成功或相关错误）
 * 返回值：动作返回编号及action错误码，详见常量表
 */
int Core_List::addRelation(Coordinate* object1, int BlockDR, int BlockUR, int eventType, bool respond, int type)
{
    //必须是农民才能建造
    if (object1 == NULL) return ACTION_INVALID_NULLWORKER;
    if (object1->getSort() != SORT_FARMER)return ACTION_INVALID_SN;
    if (((Farmer*)object1)->get_farmerType() != FARMERTYPE_FARMER)return ACTION_INVALID_SN;
    //
    if (relate_AllObject[object1].isExist && relate_AllObject[object1].respondConduct) suspendRelation(object1);

    if (!relate_AllObject[object1].isExist)
    {
        BloodHaver* bloodOb = NULL;
        Building* buildOb = NULL;
        QString chineseName;
        object1->printer_ToBloodHaver((void**)&bloodOb);
        if (bloodOb != NULL && bloodOb->isDie()) return ACTION_INVALID_SN;

        //判断行动为CoreEven_CreatBuilding，紧接着判断地图上建筑范围内是否有障碍物，最后判断player是否有足够资源并进行资源扣除
        if (eventType == CoreEven_CreatBuilding)
        {
            int playerRepresent = object1->getPlayerRepresent();
            int wrongCode = is_BuildingCanBuild(type, BlockDR, BlockUR, playerRepresent, chineseName);

            if (wrongCode < 0) return wrongCode;

            //判断是否已解锁建筑
            if (!player[playerRepresent]->get_isBuildingShowAble(type))
            {
                call_debugText("red", " 在(" + QString::number(BlockDR) + "," + QString::number(BlockUR) + ")建造" + chineseName + " 建造失败,该建筑尚未解锁", object1->getPlayerRepresent());
                return ACTION_INVALID_HUMANBUILD_LOCK;
            }


            if (!player[playerRepresent]->get_isBuildingAble(type))
            {
                call_debugText("red", " 在(" + QString::number(BlockDR) + "," + QString::number(BlockUR) + ")建造" + chineseName + " 建造失败,当前资源不足", object1->getPlayerRepresent());
                return ACTION_INVALID_RESOURCE;
            }


            player[playerRepresent]->changeResource_byBuild(type);

            buildOb = player[playerRepresent]->addBuilding(type, BlockDR, BlockUR);

            theMap->add_Map_Object(buildOb);    //防止同一帧内建筑重叠
            return addRelation(object1, buildOb, CoreEven_FixBuilding, respond);
        }
    }

    return ACTION_INVALID_ISNTFREE;
}

/** ********
 * 函数：Core_List::addRelation
 * 参数：object1: 行动的主体，该重载保证object1为Building或其派生类的指针
 *      envenType:  行动的类型，见常量表 “关系事件名称”
 *      actNum: 建筑行动的编号，见常量表 “建筑动作”
 *
 * 内容：添加建筑的行动至关系表中，并返回该操作的结果（成功或相关错误）
 * 返回值：动作返回编号及action错误码，详见常量表
 */
int Core_List::addRelation(Coordinate* object1, int evenType, int actNum)
{
    if (object1 == NULL) return ACTION_INVALID_NULLWORKER;

    if (object1->getSort() == SORT_BUILDING && !relate_AllObject[object1].isExist)
    {
        BloodHaver* bloodOb = NULL;
        Building* buildOb = NULL;
        int oper = 0;
        object1->printer_ToBuilding((void**)&buildOb);
        object1->printer_ToBloodHaver((void**)&bloodOb);
        if (!buildOb || !bloodOb || bloodOb->isDie()) return ACTION_INVALID_SN;

        if (!(buildOb->isConstructed() && buildOb->isFinish())) return ACTION_INVALID_BUILDACT_NEEDBUILT;

        if (!player[buildOb->getPlayerRepresent()]->get_isBuildActionShowAble(buildOb->getNum(), actNum))
            return ACTION_INVALID_BUILDACT_LOCK;

        //判断行动是否可进行，是否可进行主要受资源数量限制，对于产人行动，还受当前人口数量限制。
        if (!player[buildOb->getPlayerRepresent()]->get_isBuildActionAble(buildOb, actNum, &oper))
        {
            /** oper == 1 说明是由于人口上限
             *  oper == 0 说明是由于资源数量
            */
            if (oper == 1) return ACTION_INVALID_BUILDACT_MAXHUMAN;
            else if (oper == 0) return ACTION_INVALID_RESOURCE;
        }

        player[buildOb->getPlayerRepresent()]->changeResource_byBuildAction(buildOb, actNum);
        buildOb->setAction(actNum);
        relate_AllObject[object1] = relation_Object(evenType);
        return ACTION_SUCCESS;
    }
    return ACTION_INVALID_ISNTFREE;
}

void Core_List::suspendRelation(Coordinate* object)
{
    if (relate_AllObject[object].isExist)
    {
        Building* buildOb = NULL;
        MoveObject* moveOb = NULL;
        object->printer_ToBuilding((void**)&buildOb);
        object->printer_ToMoveObject((void**)&moveOb);
        if (buildOb != NULL) player[buildOb->getPlayerRepresent()]->back_Resource_TS(buildOb); //返还资源
        if (moveOb != NULL)   //清空路径
        {
            moveOb->setPreStand();
            moveOb->setPath(stack<Point>(), object->getDR(), object->getUR());
        }
        object->initAction();  //行动全部重置

        relate_AllObject[object].isExist = false;
    }
}

void Core_List::manageRelationList()
{

    Coordinate* object1;
    Coordinate* object2;

    map<Coordinate*, relation_Object>::iterator iter = relate_AllObject.begin();

    while (iter != relate_AllObject.end())
    {
        object1 = iter->first;
        relation_Object& thisRelation = iter->second;
        if (thisRelation.isExist)
        {
            //更新object1对应表内信息
            manageRelation_updateMassage(object1);
            int& nowPhaseNum = thisRelation.nowPhaseNum;
            int exePhaseNum = nowPhaseNum;
            detail_EventPhase& thisDetailEven = relation_Event_static[thisRelation.relationAct];
            object2 = thisRelation.goalObject;
            conditionF* recordCondition;
            //该部分为强制中止行动部分代码
            list<conditionF>& forcedInterrupCondition = thisDetailEven.forcedInterrupCondition;
            list<conditionF>::iterator iter_list = forcedInterrupCondition.begin(), iter_liste = forcedInterrupCondition.end();
            while (iter_list != iter_liste)
            {
                if (iter_list->condition(object1, thisRelation, iter_list->variableArgu, iter_list->isNegation))
                {
                    thisRelation.nowPhaseNum = thisDetailEven.phaseInterrup;
                    break;
                }
                iter_list++;
            }
            //判断是否在loop中，是否需要中止
            if (thisDetailEven.isLoop(nowPhaseNum))
            {
                list<conditionF>& overCondition = thisDetailEven.loopOverCondition[nowPhaseNum];
                list<conditionF>::iterator iterOver = overCondition.begin(), iterOvere = overCondition.end();
                while (iterOver != iterOvere)
                {
                    if (iterOver->condition(object1, thisRelation, iterOver->variableArgu, iterOver->isNegation))
                    {
                        nowPhaseNum++;
                        break;
                    }
                    iterOver++;
                }
            }

            recordCondition = &thisDetailEven.chageCondition[nowPhaseNum];
            //判断是否需要切换当前阶段
            if (nowPhaseNum < thisDetailEven.phaseAmount && recordCondition->condition(object1, thisRelation, recordCondition->variableArgu, recordCondition->isNegation))
                nowPhaseNum = thisDetailEven.changeLinkedList[nowPhaseNum];
            //
            //实际执行行动
            if (nowPhaseNum == exePhaseNum)  //nowphase变化后，防止提前行动。变化后的行动在下一帧开始做
            {
                switch (thisDetailEven.phaseList[nowPhaseNum])
                {
                case CoreDetail_Unload:
                    object_Unload(object1, object2);
                    break;
                case CoreDetail_Transport:
                    object_Transport(object1, object2);
                    break;
                case CoreDetail_Move:
                    if (thisRelation.isUseAlterGoal)object_Move(object1, thisRelation.DR_alter, thisRelation.UR_alter);
                    else object_Move(object1, thisRelation.DR_goal, thisRelation.UR_goal);
                    break;
                case CoreDetail_Attack:
                    object_Attack(object1, object2);
                    break;
                case CoreDetail_Gather:
                    object_Gather(object1, object2);
                    break;
                case CoreDetail_ResourceIn:
                    object_ResourceChange(object1, thisRelation);
                    break;
                case CoreDetail_UpdateRatio:
                    object_RatioChange(object1, thisRelation);
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
                MoveObject* moveOb = NULL;
                switch (thisDetailEven.phaseList[nowPhaseNum])
                {
                case CoreDetail_Move:
                    object1->printer_ToMoveObject((void**)&moveOb);
                    if (moveOb != NULL)
                        moveOb->setPath(stack<Point>(), object1->getDR(), object1->getUR());
                    break;
                case CoreDetail_Attack:
                    if (thisRelation.relationAct == CoreEven_MissileAttack) thisRelation.set_ExecutionTime(1);
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

void Core_List::manageRelation_deleteGoalOb(Coordinate* goalObject)
{
    map<Coordinate*, relation_Object>::iterator iterNow = relate_AllObject.begin(), iterEnd = relate_AllObject.end();

    //遍历整个动态表，删除goalObject
    while (iterNow != iterEnd)
    {
        bool flag = 0;
        if (iterNow->second.goalObject == goalObject)
        {
            flag = 1;
            iterNow->second.update_GoalPoint();
            iterNow->second.goalObject = NULL;
        }

        if (iterNow->second.alterOb == goalObject)
        {
            flag = 1;
            iterNow->second.update_Attrib_alter();
            iterNow->second.init_AlterOb();
        }
        //////把这个关系表直接取消
        if (flag)
        {
            Coordinate* coord = iterNow->first;
            //对于飞行物类别,不需要这样,因为他要射到目的地才行
            void* missile = 0;
            coord->printer_ToMissile((void**)(&missile));
            if (missile)continue;
            iterNow->second.isExist = false;
            //对于Human类对象,需要对其路径重置
            Human* obj = 0;
            coord->printer_ToHuman((void**)(&obj));
            if (obj) {
                obj->setPosForced(obj->getDR(), obj->getUR());
                obj->setPreStand();
                obj->setNowState(MOVEOBJECT_STATE_STAND);
                obj->setTransported(0);
            }
        }
        //////
        iterNow++;
    }
}

//实时更新关系表内部信息
void Core_List::manageRelation_updateMassage(Coordinate* object1)
{
    relation_Object& relation = relate_AllObject[object1];

    if (relation.alterOb == NULL || needReset_resBuild)
    {
        if (relation.needResourceBuilding) findResourceBuiding(relation, player[object1->getPlayerRepresent()]->build, object1);
    }

    relation.reset_Object1Predicted(object1);
    relation.height_Object = theMap->get_MapHeight(object1->getBlockDR(), object1->getBlockUR());
    relation.update_GoalPoint();
    if (relation.goalObject != NULL)
        relation.height_GoalObject = theMap->get_MapHeight(relation.goalObject->getBlockDR(), relation.goalObject->getBlockUR());
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
void Core_List::findResourceBuiding(relation_Object& relation, list<Building*>& building, Coordinate* obj1)
{
    int type = relation.resourceBuildingType;
    double dis, dis_opti = relation.distance_Record, dr = relation.DR_goal, ur = relation.UR_goal;
    Building* judeBuild, * optimum = NULL;

    list<Building*>::iterator iterNow = building.begin(), iterEnd = building.end();
    //如果是渔船，只能选择船坞
    bool flag = !(obj1 && obj1->getSort() == SORT_FARMER && ((Farmer*)obj1)->get_farmerType() == FARMERTYPE_SAILING);
    //
    while (iterNow != iterEnd)
    {
        judeBuild = *iterNow;
        if (judeBuild->isFinish() && ((judeBuild->getNum() == BUILDING_CENTER && flag) || judeBuild->getNum() == type))
        {
            dis = calculateManhattanDistance(dr, ur, judeBuild->getDR(), judeBuild->getUR());
            if (dis < dis_opti)
            {
                optimum = judeBuild;
                dis_opti = dis;
            }
        }
        iterNow++;
    }
    if (optimum != NULL) relation.set_AlterOb(optimum, dis_opti);
}

bool Core_List::canBuildDock(int BlockDR, int BlockUR, int w)
{
    //只能建在海洋里面
    for (int i = 0;i < w;++i) {
        for (int j = 0;j < w;++j) {
            int ii = i + BlockDR, jj = j + BlockUR;
            if (ii < 0 || ii >= MAP_L || jj < 0 || jj >= MAP_U)return 0;
            if (theMap->cell[ii][jj].getMapType() != MAPTYPE_OCEAN)return 0;
        }
    }
    return 1;
}

//判断是否可以建造建筑
int Core_List::is_BuildingCanBuild(int buildtype, int BlockDR, int BlockUR, int playerID, QString& chineseName)
{
    int DRL = -1, DRR = -1, URD = -1, URU = -1;    //记录边界
    int blockSideLength;
    Building* tempBuild = NULL;

    //预创建
    if (buildtype == BUILDING_FARM)
    {
        Building_Resource* tempBuild_resource = new Building_Resource(buildtype, BlockDR, BlockUR);
        tempBuild = tempBuild_resource;
    }
    else tempBuild = new Building(buildtype, BlockDR, BlockUR);

    blockSideLength = tempBuild->get_BlockSizeLen();
    chineseName = tempBuild->getChineseName();

    delete tempBuild;

    DRL = BlockDR;
    DRR = BlockDR + blockSideLength;
    URD = BlockUR;
    URU = BlockUR + blockSideLength;

    if (DRL<0 || DRR>MAP_L || URD<0 || URU>MAP_U)
    {
        call_debugText("red", " 在(" + QString::number(BlockDR) + "," + QString::number(BlockUR) + ")建造" + chineseName + " 建造失败,选中位置越界", playerID);
        return ACTION_INVALID_HUMANBUILD_OVERBORDER;
    }

    if (!theMap->cell[DRL][URD].Explored)
    {
        call_debugText("red", " 在(" + QString::number(BlockDR) + "," + QString::number(BlockUR) + ")建造" + chineseName + " 建造失败,选中位置未被探索", playerID);
        return  ACTION_INVALID_HUMANBUILD_UNEXPLORE;
    }

    //如果以上限制均未触发，判断是否有重叠
    int bDR_ba, bUR_ba;
    if (theMap->isHaveObject(BlockDR, BlockUR, bDR_ba, bUR_ba, blockSideLength))
    {
        call_debugText("red", " 在(" + QString::number(BlockDR) + "," + QString::number(BlockUR) + ")建造" + chineseName + " 建造失败:放置位置非空地，在(" + QString::number(bDR_ba) + "," + QString::number(bUR_ba) + ")处与其他物体重叠", playerID);
        return ACTION_INVALID_HUMANBUILD_OVERLAP;
    }

    if (buildtype != BUILDING_DOCK && !theMap->isFlat(BlockDR, BlockUR, blockSideLength))
    {
        call_debugText("red", " 在(" + QString::number(BlockDR) + "," + QString::number(BlockUR) + ")建造" + chineseName + " 建造失败:放置位置存在高度差或斜坡", playerID);
        return ACTION_INVALID_HUMANBUILD_DIFFERENTHIGH;
    }
    //判断是否建立在合适的位置
    {
        if (buildtype == BUILDING_DOCK) {
            if (!canBuildDock(BlockDR, BlockUR, blockSideLength))
                return ACTION_INVALID_POSITION_NOT_FIT;
        }
        //其余的建筑必须建在陆地
        else {
            for (int i = 0;i < blockSideLength;++i) {
                for (int j = 0;j < blockSideLength;++j) {
                    int l = i + BlockDR, u = j + BlockUR;
                    if (theMap->cell[l][u].getMapType() == MAPTYPE_OCEAN)return ACTION_INVALID_POSITION_NOT_FIT;
                }
            }
        }

    }
    return ACTION_SUCCESS;
}

int Core_List::getObjectSN(Coordinate* object)
{
    relation_Object& thisRelation = relate_AllObject[object];

    if (thisRelation.isExist && thisRelation.goalObject != nullptr)
        return thisRelation.goalObject->getglobalNum();

    return -1;
}

//请求发出音效
void Core_List::requestSound_Action(Coordinate* object, int actionType, Coordinate* goalObject)
{
    if (object->getPlayerRepresent() != NOWPLAYERREPRESENT) return;

    Animal* aniOb = NULL;
    StaticRes* staticOb = NULL;

    if (object->getSort() == SORT_FARMER)
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

            if (aniOb != NULL)
            {
                if (aniOb->isTree()) soundQueue.push("Villager_Lumber");
                else soundQueue.push("Villager_Hunter");
            }
            else if (staticOb != NULL)
            {
                if (staticOb->getNum() == NUM_STATICRES_Bush)
                    soundQueue.push("Villager_Gather");
                else
                    soundQueue.push("Villager_Miner");
            }
            else if (goalObject->getSort() == SORT_Building_Resource)
                soundQueue.push("Villager_Gather");

            break;
        default:
            break;
        }
    }
    else if (object->getSort() == SORT_ARMY)
    {
        soundQueue.push("Army_Walk");
    }
}


//****************************************************************************************
//通用的控制对象行动函数
void Core_List::object_Move(Coordinate* object, double DR, double UR)
{
    MoveObject* moveObject = NULL;
    Coordinate* goalOb;
    object->printer_ToMoveObject((void**)&moveObject);
    //如果有目标点，设置目标点
    if (moveObject) moveObject->stateCrash = false;//重置碰撞状态（tagGame）

    if (relate_AllObject[object].isUseAlterGoal)
        goalOb = relate_AllObject[object].alterOb;
    else
        goalOb = relate_AllObject[object].goalObject;

    if (moveObject)
    {
        if (relate_AllObject[object].isWaiting())
            relate_AllObject[object].useless();
        else
        {
            double DR0, UR0, thisDR, thisUR;
            DR0 = moveObject->getDR0();
            UR0 = moveObject->getUR0();
            thisDR = moveObject->getDR();
            thisUR = moveObject->getUR();

            if (thisDR == DR0 && thisUR == UR0 && (DR0 != DR || UR0 != UR) || countdistance(DR0, UR0, DR, UR) > 1e7)
            {
                //为moveObject设置路径
                setPath(moveObject, goalOb, DR, UR);
            }
            else
            {
                if (!moveObject->isWalking()) moveObject->setPreWalk();
                else if (moveObject->getCrashOb() != NULL)
                {
                    //处理碰撞
                    crashHandle(moveObject);
                    relate_AllObject[object].useless();
                }
                else if (relate_AllObject[object].crash_DealPhase)
                {
                    //碰撞后的调整
                    work_CrashPhase(moveObject);
                    relate_AllObject[object].useless();
                }
                else relate_AllObject[object].useOnce();
            }
        }

        moveObject->initCrash();
    }
    else
    {
        relate_AllObject[object].useless();
        if (moveObject) moveObject->stateCrash = true;
    }

}

void Core_List::object_Attack(Coordinate* object1, Coordinate* object2)
{
    bool calculateDamage = false;
    int damage; //记录伤害
    int extra_damage = 0;
    BloodHaver* attacker = NULL;    //攻击者
    BloodHaver* attackee = NULL;    //受攻击者
    Animal* animalOb = NULL;
    MoveObject* moveOb = NULL;
    Missile* missile = NULL;

    object1->printer_ToBloodHaver((void**)&attacker);   //攻击者指针赋值(object1强制转换)
    if (object2) object2->printer_ToBloodHaver((void**)&attackee);   //受攻击者指针赋值(object2强制转换)
    object1->printer_ToMissile((void**)&missile);   //判断obect1是否为投射物

    if (attackee != NULL && attacker != NULL && attacker->canAttack())  //若指针均非空
    {
        if (!attacker->isAttacking())
        {
            object2->printer_ToAnimal((void**)&animalOb);
            if (animalOb == NULL || !animalOb->isTree())
            {
                call_debugText("red", " " + object1->getChineseName() + "(编号:" + QString::number(object1->getglobalNum()) + \
                    ")开始向" + object2->getChineseName() + "(编号：" + QString::number(object2->getglobalNum()) + ")攻击", REPRESENT_BOARDCAST_MESSAGE);
            }
            attacker->setPreAttack();
        }
        else
        {
            object1->printer_ToMoveObject((void**)&moveOb);
            if (moveOb != NULL) moveOb->adjustAngle(object2->getDR(), object2->getUR());
            //非祭司,是普通的伤害计算公式
            /** 后续版本若有投石车等喷溅伤害,判断还需细化*/
            if (attacker->is_missileAttack())
            {
                if (attacker->is_missileThrow())
                {
                    addRelation(creatMissile(object1, object2), object2, CoreEven_MissileAttack, false);
                    attacker->haveAttack();
                }
            }
            else if (attacker->is_attackHit())
            {
                calculateDamage = true;
                attacker->haveAttack();
                if (attacker->get_isRangeAttack()) deal_RangeAttack(object1, object2);
                if (!attackee->isGotAttack())
                {
                    attackee->setAvangeObject(object1);
                    if (object1->getPlayerRepresent() != 0) object1->visibleSomeTimes();
                }
            }
        }
    }
    else if (missile != NULL && missile->is_HitTarget() && attackee != NULL)
    {
        calculateDamage = true;
        attacker = missile->getAttackAponsor();

        extra_damage += missile->get_AttackAddition_Height(theMap->get_MapHeight(object2->getBlockDR(), object2->getBlockUR()));
        if (!attackee->isGotAttack())
        {
            if (missile->isAttackerHaveDie())
            {
                double DR, UR;
                missile->get_AttackSponsor_Position(DR, UR);
                attackee->setAvangeObject(DR, UR);
            }
            else
            {
                attackee->setAvangeObject(missile->getAttacker());

                if (missile->getAttacker()->getPlayerRepresent() != 0) missile->getAttacker()->visibleSomeTimes();
            }
        }
    }

    if (calculateDamage)
    {
        bool isDead = attackee->isDie();
        damage = attacker->getATK() - attackee->getDEF(attacker->get_AttackType()) + extra_damage;   //统一伤害计算公式
        if (damage < 0) damage = 0;
        attackee->updateBlood(damage);  //damage反映到受攻击者血量减少

        //更新得分
        if (!isDead && attackee->isDie() && object2->getPlayerRepresent() == 1 && object2->getSort() == SORT_ARMY && object1->getPlayerRepresent() == NOWPLAYERREPRESENT)
        {
            if (object2->getNum() > 3) {
                usrScore.update(_KILL10);
            }
            else {
                usrScore.update(_KILL2);
            }
        }

        if (!isDead && attackee->isDie() && object2->getPlayerRepresent() == 0 && object1->getPlayerRepresent() == 1) {
            if (object2->getSort() == SORT_BUILDING) {
                switch (object2->getNum()) {
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
            }
            else {
                enemyScore.update(_KILL2);
            }
        }

    }
}

void Core_List::object_Gather(Coordinate* object1, Coordinate* object2)
{
    Farmer* gatherer = (Farmer*)object1;
    Resource* res = NULL;

    StaticRes* s_res = NULL;
    object2->printer_ToResource((void**)&res);

    object2->printer_ToStaticRes((void**)&s_res);
    if (res != NULL)
    {
        if (!gatherer->isWorking())
        {
            gatherer->setPreWork();
            gatherer->adjustAngle(object2->getDR(), object2->getUR());
            if (gatherer->getResourceSort() != res->get_ResourceSort())
            {
                gatherer->set_ResourceSort(res->get_ResourceSort());
                gatherer->update_resourceClear();
            }
        }
        else if (res->isFarmerGatherable(gatherer) && gatherer->get_isActionEnd())
        {
            res->updateCnt_byGather(gatherer->get_quantityGather());

            //更新首次收集得分
            if (object2->getSort() == SORT_STATICRES && s_res->getNum() == 0)
            {
                usrScore.update(_BERRY);
            }
            else if (object2->getSort() == SORT_ANIMAL)
            {
                Animal* animal = NULL;
                object2->printer_ToAnimal((void**)&animal);
                if (animal->getNum() == ANIMAL_GAZELLE)
                {
                    usrScore.update(_GAZELLE);
                }
                else if (animal->getNum() == ANIMAL_ELEPHANT)
                {
                    usrScore.update(_ELEPHANT);
                }
            }
            else if (object2->getSort() == SORT_Building_Resource)
            {
                usrScore.update(_FARM);
            }
            if (res->get_ResourceSort() == HUMAN_WOOD)
            {
                usrScore.update(_ISWOOD);
            }
            else if (res->get_ResourceSort() == HUMAN_STONE)
            {
                usrScore.update(_ISSTONE);
            }
            else if (res->get_ResourceSort() == HUMAN_GOLD)
            {
                usrScore.update(_ISGOLD);
            }

            gatherer->update_addResource();
        }
    }
}

void Core_List::object_Transport(Coordinate* object1, Coordinate* object2)
{
    //
    Human* human1 = (Human*)object1;
    Farmer* human2 = (Farmer*)object2;
    if (human2->getResourceNowHave() < 5) {
        human1->setTransported(true);
        human2->update_transportHuman(human1);
        human2->set_ResourceSort(SORT_HUMAN);
    }
    suspendRelation(human1);
}

void Core_List::object_Unload(Coordinate* object1, Coordinate* object2)
{
    Farmer* ship = (Farmer*)object1;
    //寻找没有障碍的陆地
    extern Map* GlobalMap;
    vector<array<double, 2>>satisfy;
    for (int i = -UNLOAD_RADIAN;i <= UNLOAD_RADIAN;++i)
        for (int j = -UNLOAD_RADIAN;j <= UNLOAD_RADIAN;++j)
        {
            int L = i + ship->getBlockDR(), U = j + ship->getBlockUR();
            if (L >= 0 && L < MAP_L && U >= 0 && U < MAP_U) {
                Block& block = GlobalMap->cell[L][U];
                if (block.getMapType() != MAPTYPE_OCEAN && GlobalMap->map_Object[L][U].empty()) {//如果不为海洋，那就是陆地，并且无障碍物
                    satisfy.push_back({ block.getDR(),block.getUR() });
                }
            }
        }
    //随机生成浮点数
    static auto generateRandomDouble = [&](double min, double max) {
        // 生成一个0到RAND_MAX之间的随机整数
        double random = rand() * 1.0 / RAND_MAX;
        // 将随机整数映射到指定的范围
        return min + random * (max - min);
        };
    //
    if (satisfy.size()) {
        auto&& humans = ship->getHumanTransport();
        for (Human* human : humans) {
            int idx = rand() % satisfy.size();
            double targetDr = satisfy[idx][0], targetUr = satisfy[idx][1];
            human->setPosForced(targetDr + generateRandomDouble(1.0, BLOCKSIDELENGTH - 1.0), targetUr + generateRandomDouble(1.0, BLOCKSIDELENGTH - 1.0));
            human->setPreStand();
            human->setNowState(MOVEOBJECT_STATE_STAND);
            human->setTransported(0);
        }
        ship->update_resourceClear();
        humans.clear();
    }
}

void Core_List::object_ResourceChange(Coordinate* object1, relation_Object& relation)
{
    if (relation.relationAct == CoreEven_Gather && object1->getSort() == SORT_FARMER)
    {
        Farmer* worker = (Farmer*)object1;
        if (relation.alterOb == NULL) worker->setPreStand();
        else
        {
            player[worker->getPlayerRepresent()]->changeResource(worker->getResourceSort(), worker->getResourceNowHave());
            //更新累计收集得分
            switch (worker->getResourceSort()) {
            case HUMAN_WOOD:
                usrScore.update(_WOOD, worker->getResourceNowHave());
                break;
            case HUMAN_STONE:
                usrScore.update(_STONE, worker->getResourceNowHave());
                break;
            case HUMAN_GOLD:
                usrScore.update(_GOLD, worker->getResourceNowHave());
                break;
            case HUMAN_DOCKFOOD:
            case HUMAN_GRANARYFOOD:
            case HUMAN_STOCKFOOD:
                usrScore.update(_MEAT, worker->getResourceNowHave());
                break;
            }
            worker->update_resourceClear();
        }
    }
}

void Core_List::object_RatioChange(Coordinate* object1, relation_Object& relation)
{
    Building* buildOb = NULL;
    Building* buildGoalOb = NULL;
    object1->printer_ToBuilding((void**)&buildOb);
    if (relation.goalObject != NULL)
        relation.goalObject->printer_ToBuilding((void**)&buildGoalOb);

    if (relation.relationAct == CoreEven_FixBuilding && buildGoalOb != NULL)
    {
        Farmer* farmer = (Farmer*)object1;
        if (!farmer->isWorking()) farmer->setPreWork();
        buildGoalOb->update_Build();
    }
    else if (relation.relationAct == CoreEven_BuildingAct && buildOb != NULL)
        buildOb->update_Action();
}

void Core_List::object_FinishAction_Absolute(Coordinate* object1)
{
    MoveObject* moOb = NULL;
    Building* buiOb = NULL;
    object1->printer_ToMoveObject((void**)&moOb);
    object1->printer_ToBuilding((void**)&buiOb);
    object1->resetCoreAttribute();
    object1->resetINterAct();
    if (moOb != NULL) moOb->setPreStand();
    if (buiOb != NULL) buiOb->init_BuildAttackAct();

    suspendRelation(object1);
}

void Core_List::object_FinishAction(Coordinate* object1)
{
    Missile* misOb = NULL;
    Building* buildOb = NULL;
    //    Building* buildGoalOb = NULL;
    Building_Resource* buildResGoalOb = NULL;
    vector<pair<Point, int>> Block_Free;
    int actNum = -1;

    switch (relate_AllObject[object1].relationAct) {
    case CoreEven_FixBuilding:
        if (relate_AllObject[object1].goalObject != NULL)
        {
            //            relate_AllObject[object1].goalObject->printer_ToBuilding((void**)&buildGoalOb);
            relate_AllObject[object1].goalObject->printer_ToBuilding_Resource((void**)&buildResGoalOb);

            if (buildResGoalOb != NULL && buildResGoalOb->getNum() == BUILDING_FARM) //是农田
            {
                object1->initAction();
                addRelation(object1, buildResGoalOb, CoreEven_Gather);
                return;
            }
        }
        break;
    case CoreEven_BuildingAct:
        Block_Free = theMap->findBlock_Free(object1);
        object1->printer_ToBuilding((void**)&buildOb);

        if (buildOb != NULL)
            player[object1->getPlayerRepresent()]->enforcementAction(buildOb, Block_Free);  //进行建筑行动的结果处理

        actNum = object1->getActNum();
        if (actNum == BUILDING_CENTER_CREATEFARMER || actNum == BUILDING_ARMYCAMP_CREATE_CLUBMAN
            || actNum == BUILDING_ARMYCAMP_CREATE_SLINGER || actNum == BUILDING_RANGE_CREATE_BOWMAN || actNum == BUILDING_DOCK_CREATE_SAILING)
            usrScore.update(_HUMAN1);
        else if (actNum == BUILDING_STABLE_CREATE_SCOUT || actNum == BUILDING_DOCK_CREATE_SHIP || actNum == BUILDING_DOCK_CREATE_WOOD_BOAT)
            usrScore.update(_HUMAN2);
        else
            usrScore.update(_TECH);

        break;
    case CoreEven_MissileAttack:
        object1->printer_ToMissile((void**)&misOb);
        if (misOb != NULL) misOb->needDelete();
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
    double dr, ur;
    Coordinate* attacker = NULL;
    BloodHaver* attackee = NULL;
    object->printer_ToBloodHaver((void**)&attackee);

    if (attackee == NULL || !attackee->isGotAttack())
        return;

    if (object->getPlayerRepresent() < MAXPLAYER)
        player[object->getPlayerRepresent()]->beginAttack();

    if (!attackee->isDie())
    {

        //判断攻击者是否为空指针
        attacker = attackee->getAvangeObject();
        if (attacker != NULL)
        {
            attackee->updateAvangeObjectPosition();
        }
        //设置攻击者坐标
        attackee->get_AvangeObject_Position(dr, ur);

        //根据受攻击者的类别分别处理
        if (!relate_AllObject[object].isExist || relate_AllObject[object].respondConduct)
        {
            if (object->getSort() == SORT_ANIMAL)
            {
                switch (((Animal*)object)->get_Friendly())
                {
                case FRIENDLY_FRI:
                    calMirrorPoint(dr, ur, object->getDR(), object->getUR(), 3.5 * BLOCKSIDELENGTH);
                    suspendRelation(object);
                    if (addRelation(object, dr, ur, CoreEven_JustMoveTo, false) == ACTION_SUCCESS) ((MoveObject*)object)->beginRun();
                    break;
                case FRIENDLY_ENEMY:
                case FRIENDLY_FENCY:
                    if (attacker != NULL) addRelation(object, attacker, CoreEven_Attacking, false);
                    break;
                default:
                    break;
                }
            }
            else if (object->getSort() == SORT_FARMER)
            {
                if (object->getPlayerRepresent() == NOWPLAYERREPRESENT && object->isInWidget())
                    soundQueue.push("Villager_BeAttacked");

                if (attacker != NULL && (isObject_Free(object) || relate_AllObject[object].relationAct != CoreEven_Attacking \
                    && relate_AllObject[object].relationAct != CoreEven_JustMoveTo))
                {
                    if (attacker->getSort() == SORT_ANIMAL)  addRelation(object, attacker, CoreEven_Attacking);
                    else
                    {
                        //受到敌对ai的攻击，逃命
                        calMirrorPoint(dr, ur, object->getDR(), object->getUR(), 3.5 * BLOCKSIDELENGTH);
                        suspendRelation(object);
                        addRelation(object, dr, ur, CoreEven_JustMoveTo);
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
    map< Coordinate*, Coordinate* >pendingLab;
    Coordinate* ob_m, * ob_ed;
    map< Coordinate*, Coordinate* >::iterator iter, itere;
    double dr, ur;

    int size_ob, size_vision;

    for (int x = 0; x < MAP_L;x++)
        for (int y = 0; y < MAP_U;y++)
        {
            size_ob = theMap->map_Object[x][y].size();
            size_vision = theMap->map_Vision[x][y].size();
            if (!size_vision) continue;
            if (!size_ob) continue;

            for (int i = 0; i < size_vision;i++)
            {
                for (int j = 0;j < size_ob;j++)
                {
                    ob_m = theMap->map_Vision[x][y][i];
                    ob_ed = theMap->map_Object[x][y][j];
                    if (ob_m == ob_ed) continue;

                    if (ob_m->isMonitorObject(ob_ed))
                    {
                        if (pendingLab[ob_m] == NULL) pendingLab[ob_m] = ob_ed;
                        else if (ob_m->getDis_E_Detail(ob_ed) < ob_m->getDis_E_Detail(pendingLab[ob_m]))
                            pendingLab[ob_m] = ob_ed;
                    }
                }
            }
        }

    iter = pendingLab.begin();
    itere = pendingLab.end();

    while (iter != itere)
    {
        ob_m = iter->first;
        ob_ed = iter->second;

        if (ob_ed)
        {
            if (ob_m->getSort() == SORT_ANIMAL && ob_m->getNum() == ANIMAL_GAZELLE)
            {
                static const double dis = 3.5 * BLOCKSIDELENGTH;
                dr = ob_ed->getDR();
                ur = ob_ed->getUR();
                calMirrorPoint(dr, ur, ob_m->getDR(), ob_m->getUR(), dis);
                //对计算出来的位置进行判断，如果是海洋，那么随机选100次点直到满足
                bool flag = 0;
                int loopCnt = 0;
                do {
                    int L = dr / BLOCKSIDELENGTH, U = ur / BLOCKSIDELENGTH;
                    if (L >= 0 && U >= 0 && L < MAP_L && U < MAP_U && theMap->cell[L][U].getMapType() == MAPTYPE_OCEAN) {
                        double dir0 = rand() * 1.0 / RAND_MAX;
                        double dir1 = sqrt(1.0 - dir0 * dir0);
                        dr = ob_m->getDR() + dir0 * dis;
                        ur = ob_m->getUR() + dir1 * dis;
                        flag = 1;
                    }
                    else flag = 0;
                    ++loopCnt;
                } while (flag && loopCnt <= 100);
                //如果找到了合适的点
                if (!flag) {
                    suspendRelation(ob_m);
                    if (addRelation(ob_m, dr, ur, CoreEven_JustMoveTo, false) == ACTION_SUCCESS)
                        ((MoveObject*)ob_m)->beginRun();
                }
            }
            else addRelation(ob_m, ob_ed, CoreEven_Attacking);
        }
        iter++;
    }

    return;
}

void Core_List::deal_RangeAttack(Coordinate* attacker, Coordinate* attackee)
{
    int bx, by;
    int size, damage;
    BloodHaver* blooder, * bloodee;
    Coordinate* judOb;
    attacker->printer_ToBloodHaver((void**)&blooder);
    for (int x = -2; x < 3; x++)
    {
        for (int y = -2; y < 3; y++)
        {
            bx = x + attacker->getBlockDR();
            by = y + attacker->getBlockUR();
            if (bx < 0 || by < 0 || bx >= MAP_L || by >= MAP_U) continue;
            size = theMap->map_Object[bx][by].size();
            bloodee = NULL;
            for (int i = 0; i < size; i++)
            {
                judOb = theMap->map_Object[bx][by][i];
                if (judOb == attackee) continue;
                if (judOb->getSort() == SORT_ANIMAL && ((Animal*)judOb)->isTree()) continue;
                if (attacker->getPlayerRepresent() == judOb->getPlayerRepresent()) continue;

                judOb->printer_ToBloodHaver((void**)&bloodee);
                if (bloodee && countdistance(attacker->getDR(), attacker->getUR(), judOb->getDR(), judOb->getUR()) <= blooder->getDis_attack())
                {
                    damage = blooder->getATK() - bloodee->getDEF(blooder->get_AttackType());   //统一伤害计算公式
                    if (damage < 0) damage = 0;
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
    Point start = Point(tranBlockDR(moveOb->getDR()), tranBlockUR(moveOb->getUR()));
    Point destination = Point(tranBlockDR(DR0), tranBlockUR(UR0));
    Point crashBlockPoint;
    stack<Point> path;

    auto& findPathMap = theMap->loadfindPathMap(moveOb);
    int* pre = 0, preval = 0;//用于恢复寻路模板
    if (relate_AllObject[moveOb].crashPointLab.size())   //如果之前经历了碰撞，将碰撞处设为障碍物
    {
        crashBlockPoint = relate_AllObject[moveOb].crashPointLab.top();
        relate_AllObject[moveOb].crashPointLab.pop();
        pre = &findPathMap[crashBlockPoint.x][crashBlockPoint.y];
        preval = *pre;
        *pre = 1;
    }

    if (moveOb->getSort() == SORT_MISSILE) path.push(destination);
    else {
        auto&& ret = findPath(findPathMap, theMap, start, destination, moveOb, goalOb);
        path = ret.first;
        auto& dest = ret.second;
        if (dest[0] < 1e9) {
            DR0 = dest[0];
            UR0 = dest[1];
        }
    }
    if (pre)*pre = preval;//恢复寻路模板
    relate_AllObject[moveOb].nullPath = path.empty();
    moveOb->setPath(path, DR0, UR0);
}

void Core_List::crashHandle(MoveObject* moveOb)
{
    Point PreviousPoint, nextBlockPoint;

    //处理碰撞
    moveOb->GoBackLU();
    if (/*relate_AllObject[object].nullPath || */relate_AllObject[moveOb].relationAct == CoreEven_JustMoveTo && moveOb->getPath_size() == 0)
    {

        /*call_debugText("red", moveOb->getChineseName()+ "(编号:" + QString::number(moveOb->getglobalNum()) + "当前位置为 ("+\
                       QString::number(moveOb->getDR()) +"," + QString::number(moveOb->getUR())+") , 目标点 ("+\
                       QString::number(moveOb->getDR0()) + "," + QString::number(moveOb->getUR0()) + ") 附近不可抵达",moveOb->getPlayerRepresent());*/
        relate_AllObject[moveOb].wait(100);

        moveOb->stateCrash = true;
        if (!moveOb->isStand()) moveOb->setPreStand();
    }
    else
    {
        relate_AllObject[moveOb].crash_DealPhase = true;
        nextBlockPoint = moveOb->get_NextBlockPoint();
        PreviousPoint = moveOb->get_PreviousBlock();

        if (moveOb->is_MoveFirstStep() || PreviousPoint == nextBlockPoint)
        {
            //判断对象是否是船
            bool ship = JudgeMoveObjIsShip(moveOb);
            //
            auto& blockLab_canMove = theMap->findBlock_Free(nextBlockPoint, 1, !ship);

            if (blockLab_canMove.size()) PreviousPoint = blockLab_canMove[rand() % blockLab_canMove.size()];
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
    if (!(moveOb->get_NextBlockPoint() == relate_AllObject[moveOb].crashMove_Point))
    {
        relate_AllObject[moveOb].crash_DealPhase = false;
        moveOb->stateCrash = true;
        if (moveOb->getPlayerRepresent() == relate_AllObject[moveOb].crashRepresent)
            relate_AllObject[moveOb].wait(rand() % 50);
        else relate_AllObject[moveOb].wait(rand() % 20);

        if (rand() % 8 > 0) moveOb->setPath(stack<Point>(), moveOb->getDR(), moveOb->getUR());
        if (!moveOb->isStand()) moveOb->setPreStand();
    }
}

bool Core_List::JudgeMoveObjIsShip(MoveObject* moveOb)
{
    bool ship = 0;
    {
        Human* human = 0;moveOb->printer_ToHuman((void**)&human);
        if (human) {
            if (human->getSort() == SORT_ARMY) {
                Army* army = (Army*)human;
                if (army->getNum() == AT_SHIP)ship = 1;
            }
            else if (human->getSort() == SORT_FARMER) {
                Farmer* farmer = (Farmer*)human;
                if (farmer->get_farmerType() == FARMERTYPE_SAILING || farmer->get_farmerType() == FARMERTYPE_WOOD_BOAT)
                    ship = 1;
            }
        }
    }
    return ship;
}

pair<stack<Point>, array<double, 2>> Core_List::findPath(const int(&findPathMap)[MAP_L][MAP_U], Map* map, const Point& start, Point destination, Coordinate* object, Coordinate* goalOb)
{
    /////////////////////////////////////////////////////////
    static unsigned long long mask = 0;
    using Data = array<int, 2>;
    //8个移动方向 <4为斜线方向，>=4为水平竖直方向
    static Point dire[8] = { Point(1,1) , Point(1,-1) ,Point(-1,-1),Point(-1,1),\
                             Point(1,0),Point(-1,0) , Point(0,1), Point(0,-1) };
    static vector<vector<Data>> preNode(MAP_L + 1, vector<Data>(MAP_U + 1));
    static vector<vector<float>>mndis(MAP_L + 1, vector<float>(MAP_U + 1));
    static vector<Data>vis;
    static double Sqrt2 = sqrt(2.0);
    static vector<Data>goalPoint;
    /////////////////////////////////////////////////////////启发函数
    static auto PredictDistance = [&](const Data& start, const Data& end)->double {
        return abs(end[0] - start[0]) + abs(end[1] - start[1]);
        };
    /////////////////////////////////////////////////////////对于农民对渔场寻路，如果渔场靠岸那么我们在岸边找一点使得农民可达
    if (goalOb && (goalOb->getNum() == NUM_STATICRES_Fish || goalOb->getNum() == BUILDING_DOCK) && object->getSort() == SORT_FARMER && ((Farmer*)object)->get_farmerType() == FARMERTYPE_FARMER) {
        //寻找沿岸(与玩家所在大陆一致)
        auto check = [&](int i, int j)->bool {
            if (i >= 0 && j >= 0 && i < MAP_L && j < MAP_U) {
                return map->blockIndex[i][j] == map->blockIndex[start.x][start.y];
            }
            return 0;
            };
        int x = goalOb->getBlockDR(), y = goalOb->getBlockUR();
        int tx = INT_MAX, ty = INT_MAX;
        bool flag = 1;
        for (int i = 0;i < ((Building*)goalOb)->get_BlockSizeLen() && flag;++i) {
            for (int j = 0;j < ((Building*)goalOb)->get_BlockSizeLen() && flag;++j) {
                static const int off[][2] = { {0,1},{0,-1},{1,0},{-1,0} };
                for (auto* o : off) {
                    int xx = o[0] + i + x, yy = o[1] + j + y;
                    if (check(xx, yy)) {
                        tx = xx;ty = yy;
                        flag = 0;
                    }
                }
            }
        }
        //
        if (!flag && (tx != start.x || ty != start.y)) {
            static const double fac = 0.001;
            goalOb = 0;
            destination.x = tx, destination.y = ty;
        }
    }
    /////////////////////////////////////////////////////////
    goalPoint.clear();
    stack<Point> path;
    bool meetGoal = false;
    double dr0 = 1e9, ur0 = 1e9;
    ++mask;//把寻路掩码递增1
    //initMap_HaveJud();
    //memset(goalMap,0,sizeof(goalMap));
    //判断是不是船
    bool isShip = false;
    if (object) {
        Human* ship = 0;
        object->printer_ToHuman((void**)&ship);
        if (ship) {
            int sort = ship->getSort();
            if (sort == SORT_FARMER) {
                if (((Farmer*)ship)->get_farmerType() != FARMERTYPE_FARMER)isShip = 1;
            }
            else if (sort == SORT_ARMY)
            {
                if (((Army*)ship)->getNum() == AT_SHIP)isShip = 1;
            }
        }
    }

    //预处理能否到达
    Point end = goalOb ? Point(goalOb->getBlockDR(), goalOb->getBlockUR()) : Point(destination.x, destination.y);
    Point res = GetSameBlockInLine(start, end);
    //如果res!=end，说明不可达，也就是目的地变成res了
    if (res.x != end.x || res.y != end.y)
    {
        dr0 = (res.x + 0.5) * BLOCKSIDELENGTH;
        ur0 = (res.y + 0.5) * BLOCKSIDELENGTH;
        destination = res, goalOb = 0;
    }
    //判断目的地和起点是否位于同一个连通块,如果不在，那么正向寻找一个最优点
    if (map->blockIndex[start.x][start.y] != map->blockIndex[res.x][res.y]) {
        Point res = GetSameBlockInLineNearest(start, end);
        dr0 = (res.x + 0.5) * BLOCKSIDELENGTH;
        ur0 = (res.y + 0.5) * BLOCKSIDELENGTH;
        destination = res, goalOb = 0;
    }
    //起始点标记
    map_HaveJud[start.x][start.y] = mask;
    bool isNoPath = true;
    int ix, iy, jx, jy;
    //标记终点
    if (goalOb != NULL)
    {
        auto&& objectBlock = map->get_ObjectBlock(goalOb);
        for (Point& p : objectBlock) {
            ix = p.x;
            iy = p.y;
            goalMap[ix][iy] = mask;
            goalPoint.push_back({ ix,iy });
            if (!isNoPath)
                continue;
            for (int i = 4; i < 8; i++)
            {
                jx = ix + dire[i].x;
                jy = iy + dire[i].y;
                if (jx < 0 || jy < 0 || jx >= MAP_L || jy >= MAP_U) continue;

                if (findPathMap[jx][jy] == 0)
                {
                    isNoPath = false;
                    break;
                }
            }
        }
    }
    else
    {
        ix = destination.x;
        iy = destination.y;
        goalMap[ix][iy] = mask;
        goalPoint.push_back({ ix,iy });
        for (int i = 4; i < 8; i++)
        {
            jx = ix + dire[i].x;
            jy = iy + dire[i].y;
            if (jx < 0 || jy < 0 || jx >= MAP_L || jy >= MAP_U) continue;

            if (findPathMap[jx][jy] == 0)
            {
                isNoPath = false;
                break;
            }
        }
    }

    //if (isNoPath) return { path,{1e9,1e9} };
    goalMap[destination.x][destination.y] = mask;
    if (goalMap[start.x][start.y] == mask)
    {
        meetGoal = true;
    }
    using Info = tuple<double, int, int>;
    priority_queue<Info, vector<Info>, greater<Info>>qu;//这里我采用dijstra+A*算法寻路,因为很明显斜着的路劲和直走的路径的全值是不一样的
    qu.push(Info{ 0.0f,start.x,start.y });
    mndis[start.x][start.y] = 0.0;
    Data targetPoint = { start.x,start.y };
    vis.clear();
    vis.push_back({ start.x,start.y });
    preNode[start.x][start.y] = { -1,-1 };
    while (!qu.empty() && !meetGoal)
    {
        auto p = qu.top();
        auto d = get<0>(p);
        int x = get<1>(p), y = get<2>(p);
        qu.pop();
        //
        for (int i = 0;i < 8;++i) {
            auto& offset = dire[i];
            int xx = x + offset.x, yy = y + offset.y;
            if (xx >= 0 && yy >= 0 && xx < MAP_L && yy < MAP_U) {
                Block& block = theMap->cell[xx][yy];
                bool isLand = block.getMapType() != MAPTYPE_OCEAN;
                float dd = mndis[x][y] + (i < 4 ? Sqrt2 : 1.0);
                //判断格子是否可走、未走过
                //斜线方向需要多判断马脚操作
                if (isLand ^ isShip) {
                    if (theMap->cell[xx][yy].getMapType() != MAPTYPE_OCEAN)std::cout << "SB" << std::endl;
                    if ((map_HaveJud[xx][yy] != mask || mndis[xx][yy] > dd) && !(findPathMap[xx][yy] && goalMap[xx][yy] != mask || i < 4 && (findPathMap[xx][y] || findPathMap[x][yy]))) {
                        preNode[xx][yy] = { x,y };
                        mndis[xx][yy] = dd;
                        //算出预估最小值
                        Data start = { xx,yy };
                        double mnPd = 1e9;
                        for (auto& tp : goalPoint)mnPd = min(mnPd, PredictDistance(start, tp));
                        //
                        qu.push(Info{ dd + mnPd,xx,yy });
                        vis.push_back({ xx,yy });
                        map_HaveJud[xx][yy] = mask;
                        if (goalMap[xx][yy] == mask) {
                            meetGoal = 1;
                            targetPoint = { xx,yy };
                        }
                    }
                }
            }
            if (meetGoal)break;
        }
        if (meetGoal)break;
    }
    {
        //如果没有能有一条合法的路径到终点
        if (!meetGoal) {
            targetPoint = { -119,-119 };
            //////////////选择离目的地最近的节点
            for (Data& point : vis) {
                if (targetPoint[0] == -119) {
                    targetPoint = point;
                }
                else {
                    double dr0 = point[0] - destination.x, ur0 = point[1] - destination.y;
                    double dr1 = targetPoint[0] - destination.x, ur1 = targetPoint[1] - destination.y;
                    if (dr0 * dr0 + ur0 * ur0 < dr1 * dr1 + ur1 * ur1)targetPoint = point;
                }
            }
            //////////////
            MoveObject* obj = 0;
            object->printer_ToMoveObject((void**)&obj);
            if (obj && targetPoint[0] != -119)
            {
                dr0 = (targetPoint[0] + 0.5) * BLOCKSIDELENGTH, ur0 = (targetPoint[1] + 0.5) * BLOCKSIDELENGTH;
            }
        }
        //
        while (targetPoint[0] != -1)
        {
            path.push(Point{ targetPoint[0],targetPoint[1] });
            targetPoint = preNode[targetPoint[0]][targetPoint[1]];
        }
        //当前所在格子不加入栈
        if (path.size())
            path.pop();
    }

    return { path,{dr0,ur0} };
}

Point Core_List::GetSameBlockInLine(const Point& point0, const Point& point1)
{
    //保证传入的点都在范围之内
    bool ocean = theMap->cell[point0.x][point0.y].getMapType() == MAPTYPE_OCEAN;
    bool targetOcean = theMap->cell[point1.x][point1.y].getMapType() == MAPTYPE_OCEAN;
    //如果起点和终点都是同一种地形类型，直接返回
    if (ocean == targetOcean)return point1;
    //寻找从终点出发与目的地最近的合法点
    int x1 = point1.x, y1 = point1.y, x2 = point0.x, y2 = point0.y;
    int dx = x2 - x1, dy = y2 - y1;
    int s1 = (dx >= 0 ? 1 : -1), s2 = (dy >= 0 ? 1 : -1);
    dx = abs(dx);
    dy = abs(dy);
    int xbase = x1, ybase = y1;
    if (dx >= dy)
    {
        int e = (dy << 1) - dx, deta1 = dy << 1, deta2 = (dy - dx) << 1;
        while (xbase != x2)
        {
            if (e >= 0)//y方向增量为1
                xbase += s1, ybase += s2, e += deta2;
            else xbase += s1, e += deta1;
            //检查如果和起点类型一致，那么就可以返回了
            if (!(xbase < 0 || xbase >= MAP_L || ybase < 0 || ybase >= MAP_U)) {
                bool flag = theMap->cell[xbase][ybase].getMapType() == MAPTYPE_OCEAN;
                if (flag == ocean)return { xbase,ybase };
            }
        }

    }
    else
    {
        int e = (dx << 1) - dy, deta1 = dx << 1, deta2 = (dx - dy) << 1;
        while (ybase != y2)
        {
            if (e >= 0)//x方向增量为1
                xbase += s1, ybase += s2, e += deta2;
            else ybase += s2, e += deta1;
            //检查如果和起点类型一致，那么就可以返回了
            if (!(xbase < 0 || xbase >= MAP_L || ybase < 0 || ybase >= MAP_U)) {
                bool flag = theMap->cell[xbase][ybase].getMapType() == MAPTYPE_OCEAN;
                if (flag == ocean)return { xbase,ybase };
            }
        }
    }

    return point0;
}

Point Core_List::GetSameBlockInLineNearest(const Point& point0, const Point& point1)
{
    //保证传入的点都在范围之内,并且Point0和point1他们所属的块不一样
    bool ocean = theMap->cell[point0.x][point0.y].getMapType() == MAPTYPE_OCEAN;
    //寻找从终点出发与目的地最近的合法点
    int x1 = point0.x, y1 = point0.y, x2 = point1.x, y2 = point1.y;
    int dx = x2 - x1, dy = y2 - y1;
    int s1 = (dx >= 0 ? 1 : -1), s2 = (dy >= 0 ? 1 : -1);
    dx = abs(dx);
    dy = abs(dy);
    int xbase = x1, ybase = y1;
    int xpre, ypre;
    if (dx >= dy)
    {
        int e = (dy << 1) - dx, deta1 = dy << 1, deta2 = (dy - dx) << 1;
        while (xbase != x2)
        {
            xpre = xbase, ypre = ybase;
            if (e >= 0)//y方向增量为1
                xbase += s1, ybase += s2, e += deta2;
            else xbase += s1, e += deta1;
            //检查如果下一个点和起点类型不一致，那么可以返回了
            if (!(xbase < 0 || xbase >= MAP_L || ybase < 0 || ybase >= MAP_U)) {
                bool flag = theMap->cell[xbase][ybase].getMapType() == MAPTYPE_OCEAN;
                if (flag != ocean)return { xpre,ypre };
            }
        }

    }
    else
    {
        int e = (dx << 1) - dy, deta1 = dx << 1, deta2 = (dx - dy) << 1;
        while (ybase != y2)
        {
            xpre = xbase, ypre = ybase;
            if (e >= 0)//x方向增量为1
                xbase += s1, ybase += s2, e += deta2;
            else ybase += s2, e += deta1;
            //检查如果下一个点和起点类型不一致，那么可以返回了
            if (!(xbase < 0 || xbase >= MAP_L || ybase < 0 || ybase >= MAP_U)) {
                bool flag = theMap->cell[xbase][ybase].getMapType() == MAPTYPE_OCEAN;
                if (flag != ocean)return { xpre,ypre };
            }
        }
    }

    return point1;
}

bool Core_List::checkIsCoast(int x, int y)
{
    if (x < 0 || y < 0 || x >= MAP_L || y >= MAP_U)return 0;
    if (theMap->cell[x][y].getMapType() == MAPTYPE_OCEAN)return 0;
    const static int offset[][2] = { {0,-1},{0,1},{-1,0},{1,0} };
    for (auto* o : offset) {
        int i = o[0] + x, j = o[1] + y;
        if (i < 0 || i < 0 || i >= MAP_L || i >= MAP_U)continue;
        if (theMap->cell[i][j].getMapType() == MAPTYPE_OCEAN)return 1;
    }
    return 0;
}


//*************************************************************
//行动预备处理
//创建投掷物
Missile* Core_List::creatMissile(Coordinate* attacker, Coordinate* attackee)
{
    int playerRepresent = MAXPLAYER;
    Human* judHuman = NULL;
    Building* judBuild = NULL;

    attacker->printer_ToBuilding((void**)&judBuild);
    attacker->printer_ToHuman((void**)&judHuman);

    if (judHuman != NULL) playerRepresent = judHuman->getPlayerRepresent();
    if (judBuild != NULL) playerRepresent = judBuild->getPlayerRepresent();

    if (playerRepresent < MAXPLAYER)
        return player[playerRepresent]->addMissile(attacker, attackee, theMap->get_MapHeight(attacker->getBlockDR(), attacker->getBlockUR()));
    else return NULL;
}


/**************************************************************/
//建立行动细节的静态表
void Core_List::initDetailList()
{
    int* phaseList;
    conditionF* conditionList;
    list<conditionF>forcedInterrupCondition;
    list<conditionF>overCondition;

    //添加静态表
    //行动：只移动*************************************
    {
        phaseList = new int(CoreDetail_Move);
        conditionList = new (conditionF)(conditionF(condition_ObjectNearby));
        forcedInterrupCondition.push_back(conditionF(condition_UselessAction, OPERATECON_TIMES_USELESSACT_MOVE));

        relation_Event_static[CoreEven_JustMoveTo] = detail_EventPhase(1, phaseList, conditionList, forcedInterrupCondition);
        relation_Event_static[CoreEven_JustMoveTo].setEnd_Absolute();
        delete phaseList;
        delete conditionList;
        forcedInterrupCondition.clear();
    }


    //行动: 攻击**************************************
    {
        phaseList = new int[2] { CoreDetail_Move, CoreDetail_Attack };
        conditionList = new conditionF[2]{ conditionF(condition_ObjectNearby, OPERATECON_NEAR_ATTACK_MOVE),  conditionF(condition_Object1_AttackingEnd, OPERATECON_NEAR_ATTACK) };
        forcedInterrupCondition.push_back(conditionF(condition_UselessAction, OPERATECON_TIMES_USELESSACT_MOVE));

        relation_Event_static[CoreEven_Attacking] = detail_EventPhase(2, phaseList, conditionList, forcedInterrupCondition);
        overCondition.push_back(conditionF(condition_UniObjectDie, OPERATECON_OBJECT2));
        relation_Event_static[CoreEven_Attacking].setLoop(0, 1, overCondition);   //向前跳转使用setLoop

        delete[] phaseList;
        delete[] conditionList;
        forcedInterrupCondition.clear();
        overCondition.clear();
    }


    //行动：采集**************************************
    {
        phaseList = new int[13] {/*0判断是否需要攻击*/CoreDetail_JumpPhase, \
            /*1前往攻击目标*/CoreDetail_Move,     /*2攻击*/CoreDetail_Attack,            /*3*/CoreDetail_JumpPhase, \
            /*4前往资源建筑*/CoreDetail_Move,     /*5资源放置*/CoreDetail_ResourceIn,     /*6前往资源*/CoreDetail_Move, \
            /*7采集*/CoreDetail_Gather,          /*资源被采集完毕，需要判断村民携带资源*/    /*8*/CoreDetail_JumpPhase, \
            /*9前往资源建筑*/CoreDetail_Move,    /*10资源放置*/CoreDetail_ResourceIn,    /*11前往资源原位置*/CoreDetail_Move, \
            /*12*/ CoreDetail_JumpPhase
            };
        conditionList = new conditionF[13]{ conditionF(condition_AllTrue),\
            /*1*/conditionF(condition_ObjectNearby, OPERATECON_NEAR_ATTACK),    /*2*/conditionF(condition_Object1_AttackingEnd, OPERATECON_NEAR_ATTACK),\
            /*3*/conditionF(condition_AllTrue),                                /*4*/conditionF(condition_ObjectNearby, OPERATECON_NEARALTER_WORK),\
            /*5*/conditionF(condition_Object1_EmptyBackpack),                   /*6*/conditionF(condition_ObjectNearby, OPERATECON_NEAR_WORK),\
            /*7*/conditionF(condition_Object1_FullBackpack), \
            /*8*/conditionF(condition_AllTrue),                                 /*9*/conditionF(condition_ObjectNearby, OPERATECON_NEARALTER_WORK),\
            /*10*/conditionF(condition_Object1_EmptyBackpack),                   /*11*/conditionF(condition_ObjectNearby, OPERATECON_NEAR_WORK),\
            /*12*/conditionF(condition_AllTrue)
        };

        forcedInterrupCondition.push_back(conditionF(condition_UselessAction, OPERATECON_TIMES_USELESSACT_MOVE));

        relation_Event_static[CoreEven_Gather] = detail_EventPhase(13, phaseList, conditionList, forcedInterrupCondition);
        //设置循环，1->2，攻击猎物直至可采集
        overCondition.push_back(conditionF(condition_UniObjectNULL, OPERATECON_OBJECT2));
        overCondition.push_back(conditionF(condition_Object2CanbeGather));
        relation_Event_static[CoreEven_Gather].setLoop(1, 2, overCondition);
        overCondition.clear();
        //设置循环，4->7，持续采集，目标无资源不可采集
        overCondition.push_back(conditionF(condition_Object2CanbeGather, OPERATECON_OBJECT2, true));
        relation_Event_static[CoreEven_Gather].setLoop(4, 7, overCondition);
        overCondition.clear();

        //设置循环，9->12，用于最后一次返回资源建筑，若身上无资源，则直接停止
        overCondition.push_back(conditionF(condition_Object1_EmptyBackpack));
        relation_Event_static[CoreEven_Gather].setLoop(9, 12, overCondition);
        overCondition.clear();

        //行动起始，判断是否可直接采集
        relation_Event_static[CoreEven_Gather].setJump(0, 2);
        //猎物可采集后，跳转至前往资源
        relation_Event_static[CoreEven_Gather].setJump(3, 6);
        //资源被采集完毕后，若身上无资源，则直接停止
        relation_Event_static[CoreEven_Gather].setJump(8, 12);

        delete[] phaseList;
        delete[] conditionList;
        forcedInterrupCondition.clear();
        overCondition.clear();
    }


    //行动：修建建筑***********************************
    {
        phaseList = new int[2] {CoreDetail_Move, CoreDetail_UpdateRatio };
        conditionList = new conditionF[2]{ conditionF(condition_ObjectNearby, OPERATECON_NEAR_WORK), conditionF(condition_UniObjectPercent, OPERATECON_OBJECT2) };
        forcedInterrupCondition.push_back(conditionF(condition_UniObjectNULL, OPERATECON_OBJECT2));
        forcedInterrupCondition.push_back(conditionF(condition_UselessAction, OPERATECON_TIMES_USELESSACT_MOVE));

        relation_Event_static[CoreEven_FixBuilding] = detail_EventPhase(2, phaseList, conditionList, forcedInterrupCondition);

        delete[] phaseList;
        delete[] conditionList;
        forcedInterrupCondition.clear();
    }


    //行动：建筑工作***********************************
    {
        phaseList = new int(CoreDetail_UpdateRatio);
        conditionList = new (conditionF)(conditionF(condition_UniObjectPercent, OPERATECON_OBJECT1));

        relation_Event_static[CoreEven_BuildingAct] = detail_EventPhase(1, phaseList, conditionList, forcedInterrupCondition);

        delete phaseList;
        delete conditionList;
    }

    //行动：飞行物攻击*********************************
    {

        phaseList = new int[2] { CoreDetail_Move, CoreDetail_Attack };
        conditionList = new conditionF[2]{ conditionF(condition_ObjectNearby, OPERATECON_NEAR_MISSILE) , conditionF(condition_TimesFalse) };

        relation_Event_static[CoreEven_MissileAttack] = detail_EventPhase(2, phaseList, conditionList, forcedInterrupCondition);

        delete[] phaseList;
        delete[] conditionList;
    }
    //行动：运输人*********************************
    {
        forcedInterrupCondition.push_back(conditionF(condition_UselessAction, OPERATECON_TIMES_USELESSACT_MOVE));
        phaseList = new int[2] { CoreDetail_Move, CoreDetail_Transport };
        conditionList = new conditionF[2]{ conditionF(condition_ObjectNearby,OPERATECON_NEAR_TRANSPORT),conditionF(condition_Object2_Transported) };
        relation_Event_static[CoreEven_Transport] = detail_EventPhase(2, phaseList, conditionList, forcedInterrupCondition);
        forcedInterrupCondition.clear();
        delete[] phaseList;
        delete[] conditionList;
    }
    //行动：卸货*********************************
    {
        forcedInterrupCondition.push_back(conditionF(condition_UselessAction, OPERATECON_TIMES_USELESSACT_MOVE));
        phaseList = new int[2] { CoreDetail_Move, CoreDetail_Unload };
        conditionList = new conditionF[2]{ conditionF(condition_ObjectNearby,OPERATECON_NEAR_UNLOAD),conditionF(condition_Object1_Unload) };
        relation_Event_static[CoreEven_UnLoad] = detail_EventPhase(2, phaseList, conditionList, forcedInterrupCondition);
        forcedInterrupCondition.clear();
        delete[] phaseList;
        delete[] conditionList;
    }
}


/**************************************************************/

int Core_List::getNowPhaseNum(Coordinate* object)
{
    ///获取当前object的行动，用于将信息传递给AIGame
    relation_Object& thisRelation = relate_AllObject[object];
    if (!thisRelation.isExist) {
        return HUMAN_STATE_IDLE;
    }

    switch (thisRelation.relationAct) {
    case CoreEven_JustMoveTo:
    case CoreEven_UnLoad:
        return HUMAN_STATE_WALKING;
    case CoreEven_Transport:
        return HUMAN_STATE_IDLE;
    case CoreEven_Attacking:
        return HUMAN_STATE_ATTACKING;
    case CoreEven_Gather:
    case CoreEven_FixBuilding:
    case CoreEven_CreatBuilding:
        return HUMAN_STATE_WORKING;
    default:
        return HUMAN_STATE_IDLE;
    }
}
