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

int Human::getDEF(int attackType_got)
{
    int def = 0;

    if( attackType_got == ATTACKTYPE_CLOSE||attackType_got == ATTACKTYPE_ANIMAL ) def = defence_close;
    else if(attackType_got == ATTACKTYPE_SHOOT ) def = defence_shoot;

    return (int)(def * playerScience->get_rate_Defence(getSort(),type,ARMY_INFANTRY , attackType_got) ) + playerScience->get_addition_Defence(getSort() , type , ARMY_INFANTRY , attackType_got);
}
