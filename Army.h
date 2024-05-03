#ifndef ARMY_H
#define ARMY_H

#include <Human.h>

class Army : public Human
{
public:
    Army();
    Army(double DR,double UR,int type , Development* playerScience = NULL, int playerRepresent = MAXPLAYER);
    ~Army();

  /**********************虚函数**************************/
    void nextframe();
    void setNowRes();
    void setAttribute();

    int getSort(){ return SORT_ARMY; }

    double getSpeed();
    int getMaxBlood();
    int getVision();

    int getATK();
    int getDEF(int attackType_got);
    double getDis_attack();

    //特攻,根据兵种不同,会有攻击力加成
    //需要优化，更改获取特攻、计算特攻方式。
    int get_add_specialAttack(Coordinate* attackee){ return 0; }

    /***************指针强制转化****************/
    //若要将Army类指针转化为父类指针,务必用以下函数!
    void printer_ToBloodHaver(void** ptr){ *ptr = dynamic_cast<BloodHaver*>(this); }    //传入ptr为BloodHaver类指针的地址
    /*************以上指针强制转化****************/
  /********************以上虚函数**************************/


  /********************静态函数**************************/
    static std::string getArmyName(int num , int level) {
        if (num >= 0 && num < 4) {
            return ArmyName[num][level];
        }
        return "";
    }
    static std::list<ImageResource>* getDisappear(int num, int level, int angle) { return Disappear[num][level][angle]; }
    static std::list<ImageResource>* getWalk(int num, int level, int angle) { return Walk[num][level][angle];}
    static std::list<ImageResource>* getStand(int num, int level, int angle) {return Stand[num][level][angle];}
    static std::list<ImageResource>* getAttack(int num, int level, int angle) {return Attack[num][level][angle];}
    static std::list<ImageResource>* getDie(int num, int level, int angle) {return Die[num][level][angle];}
    static void setDisappear(int num, int level, int angle, std::list<ImageResource>* newValue) {Disappear[num][level][angle] = newValue;}
    static void setWalk(int num, int level, int angle, std::list<ImageResource>* newValue) {Walk[num][level][angle] = newValue;}
    static void setStand(int num, int level, int angle, std::list<ImageResource>* newValue) { Stand[num][level][angle] = newValue;}
    static void setAttack(int num, int level, int angle, std::list<ImageResource>* newValue) {Attack[num][level][angle] = newValue;}
    static void setDie(int num, int level, int angle, std::list<ImageResource>* newValue) { Die[num][level][angle] = newValue;}
    static void allocateDisappear(int num, int level, int angle) { Disappear[num][level][angle] = new std::list<ImageResource>;}
    static void allocateWalk(int num, int level, int angle) { Walk[num][level][angle]= new std::list<ImageResource>;}
    static void allocateStand(int num, int level, int angle) { Stand[num][level][angle] = new std::list<ImageResource>;}
    static void allocateAttack(int num, int level, int angle) {Attack[num][level][angle] = new std::list<ImageResource>;  }
    static void allocateDie(int num, int level, int angle) { Die[num][level][angle] = new std::list<ImageResource>;}
    static void deallocateDisappear(int num, int level, int angle)
    {
        delete Disappear[num][level][angle];
        Disappear[num][level][angle] = nullptr;
    }
    static void deallocateWalk(int num, int level, int angle)
    {
        delete Walk[num][level][angle];
        Walk[num][level][angle] = nullptr;
    }
    static void deallocateStand(int num, int level, int angle)
    {
        delete Stand[num][level][angle];
        Stand[num][level][angle] = nullptr;
    }
    static void deallocateAttack(int num, int level, int angle)
    {
        delete Attack[num][level][angle];
        Attack[num][level][angle] = nullptr;
    }
    static void deallocateDie(int num, int level, int angle)
    {
        delete Die[num][level][angle];
        Die[num][level][angle] = nullptr;
    }
  /********************以上静态函数**************************/


    int getArmyClass(){ return armyClass; }
    int getLevel();

private:
    int armyClass;
    bool upgradable = false;
    int dependBuildNum;
    int dependBuildAct;

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


    /*************静态成员************/
    static std::list<ImageResource> *Walk[4][2][8];
    static std::list<ImageResource> *Stand[4][2][8];
    static std::list<ImageResource> *Attack[4][2][8];
    static std::list<ImageResource> *Die[4][2][8];
    static std::list<ImageResource> *Disappear[4][2][8];
    static std::string ArmyName[4][2];
    /*************静态成员************/

};

#endif // ARMY_H
