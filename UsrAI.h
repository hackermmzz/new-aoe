#ifndef USRAI_H
#define USRAI_H
#include "AI.h"

extern tagGame* tagUsrGame;
extern QMutex tagUsrGameLock;
extern ins UsrIns;

class UsrAI:public AI
{
public:
    UsrAI(){this->id=0;}
    tagGame* getGameInfo() override{
        QMutexLocker locker(&tagUsrGameLock);
        return tagUsrGame;
    }
    void processData() override;
    int AddToIns(instruction ins) override{
        UsrIns.lock.lock();
        ins.id=UsrIns.g_id;
        UsrIns.g_id++;
        UsrIns.instructions.push(ins);
        UsrIns.lock.unlock();
        return ins.id;
    }
    void clearInsRet() override{
        tagUsrGameLock.lock();
        tagUsrGame->ins_ret.clear();
        tagUsrGameLock.unlock();
    }
    ~UsrAI(){
        ;
    }
};

#endif // USRAI_H
