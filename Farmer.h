#ifndef FARMER_H
#define FARMER_H

#include "Human.h"

class Farmer:public Human
{
public:
    static array<std::string,8> FarmerName;
    static array<std::string,8> FarmerCarry;
    static array<std::string,8> FarmerDisplayName;
    static string sound_click;
    static array<std::string,8> sound_work;
public:
    Farmer(){}
    Farmer(Double DR,Double UR , Development* playerScience = NULL, int playerRepresent = MAXPLAYER,int FarmerType=FARMERTYPE_FARMER);

  /**********************虚函数**************************/
    void nextframe();
    void setNowRes();
    int getSort(){return SORT_FARMER;}
    Double getDis_attack();
    int get_AttackType();
    int get_add_specialAttack();
    QString getChineseName(){ return "村民"; }
    bool is_missileAttack(){return get_AttackType() == ATTACKTYPE_SHOOT;}
    int get_farmerType(){return FarmerType;}
    string getSound_Click(){return sound_click;}
    void setSpeed();
    /***************指针强制转化****************/
    //若要将Farmer类指针转化为父类指针,务必用以下函数!
    void printer_ToBloodHaver(void** ptr){*ptr = dynamic_cast<BloodHaver*>(this); }    //传入ptr为BloodHaver类指针的地址
    /*************以上指针强制转化****************/
  /********************以上虚函数**************************/


    static std::string getFarmerName(int index) {
        if (index >= 0 && index < 8) {
            return FarmerName[index];
        }
        return "";
    }
    static std::string getFarmerCarry(int index) {
        if (index >= 0 && index < 7) {
            return FarmerCarry[index];
        }
        return "";
    }
    static std::list<ImageResource>* getCarry(int i, int j) {return Carry[i][j];}
    static std::list<ImageResource>* getWork(int i, int j) {return Work[i][j];}

