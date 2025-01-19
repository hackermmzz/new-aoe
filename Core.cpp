#include "SelectWidget.h"
#include "Core.h"
#include "EnemyAI.h"
#include "UsrAI.h"

tagInfo Buffer0[2];
tagInfo Buffer1[2];
int buff=0;
tagInfo* currentBuff;

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

    updateByObject();

    loadRelationMap();

    //更新AI用的资源表，该资源表是User/Enemy的通用模板
    theMap->reset_resMap_AI();

    //刷新视野并处理区域探索结果
    theMap->reset_ObjectExploreAndVisible();

    judge_Crush();  //判断并标记碰撞，在Corelist里处理碰撞

    if(mouseEvent->mouseEventType!=NULL_MOUSEEVENT)
    {
        if( mapmoveFrequency == 8) resetNowObject_Click();
        else manageMouseEvent();
    }

    manageOrder(0);
    manageOrder(1);

    interactionList->update();

}

void Core::updateByObject()
{
    //player管理的各个ob更新状态
    for(int playerIndx = 0; playerIndx < MAXPLAYER ; playerIndx++)
    {
        std::list<Human *>::iterator humaniter=player[playerIndx]->human.begin() , humaniterEnd = player[playerIndx]->human.end();
        list<Building*>::iterator builditer = player[playerIndx]->build.begin() , builditerEnd = player[playerIndx]->build.end();
        list<Missile*>::iterator missileiter = player[playerIndx]->missile.begin() , missileiterEnd = player[playerIndx]->missile.end();

        //更新human子类的状态
        while(humaniter != humaniterEnd)
        {
            //如果当前对象需要变换行动状态，如从采集浆果->移动
            if((*humaniter)->needTranState())
            {

                (*humaniter)->setNowState((*humaniter)->getPreState());

                //需要变化的行动为“死亡”，在交互行动表中将其删除
                if((*humaniter)->isDying())
                {
                    call_debugText("red"," "+(*humaniter)->getChineseName()+\
                                   "(编号"+QString::number((*humaniter)->getglobalNum())+")死亡",(*humaniter)->getPlayerRepresent());
                    //在交互行动表中将其删除——删除其作为主体的行动、其作为目标的行动中将目标设置为NULL
                    interactionList->eraseObject(*humaniter);
                    g_Object[(*humaniter)->getglobalNum()] = NULL;

                    //如果Missle的投出者是该ob，则让该missle使用投出者记录
                    player[playerIndx]->deleteMissile_Attacker(*humaniter);
                    player[playerIndx]->humanNumDecrease(*humaniter);
                    deleteOb_setNowobNULL(*humaniter);
                }
                (*humaniter)->setPreStateIsIdle();
            }

            if((*humaniter)->getBlockDR() != INT_MAX && (*humaniter)->getBlockUR() != INT_MAX)
            {
                // 更新人物Y轴偏移（伪三维）
                Human* theHuman = (*humaniter);
                int blockDR = theHuman->getBlockDR(), blockUR = theHuman->getBlockUR();
                int curMapHeight = theMap->cell[blockDR][blockUR].getMapHeight();

                // 斜坡
                if(theMap->isSlope(blockDR, blockUR))
                {
                    if(curMapHeight!=MAPHEIGHT_OCEAN&&(curMapHeight > MAPHEIGHT_MAX - 1 || curMapHeight < MAPHEIGHT_FLAT))
                    {
                        qDebug() << "ERROR: Calculation error in drawing human parts in the function gameUpdate()";
                        qDebug() << "gameUpdate()函数中绘制人类的部分计算错误";
                    }

                    // 判断mapType以确定上升方向
                    int curMapType = theMap->cell[blockDR][blockUR].getMapType();
                    pair<double, double> curHumanCoor = {theHuman->getDR(), theHuman->getUR()};   // 当前人物细节坐标
                    pair<double, double> curBlockCoor = {blockDR * BLOCKSIDELENGTH, blockUR * BLOCKSIDELENGTH}; // 当前人物所在格（最左端的）细节坐标

                    // 左高右低：
                    if(curMapType == MAPTYPE_L1_UPTOLU || curMapType == MAPTYPE_A1_UPTOL || curMapType == MAPTYPE_A3_DOWNTOR || curMapType == MAPTYPE_L0_UPTOLD)
                    {
                        double leftOffsetPercent = fabs((16.0 * gen5 - (curHumanCoor.first - curBlockCoor.first)) / (16.0 * gen5));
                        if(leftOffsetPercent > 1) leftOffsetPercent = 1;
                        theHuman->setMapHeightOffsetY(DRAW_OFFSET * curMapHeight + DRAW_OFFSET * leftOffsetPercent);
                        //qDebug() << "左高右低，leftOffsetPercent == " << leftOffsetPercent << ", MapHeightOffsetY == " << DRAW_OFFSET * curMapHeight + DRAW_OFFSET * leftOffsetPercent;
                    }
                    // 右高左低：
                    else if(curMapType == MAPTYPE_L2_UPTORU || curMapType == MAPTYPE_A3_UPTOR || curMapType == MAPTYPE_A1_DOWNTOL || curMapType == MAPTYPE_L3_UPTORD)
                    {
                        double rightOffsetPercent = fabs((curHumanCoor.second - curBlockCoor.second) / (16.0 * gen5));
                        if(rightOffsetPercent > 1) rightOffsetPercent = 1;
                        theHuman->setMapHeightOffsetY(DRAW_OFFSET * curMapHeight + DRAW_OFFSET * rightOffsetPercent);
                        //qDebug() << "右高左低， rightOfsetPercent == " << rightOffsetPercent << ", MapHeightOffsetY == " << DRAW_OFFSET * curMapHeight + DRAW_OFFSET * rightOffsetPercent;
                    }
                    // 下高上低：
                    else if(curMapType == MAPTYPE_A0_UPTOD || curMapType == MAPTYPE_A2_DOWNTOU)
                    {
                        double downOffsetPercent = (16.0 * gen5 - ((curHumanCoor.second - curBlockCoor.second) - (curHumanCoor.first - curBlockCoor.first))) / (16.0 * gen5);
                        if(downOffsetPercent > 1) downOffsetPercent = 1;
                        theHuman->setMapHeightOffsetY(DRAW_OFFSET * curMapHeight + DRAW_OFFSET * downOffsetPercent);
                        //qDebug() << "下高上低，downOffsetPercent == " << downOffsetPercent << ", MapHeightOffsetY == " << DRAW_OFFSET * curMapHeight + DRAW_OFFSET * downOffsetPercent;
                    }
                    // 上高下低：
                    else if(curMapType == MAPTYPE_A2_UPTOU || curMapType == MAPTYPE_A0_DOWNTOD)
                    {
                        double upOffsetPercent = ((curHumanCoor.second - curBlockCoor.second) - (curHumanCoor.first - curBlockCoor.first)) / (16.0 * gen5);
                        if(upOffsetPercent > 1) upOffsetPercent = 1;
                        theHuman->setMapHeightOffsetY(DRAW_OFFSET * curMapHeight + DRAW_OFFSET * upOffsetPercent);
                        //qDebug() << "上高下低，upOffsetPercent == " << upOffsetPercent << ", MapHeightOffsetY == " << DRAW_OFFSET * curMapHeight + DRAW_OFFSET * upOffsetPercent;
                    }
                }
                // 平地或者海洋
                else theHuman->setMapHeightOffsetY(curMapHeight * DRAW_OFFSET);
            }

            if((*humaniter)->isDying())
            {
                if((*humaniter)->get_isActionEnd())
                {
                    humaniter = player[playerIndx]->deleteHuman(humaniter);
                }
                else
                {
                    (*humaniter)->nextframe();
                    humaniter++;
                }
            }
            else
            {
//                if((*humaniter)->getSort() == SORT_ARMY && (*humaniter)->getNum()!=AT_SCOUT && get_IsObjectFree(*humaniter))
//                    theMap->add_Map_Vision(*humaniter);

                interactionList->conduct_Attacked(*humaniter);
                (*humaniter)->updateLU();

                (*humaniter)->nextframe();

                //移动后，记录当前位置
                theMap->add_Map_Object(*humaniter);
                //如果正在移动，需要判断碰撞，加入判断碰撞对象表
                if((*humaniter)->isWalking()) moveOb_judCrush.push_back(*humaniter);

                //更新视野
                if(playerIndx == 0) theMap->reset_CellExplore(*humaniter);

                humaniter++;
            }

        }

        while(builditer != builditerEnd)
        {
            interactionList->conduct_Attacked(*builditer);
            if((*builditer)->isDie() || ((*builditer)->getSort() == SORT_Building_Resource && !((Building_Resource*)(*builditer))->is_Surplus()))
            {
                if(!(*builditer)->isDie())
                    call_debugText("green"," "+(*builditer)->getChineseName()+\
                                   "(编号:"+QString::number((*builditer)->getglobalNum())+")采集完成",(*builditer)->getPlayerRepresent());
                else
                    call_debugText("red"," "+(*builditer)->getChineseName()+\
                                   "(编号:"+QString::number((*builditer)->getglobalNum())+")被摧毁",(*builditer)->getPlayerRepresent());

                g_Object[(*builditer)->getglobalNum()] = NULL;
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

                if((*builditer)->isFinish() && !(*builditer)->isConstructed())
                {
                    std::string clickSound;

                    (*builditer)->initAction();

                    if((*builditer)->getNum()==BUILDING_HOME||(*builditer)->getNum()==BUILDING_FARM)
                        usrScore.update(_BUILDING1);
                    else
                        usrScore.update(_BUILDING2);

                    player[playerIndx]->finishBuild(*builditer);
                    if((*builditer)->getNum() == BUILDING_STOCK || (*builditer)->getNum() == BUILDING_GRANARY)
                        interactionList->resourceBuildHaveChange();

                    clickSound = (*builditer)->getSound_Click();

                    if(!clickSound.empty()) //建筑建造完成时，出发一次点击音效
                        soundQueue.push(clickSound);
                }

                (*builditer)->nextframe();

                builditer++;
            }
        }

        while(missileiter != missileiterEnd)
        {
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
                        call_debugText("red"," "+(*animaliter)->getChineseName()+"(编号"+QString::number((*animaliter)->getglobalNum())+")死亡", REPRESENT_BOARDCAST_MESSAGE);

                    interactionList->eraseRelation(*animaliter);
                }
                (*animaliter)->setPreStateIsIdle();
            }

            if((*animaliter)->isDie())
            {
                (*animaliter)->nextframe();
            }
            else
            {
                (*animaliter)->updateLU();
                (*animaliter)->nextframe();
                if((*animaliter)->isTree())
                    (*animaliter)->initAvengeObject();
                interactionList->conduct_Attacked(*animaliter);

                //没有死亡的瞪羚和狮子需要监视地图
                if(((*animaliter)->getNum() == ANIMAL_GAZELLE ||(*animaliter)->getNum() == ANIMAL_LION) && get_IsObjectFree(*animaliter) )
                    theMap->add_Map_Vision(*animaliter);

                //移动后，记录当前位置
                theMap->add_Map_Object(*animaliter);
                //如果正在移动，需要判断碰撞，加入判断碰撞对象表
                if((*animaliter)->isWalking()) moveOb_judCrush.push_back(*animaliter);
            }
            animaliter++;
        }
        else if(!(*animaliter)->isDisappearing())
        {
            (*animaliter)->nextframe();
            call_debugText("green"," "+(*animaliter)->getChineseName()+"(编号:"+QString::number((*animaliter)->getglobalNum())+")采集完成",0);
            g_Object[(*animaliter)->getglobalNum()] = NULL;
            interactionList->eraseObject(*animaliter);   //行动表中animal设为null
            deleteOb_setNowobNULL(*animaliter);

            animaliter++;
        }
        else
        {
            if((*animaliter)->get_isActionEnd())
            {
                animaliter = theMap->deleteAnimal(animaliter);
            }
            else
            {
                (*animaliter)->nextframe();
                animaliter++;
            }
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
            call_debugText("green"," "+(*SRiter)->getChineseName()+"(编号:"+QString::number((*SRiter)->getglobalNum())+")采集完成",0);

            g_Object[(*SRiter)->getglobalNum()] = NULL;
            interactionList->eraseObject(*SRiter);
            deleteOb_setNowobNULL(*SRiter);
            SRiter = theMap->deleteStaticRes(SRiter);
        }
    }
}

