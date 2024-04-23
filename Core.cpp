#include "SelectWidget.h"
#include "Core.h"

Core::Core(Map* theMap, Player* player[], int** memorymap,MouseEvent *mouseEvent)
{
    this->theMap = theMap;
    this->player = player;
    this->memorymap = memorymap;
    this->mouseEvent = mouseEvent;
    this->interactionList = new Core_List( this->theMap , this->player );
}

void Core::gameUpdate()
{
    for(int playerIndx = 0; playerIndx < MAXPLAYER ; playerIndx++)
    {
        std::list<Human *>::iterator humaniter=player[playerIndx]->human.begin() , humaniterEnd = player[playerIndx]->human.end();
        list<Building*>::iterator builditer = player[playerIndx]->build.begin() , builditerEnd = player[playerIndx]->build.end();
        list<Missile*>::iterator missileiter = player[playerIndx]->missile.begin() , missileiterEnd = player[playerIndx]->missile.end();

        while(humaniter!=humaniterEnd)
        {       
            if((*humaniter)->needTranState())
            {
                (*humaniter)->setNowState((*humaniter)->getPreState());
                if((*humaniter)->isDying())
                {
                    interactionList->eraseObject(*humaniter);
                    player[playerIndx]->deleteMissile_Attacker(*humaniter);
                }
                (*humaniter)->setPreStateIsIdle();
            }
            interactionList->conduct_Attacked(*humaniter);

            if((*humaniter)->isDying() && (*humaniter)->get_isActionEnd()) humaniter = player[playerIndx]->deleteHuman(humaniter);
            else
            {
                (*humaniter)->nextframe();
                (*humaniter)->updateLU();

                humaniter++;
            }
        }

        while(builditer!= builditerEnd)
        {
            interactionList->conduct_Attacked(*builditer);
            if((*builditer)->isDie())
            {
                player[playerIndx]->deleteMissile_Attacker(*builditer);
                interactionList->eraseObject(*builditer);
                builditer= player[playerIndx]->deleteBuilding(builditer);
            }
            else
            {
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
                    interactionList->suspendRelation(*animaliter);
                }
                (*animaliter)->setPreStateIsIdle();
            }
            interactionList->conduct_Attacked(*animaliter);
            (*animaliter)->nextframe();
            (*animaliter)->updateLU();

            animaliter++;
        }
        else
        {
            interactionList->eraseObject(*animaliter);   //行动表中animal设为null
            animaliter = theMap->deleteAnimal(animaliter);
        }
    }

    std::list<StaticRes*>::iterator SRiter=theMap->staticres.begin();
    while(SRiter!=theMap->staticres.end())
    {
        (*SRiter)->nextframe();
        SRiter++;
    }

    if(mouseEvent->mouseEventType!=NULL_MOUSEEVENT) manageMouseEvent();
    if(AIfinished) { manageOrder();}
    interactionList->manageRelationList();
}

void Core::infoShare(){
    Player* self=player[0];
    AIGame.Human_MaxNum=self->getMaxHumanNum();
    AIGame.Gold=self->getGold();
    AIGame.Stone=self->getStone();
    AIGame.Meat=self->getFood();
    AIGame.Wood=self->getWood();
    AIGame.civilizationStage=self->getCiv();
    AIGame.GameFrame=g_frame;
    AIGame.humans.clear();
    for(Human* human:self->human){
        tagHuman taghuman;
        taghuman.SN=human->getglobalNum();
        taghuman.Blood=human->getBlood();
        taghuman.L=human->getDR();
        taghuman.U=human->getUR();
        taghuman.BlockL=human->getBlockDR();
        taghuman.BlockU=human->getBlockUR();
        taghuman.Blood=human->getBlood();
        taghuman.NowState=interactionList->getNowPhaseNum(human);
        if(taghuman.NowState==HUMAN_STATE_IDLE){
            taghuman.WorkObjectSN=-1;
        }else{
            taghuman.WorkObjectSN=interactionList->getObjectSN(human);
        }
        taghuman.L0=human->getDR0();
        taghuman.U0=human->getUR0();
        if(human->getSort()==SORT_FARMER){
            Farmer* farmer=static_cast<Farmer*> (human);
            taghuman.Resource=farmer->getResourceNowHave();
            if(taghuman.Resource==0){
                taghuman.ResourceSort=-1;
            }else{
                taghuman.ResourceSort=farmer->getResourceSort();
            }
        }else{
            taghuman.ResourceSort=-1;
            taghuman.Resource=0;
        }
        AIGame.humans.push_back(taghuman);
    }

    AIGame.resources.clear();
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
        AIGame.resources.push_back(resource);
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
        AIGame.resources.push_back(resource);
    }

    AIGame.buildings.clear();
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
        if(build->getSort()==SORT_FARM){
            building.Type=BUILDING_FARM;
            building.Cnt=build->getCnt();
        }else{
            building.Type=build->getNum();
            building.Cnt=-1;
        }
        AIGame.buildings.push_back(building);
    }
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
            if( nowobject->getSort() == SORT_FARMER || nowobject->getSort() == SORT_ARMY )
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
                    switch (object_click->getSort())
                    {
                        case SORT_STATICRES:
                        case SORT_ANIMAL:
                            interactionList->addRelation(nowobject , object_click , CoreEven_Gather);
                            break;
                        case SORT_BUILDING:
                            interactionList->addRelation(nowobject , object_click , CoreEven_FixBuilding);
                        default:
                            break;
                    }
                    break;

                case SORT_ARMY:
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
void Core::manageOrder()
{
    while(!instructions.empty()){
        instruction cur=instructions.front();
        instructions.pop();
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
        }
        case 3:{    ///type 3:命令村民self在块坐标BlockL,BlockU处建造类型为option的新建筑
//            interactionList->addRelation(self,BLOCKSIDELENGTH*cur.BL,BLOCKSIDELENGTH*cur.BU,CoreEven_CreatBuilding,0,cur.option);
            break;
        }
        case 4:{    ///type 4:命令建筑self进行option工作

//            int foodcost=0;
//            int woodcost=0;
//            int stonecost=0;
//            switch (cur.option) {
//            case BUILDING_CENTER_CREATEFARMER:
//                foodcost=BUILDING_CENTER_CREATEFARMER_FOOD;
//                break;
//            case BUILDING_CENTER_UPGRADE:
//                foodcost=BUILDING_CENTER_UPGRADE_FOOD;
//                break;
//            case BUILDING_GRANARY_ARROWTOWER:
//                foodcost=BUILDING_GRANARY_ARROWTOWER_FOOD;
//                break;
//            case BUILDING_MARKET_WOOD_UPGRADE:
//                foodcost=BUILDING_MARKET_WOOD_UPGRADE_FOOD;
//                woodcost=BUILDING_MARKET_WOOD_UPGRADE_WOOD;
//                break;
//            case BUILDING_MARKET_STONE_UPGRADE:
//                foodcost=BUILDING_MARKET_STONE_UPGRADE_FOOD;
//                stonecost=BUILDING_MARKET_STONE_UPGRADE_STONE;
//                break;
//            case BUILDING_MARKET_FARM_UPGRADE:
//                foodcost=BUILDING_MARKET_FARM_UPGRADE_FOOD;
//                woodcost=BUILDING_MARKET_FARM_UPGRADE_WOOD;
//                break;
//            case BUILDING_CANCEL:
//                break;
//            default:
//                break;
//            }


        }

        default:
            break;
        }
    }
}

