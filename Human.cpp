#include "Human.h"

Human::Human()
{

}

Human::Human(int Num, double DR, double UR)
{
    this->Num=Num;
    this->DR=DR;
    this->UR=UR;

    speed = HUMAN_SPEED;
}

void Human::nextframe()
{

}
