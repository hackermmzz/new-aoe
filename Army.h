#ifndef ARMY_H
#define ARMY_H

#include "Human.h"

class Army : public Human
{
public:
    Army();
    Army(double DR,double UR,int type , Development* playerScience = NULL, int playerRepresent = MAXPLAYER);
    Army(double DR,double UR,int type ,int status, Development* playerScience=NULL, int playerRepresent=MAXPLAYER,int starttime=15,int finishtime=37500,double dDR=0,double dUR=0);
    ~Army();
    bool ifAttack;
    int timelock;
  /**********************虚函数**************************/
    void nextframe();
    void setNowRes();
    void setAttribute();

    int getSort(){ return SORT_ARMY; }

    double getSpeed();
    int getMaxBlood();
    int getVision();

    bool isMonitorObject(Coordinate* judOb)
    {
        return judOb->isPlayerControl() && judOb->getPlayerRepresent()!= getPlayerRepresent();
    }

    QString getChineseName(){ return QString::fromStdString(getArmyDisplayNum(Num,getLevel())); }

    string getSound_Click(){return click_sound;}

    int getATK();
    int getDEF(int attackType_got);
    double getDis_attack();
    //军队自动化
    int getstatus();
    int getstarttime();
    int getfinishtime();
    double getstartpointDR();
    double getstartpointUR();
    double getdestinaDR();
    double getdestinaUR();
    bool getifAttack();
    int gettimelock();

    //特攻,根据兵种不同,会有攻击力加成
    int get_add_specialAttack();

    //用于显示的战斗属性
    int showATK_Basic();
    int showATK_Addition(){return playerScience->get_addition_Attack(getSort(),Num,armyClass,get_AttackType());}

    int showDEF_Close();
    int showDEF_Shoot();
    int showDEF_Close_Addition(){ return playerScience->get_addition_Defence(getSort(),Num,armyClass,ATTACKTYPE_CLOSE);}
    int showDEF_Shoot_Addition(){ return playerScience->get_addition_Defence(getSort(),Num,armyClass,ATTACKTYPE_SHOOT);}
    /***************指针强制转化****************/
    //若要将Army类指针转化为父类指针,务必用以下函数!
    void printer_ToBloodHaver(void** ptr){ *ptr = dynamic_cast<BloodHaver*>(this); }    //传入ptr为BloodHaver类指针的地址
    /*************以上指针强制转化****************/
  /********************以上虚函数**************************/


  /********************静态函数**************************/
    static std::string getArmyName(int num , int level) {
        if (num >= 0 && num < 8 && level>=0&& level<2) {
            return ArmyName[num][level];
        }
        return "";
    }

    static std::string getArmyDisplayNum(int num , int level) {
        if (num >= 0 && num <8&& level>=0&& level<2) {
            return ArmyDisplayName[num][level];
        }
        return "";
    }

    //获取图像资源数组
    static std::list<ImageResource>* getDisappear(int represent, int num, int level, int angle) { return Disappear[represent][num][level][angle]; }
    static std::list<ImageResource>* getWalk(int represent, int num, int level, int angle) { return Walk[represent][num][level][angle];}
    static std::list<ImageResource>* getStand(int represent, int num, int level, int angle) {return Stand[represent][num][level][angle];}
    static std::list<ImageResource>* getAttack( int represent, int num, int level, int angle) {return Attack[represent][num][level][angle];}
    static std::list<ImageResource>* getDie(int represent, int num, int level, int angle) {return Die[represent][num][level][angle];}

    //设置图像资源
    static void setDisappear(int represent, int num, int level, int angle, std::list<ImageResource>* newValue) {Disappear[represent][num][level][angle] = newValue;}
    static void setWalk( int represent, int num, int level, int angle, std::list<ImageResource>* newValue) {Walk[represent][num][level][angle] = newValue;}
    static void setStand(int represent, int num, int level, int angle, std::list<ImageResource>* newValue) { Stand[represent][num][level][angle] = newValue;}
    static void setAttack(int represent,int num, int level, int angle, std::list<ImageResource>* newValue) {Attack[represent][num][level][angle] = newValue;}
    static void setDie(int represent, int num, int level, int angle, std::list<ImageResource>* newValue) { Die[represent][num][level][angle] = newValue;}

    //为图像资源数组开辟空间
    static void allocateDisappear(int represent, int num, int level, int angle) { Disappear[represent][num][level][angle] = new std::list<ImageResource>;}
    static void allocateWalk(int represent, int num, int level, int angle) { Walk[represent][num][level][angle]= new std::list<ImageResource>;}
    static void allocateStand(int represent, int num, int level, int angle) { Stand[represent][num][level][angle] = new std::list<ImageResource>;}
    static void allocateAttack(int represent, int num, int level, int angle) {Attack[represent][num][level][angle] = new std::list<ImageResource>;  }
    static void allocateDie(int represent, int num, int level, int angle) { Die[represent][num][level][angle] = new std::list<ImageResource>;}

    //释放图像资源数组
    static void deallocateDisappear(int represent, int num, int level, int angle)
    {
        delete Disappear[represent][num][level][angle];
        Disappear[represent][num][level][angle] = nullptr;
    }
    static void deallocateWalk(int represent, int num, int level, int angle)
    {
        delete Walk[represent][num][level][angle];
        Walk[represent][num][level][angle] = nullptr;
    }
    static void deallocateStand(int represent, int num, int level, int angle)
    {
        delete Stand[represent][num][level][angle];
        Stand[represent][num][level][angle] = nullptr;
    }
    static void deallocateAttack(int represent, int num, int level, int angle)
    {
        delete Attack[represent][num][level][angle];
        Attack[represent][num][level][angle] = nullptr;
    }
    static void deallocateDie(int represent, int num, int level, int angle)
    {
        delete Die[represent][num][level][angle];
        Die[represent][num][level][angle] = nullptr;
    }
  /********************以上静态函数**************************/

    //获取士兵的部队种类 如步兵、骑兵、弓兵等
    int getArmyClass(){ return armyClass; }
    //获取士兵等级
    int getLevel();

private:
    int armyClass;
    bool upgradable = false;
    int dependBuildNum;
    int dependBuildAct;
    int status;
    int starttime;
    int finishtime;
    double startpointDR;
    double startpointUR;
    double destinaDR;
    double destinaUR;

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
    static std::list<ImageResource> *Walk[2][20][2][8];
    static std::list<ImageResource> *Stand[2][20][2][8];
    static std::list<ImageResource> *Attack[2][20][2][8];
    static std::list<ImageResource> *Die[2][20][2][8];
    static std::list<ImageResource> *Disappear[2][20][2][8];
    static std::string ArmyName[20][2];
    static std::string ArmyDisplayName[20][2];

    static string click_sound;
    /*************静态成员************/


    void requestSound_Attack();
    void requestSound_Die();
    void requestSound_Walk();
};

#endif // ARMY_H
