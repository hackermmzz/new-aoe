#include "SelectWidget.h"
#include "Core.h"

Core::Core(Map* theMap, Player* player[], int** memorymap,MouseEvent *mouseEvent)
{
    this->theMap = theMap;  //mainWidget的map对象
    this->player = player;  //所有player的数组
    this->memorymap = memorymap;    //内存图
    this->mouseEvent = mouseEvent;  //点击窗口的鼠标事件
    this->interactionList = new Core_List( this->theMap , this->player );   //本类中管理的对象交互动态表
}

void Core::gameUpdate()
{
    theMap->clear_CellVisible();     //清空上一帧的视野
    theMap->init_Map_UseToMonitor(); //初始化各ob所处位置的信息地图和需要监视的ob的视野地图

    //player管理的各个ob更新状态
    for(int playerIndx = 0; playerIndx < MAXPLAYER ; playerIndx++)
    {
        std::list<Human *>::iterator humaniter=player[playerIndx]->human.begin() , humaniterEnd = player[playerIndx]->human.end();
        list<Building*>::iterator builditer = player[playerIndx]->build.begin() , builditerEnd = player[playerIndx]->build.end();
        list<Missile*>::iterator missileiter = player[playerIndx]->missile.begin() , missileiterEnd = player[playerIndx]->missile.end();

        //更新human子类的状态
        while(humaniter!=humaniterEnd)
        {       
            //如果当前对象需要变换行动状态，如从采集浆果->移动
            if((*humaniter)->needTranState())
            {
                (*humaniter)->setNowState((*humaniter)->getPreState());

                //需要变化的行动为“死亡”，在交互行动表中将其删除
                if((*humaniter)->isDying())
                {
                    call_debugText("red",(*humaniter)->getChineseName()+"(编号"+QString::number((*humaniter)->getglobalNum())+")死亡");
                    //在交互行动表中将其删除——删除其作为主体的行动、其作为目标的行动中将目标设置为NULL
                    interactionList->eraseObject(*humaniter);
                    //如果Missle的投出者是该ob，则让该missle使用投出者记录
                    player[playerIndx]->deleteMissile_Attacker(*humaniter);
                }
                (*humaniter)->setPreStateIsIdle();
            }


            if((*humaniter)->isDying() && (*humaniter)->get_isActionEnd())
            {
                deleteOb_setNowobNULL(*humaniter);
                humaniter = player[playerIndx]->deleteHuman(humaniter);
            }
            else
            {
                theMap->add_Map_Object(*humaniter);
                //更新视野
                if(playerIndx == 0) theMap->reset_CellExplore(*humaniter);

                if((*humaniter)->getSort() == SORT_ARMY && (*humaniter)->getNum()!=AT_SCOUT && get_IsObjectFree(*humaniter))
                    theMap->add_Map_Vision(*humaniter);

                interactionList->conduct_Attacked(*humaniter);
                (*humaniter)->nextframe();
                (*humaniter)->updateLU();

                if((*humaniter)->getBlockDR() != INT_MAX && (*humaniter)->getBlockUR() != INT_MAX)
                {
                    // 更新人物Y轴偏移（伪三维）
                    int curMapHeight = theMap->cell[(*humaniter)->getBlockDR()][(*humaniter)->getBlockUR()].getMapHeight();

                    // 斜坡
                    if(theMap->isSlope((*humaniter)->getBlockDR(), (*humaniter)->getBlockUR()))
                    {
                        if(curMapHeight > MAPHEIGHT_MAX - 1 || curMapHeight < MAPHEIGHT_FLAT)
                        {
                            qDebug() << "ERROR: Calculation error in drawing human parts in the function gameUpdate()";
                            qDebug() << "gameUpdate()函数中绘制人类的部分计算错误";
                        }

                        // 判断mapType以确定上升方向
                        int curMapType = theMap->cell[(*humaniter)->getBlockDR()][(*humaniter)->getBlockUR()].getMapType();
                        pair<double, double> curHumanCoor = {(*humaniter)->getDR(), (*humaniter)->getUR()};   // 当前人物细节坐标
                        pair<double, double> curBlockCoor = {(*humaniter)->getBlockDR() * 16.0 * gen5, (*humaniter)->getBlockUR() * 16.0 * gen5}; // 当前人物所在格（最左端的）细节坐标

                        // 左高右低：
                        if(curMapType == MAPTYPE_L1_UPTOLU || curMapType == MAPTYPE_A1_UPTOL || curMapType == MAPTYPE_A3_DOWNTOR || curMapType == MAPTYPE_L0_UPTOLD)
                        {
                            double leftOffsetPercent = fabs((16.0 * gen5 - (curHumanCoor.first - curBlockCoor.first)) / (16.0 * gen5));
                            if(leftOffsetPercent > 1) leftOffsetPercent = 1;
                           (*humaniter)->setMapHeightOffsetY(DRAW_OFFSET * curMapHeight + DRAW_OFFSET * leftOffsetPercent);
//                            qDebug() << "左高右低，leftOffsetPercent == " << leftOffsetPercent << ", MapHeightOffsetY == " << DRAW_OFFSET * curMapHeight + DRAW_OFFSET * leftOffsetPercent;
                        }
                        // 右高左低：
                        else if(curMapType == MAPTYPE_L2_UPTORU || curMapType == MAPTYPE_A3_UPTOR || curMapType == MAPTYPE_A1_DOWNTOL || curMapType == MAPTYPE_L3_UPTORD)
                        {
                            double rightOffsetPercent = fabs((curHumanCoor.second - curBlockCoor.second) / (16.0 * gen5));
                            if(rightOffsetPercent > 1) rightOffsetPercent = 1;
                           (*humaniter)->setMapHeightOffsetY(DRAW_OFFSET * curMapHeight + DRAW_OFFSET * rightOffsetPercent);
//                            qDebug() << "右高左低， rightOfsetPercent == " << rightOffsetPercent << ", MapHeightOffsetY == " << DRAW_OFFSET * curMapHeight + DRAW_OFFSET * rightOffsetPercent;
                        }
                        // 下高上低：
                        else if(curMapType == MAPTYPE_A0_UPTOD || curMapType == MAPTYPE_A2_DOWNTOU)
                        {
                            double downOffsetPercent = (16.0 * gen5 - ((curHumanCoor.second - curBlockCoor.second) - (curHumanCoor.first - curBlockCoor.first))) / (16.0 * gen5);
                            if(downOffsetPercent > 1) downOffsetPercent = 1;
                            (*humaniter)->setMapHeightOffsetY(DRAW_OFFSET * curMapHeight + DRAW_OFFSET * downOffsetPercent);
//                            qDebug() << "下高上低，downOffsetPercent == " << downOffsetPercent << ", MapHeightOffsetY == " << DRAW_OFFSET * curMapHeight + DRAW_OFFSET * downOffsetPercent;
                        }
                        // 上高下低：
                        else if(curMapType == MAPTYPE_A2_UPTOU || curMapType == MAPTYPE_A0_DOWNTOD)
                        {
                            double upOffsetPercent = ((curHumanCoor.second - curBlockCoor.second) - (curHumanCoor.first - curBlockCoor.first)) / (16.0 * gen5);
                            if(upOffsetPercent > 1) upOffsetPercent = 1;
                            (*humaniter)->setMapHeightOffsetY(DRAW_OFFSET * curMapHeight + DRAW_OFFSET * upOffsetPercent);
//                            qDebug() << "上高下低，upOffsetPercent == " << upOffsetPercent << ", MapHeightOffsetY == " << DRAW_OFFSET * curMapHeight + DRAW_OFFSET * upOffsetPercent;
                        }
                    }
                    // 平地
                    else (*humaniter)->setMapHeightOffsetY(curMapHeight * DRAW_OFFSET);
                }

                humaniter++;
            }
        }

        while(builditer!= builditerEnd)
        {
            interactionList->conduct_Attacked(*builditer);
            if((*builditer)->isDie()||( (*builditer)->getSort()== SORT_Building_Resource && !((Building_Resource*)(*builditer))->is_Surplus()))
            {
                if(!(*builditer)->isDie())
                    call_debugText("red"," "+(*builditer)->getChineseName()+"(编号:"+QString::number((*builditer)->getglobalNum())+")采集完成");
                else
                    call_debugText("red"," "+(*builditer)->getChineseName()+"(编号:"+QString::number((*builditer)->getglobalNum())+")被摧毁");

                player[playerIndx]->deleteMissile_Attacker(*builditer);
                interactionList->eraseObject(*builditer);
                deleteOb_setNowobNULL(*builditer);
                builditer= player[playerIndx]->deleteBuilding(builditer);
            }
            else
            {
                theMap->add_Map_Object(*builditer);
                //更新视野 用户控制的对象
                if(playerIndx == 0)  theMap->reset_CellExplore(*builditer);

                if((*builditer)->getNum() == BUILDING_ARROWTOWER && get_IsObjectFree(*builditer))
                    theMap->add_Map_Vision(*builditer);
                (*builditer)->nextframe();
                builditer++;
            }
        }

        while(missileiter!=missileiterEnd)
        {
//            qDebug()<<playerIndx<<(player[playerIndx]->missile.size())<<((*missileiter)->isNeedDelete());s
            if((*missileiter)->isNeedDelete())
            {
                interactionList->eraseObject(*missileiter);
                missileiter = player[playerIndx]->deleteMissile(missileiter);
            }
            else
            {
                (*missileiter)->nextframe();
                missileiter++;
            }
        }
    }

    std::list<Animal*>::iterator animaliter=theMap->animal.begin();
    while(animaliter!=theMap->animal.end())
    {
        if((*animaliter)->is_Surplus())
        {
            if((*animaliter)->needTranState())
            {
                (*animaliter)->setNowState((*animaliter)->getPreState());
                if((*animaliter)->isDying())
                {
                    if(!(*animaliter)->isTree())
                        call_debugText("red"," "+(*animaliter)->getChineseName()+"(编号"+QString::number((*animaliter)->getglobalNum())+")死亡");

                    interactionList->eraseRelation(*animaliter);
                }
                (*animaliter)->setPreStateIsIdle();
            }

            if((*animaliter)->isTree())
                (*animaliter)->initAvengeObject();
            interactionList->conduct_Attacked(*animaliter);
            (*animaliter)->nextframe();
            (*animaliter)->updateLU();

            if(!(*animaliter)->isDie() \
               && ((*animaliter)->getNum() == ANIMAL_GAZELLE ||(*animaliter)->getNum() == ANIMAL_LION) && get_IsObjectFree(*animaliter) )
                theMap->add_Map_Vision(*animaliter);
            theMap->add_Map_Object(*animaliter);

            animaliter++;
        }
        else
        {
            call_debugText("red"," "+(*animaliter)->getChineseName()+"(编号:"+QString::number((*animaliter)->getglobalNum())+")采集完成");
            interactionList->eraseObject(*animaliter);   //行动表中animal设为null
            deleteOb_setNowobNULL(*animaliter);
            animaliter = theMap->deleteAnimal(animaliter);
        }
    }

    std::list<StaticRes*>::iterator SRiter=theMap->staticres.begin();
    while(SRiter!=theMap->staticres.end())
    {
        if((*SRiter)->is_Surplus())
        {
            theMap->add_Map_Object(*SRiter);

            (*SRiter)->nextframe();
            SRiter++;
        }
        else
        {
            call_debugText("red"," "+(*SRiter)->getChineseName()+"(编号:"+QString::number((*SRiter)->getglobalNum())+")采集完成");

            interactionList->eraseObject(*SRiter);
            deleteOb_setNowobNULL(*SRiter);
            SRiter = theMap->deleteStaticRes(SRiter);
        }
    }

    //更新AI用的资源表，该资源表是User/Enemy的通用模板
    theMap->reset_resMap_AI();

    theMap->reset_ObjectExploreAndVisible();    //刷新视野并处理区域探索结果

    if(mouseEvent->mouseEventType!=NULL_MOUSEEVENT) manageMouseEvent();

    manageOrder(0);

    //对正在监视的Object，进行行动处理
    interactionList->manageMontorAct();

    theMap->loadBarrierMap();//更新寻路用障碍表
    interactionList->manageRelationList();

}

