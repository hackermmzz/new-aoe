#ifndef ENEMYAI_H
#define ENEMYAI_H
#include "AI.h"

class EnemyAI:public AI
{
public:
    EnemyAI();
    void processData() override;
};

#endif // ENEMYAI_H
