#ifndef ENEMYAI_H
#define ENEMYAI_H
#include "AI.h"

extern tagGame* tagEnenmyGame;
extern QMutex tagEnemyGameLock;
extern ins EnenmyIns;

class EnemyAI:public AI
{
public:
    EnemyAI(){this->id=1;}
    ~EnemyAI(){
        ;
    }
private:
    void processData() override;
    tagGame getGameInfo() override{
        QMutexLocker locker(&tagEnemyGameLock);
        return *tagEnenmyGame;
    }
    int AddToIns(instruction ins) override{
        EnenmyIns.lock.lock();
        ins.id=EnenmyIns.g_id;
        EnenmyIns.g_id++;
        EnenmyIns.instructions.push(ins);
        EnenmyIns.lock.unlock();
        return ins.id;
    }
    void clearInsRet() override{
        tagEnemyGameLock.lock();
        tagEnenmyGame->ins_ret.clear();
        tagEnemyGameLock.unlock();
    }

};

#endif // ENEMYAI_H