void Core::infoShare(int playerID){
    Player* self=player[playerID];
    tagGame* tagAIGame;
    QMutex tagAIGameLock;
    if(playerID==0){
        tagAIGame=&tagUsrGame;
    }else{
        /**
          Todo:其他玩家初始化
          */
    }
    //如果AI线程正在读取tagGame数据，则直接
    if(!tagAIGameLock.tryLock()){
        return;
    }
    for(int i=0;i<MAP_L;i++){
        for(int j=0;j<MAP_U;j++){
            if(!tagAIGame->blocks[i][j].explored&&theMap->cell[i][j].Explored){
                tagAIGame->blocks[i][j].height=theMap->cell[i][j].getMapHeight();
                tagAIGame->blocks[i][j].explored=true;
            }
        }
    }

    tagAIGame->Human_MaxNum=self->getMaxHumanNum();
    tagAIGame->Gold=self->getGold();
    tagAIGame->Stone=self->getStone();
    tagAIGame->Meat=self->getFood();
    tagAIGame->Wood=self->getWood();
    tagAIGame->civilizationStage=self->getCiv();
    tagAIGame->GameFrame=g_frame;
    tagAIGame->farmers.clear();
    for(Human* human:self->human){
        tagHuman taghuman;
        taghuman.Owner=playerID;
        taghuman.SN=human->getglobalNum();
        taghuman.Blood=human->getBlood();
        taghuman.L=human->getDR();
        taghuman.U=human->getUR();
        taghuman.BlockL=human->getBlockDR();
        taghuman.BlockU=human->getBlockUR();
        taghuman.Blood=human->getBlood();
        taghuman.NowState=interactionList->getNowPhaseNum(human);
        if(human->getSort()==HUMAN_STATE_IDLE){
            taghuman.WorkObjectSN=-1;
        }else{
            taghuman.WorkObjectSN=interactionList->getObjectSN(human);
        }
        taghuman.L0=human->getDR0();
        taghuman.U0=human->getUR0();
        if(human->getSort()==SORT_FARMER){
            Farmer* farmer=static_cast<Farmer*> (human);
            tagFarmer tagfarmer;
            tagfarmer.cast_from(taghuman);
            tagfarmer.Resource=farmer->getResourceNowHave();
            if(tagfarmer.Resource==0){
                tagfarmer.ResourceSort=-1;
            }else{
                tagfarmer.ResourceSort=farmer->getResourceSort();
            }
            tagAIGame->farmers.push_back(tagfarmer);
            //Todo:同步跟新其他ai的信息
//            for(int i=0;i<NOWPLAYER;i++){
//                if(i==playerID){
//                    continue;
//                }else{
//                    if(!tagGamelocks[playerID].tryLock()){
//                        continue;
//                    }
//                    AIGame[i].enemy_farmers.push_back(tagfarmer.toEnemy());
//                    tagGamelocks[playerID].unlock();
//                }
//            }
        }else if(human->getSort()==SORT_ARMY){
            Army* army=static_cast<Army*> (human);
            tagArmy tagarmy;
            tagarmy.cast_from(taghuman);
            tagarmy.Sort=army->getNum();
            tagAIGame->armies.push_back(tagarmy);
//            for(int i=0;i<NOWPLAYER;i++){
//                if(i==playerID){
//                    continue;
//                }else{
//                    if(!tagGamelocks[playerID].tryLock()){
//                        continue;
//                    }
//                    AIGame[i].enemy_armies.push_back(tagarmy.toEnemy());
//                    tagGamelocks[playerID].unlock();
//                }
//            }
        }
    }

    AIGame[playerID].resources.clear();
    for(Animal* animal:theMap->animal){
        tagResource resource;
        resource.SN=animal->getglobalNum();
        switch(animal->getNum()){
        case ANIMAL_ELEPHANT:
            resource.Type=RESOURCE_ELEPHANT;
            resource.ProductSort=HUMAN_STOCKFOOD;
            break;
        case ANIMAL_GAZELLE:
            resource.Type=RESOURCE_GAZELLE;
            resource.ProductSort=HUMAN_STOCKFOOD;
            break;
        case ANIMAL_LION:
            resource.Type=RESOURCE_LION;
            resource.ProductSort=HUMAN_STOCKFOOD;
            break;
        case ANIMAL_TREE:
            resource.Type=RESOURCE_TREE;
            resource.ProductSort=HUMAN_WOOD;
            break;
        case ANIMAL_FOREST:
            resource.Type=RESOURCE_TREE;
            resource.ProductSort=HUMAN_WOOD;
            break;
        default:
            resource.Type=-1;
            resource.ProductSort=-1;
        }
        resource.BlockL=animal->getBlockDR();
        resource.BlockU=animal->getBlockUR();
        resource.L=animal->getDR();
        resource.U=animal->getUR();
        resource.Blood=animal->getBlood();
        resource.Cnt=animal->get_Cnt();
        tagAIGame->resources.push_back(resource);
    }
    for(StaticRes* staticRes: theMap->staticres){
        tagResource resource;
        resource.SN=staticRes->getglobalNum();
        resource.L=staticRes->getDR();
        resource.U=staticRes->getUR();
        resource.BlockL=staticRes->getBlockDR();
        resource.BlockU=staticRes->getBlockUR();
        switch(staticRes->getNum()){
        case 0:
            resource.Type=RESOURCE_BUSH;
            resource.ProductSort=HUMAN_STOCKFOOD;
            break;
        case 1:
            resource.Type=RESOURCE_STONE;
            resource.ProductSort=HUMAN_STONE;
            break;
        default:
            resource.Type=-1;
            resource.ProductSort=-1;
        }
        resource.Cnt=staticRes->get_Cnt();
        resource.Blood=-1;
        tagAIGame->resources.push_back(resource);
    }

    tagAIGame->buildings.clear();
    for(Building* build:self->build){
        tagBuilding building;
        building.SN=build->getglobalNum();
        building.BlockL=build->getBlockDR();
        building.BlockU=build->getBlockUR();
        building.Blood=build->getBlood();
        building.MaxBlood=build->getMaxBlood();
        building.Percent=build->getPercent();
        building.Project=build->getActNum();
        building.ProjectPercent=build->getActPercent();
        building.Owner=playerID;
//        if(build->getSort()==SORT_FARM){
//            building.Type=BUILDING_FARM;
//            building.Cnt=build->getCnt();
//        }else{
            building.Type=build->getNum();
            building.Cnt=-1;
//        }
        tagAIGame->buildings.push_back(building);
//        for(int i=0;i<NOWPLAYER;i++){
//            if(i==playerID){
//                continue;
//            }else{
//                if(!tagGamelocks[playerID].try_lock()){
//                    continue;
//                }
//                AIGame[i].enemy_buildings.push_back(building.toEnemy());
//                tagGamelocks[playerID].unlock();
//            }
//        }
    }
    tagAIGameLock.unlock();
}

