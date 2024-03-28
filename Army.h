#ifndef ARMY_H
#define ARMY_H

#include <Human.h>

class Army : public Human
{
public:
    Army();
    Army(double DR,double UR,int type);
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
    int getArmyClass(){ return armyClass; }

    static std::string getArmyName(int index) {
        if (index >= 0 && index < 4) {
            return ArmyName[index];
        }
        return "";
    }
    static std::list<ImageResource>* getDisappear(int i, int j) {
        return Disappear[i][j];
    }
    static std::list<ImageResource>* getWalk(int i, int j) {
        return Walk[i][j];
    }
    static std::list<ImageResource>* getStand(int i, int j) {
        return Stand[i][j];
    }
    static std::list<ImageResource>* getAttack(int i, int j) {
        return Attack[i][j];
    }
    static std::list<ImageResource>* getDie(int i, int j) {
        return Die[i][j];
    }
    static void setDisappear(int i, int j, std::list<ImageResource>* newValue) {
        Disappear[i][j] = newValue;
    }
    static void setWalk(int i, int j, std::list<ImageResource>* newValue) {
        Walk[i][j] = newValue;
    }
    static void setStand(int i, int j, std::list<ImageResource>* newValue) {
        Stand[i][j] = newValue;
    }
    static void setAttack(int i, int j, std::list<ImageResource>* newValue) {
        Attack[i][j] = newValue;
    }
    static void setDie(int i, int j, std::list<ImageResource>* newValue) {
        Die[i][j] = newValue;
    }
    static void allocateDisappear(int i, int j) {
        Disappear[i][j] = new std::list<ImageResource>;
    }
    static void allocateWalk(int i, int j) {
        Walk[i][j] = new std::list<ImageResource>;
    }
    static void allocateStand(int i, int j) {
        Stand[i][j] = new std::list<ImageResource>;
    }
    static void allocateAttack(int i, int j) {
        Attack[i][j] = new std::list<ImageResource>;
    }
    static void allocateDie(int i, int j) {
        Die[i][j] = new std::list<ImageResource>;
    }
    static void deallocateDisappear(int i, int j) {
        delete Disappear[i][j];
        Disappear[i][j] = nullptr;
    }
    static void deallocateWalk(int i, int j) {
        delete Walk[i][j];
        Walk[i][j] = nullptr;
    }
    static void deallocateStand(int i, int j) {
        delete Stand[i][j];
        Stand[i][j] = nullptr;
    }
    static void deallocateAttack(int i, int j) {
        delete Attack[i][j];
        Attack[i][j] = nullptr;
    }
    static void deallocateDie(int i, int j) {
        delete Die[i][j];
        Die[i][j] = nullptr;
    }


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


    static std::list<ImageResource> *Walk[4][8];

    static std::list<ImageResource> *Stand[4][8];

    static std::list<ImageResource> *Attack[4][8];

    static std::list<ImageResource> *Die[4][8];

    static std::list<ImageResource> *Disappear[4][8];

    static std::string ArmyName[4];


};

#endif // ARMY_H
