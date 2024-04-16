#ifndef AI_H
#define AI_H
#include "GlobalVariate.h"
#include "Coordinate.h"

class AI:public QThread
{
public:
/*##########INTERFACE BEGINS HERE##########*/
    int HumanMove(int SN, double L0, double U0){
        Coordinate* self=g_Object[SN];
        if(!isHuman(self))
            return ACTION_INVALID_SN;
        if(L0 > 2575 || L0 < 0||U0 > 2575 || U0 < 0)
            return ACTION_INVALID_LOCATION;
        instructions.push(instruction(1,self,L0,U0));
        return ACTION_SUCCESS;
    }

    int HumanAction(int SN,int obSN){
        Coordinate* self=g_Object[SN];
        Coordinate* obj=g_Object[obSN];
        if(!isHuman(self))
            return ACTION_INVALID_SN;
        if(obj->getSort()!=SORT_ANIMAL&&obj->getSort()!=SORT_STATICRES){
            return ACTION_INVALID_OBSN;
        }
        instructions.push(instruction(2,self,obj));
        return ACTION_SUCCESS;
    }
/*###########INTERFACE ENDS HERE###########*/
    AI();
    void processData();
    void run() override{
        if(AIGame.GameFrame>10&&AIfinished){
            qDebug()<<g_frame<<":"<<"调用AI";
            processData();
        }
        }
private:
    bool isHuman(Coordinate* self){
        return (self!=nullptr&&(self->getSort()==SORT_FARMER||self->getSort()==SORT_ARMY));
    }
};

#endif // AI_H
