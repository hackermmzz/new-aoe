#ifndef BLOODHAVER_H
#define BLOODHAVER_H
#include <config.h>
#include <Coordinate.h>

//####################################################
//BloodHaver类,用于管理一切与血量有关的数据,数据变化方式的定义
//如:血量本身,血量减少的计算途径,如攻击力,防御力等
class BloodHaver
{
public:
    BloodHaver();

    /***********虚函数************/
    virtual int getMaxBlood(){ return MaxBlood; }

    virtual int getATK(){ return atk; }
    virtual int getDEF(int attackType_got){  if(attackType_got == ATTACKTYPE_CLOSE) return defence_close;
                                             else if(attackType_got == ATTACKTYPE_SHOOT) return defence_shoot;
                                             else return 0;}
    virtual double getDis_attack(){ return dis_Attack; }
    virtual int get_AttackType(){ return attackType; }
    /*********以上虚函数************/

    bool isDie(){return Blood<=0;}
    void setAttackObject(Coordinate* attackObject){ this->attackObject = attackObject; }
    void setAvangeObject( Coordinate* avangeObject ){ this->avangeObject = avangeObject; }
    int getBlood(){ return (int)( Blood*getMaxBlood() ); }
    Coordinate* getAvangeObject(){ return avangeObject; }
    void updateBlood(int damage){ Blood -= (double)damage/(double)getMaxBlood(); }

protected:
    double Blood;   //Blood区间[0,1],以血量百分比表示当前血量. 当前血量数值为Blood*当前的血量最大值
    int MaxBlood;

    //攻击相关
    Coordinate* attackObject = NULL;    //攻击目标
    Coordinate* avangeObject = NULL;    //受到攻击的来源
    int attackType = ATTACKTYPE_CANTATTACK;     //攻击类型
    int atk = 0;    //攻击力
    double dis_Attack = DISTANCE_ATTACK_CLOSE;  //攻击距离

    /** 攻击间隔尚未理解其意义,故没有其的获取函数*/
    double inter_Attack = 0; //攻击间隔
    int defence_close = 0;  //肉搏防御
    int defence_shoot = 0;  //投射防御
};

#endif // BLOODHAVER_H
