#include "UsrAI.h"
#include <iostream>
tagGame tagUsrGame;
ins UsrIns;
/*##########DO NOT MODIFY THE CODE ABOVE##########*/
#include <bits/stdc++.h>
#define FARMER_FARMER 0
#define FARMER_TRANSPORTSHIP 1
#define FARMRE_FISHINGBOAT 2

tagInfo myInfo;
tagBuilding myTownCenter;         // 用于记录唯一的市镇中心

// 全局变量定义
int pre_farmer = 0;               // 上一帧农民数量

int pre_house = 0;                // 上一帧房屋数量


int pre_wood = 0;                 // 上一帧木材数量
int pre_meat = 0;                 // 上一帧食物数量
int pre_stone = 0;                // 上一帧石头数量
int pre_gold = 0;                 // 上一帧黄金数量

int pre_maxHuman = 0;             // 上一帧最大人口数
bool showStage = false;           // 用于判断是否打印阶段信息


void UsrAI::processData()
{
    // 每帧更新最新的信息
    myInfo = getInfo();

    // 局部变量定义
    int farmer = 0;                // 用于记录当前帧农民数量

    int house = 0;                 // 用于记录当前帧房屋数量

    int wood = 0;                  // 用于记录当前帧木材数量
    int meat = 0;                  // 用于记录当前帧食物数量
    int stone = 0;                 // 用于记录当前帧石头数量
    int gold = 0;                  // 用于记录当前帧黄金数量
    
    // 遍历并统计建筑情况
    for(tagBuilding building: myInfo.buildings)
    {
        // 当建筑完成建造后，加入计数
        if(building.Percent == 100)
        {
            if(building.Type == BUILDING_HOME)
            {
                house++;
            }
            else if(building.Type == BUILDING_CENTER)
            {
                // 获取市镇中心相关信息
                myTownCenter = building;
            }
        }
    }

    // 遍历并统计农民情况
    for(tagFarmer myfarmer:myInfo.farmers)
    {
        if(myfarmer.FarmerSort == FARMER_FARMER)
        {
            farmer++;
        }
    }

    // 遍历并统计军队情况

    // 更新最大人口
    int maxHuman = myInfo.Human_MaxNum;

    // 打印当前帧发生变化的信息
    if(farmer != pre_farmer)
    {
        DebugText("农民数量发生变化，当前值:"+QString::number(farmer));
        pre_farmer = farmer;
    }
    else if(house != pre_house)
    {
        DebugText("房屋数量发生变化，当前值:"+QString::number(house));
        pre_house = house;
    }
    else if(wood != pre_wood)
    {
        DebugText("木材数量发生变化，当前值:"+QString::number(wood));
        pre_wood = wood;
    }
    else if(meat != pre_meat)
    {
        DebugText("食物数量发生变化，当前值:"+QString::number(meat));
        pre_meat = meat;
    }
    else if(stone != pre_stone)
    {
        DebugText("石头数量发生变化，当前值:"+QString::number(stone));
        pre_stone = stone;
    }
    else if(gold != pre_gold)   
    {
        DebugText("黄金数量发生变化，当前值:"+QString::number(gold));
        pre_gold = gold;
    }
    else if(maxHuman != pre_maxHuman)
    {
        DebugText("最大人口发生变化，当前值:"+QString::number(maxHuman));
        pre_maxHuman = maxHuman;
        showStage = true;
    }

    // 根据最大人口数判断游戏阶段并采取对应策略
    if(maxHuman >= 4)
    {
        if(maxHuman == 4 && showStage)
        {
            DebugText("当前最大人口数为4，处于阶段一");
            showStage = false;              // 控制只打印一次
        }
    }

    if(maxHuman >= 8)
    {
        if(maxHuman == 8 && showStage)
        {
            DebugText("当前最大人口数为8，处于阶段二");
            showStage = false;
        }
    }


    return ;
}
