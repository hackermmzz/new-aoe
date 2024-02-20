#ifndef HUMAN_H
#define HUMAN_H

#include <MoveObject.h>
#include <Development.h>
#include <Bloodhaver.h>

class Human:public MoveObject,public BloodHaver
{
public:
    Human();
    Human(int Num,double DR,double UR);

  /**********************虚函数**************************/
    void nextframe();
    double getSpeed(){ return speed*playerScience->get_rate_Move(getSort(),type); }
    int getMaxBlood(){ return MaxBlood*playerScience->get_rate_Blood(getSort(),type)+ playerScience->get_addition_Blood(getSort(),type); }

    int getATK(){ return (int)(atk*playerScience->get_rate_Attack(getSort(),type,ARMY_INFANTRY,get_AttackType())) + playerScience->get_addition_Attack(getSort(),type,ARMY_INFANTRY,get_AttackType()); }
    virtual int get_add_specialAttack( ){ return 0; }

    int getDEF(int attackType_got);

    /***************指针强制转化****************/
    //若要将Human类指针转化为父类指针,务必用以下函数!
    void printer_ToHuman(void** ptr){ *ptr = this; }        //传入ptr为Human类指针的地址,需要强制转换为（void**）
    void printer_ToBloodHaver(void** ptr){ *ptr = dynamic_cast<BloodHaver*>(this); }    //传入ptr为BloodHaver类指针的地址,需要强制转换为（void**）
    /*************以上指针强制转化****************/
  /********************以上虚函数**************************/

    void setPlayerScience( Development* science ){ this->playerScience = science; }

    int getType(){ return type; }

protected:
    Development* playerScience = NULL;
    int type = 0;
};

#endif // HUMAN_H
