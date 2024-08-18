#ifndef USRAI_H
#define USRAI_H
#include "AI.h"
extern tagGame tagUsrGame;
extern ins UsrIns;
/*##########请勿修改以上部分##########*/

class UsrAI:public AI
{
public:
    UsrAI(){this->id=0;}
    ~UsrAI(){}
private:
  /***************AI通信函数，请勿修改***************/
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
  /***************AI通信函数，请勿修改***************/
};


#endif // USRAI_H
