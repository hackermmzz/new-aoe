#include "Missile.h"

std::string Missile::missilename[1] = { "Spear" };

std::list<ImageResource> Missile::*missile[1];

Missile::Missile()
{

}

Missile::Missile(int type, double beginDR , double beginUR , double goalDR , double goalUR , double damage)
{
    this->Num = type;
    this->DR = beginDR;
    this->UR = beginUR;
    this->DR0 = goalDR;
    this->UR0 = goalUR;
    this->damage = damage;

    setAttribute();
}

void Missile::setAttribute()
{
    switch (Num) {
    case Missile_Spear:
        isAOE = false;
        speed = Missile_Speed_Spear;
        break;

    case Missile_Arrow:
        isAOE = false;
        speed = Missile_Speed_Arrow;
        break;

    default:
        break;
    }

}


void Missile::nextframe()
{

}
