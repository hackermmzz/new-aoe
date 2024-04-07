#ifndef MISSILE_H
#define MISSILE_H

#include <Animal.h>
#include <Building.h>
#include <Army.h>
#include <Farmer.h>
#include <Development.h>

class Missile : public MoveObject
{
public:
    Missile();
    Missile(int type, Coordinate* attacker , Coordinate* attackee);

  /**********************虚函数**************************/
    int calculateAngle(double nextDR, double nextUR);
    void nextframe();
    void setNowRes();
    void setAttribute();
    void updateMove();

    int getSort(){ return SORT_MISSILE; }
    void printer_ToMissile(void** ptr){ *ptr = this; }
  /********************以上虚函数**************************/

  /**********************图像资源设置函数**************************/

    static void allocatemissile(int index){ missile[index]=new std::list<ImageResource>; }

    static std::string getMissilename(int index){ return missilename[index];}
    static std::list<ImageResource>* getmissile(int index) {return missile[index];  }

    static void deallocatemissile(int index)
    {
        delete missile[index];
        missile[index] = nullptr;
    }

  /********************以上图像资源设置函数**************************/

    void recordAttacker(Coordinate*);

    void calculateDMove();

    //抵达目标点
    bool is_Arrive(){ return distanceToGoal()<DISTANCE_Manhattan_MoveEndNEAR; }
    bool is_haveToArrive(){ return isMandatoryArrive; }

    bool isMissileFinishTask(){ return is_HitTarget() || is_Arrive(); }
    bool is_HitTarget(){ return haveHitTarget; }

    bool isNeedDelete(){ return taskEnd; }

    bool isAttackerHaveDie(){ return isAttackerDie; }

    //命中目标
    void hitTarget(){haveHitTarget = true;}
    void setPlayerScience(Development* science){ playerScience = science; }

    void needDelete(){ taskEnd = true; }
    //记录隶属的player阵营
    void setPlayerRepresent( int represent ){ playerRepresent = represent; }

    void set_attackerDie(){ isAttackerDie = true; }


    double distanceToGoal(){ return countdistance(DR , UR , DR0 , UR0); }

    double get_Distance_hitTarget(){ return DISTANCE_HIT_TARGET*playerScience->get_rate_HitTarget(); }

    BloodHaver* getAttackAponsor();

    Coordinate* getAttacker(){return AttackSponsor;}

    void get_AttackSponsor_Position(double& DR , double& UR);

    //攻击者死亡，设置missile的攻击发起者为NULL
    void deleteAttackerSponsor(Coordinate* attacker){if(attacker == AttackSponsor)
                                                      { set_attackerDie();
                                                        AttackSponsor = NULL;}
                                                    }

private:
    bool isAttackerDie = false;
    Coordinate* AttackSponsor = NULL;
    Coordinate* AttackerRecord = NULL;
    Development* playerScience = NULL;
    int Sort_attacker;
    int playerRepresent;//记录隶属的player阵营

    double dDR, dUR;    //每帧移动量

    bool isAOE; //是否具有喷溅伤害
    bool haveHitTarget = false;
    bool isMandatoryArrive;

    bool taskEnd = false;

    std::map<int ,int > lab_SpecialAttack;


    //图像资源相关
    static std::string missilename[NUMBER_MISSILE];
    static std::list<ImageResource> *missile[NUMBER_MISSILE];
};

#endif // MISSILE_H
