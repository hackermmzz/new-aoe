#ifndef USRAI_H
#define USRAI_H
#include "AI.h"

class UsrAI:public AI
{
public:
    UsrAI();
    void processData() override;
};

#endif // USRAI_H
