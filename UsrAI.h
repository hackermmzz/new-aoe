#ifndef USRAI_H
#define USRAI_H
#include "AI.h"

extern tagGame tagUsrGame;
extern QMutex tagUsrGameLock;
extern ins UsrIns;

class UsrAI:public AI
{
public:
    UsrAI();
    tagGame getGameInfo() override{
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
    ~UsrAI(){
        ;
    }
};

#endif // USRAI_H
