#ifndef USRAI_H
#define USRAI_H

#include "ai.h"
#include <unordered_map>

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
    void collectResource();
    void arrowTowerAttack(tagBuilding);
    void getLegalPlace(tagObj, int, int);


};

/*##########YOUR CODE BEGINS HERE##########*/

bool compareSN(tagObj, tagObj);
bool compareDistance(tagObj, tagObj);
bool compareBlood(tagHuman, tagHuman);
Double getDistance(tagObj, tagObj);
void getBuilding();
void getResource();
void getOccupied();
tagBuilding getLastBuilding(int);
void occupied(tagObj, int);



/*##########YOUR CODE ENDS HERE##########*/
#endif // USRAI_H
