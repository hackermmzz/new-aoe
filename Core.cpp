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
        std::list<Human *>::iterator humaniter=player[playerIndx]->human.begin() , iterEnd = player[playerIndx]->human.end();
        while(humaniter!=iterEnd)
        {       
            if((*humaniter)->needTranState())
            {
                (*humaniter)->setNowState((*humaniter)->getPreState());
                if((*humaniter)->isDying())
                {
                    interactionList->eraseObject(*humaniter);
                }
                (*humaniter)->setPreStateIsIdle();
            }
            interactionList->conduct_Attacked(*humaniter);

            if((*humaniter)->isDying() && (*humaniter)->get_isActionEnd()) humaniter = player[playerIndx]->human.erase(humaniter);
            else
            {
                (*humaniter)->nextframe();
                (*humaniter)->updateLU();

                humaniter++;
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
                switch (object_click->getSort()) {
                case SORT_ANIMAL:
                    ((Farmer*)nowobject)->setState(4);  //设置state为猎人
                    interactionList->addRelation(nowobject , object_click , CoreEven_Gather);
                    break;
                default:
                    break;
                }
                break;

            case SORT_ARMY:
                switch (object_click->getSort()) {
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

