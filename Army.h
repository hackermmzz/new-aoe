#ifndef ARMY_H
#define ARMY_H

#include <Human.h>

class Army : public Human
{
public:
    Army();
    Army(double DR,double UR,int type);
    ~Army();

    /*********虚函数**********/
    void nextframe();
    void setNowRes();

    int getSort(){ return SORT_ARMY; }

    double getSpeed();
    int getBlood();
    int getVision();

    int getATK();
    int getDEF(int attackType_got);
    double getDis_attack();

    //特攻,根据兵种不同,会有攻击力加成
    int get_add_specialAttack(){ return 0; }

    /*******以上虚函数**********/

private:
    int armyClass;
    bool upgradable;

    /*****************/
    //该部分数组存储可升级的兵种的属性,下标0表示初始兵种,下标又即升级次数
    int* MaxBlood_change = NULL;    //  最大血量
    double* speed_change = NULL;    //移速
    int* vision_change = NULL;      //视野

    int* atk_change  = NULL;        //攻击力
    double* dis_Attack_change  = NULL;  //攻击距离
    double* inter_Attack_change = NULL; //攻击间隔
    int* defence_close_change  = NULL;  //肉搏防御
    int* defence_shoot_change  = NULL;  //投射防御
    /*****************/

    void setAttribute();

};

#endif // ARMY_H