void Core::updateByPlayer(int id){
    Player* self=player[id];
    tagInfo& taginfo = currentBuff[id];
    //更新基础数据
    taginfo.Human_MaxNum=self->getMaxHumanNum();
    taginfo.Gold=self->getGold();
    taginfo.Stone=self->getStone();
    taginfo.Meat=self->getFood();
    taginfo.Wood=self->getWood();
    taginfo.civilizationStage=self->getCiv();
    taginfo.GameFrame=g_frame;
    //更新人口数据
    for(Human* human:self->human)
    {
        if(g_Object[human->getglobalNum()] == NULL) continue;

        tagHuman taghuman;
        taghuman.SN=human->getglobalNum();
        taghuman.Blood=human->getBlood();
        taghuman.DR=human->getDR();
        taghuman.UR=human->getUR();
        taghuman.BlockDR=human->getBlockDR();
        taghuman.BlockUR=human->getBlockUR();
        taghuman.Blood=human->getBlood();
        taghuman.NowState=interactionList->getNowPhaseNum(human);
        taghuman.attack=human->getATK();
        taghuman.meleeDefense=human->getDEF(ATTACKTYPE_CLOSE);
        taghuman.rangedDefense=human->getDEF(ATTACKTYPE_SHOOT);

        taghuman.WorkObjectSN=interactionList->getObjectSN(human);

        taghuman.DR0=human->getDR0();
        taghuman.UR0=human->getUR0();
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
            taginfo.farmers.push_back(tagfarmer);
            //同步更新其他ai的信息
            for(int i=0;i<NOWPLAYER;i++){
                if(i==id) {continue;}
                if(farmer->getvisible()==1||i!=0)
                    currentBuff[i].enemy_farmers.push_back(tagfarmer.toEnemy());
            }
        }else if(human->getSort()==SORT_ARMY)
        {
            Army* army=static_cast<Army*> (human);
            tagArmy tagarmy;
            tagarmy.cast_from(taghuman);
            tagarmy.Sort=army->getNum();
            taginfo.armies.push_back(tagarmy);
            //同步更新其他ai的信息
            for(int i=0;i<NOWPLAYER;i++)
            {
                if(i==id) {continue;}
                if(army->getvisible()==1||i!=0)
                    currentBuff[i].enemy_armies.push_back(tagarmy.toEnemy());
            }
        }
    }

    //更新动物数据
    for(Animal* animal:theMap->animal)
    {
        if(g_Object[animal->getglobalNum()] == NULL) continue;

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
        resource.BlockDR=animal->getBlockDR();
        resource.BlockUR=animal->getBlockUR();
        resource.DR=animal->getDR();
        resource.UR=animal->getUR();
        resource.Blood=animal->getBlood();
        resource.Cnt=animal->get_Cnt();
        if(id==1||animal->getexplored()==1)
            taginfo.resources.push_back(resource);
    }

    //更新资源数据
    for(StaticRes* staticRes: theMap->staticres){
        tagResource resource;
        resource.SN=staticRes->getglobalNum();
        resource.DR=staticRes->getDR();
        resource.UR=staticRes->getUR();
        resource.BlockDR=staticRes->getBlockDR();
        resource.BlockUR=staticRes->getBlockUR();
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
        if(id==1||staticRes->getexplored()==1)
            taginfo.resources.push_back(resource);
    }

    //更新建筑数据
    for(Building* build:self->build){
        tagBuilding building;
        building.SN=build->getglobalNum();
        building.BlockDR=build->getBlockDR();
        building.BlockUR=build->getBlockUR();
        building.Blood=build->getBlood();
        building.MaxBlood=build->getMaxBlood();
        building.Percent=build->getPercent();
        if(build->getNum()==BUILDING_ARROWTOWER){
            int obj=interactionList->getObjectSN(build);
            if(build->isAttacking()&&obj!=-1){
                building.Project=obj;
            }else{
                building.Project=0;
            }
        }else{
            building.Project=build->getActNum();
        }
        building.ProjectPercent=build->getActPercent();
        if(build->getSort()==SORT_Building_Resource){
            building.Type=BUILDING_FARM;
            Building_Resource* building_resource=static_cast<Building_Resource*> (build);
            building.Cnt=building_resource->get_Cnt();
        }else{
            building.Type=build->getNum();
            building.Cnt=-1;
        }
        taginfo.buildings.push_back(building);
        for(int i=0;i<NOWPLAYER;i++){
            if(i==id){continue;}
            if(build->getexplored()==1||i!=0)
                currentBuff[i].enemy_buildings.push_back(building.toEnemy());
        }
    }

}
/**
 *更新tagGame中的数组大小，资源地图
 */
