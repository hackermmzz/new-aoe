#ifndef ENEMYAI_H
#define ENEMYAI_H
#include "AI.h"

extern tagGame tagEnemyGame;
extern ins EnemyIns;

class EnemyAI:public AI
{
public:
    EnemyAI(){this->id=1;}
    ~EnemyAI(){
        ;
    }
private:
    void processData() override;

    tagInfo getInfo(){
        return tagEnemyGame.getInfo();
    }
    int AddToIns(instruction ins) override{
        EnemyIns.lock.lock();
        ins.id=EnemyIns.g_id;
        EnemyIns.g_id++;
        EnemyIns.instructions.push(ins);
        EnemyIns.lock.unlock();
        return ins.id;
    }
    void clearInsRet() override{
        tagEnemyGame.clearInsRet();
    }
};
/*##########DO NOT EDIT ABOVE##########*/



#endif // ENEMYAI_H
