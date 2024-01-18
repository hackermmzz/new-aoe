#ifndef HUMAN_H
#define HUMAN_H

#include <MoveObject.h>

class Human:public MoveObject
{
public:
    Human();
    Human(int Num,double DR,double UR);
    void nextframe();
};

#endif // HUMAN_H
