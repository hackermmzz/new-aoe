#ifndef AI_H
#define AI_H
#include "GlobalVariate.h"
#include "Coordinate.h"

class AI:public QThread
{
public:
    int HumanMove(int SN, int L0, int U0)
    {
        Coordinate* self=g_Object[SN];
        if(self->getSort()!=SORT_FARMER&&self->getSort()!=SORT_ARMY)
            return ACTION_INVALID_SN;
        if(L0 > 2575 || L0 < 0||U0 > 2575 || U0 < 0)
            return ACTION_INVALID_LOCATION;
        instructions.push(instruction(1,self,Point({L0,U0})));
        return ACTION_SUCCESS;
    }

    AI();
    bool AIstop=false;
    void processData();
    void run() override{
        while(!AIstop){
            if(AIGame.GameFrame>10)
                processData();
        }
    }
};

#endif // AI_H
