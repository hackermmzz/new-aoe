#ifndef HUMAN_H
#define HUMAN_H

#include <MoveObject.h>
#include <Development.h>

class Human:public MoveObject
{
public:
    Human();
    Human(int Num,double DR,double UR);

    /*********虚函数**********/
    void nextframe();
    double getSpeed(){ return speed*playerScience->get_rate_Move(getSort(),type); }
    int getBlood(){ return  (int) ( ( MaxBlood*playerScience->get_rate_Blood(getSort(),type)+ playerScience->get_addition_Blood(getSort(),type) )*Blood ); }

    int getATK(){ return (int)(atk*playerScience->get_rate_Attack(getSort(),type,ARMY_INFANTRY,get_AttackType())) + playerScience->get_addition_Attack(getSort(),type,ARMY_INFANTRY,get_AttackType()); }
    virtual int get_add_specialAttack( ){ return 0; }

    int getDEF(int attackType_got);
    /*******以上虚函数**********/

    void setPlayerScience( Development* science ){ this->playerScience = science; }

protected:
    Development* playerScience = NULL;
    int type = 0;
};

#endif // HUMAN_H