void Core::getPlayerNowResource( int playerRepresent, int& wood, int& food, int& stone, int& gold )
{
    wood = player[playerRepresent]->getWood();
    food = player[playerRepresent]->getFood();
    stone = player[playerRepresent]->getStone();
    gold = player[playerRepresent]->getGold();
}

//处理鼠标事件
void Core::manageMouseEvent()
{
    Coordinate* object_click = g_Object[memorymap[mouseEvent->memoryMapX][mouseEvent->memoryMapY]];
    if(mouseEvent->mouseEventType==LEFT_PRESS)
    {
        nowobject=object_click;
//        emit clickOnObject();
        sel->initActs();
        mouseEvent->mouseEventType=NULL_MOUSEEVENT;
    }
    if(mouseEvent->mouseEventType==RIGHT_PRESS && nowobject!=NULL)
    {
        if( object_click== NULL )
        {
            if( (nowobject->getSort() == SORT_FARMER || nowobject->getSort() == SORT_ARMY) && nowobject->getPlayerRepresent() == 0)
            {
                interactionList->addRelation(nowobject , mouseEvent->DR,mouseEvent->UR , CoreEven_JustMoveTo);
            }
            mouseEvent->mouseEventType=NULL_MOUSEEVENT;
        }
        else
        {
            switch(nowobject->getSort())
            {
                case SORT_FARMER:
                    if(nowobject->getPlayerRepresent() != 0) break;
                    switch (object_click->getSort())
                    {
                        case SORT_STATICRES:
                        case SORT_ANIMAL:
                            interactionList->addRelation(nowobject , object_click , CoreEven_Gather);
                            break;
                        case SORT_Building_Resource:
                            if(((Building_Resource*)nowobject)->get_Gatherable()) interactionList->addRelation(nowobject,object_click,CoreEven_Gather);
                            else interactionList->addRelation(nowobject , object_click , CoreEven_FixBuilding);
                            break;
                        case SORT_BUILDING:
                            interactionList->addRelation(nowobject , object_click , CoreEven_FixBuilding);
                            break;
                        default:
                            break;
                    }
                    break;

                case SORT_ARMY:
                    if(nowobject->getPlayerRepresent() != 0) break;
                    switch (object_click->getSort())
                    {
                        case SORT_ANIMAL:
                            interactionList->addRelation(nowobject , object_click , CoreEven_Attacking );
                            break;
                        default:
                            break;
                    }
                    break;

                case SORT_BUILDING:
                    if(nowobject->getPlayerRepresent() != 0 || nowobject->getNum()!= BUILDING_ARROWTOWER) break;

                    switch (object_click->getSort())
                    {
                        case SORT_ANIMAL:
                            interactionList->addRelation(nowobject , object_click , CoreEven_Attacking );
                            break;
                        default:
                            break;
                    }

                    break;
                case SORT_ANIMAL:
                    if(object_click->getSort() == SORT_FARMER)  interactionList->addRelation(nowobject , object_click , CoreEven_Attacking );

                default:
                    break;
            }

            mouseEvent->mouseEventType=NULL_MOUSEEVENT;
        }
    }
}
//后续编写，用于处理AI指令
void Core::manageOrder(int id)
{
//    static bool once = true;
//    static bool second = true;
//    if(once && player[0]->human.size())
//    {
//        once = false;
//        interactionList->addRelation(*(player[0]->human.begin()) , 30*BLOCKSIDELENGTH , 25*BLOCKSIDELENGTH  , CoreEven_CreatBuilding, true, BUILDING_CENTER);
//    }

//    if(second && player[0]->build.size())
//    {
//        second = false;
//        interactionList->addRelation(*(player[0]->build.begin()),CoreEven_BuildingAct , BUILDING_CENTER_CREATEFARMER);
//    }
    ins* NowIns;
    tagGame* tagAIGame;
    if(id==0){
        NowIns=&UsrIns;
        tagAIGame=&tagUsrGame;
    }else{
//        NowIns=&EnemyIns;
    }
    NowIns->lock.lock();
    while(!NowIns->instructions.empty()){
        instruction cur=NowIns->instructions.front();
        NowIns->instructions.pop();
        Coordinate* self=cur.self;
        switch (cur.type) {
        case 0:{    /// type 0:终止对象self的动作
            interactionList->suspendRelation(self);
            break;
        }
        case 1:{    /// type 1:命令村民self走向指定坐标L，U
            interactionList->addRelation(self,cur.L,cur.U,CoreEven_JustMoveTo);
            break;
        }
        case 2:{    /// type 2:命令村民self将工作目标设为obj
            Coordinate* obj=cur.obj;
            switch(self->getSort()){
            case SORT_FARMER:
                switch (obj->getSort()){
                case SORT_STATICRES:
                case SORT_ANIMAL:
                    interactionList->addRelation(self,obj,CoreEven_Gather);
                    break;
                case SORT_BUILDING:
                    interactionList->addRelation(self,obj,CoreEven_FixBuilding);
                default:
                    break;
                }
                break;
            case SORT_ARMY:
                switch (obj->getSort()){
                case SORT_ANIMAL:
                    interactionList->addRelation(self,obj,CoreEven_Attacking );
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }
            break;
        }
        case 3:{    ///type 3:命令村民self在块坐标BlockL,BlockU处建造类型为option的新建筑
            interactionList->addRelation(self,cur.BL,cur.BU,CoreEven_CreatBuilding,0,cur.option);
            break;
        }
        case 4:{    ///type 4:命令建筑self进行option工作
            interactionList->addRelation(self,CoreEven_BuildingAct,cur.option);
            break;
        }
        default:
            break;
        }
        cur.ret=666;
        lockTagGame(id);
        tagAIGame->ins_ret.insert(make_pair(cur.id,cur));
        unlockTagGame(id);
    }
    NowIns->lock.unlock();
}
<<<<<<< HEAD



=======
>>>>>>> f959cb415f8ddd0f7afcb8745459745b43f828b8
