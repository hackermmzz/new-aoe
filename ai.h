#ifndef AI_H
#define AI_H
#include "GlobalVariate.h"

class AI:public QThread
{
public:
    AI();
    bool stop=false;
    void processData();
    int HumanMove(int SN, int L0, int U0)
    {
        return 0;
    }
    void run() override{
        while(!stop){
            processData();
        }
    }
};

#endif // AI_H