    static std::list<ImageResource>* getWalk(int i, int j) {return Walk[i][j];}
    static std::list<ImageResource>* getStand(int i, int j) {return Stand[i][j];}
    static std::list<ImageResource>* getAttack(int i, int j) {return Attack[i][j];}
    static std::list<ImageResource>* getDie(int i, int j) {return Die[i][j];}
    static std::list<ImageResource>* getDisappear(int state,int angle) { return Disappear[state][angle]; }
    static std::list<ImageResource>* getShipStand(int i,int j){return ShipStand[i][j];}
    static void setCarry(int i, int j, std::list<ImageResource>* newValue) {Carry[i][j] = newValue;}
    static void setWork(int i, int j, std::list<ImageResource>* newValue) {Work[i][j] = newValue;}
    static void setWalk(int i, int j, std::list<ImageResource>* newValue) {Walk[i][j] = newValue;}
    static void setStand(int i, int j, std::list<ImageResource>* newValue) {Stand[i][j] = newValue;}
    static void setAttack(int i, int j, std::list<ImageResource>* newValue) {Attack[i][j] = newValue;}
    static void setDie(int i, int j, std::list<ImageResource>* newValue) {Die[i][j] = newValue;}
    static void setDisappear(int state, int angle, std::list<ImageResource>* newValue) {Disappear[state][angle] = newValue;}
    static void allocateCarry(int i, int j) {Carry[i][j] = new std::list<ImageResource>;}
    static void allocateWork(int i, int j){ Work[i][j] = new std::list<ImageResource>;}
    static void allocateWalk(int i, int j) {Walk[i][j] = new std::list<ImageResource>;}
    static void allocateStand(int i, int j) {Stand[i][j] = new std::list<ImageResource>;}
    static void allocateAttack(int i, int j){Attack[i][j] = new std::list<ImageResource>;}
    static void allocateDie(int i, int j) {Die[i][j] = new std::list<ImageResource>;}
    static void allocateDisappear(int state, int angle) { Disappear[state][angle] = new std::list<ImageResource>;}
    static void allocateShipStand(int i,int j){ShipStand[i][j]=new std::list<ImageResource>;}
    static void deallocateCarry(int i, int j) {
        delete Carry[i][j];
        Carry[i][j] = nullptr;
    }
    static void deallocateWork(int i, int j) {
        delete Work[i][j];
        Work[i][j] = nullptr;
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
    static void deallocateDisappear(int state, int angle)
    {
        delete Disappear[state][angle];
        Disappear[state][angle] = nullptr;
    }
    std::string getDisplayName(int num){return FarmerDisplayName[num];}

    int getState(){return state;}

    Double getResourceNowHave(){ return resource; }
    int getResourceHave_Max(){ return resource_Max + playerScience->get_addition_ResourceSort(resourceSort)+farmer_addition_ResourceHold(); }
    int getResourceSort(){ return resourceSort; }
    Double get_quantityGather(){ return quantity_GatherOnce * playerScience->get_rate_ResorceGather(resourceSort); }
    bool get_isFullBackpack(){ return resource >= Double(getResourceHave_Max()); }//用于普通情况下判断farmer是否满背包
    //用于采集时，考虑不同种类资源类型情况，判断farmer背包满
    bool get_isFullBackpack( int resourceSort ){ return resourceSort == this->resourceSort && resource >= Double(getResourceHave_Max()); }

    bool get_isEmptyBackpack(){ return resource == Double(0); }

    //判断所持物与村民状态是否匹配
    bool get_MatchingOfResourceAndCarry(){ return (state == FARMER_LUMBER && resourceSort == HUMAN_WOOD)\
                                         || (state == FARMER_MINER && (resourceSort == HUMAN_STONE || resourceSort == HUMAN_GOLD ))\
                                         || ((state == FARMER_GATHERER|| state == FARMER_FARMER) && resourceSort == HUMAN_GRANARYFOOD) \
                                         || (state == FARMER_HUNTER && resourceSort == HUMAN_STOCKFOOD)
                                           ||(state==FARMER_FISHER&&resourceSort==HUMAN_DOCKFOOD); }
    void setState( int state ){ this->state = state; }
    void set_ResourceSort( int sort ){ this->resourceSort = sort; }
    void update_addResource(){ resource+=quantity_GatherOnce; }
    void update_resourceClear(){ resource = 0; }
    void update_transportHuman(Human*human){resource+=1;HumanTransport.push_back(human);}
    void updateState();
    int farmer_addition_ResourceHold();
    vector<Human *>& getHumanTransport();
    bool isShip(){
        return FarmerType!=0;
    }

private:
    vector<Human*>HumanTransport;
    //携带的人类的SN
    int state;
    //指示状态 指示具体动作
    //暂定 	0为无职 1为樵夫 2为矿工 3为采集者 4为猎人 5为种地农民 6为工人
    int workstate=HUMAN_STATE_IDLE;
    //现在的工作流程

    /* 0代表为空闲状态
     * 1代表为正在前往当前对象的状态
     * 2代表为正在砍树的状态（收集木头）
     * 3代表为正在挖石头的状态
     * 4代表为正在挖金子的状态
     * 5代表正在采集果子的状态
     * 6代表正在建造建筑的状态
     * 7代表正在修理建筑的状态
     * 8代表为进行攻击的状态 在攻击状态的下边进行特判 采用不同的方式计算
     * 9代表为正在返回资源建筑的状态，即放置资源
     * 10代表正在前往攻击的状态
     * 11代表人物遇到障碍物停止移动的状态
     * 后续补充
     */
    int FarmerType;
    //0表示普通的农民,其余表示船
    int holdResource=0;
    //0表示当前不抱有状态

    Coordinate *workobject=NULL;
    //工作对象（资源）

    Coordinate *nowobject=NULL;
    //当前工作对象

    Double resource;
    //当前资源携带量

    int resource_Max = 10;
    //最大资源携带量

    Double quantity_GatherOnce = Double("0.55");

    int resourceSort = 0;
    //指示所携带资源的类型
    //1指代木头 2指代肉 3指代石头 4指代金子
    //eg:HUMAN_WOOD
    static std::list<ImageResource> *Walk[8][8];

    static std::list<ImageResource> *Work[8][8];

    static std::list<ImageResource> *Stand[8][8];

    static std::list<ImageResource> *Attack[8][8];

    static std::list<ImageResource> *Die[8][8];

    static std::list<ImageResource> *Carry[7][8];

    static std::list<ImageResource> *Disappear[8][8];
    /////////////船
    static std::list<ImageResource>* ShipStand[10][8];



    void requestSound_Work();
    void requestSound_Die(){if(isInWidget()) soundQueue.push("Army_Die");}
};

#endif // FARMER_H
