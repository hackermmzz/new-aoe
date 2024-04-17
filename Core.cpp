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


            if((*humaniter)->isDying() && (*humaniter)->get_isActionEnd()) humaniter = player[playerIndx]->deleteHuman(humaniter);
            else
            {
                interactionList->conduct_Attacked(*humaniter);
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
                    interactionList->eraseRelation(*animaliter);
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
        if((*SRiter)->is_Surplus())
        {
            (*SRiter)->nextframe();
            SRiter++;
        }
        else
        {
            interactionList->eraseObject(*SRiter);
            SRiter = theMap->deleteStaticRes(SRiter);
        }
    }

    if(mouseEvent->mouseEventType!=NULL_MOUSEEVENT) manageMouseEvent();
    manageOrder();
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

//    while(!instructions.empty()){
//        instruction cur=instructions.front();
//        instructions.pop();
//        Coordinate* self=cur.self;
//        switch (cur.type) {
//        case 0:{    /// type 0:终止对象self的动作
//            interactionList->suspendRelation(self);
//            break;
//        }
//        case 1:{    /// type 1:命令村民self走向指定坐标L0，U0
//            Point des=cur.destination;
//            interactionList->addRelation(self,des.x,des.y,CoreEven_JustMoveTo);
//            break;
//        }
//        default:
//            break;
//        }
//    }
}