void Core::updateCommon(tagInfo* taginfo){
    for (int i = 0; i < MAP_L; ++i) {
        for (int j = 0; j < MAP_U; ++j) {
            taginfo->theMap[i][j] = theMap->map_Height[i][j];
        }
    }
}


void Core::infoShare(){
    currentBuff=(buff == 0) ? Buffer0 : Buffer1;
    for(int i=0;i<NOWPLAYER;i++){
        currentBuff[i].clear();
    }
    for(int i=0;i<NOWPLAYER;i++){
        updateByPlayer(i);
    }
    for(int i=0;i<NOWPLAYER;i++){
        updateCommon(&currentBuff[i]);
    }

    tagUsrGame.update(&currentBuff[0]);
    tagEnemyGame.update(&currentBuff[1]);
    buff=1-buff;    //轮换缓存
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
    resetNowObject_Click();
    if(mouseEvent->mouseEventType==RIGHT_PRESS && nowobject!=NULL)
    {
        if( object_click== NULL )
        {
            if( (nowobject->getSort() == SORT_FARMER || nowobject->getSort() == SORT_ARMY)\
                    && nowobject->getPlayerRepresent() == NOWPLAYERREPRESENT)
                interactionList->addRelation(nowobject , mouseEvent->DR,mouseEvent->UR , CoreEven_JustMoveTo);
            mouseEvent->mouseEventType=NULL_MOUSEEVENT;
        }
        else
        {
            Farmer* farmer = NULL;
            Building* buildOb = NULL;

            switch(nowobject->getSort())
            {
                case SORT_FARMER:
                    if(nowobject->getPlayerRepresent() != NOWPLAYERREPRESENT) break;
                    switch (object_click->getSort())
                    {
                        case SORT_STATICRES:
                        case SORT_ANIMAL:
                            interactionList->addRelation(nowobject , object_click , CoreEven_Gather);
                            break;
                        case SORT_Building_Resource:
                            if(object_click->getPlayerRepresent() != NOWPLAYERREPRESENT)
                                interactionList->addRelation(nowobject,object_click,CoreEven_Attacking);
                            else
                            {
                                if(((Building_Resource*)object_click)->get_Gatherable())
                                    interactionList->addRelation(nowobject,object_click,CoreEven_Gather);
                                else
                                    interactionList->addRelation(nowobject , object_click , CoreEven_FixBuilding);
                            }
                            break;
                        case SORT_BUILDING:
                            if(object_click->getPlayerRepresent() != NOWPLAYERREPRESENT)
                                interactionList->addRelation(nowobject,object_click,CoreEven_Attacking);
                            else
                            {
                                farmer = (Farmer*)nowobject;
                                object_click->printer_ToBuilding((void**)&buildOb);

                                if(!farmer->get_isEmptyBackpack() && buildOb!=NULL && buildOb->isConstructed() &&\
                                        buildOb->isMatchResourceType(farmer->getResourceSort()))
                                    interactionList->addRelation(nowobject , object_click , CoreEven_Gather);
                                else
                                    interactionList->addRelation(nowobject , object_click , CoreEven_FixBuilding);
                            }
                            break;
                        case SORT_ARMY:
                        case SORT_FARMER:
                            if(object_click->getPlayerRepresent() != NOWPLAYERREPRESENT)
                                interactionList->addRelation(nowobject,object_click,CoreEven_Attacking);
                            break;
                        default:
                            break;
                    }
                    break;

                case SORT_ARMY:
                    if(nowobject->getPlayerRepresent() != NOWPLAYERREPRESENT) break;
                    switch (object_click->getSort())
                    {
//                        case SORT_ANIMAL:
//                            if(!((Animal*)object_click)->isTree())
//                                interactionList->addRelation(nowobject , object_click , CoreEven_Attacking );
//                            break;
                        case SORT_Building_Resource:
                        case SORT_BUILDING:
                        case SORT_ARMY:
                        case SORT_FARMER:
                            if(object_click->getPlayerRepresent() != nowobject->getPlayerRepresent())
                                interactionList->addRelation(nowobject,object_click,CoreEven_Attacking);
                            break;
                        default:
                            break;
                    }
                    break;

                case SORT_BUILDING:
                    if(nowobject->getPlayerRepresent() != NOWPLAYERREPRESENT || nowobject->getNum()!= BUILDING_ARROWTOWER) break;

                    switch (object_click->getSort())
                    {
//                        case SORT_ANIMAL:
//                            interactionList->addRelation(nowobject , object_click , CoreEven_Attacking );
//                            break;
                        case SORT_Building_Resource:
                            if(object_click->getPlayerRepresent() != NOWPLAYERREPRESENT)
                                interactionList->addRelation(nowobject,object_click,CoreEven_Attacking);
                            break;
                        case SORT_BUILDING:
                            if(object_click->getPlayerRepresent() != NOWPLAYERREPRESENT)
                                interactionList->addRelation(nowobject,object_click,CoreEven_Attacking);
                            else
                                interactionList->addRelation(nowobject , object_click , CoreEven_FixBuilding);
                            break;
                        case SORT_ARMY:
                        case SORT_FARMER:
                            if(object_click->getPlayerRepresent() != NOWPLAYERREPRESENT)
                                interactionList->addRelation(nowobject,object_click,CoreEven_Attacking);
                            break;
                        default:
                            break;
                    }
                    break;
                case SORT_ANIMAL:
//                    if(object_click->getSort() == SORT_FARMER)  interactionList->addRelation(nowobject , object_click , CoreEven_Attacking );

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
    ins* NowIns;
    tagGame* tagAIGame;
    Farmer* farmer = NULL;
    Building* buildOb = NULL;

    if(id==0)
    {
        NowIns=&UsrIns;
        tagAIGame=&tagUsrGame;
    }
    else
    {
        NowIns=&EnemyIns;
        tagAIGame=&tagEnemyGame;
    }
    NowIns->lock.lock();

    while(!NowIns->instructions.empty())
    {
        instruction cur=NowIns->instructions.front();
        NowIns->instructions.pop();
        Coordinate* self=cur.self;
        int ret=0;

        //判断是否是己方对象 并 再次判断SN对象是否存在（可能存在ai下达指令后对象被删除了的情况）
        if(g_Object[cur.SN] == NULL || self->getPlayerRepresent()!=id)
        {
            cur.ret=ACTION_INVALID_SN;
            tagAIGame->insertInsRet(cur.id,cur);
            continue;
        }

        switch (cur.type) {
        case 0:{    /// type 0:终止对象self的动作
            interactionList->suspendRelation(self);
            break;
        }
        case 1:{    /// type 1:命令村民self走向指定坐标L，U
            ret=interactionList->addRelation(self,cur.DR,cur.UR,CoreEven_JustMoveTo);
            if(ret == ACTION_SUCCESS)
                call_debugText("green"," HumanMove:"+self->getChineseName()+" "+QString::number(self->getglobalNum())+" 移动至 ("+QString::number(cur.DR)+","+QString::number(cur.UR)+")",id);
            break;
        }
        case 2:{    /// type 2:命令村民self将工作目标设为obj
            Coordinate* obj=cur.obj;
            if(obj==NULL || g_Object[cur.obSN] == NULL)
            {
                ret=ACTION_INVALID_OBSN;
                break;
            }
            switch(self->getSort()){
            case SORT_FARMER:
                farmer = (Farmer*)self;

                if(self==obj){
                    ret=deleteSelf(self);
                    if(ret == ACTION_SUCCESS){
                        call_debugText("green"," HumanAction:"+self->getChineseName()+" "+QString::number(self->getglobalNum())+"  被删除",id);
                    }
                    break;
                }

                switch (obj->getSort()){
                case SORT_STATICRES:
                case SORT_ANIMAL:
                    ret=interactionList->addRelation(self,obj,CoreEven_Gather);
                    if(ret == ACTION_SUCCESS)
                        call_debugText("green"," HumanAction:"+self->getChineseName()+" "+QString::number(self->getglobalNum())+" 设置工作目标为 "+ obj->getChineseName() +" "+ QString::number(obj->getglobalNum()),id);
                    break;
                case SORT_BUILDING:
                    if(self->getPlayerRepresent() == obj->getPlayerRepresent())
                    {
                        obj->printer_ToBuilding((void**)&buildOb);

                        if(!farmer->get_isEmptyBackpack() && buildOb!=NULL && buildOb->isConstructed() \
                                && buildOb->isMatchResourceType(farmer->getResourceSort()))
                            ret=interactionList->addRelation(self , obj , CoreEven_Gather);
                        else
                            ret=interactionList->addRelation(self , obj , CoreEven_FixBuilding);

                        if(ret == ACTION_SUCCESS)
                            call_debugText("green"," HumanAction:"+self->getChineseName()+" "+QString::number(self->getglobalNum())+" 设置工作目标为 "+ obj->getChineseName() +" "+ QString::number(obj->getglobalNum()),id);
                    }
                    else
                    {
                        ret=interactionList->addRelation(self,obj,CoreEven_Attacking );
                        if(ret == ACTION_SUCCESS)
                            call_debugText("green"," HumanAction:"+self->getChineseName()+" "+QString::number(self->getglobalNum())+" 设置攻击目标为 "+ obj->getChineseName() +" "+ QString::number(obj->getglobalNum()),id);
                    }
                    break;

                case SORT_Building_Resource:

                    if(self->getPlayerRepresent() == obj->getPlayerRepresent())
                    {
                        if(((Building_Resource*)obj)->get_Gatherable()) ret=interactionList->addRelation(self,obj,CoreEven_Gather);
                        else ret=interactionList->addRelation(self,obj,CoreEven_FixBuilding);

                        if(ret == ACTION_SUCCESS)
                            call_debugText("green"," HumanAction:"+self->getChineseName()+" "+QString::number(self->getglobalNum())+" 设置工作目标为 "+ obj->getChineseName() +" "+ QString::number(obj->getglobalNum()),id);
                    }else
                    {
                        ret=interactionList->addRelation(self,obj,CoreEven_Attacking );
                        if(ret == ACTION_SUCCESS)
                            call_debugText("green"," HumanAction:"+self->getChineseName()+" "+QString::number(self->getglobalNum())+" 设置攻击目标为 "+ obj->getChineseName() +" "+ QString::number(obj->getglobalNum()),id);
                    }
                    break;
                case SORT_ARMY:
                    if(self->getPlayerRepresent() != obj->getPlayerRepresent())
                    {
                        ret=interactionList->addRelation(self,obj,CoreEven_Attacking );
                        if(ret == ACTION_SUCCESS)
                            call_debugText("green"," HumanAction:"+self->getChineseName()+" "+QString::number(self->getglobalNum())+" 设置攻击目标为 "+ obj->getChineseName() +" "+ QString::number(obj->getglobalNum()),id);
                    }
                    break;
                default:
                    break;
                }
                break;

            case SORT_BUILDING:
                if(self->getNum() != BUILDING_ARROWTOWER)
                    break;
            case SORT_ARMY:
                if(self==obj&&self->getSort()==SORT_ARMY){
                    ret=deleteSelf(self);
                    if(ret == ACTION_SUCCESS){
                        call_debugText("green"," HumanAction:"+self->getChineseName()+" "+QString::number(self->getglobalNum())+"  被删除",id);
                    }
                    break;
                }
                switch (obj->getSort()){
                case SORT_ANIMAL:
                    ret=ACTION_INVALID_OBSN;
                    // if(!((Animal*)obj)->isTree())
                    // {
                    //     ret=interactionList->addRelation(self,obj,CoreEven_Attacking );
                    //     if(ret == ACTION_SUCCESS&& id == 0)
                    //         call_debugText("green"," HumanAction:"+self->getChineseName()+" "+QString::number(self->getglobalNum())+" 设置攻击目标为 "+ obj->getChineseName() +" "+ QString::number(obj->getglobalNum()));
                    // }
                    break;
                case SORT_BUILDING:
                case SORT_Building_Resource:
                case SORT_FARMER:
                case SORT_ARMY:
                    if(self->getPlayerRepresent() != obj->getPlayerRepresent())
                    {
                        ret=interactionList->addRelation(self,obj,CoreEven_Attacking );
                        if(ret == ACTION_SUCCESS )
                            call_debugText("green"," HumanAction:"+self->getChineseName()+" "+QString::number(self->getglobalNum())+" 设置攻击目标为 "+ obj->getChineseName() +" "+ QString::number(obj->getglobalNum()),id);
                    }
                default:
                    break;
                }
                break;
            default:
                ret = ACTION_INVALID_SN;
                break;
            }
            break;
        }
        case 3:{    ///type 3:命令村民self在块坐标BlockL,BlockU处建造类型为option的新建筑
            ret=interactionList->addRelation(self, cur.BlockDR, cur.BlockUR, CoreEven_CreatBuilding, true, cur.option);
            if(ret == ACTION_SUCCESS)
                call_debugText("green"," HumanBuild:"+self->getChineseName()+" "+QString::number(self->getglobalNum())+" 开始在块坐标 ("+QString::number(cur.BlockDR)+","+QString::number(cur.BlockUR)+") 处建造 Building_"+ QString::number(cur.option),id);
            break;
        }
        case 4:{    ///type 4:命令建筑self进行option工作
            if(cur.option==0){
                if(self->getSort()!=SORT_BUILDING){
                    ret=ACTION_INVALID_SN;
                }else{
                    interactionList->suspendRelation(self);
                    ret=ACTION_SUCCESS;
                }
            }else{
                ret=interactionList->addRelation(self,CoreEven_BuildingAct,cur.option);
            }
            if(ret == ACTION_SUCCESS)
                call_debugText("green"," BuildAction:"+self->getChineseName()+" "+QString::number(self->getglobalNum())+" 执行行动 ACTION_"+QString::number(cur.option),id);
            break;
        }
        default:
            break;
        }
        cur.ret=ret;
        tagAIGame->insertInsRet(cur.id,cur);
    }
    NowIns->lock.unlock();
}


int Core::deleteSelf(Coordinate* object) //删除对象，返回错误码
{
    BloodHaver *bloodOb = NULL;

    if(object->getPlayerRepresent()!=0) return ACTION_INVALID_SN;

    object->printer_ToBloodHaver((void**) &bloodOb);
    if(bloodOb && !bloodOb->isDie())
        bloodOb->updateBlood(bloodOb->getMaxBlood());

    return ACTION_SUCCESS;
}

//判断moveObject碰撞
void Core::judge_Crush()
{
    /**
    *   本函数用于判断移动中的MoveObject对象是否发生碰撞
    *
    *   函数操作基于moveOb_judCrush、 theMap->map_Object
    *   调用此函数前务必保证以上两个数组已完成本帧的更新
    */

    vector<Point> judBlock;
    int labSize_jud = moveOb_judCrush.size(),labSize,obSize;
    MoveObject* judOb;
    Coordinate* barrierOb;
    for(int i = 0 ; i<labSize_jud; i++)
    {
        judOb = moveOb_judCrush[i];
        judBlock = judOb->get_JudCrush_Block();

        labSize = judBlock.size();
        for(int j = 0 ; j<labSize; j++)
        {
            if(judOb->getCrashOb()!=NULL) break;
            obSize = theMap->map_Object[ (int)judBlock[j].x ][ (int)judBlock[j].y ].size();

            if(obSize == 0) continue;
            for(int k = 0 ; k < obSize; k++)
            {
                barrierOb = theMap->map_Object[ (int)judBlock[j].x ][ (int)judBlock[j].y ][k];
                if(judOb == barrierOb ) continue;

                /****当前取消移动物体之间的碰撞******/
//                if(barrierOb->getSort() == SORT_FARMER || barrierOb->getSort() == SORT_ARMY) continue;
//                if(barrierOb->getSort() == SORT_ANIMAL && !((Animal*)barrierOb)->isTree()) continue;
                if(barrierOb->getSort() == SORT_STATICRES && barrierOb->getNum() == NUM_STATICRES_Bush) continue;   //浆果不碰撞
                /****当前取消移动物体之间的碰撞******/

                //判断碰撞，碰撞箱有重合
                if(judOb->isCrash(barrierOb))break;
            }
        }
    }

    moveOb_judCrush.clear();
}

void Core::loadRelationMap()
{
    //更新寻路用障碍表
    theMap->loadBarrierMap_ByObjectMap();
    //更新寻路地图模板
    theMap->loadfindPathMapTemperature();
}


void Core::resetNowObject_Click(bool isStop)
{
    if(mouseEvent->mouseEventType==LEFT_PRESS)
    {
        if(isStop)
        {
            call_debugText("blue", " 细节坐标 ("+QString::number(mouseEvent->DR)+","+QString::number(mouseEvent->UR)\
                           +"), 块坐标 (" + QString::number((int)(mouseEvent->DR/BLOCKSIDELENGTH))+","+QString::number((int)(mouseEvent->UR/BLOCKSIDELENGTH))+")", REPRESENT_BOARDCAST_MESSAGE);
        }

        nowobject=g_Object[memorymap[mouseEvent->memoryMapX][mouseEvent->memoryMapY]];
        if(nowobject!=NULL)
            call_debugText("blue"," 点击对象为："+nowobject->getChineseName()+", SN:"+ QString::number(nowobject->getglobalNum()), REPRESENT_BOARDCAST_MESSAGE);
        requestSound_Click(nowobject);
        sel->initActs();
        mouseEvent->mouseEventType=NULL_MOUSEEVENT;
    }
}

void Core::requestSound_Click( Coordinate* object )
{
    if(nowobject == NULL) return;
    Building* buildOb = NULL;
    object->printer_ToBuilding((void**)&buildOb);

    if(buildOb!=NULL && !buildOb->isFinish()) return;

    std::string clickSound = object->getSound_Click();

    if(!clickSound.empty())
        soundQueue.push(clickSound);

    return;
}
