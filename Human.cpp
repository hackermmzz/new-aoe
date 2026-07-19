#include "Human.h"

Human::Human()
{

}

Human::Human(int Num, Double DR, Double UR,Development* playerScience, int playerRepresent)
{
    this->playerScience = playerScience;
    this->playerRepresent = playerRepresent;
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
    if (statFrozen) {
        if (attackType_got == ATTACKTYPE_CLOSE || attackType_got == ATTACKTYPE_ANIMAL) return frozenDEFclose;
        else if (attackType_got == ATTACKTYPE_SHOOT) return frozenDEFshoot;
        else return 0;
    }

    int def = 0;

    if( attackType_got == ATTACKTYPE_CLOSE||attackType_got == ATTACKTYPE_ANIMAL ) def = defence_close;
    else if(attackType_got == ATTACKTYPE_SHOOT ) def = defence_shoot;

    return (int)(def * playerScience->get_rate_Defence(getSort(),Num,ARMY_INFANTRY , attackType_got) ) +\
            playerScience->get_addition_Defence(getSort() , Num , ARMY_INFANTRY , attackType_got);
}

//转化冻结（村民/通用人类）：快照当前属性，之后永久锁定
void Human::freezeStats()
{
    if (statFrozen) return;     //已冻结则保持，不重复快照（永久锁定）

    frozenAtkBase  = atk;
    frozenAtkAdd   = playerScience->get_addition_Attack(getSort(), Num, ARMY_INFANTRY, get_AttackType());
    frozenDEFclose = getDEF(ATTACKTYPE_CLOSE);
    frozenDEFshoot = getDEF(ATTACKTYPE_SHOOT);
    frozenMaxBlood = getMaxBlood();
    frozenSpeed    = getSpeed();
    frozenVision   = getVision();

    statFrozen     = true;
}



bool Human::getTransported() const
{
    return transported;
}

void Human::setTransported(bool value)
{
    transported = value;
}
