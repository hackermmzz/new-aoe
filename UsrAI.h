#ifndef USRAI_H
#define USRAI_H

#include "AI.h"

extern tagGame tagUsrGame;
extern ins UsrIns;
/*##########DO NOT MODIFY THE CODE ABOVE##########*/

class UsrAI:public AI
{
public:
    UsrAI(){this->id=0;}
    ~UsrAI(){}

private:
    void processData() override;
    tagInfo getInfo(){return tagUsrGame.getInfo();}
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
    /*##########DO NOT MODIFY THE CODE IN THE CLASS##########*/
    bool tryBuildBuilding(int buildingType, int requiredWood, int maxCount, int reqiredBuilding,int is_settler=0);
};

/*##########YOUR CODE BEGINS HERE##########*/
    
/*##########YOUR CODE ENDS HERE##########*/
#endif // USRAI_H
