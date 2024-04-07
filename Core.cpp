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
    manageOrder();
    interactionList->manageRelationList();
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
                        case SORT_ANIMAL:
                            if(interactionList->addRelation(nowobject , object_click , CoreEven_Gather))
                                ((Farmer*)nowobject)->setState(4);  //设置state为猎人
                            break;
                        case SORT_BUILDING:
                            if(interactionList->addRelation(nowobject , object_click , CoreEven_FixBuilding))
                                ((Farmer*)nowobject)->setState(6);
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

}

