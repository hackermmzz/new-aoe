#ifndef FARMER_H
#define FARMER_H

#include <Human.h>

class Farmer:public Human
{
public:
    Farmer();

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
