#ifndef BLOODHAVER_H
#define BLOODHAVER_H

#include "config.h"
#include "Coordinate.h"

//####################################################
//BloodHaver类,用于管理一切与血量有关的数据,数据变化方式的定义
//如:血量本身,血量减少的计算途径,如攻击力,防御力等
class BloodHaver
{
public:
    BloodHaver(){}

    /***********虚函数************/
    virtual void setPreAttack( ){ }
    virtual bool isAttacking(){ return false;}
    virtual int getMaxBlood(){ return MaxBlood; }

    virtual int getATK(){ return atk+ get_add_specialAttack(); }
    virtual int get_add_specialAttack(){ return 0; }
    virtual int getDEF(int attackType_got);
    virtual double getDis_attack(){ return dis_Attack; }
    virtual int get_AttackType(){ return attackType; }

    virtual void init_Blood(){ Blood = 1; }

    //判断本攻击单位的攻击方式是否需要投掷物
    virtual bool is_missileAttack(){ return type_Missile != -1; }
    virtual bool is_missileThrow(){ return false; }
    virtual bool is_attackHit(){ return false; }

    //用于显示的属性
    virtual int showATK_Basic(){ return atk + get_add_specialAttack();}
    virtual int showATK_Addition(){ return 0; }
    virtual int showDEF_Close(){ return defence_close; }
    virtual int showDEF_Close_Addition(){ return 0; }
    virtual int showDEF_Shoot(){ return defence_shoot; }
    virtual int showDEF_Shoot_Addition(){ return 0; }
    /*********以上虚函数************/


    //判断死亡
    bool isDie(){return getBlood()<=0;}
    //判断满血（部分判断的边界条件）
    bool isFullHp(){ return Blood >= 1; }
    //判断是否受到了攻击
    bool isGotAttack(){ return gotAttack; }
    bool canAttack(){ return isAttackable; }

    //设置攻击目标
    void setAttackObject(Coordinate* attackObject){ this->attackObject = attackObject; }
    //受到攻击，设置“复仇”目标，用于做受到攻击时的反应
    void setAvangeObject( Coordinate* avangeObject ){ this->avangeObject = avangeObject; underAttack(); updateAvangeObjectPosition(); }
    void setAvangeObject( double DR , double UR ){ underAttack(); avangeObject = NULL; DR_avange = DR; UR_avange = UR; }
    //设置说明本单位受到了攻击
    void underAttack(){ gotAttack = true; }

    //获取血量
    double getBloodPercent(){ return Blood; }
    int getBlood(){ return  ceil(Blood*getMaxBlood()); }
    //获取“复仇”目标
    Coordinate* getAvangeObject(){ return avangeObject; }
    //如果攻击方式需要投掷物，获取投掷物类型
    int get_type_Missile(){return type_Missile;}
    //获取“复仇”目标位置
    void get_AvangeObject_Position( double& DR, double& UR ){ DR = DR_avange; UR = UR_avange; }

    void updateBlood(int damage){ Blood -= (double)damage/(double)getMaxBlood(); if(Blood<0) Blood = 0; }
    //更新“复仇”目标的当前位置
    void updateAvangeObjectPosition(){ if(avangeObject!=NULL)
                                        { DR_avange = avangeObject->getDR(); UR_avange = avangeObject->getUR(); } }

    void initAvengeObject(){ avangeObject = NULL; gotAttack = false; }

    void haveAttack(){ attack_OneCircle = false; }

    bool get_isRangeAttack(){return isRangeAttack;}

protected:
    double Blood = 0;   //Blood区间[0,1],以血量百分比表示当前血量. 当前血量数值为Blood*当前的血量最大值
    int MaxBlood = 100;

    //攻击相关
    Coordinate* attackObject = NULL;    //攻击目标 该指针很危险，需改进
    bool gotAttack = false;
    Coordinate* avangeObject = NULL;    //受到攻击的来源
    double DR_avange=0,UR_avange=0;     //攻击来源的位置


    bool isAttackable = false;
    bool isRangeAttack = false;
    int attackType = ATTACKTYPE_CANTATTACK;     //攻击类型
    int atk = 0;    //攻击力
    double dis_Attack = DISTANCE_ATTACK_CLOSE;  //攻击距离
    int type_Missile = -1;

    bool attack_OneCircle = true;   //用于限制计算重复攻击

    //需细化，特攻表
    std::map<int , int> lab_SpecialAttack;

    /** 攻击间隔尚未理解其意义,故没有其的获取函数*/
    double inter_Attack = 0; //攻击间隔
    int defence_close = 0;  //肉搏防御
    int defence_shoot = 0;  //投射防御

    void initAttack_perCircle(){ attack_OneCircle = true; }
};

#endif // BLOODHAVER_H
