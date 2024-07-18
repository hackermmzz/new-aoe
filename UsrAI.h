#ifndef USRAI_H
#define USRAI_H
#include "AI.h"

extern tagGame tagUsrGame;
extern ins UsrIns;

class UsrAI:public AI
{

public:
    UsrAI(){
        this->id=0;
    }
    ~UsrAI(){;}

private:
    tagInfo getInfo()
    {
        return tagUsrGame.getInfo();
    }
    void processData() override;
    int AddToIns(instruction ins) override
    {
        UsrIns.lock.lock();
        ins.id=UsrIns.g_id;
        UsrIns.g_id++;
        UsrIns.instructions.push(ins);
        UsrIns.lock.unlock();
        return ins.id;
    }
    void clearInsRet() override
    {
        tagUsrGame.clearInsRet();
    }
};
/*##########DO NOT EDIT ABOVE##########*/


#endif // USRAI_H
