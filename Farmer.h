#ifndef FARMER_H
#define FARMER_H

#include <Human.h>

class Farmer:public Human
{
public:
    Farmer();
    Farmer(double DR,double UR);
    void nextframe();
    int getSort();
    void setNowRes();

    static std::string getFarmerName(int index) {
        if (index >= 0 && index < 7) {
            return FarmerName[index];
        }
        return "";
    }
    static std::string getFarmerCarry(int index) {
        if (index >= 0 && index < 5) {
            return FarmerCarry[index];
        }
        return "";
    }
    static std::list<ImageResource>* getCarry(int i, int j) {
        return Carry[i][j];
    }
    static std::list<ImageResource>* getWork(int i, int j) {
        return Work[i][j];
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
    static void setCarry(int i, int j, std::list<ImageResource>* newValue) {
        Carry[i][j] = newValue;
    }
    static void setWork(int i, int j, std::list<ImageResource>* newValue) {
        Work[i][j] = newValue;
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
    static void allocateCarry(int i, int j) {
        Carry[i][j] = new std::list<ImageResource>;
    }
    static void allocateWork(int i, int j) {
        Work[i][j] = new std::list<ImageResource>;
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


private:
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

    int holdResource=0;
    //0表示当前不抱有状态

    Coordinate *workobject=NULL;
    //工作对象（资源）

    Coordinate *nowobject=NULL;
    //当前工作对象

    double resource;
    //资源携带量

    int resourceSort;
    //指示所携带资源的类型
    //1指代木头 2指代肉 3指代石头 4指代金子

    int Blood;
    int MaxBlood;

    static std::list<ImageResource> *Walk[7][8];

    static std::list<ImageResource> *Work[7][8];

    static std::list<ImageResource> *Stand[7][8];

    static std::list<ImageResource> *Attack[7][8];

    static std::list<ImageResource> *Die[7][8];

    static std::list<ImageResource> *Carry[5][8];

    static std::string FarmerName[7];
    static std::string FarmerCarry[5];
    std::string FarmerDisplayName[7]={"村民","樵夫","浆果采集者","矿工","猎人","农民","工人"};
    //    std::string actName[1]={};
    //窗口按钮资源的字符串名称 用来查找对应的qpixmap资源

};

#endif // FARMER_H
