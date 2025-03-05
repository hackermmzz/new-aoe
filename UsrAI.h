#ifndef USRAI_H
#define USRAI_H

#include "AI.h"

extern tagGame tagUsrGame;
extern ins UsrIns;
/*##########璇峰嬁淇敼浠ヤ笂閮ㄥ垎##########*/
class UsrAI:public AI
{
public:
    UsrAI(){this->id=0;}
    ~UsrAI(){}


private:
  /***************AI閫氫俊鍑芥暟锛岃鍕夸慨鏀?**************/
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
};


#endif // USRAI_H
