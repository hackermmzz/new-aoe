#ifndef HUMAN_H
#define HUMAN_H

#include "MoveObject.h"
#include "Development.h"
#include "Bloodhaver.h"

class Human:public MoveObject,public BloodHaver
{
public:
    Human();
    Human(int Num,double DR,double UR,Development* playerScience = NULL, int playerRepresent = MAXPLAYER);

  /**********************虚函数**************************/
    void nextframe();
    bool isPlayerControl(){ return true; }

    double getSpeed(){ return speed*playerScience->get_rate_Move(getSort(),Num); }
    int getMaxBlood(){ return MaxBlood*playerScience->get_rate_Blood(getSort(),Num)+\
                        playerScience->get_addition_Blood(getSort(),Num); }
    int getPlayerRepresent(){ return playerRepresent; }
    int getATK(){ return (int)(atk*playerScience->get_rate_Attack(getSort(),Num,ARMY_INFANTRY,get_AttackType())) \
                 +get_add_specialAttack() + playerScience->get_addition_Attack(getSort(),Num,ARMY_INFANTRY,get_AttackType()); }
    int getDEF(int attackType_got);

    void setPreAttack( ){ this->prestate = MOVEOBJECT_STATE_ATTACK; }
    bool isAttacking(){ return nowstate == MOVEOBJECT_STATE_ATTACK;}

    bool is_missileThrow(){ return isAttacking() && this->nowres == prev(nowlist->end(),phaseFromEnd_MissionAttack) && attack_OneCircle; }

    bool is_attackHit(){ return get_isActionEnd() && attack_OneCircle;  }
    //用于显示的属性
    int showATK_Addition(){ return  playerScience->get_addition_Attack(getSort(),Num,ARMY_INFANTRY,get_AttackType()); }
    /***************指针强制转化****************/
    //若要将Human类指针转化为父类指针,务必用以下函数!
    void printer_ToHuman(void** ptr){ *ptr = this; }        //传入ptr为Human类指针的地址,需要强制转换为（void**）
    void printer_ToBloodHaver(void** ptr){ *ptr = dynamic_cast<BloodHaver*>(this); }    //传入ptr为BloodHaver类指针的地址,需要强制转换为（void**）
    /*************以上指针强制转化****************/
  /********************以上虚函数**************************/

    //转化时使用
    void setPlayerScience( Development* science ){ this->playerScience = science; }
    void setPlayerRepresent( int represent ){ playerRepresent = represent; }
//    int getType(){ return type; }

    void setPosForced(double DR,double UR);
    bool getTransported() const;
    void setTransported(bool value);

protected:
    Development* playerScience = NULL;
    //    int type = 0;
    int playerRepresent;
    bool transported=0;
    int phaseFromEnd_MissionAttack = 0;
};

#endif // HUMAN_H
