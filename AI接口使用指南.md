# AI接口使用指南

所有对游戏信息的获取都**必须**通过tagUsrGame/tagEnemyGame

在AI的一次运行中，tagUsrGame/tagEnemyGame存储的游戏信息可能发生改变（每个游戏帧都会刷新tagUsrGame/tagEnemyGame）

以下是您可以调用的控制游戏的接口，所有接口都是AI的成员函数

```c++
    //以下函数为控制函数，若命令下达成功，则返回该指令对应的唯一id,若失败，则返回错误码
	//命令下达成功不代表能成功执行，通过指令id可以在下一帧通过inse_ret查询该指令的执行结果
	//所有的指令一旦下达，无论ai是否结束，都会在下一帧执行
	int HumanMove(int SN, double DR0, double UR0);

    int HumanAction(int SN, int obSN);

    int HumanBuild(int SN, int BuildingNum, int BlockDR, int BlockUR);

    int BuildingAction(int SN, int Action);
```

以下是您可以调用的获取游戏信息的接口，为UsrAI的成员函数

```c++
    UsrAI::tagInfo getInfo();
```

以下是上述函数返回的结构体说明

```c++
struct tagInfo
{
    vector<tagBuilding> buildings; // 我方建筑列表
    vector<tagFarmer> farmers; // 我方农民列表
    vector<tagArmy> armies; // 我方军队列表
    vector<tagBuilding> enemy_buildings; // 敌方建筑列表
    int enemy_buildings_n; // 敌方建筑数量
    vector<tagFarmer> enemy_farmers; // 敌方农民列表
    vector<tagArmy> enemy_armies; // 敌方军队列表
    vector<tagResource> resources; // 资源列表
    map<int, int> ins_ret; // 指令返回值，map<id, ret>
    int theMap[MAP_L][MAP_U]; // 高程图（存储地图的高度） theMap[BlockDR][BlockUR]
    int GameFrame; // 当前帧数
    int civilizationStage; // 文明阶段
    int Wood; // 木材数量
    int Meat; // 肉类数量
    int Stone; // 石头数量
    int Gold; // 黄金数量
    int Human_MaxNum; // 最大人口数量
};

```

```c++
struct tagBuilding
{
    int BlockDR,BlockUR; //区块坐标
    int Type; // 建筑类型
    int SN; // 序列号
    int Blood; // 当前血量
    int MaxBlood; // 最大血量
    int Percent; // 完成百分比
    int Project; // 当前项目
    int ProjectPercent; // 项目完成百分比
    int Cnt; // 剩余资源量（仅农田）
};
```

```c++
struct tagResource
{
    double DR,UR; //细节坐标
    int BlockDR,BlockUR; //区块坐标
    int Type; // 资源类型
    int SN; // 序列号
    int ProductSort; // 产品种类
    int Cnt; // 剩余资源数量
    int Blood; // 当前血量
};
```

```c++
struct tagHuman
{
    double DR,UR; //细节坐标
    int BlockDR,BlockUR; //区块坐标
    double DR0,UR0; // 目的地坐标
    int NowState; // 当前状态
    int WorkObjectSN; // 工作对象序列号
    int Blood; // 当前血量
    int SN; // 序列号
    int attack; // 攻击力
    int rangedDefense; // 远程防御
    int meleeDefense; // 近战防御
};
```

```C++
struct tagFarmer: public tagHuman
{
    int ResourceSort; // 手持资源种类
    int Resource; // 手持资源数量
};
```

```C++
struct tagArmy:public tagHuman
{
    int Sort;   //军队种类
};
```

```C++
    double calDistance(double DR1, double UR1, double DR2, double UR2) //计算距离

    void DebugText(std::string debugStr) //在侧栏输出调试信息

    void DebugText(int debugInt)

    void DebugText(double debugDouble) 
```

```C++
#include "UsrAI.h"
tagGame tagUsrGame;
ins UsrIns;
/*##########请勿修改以上部分##########*/

#include <fstream>

tagInfo myInfo;

double mid=36*BLOCKSIDELENGTH;  // 中间位置的坐标
bool once=true;
bool isCamp=false;  // 是否有军营
bool isCampFinish=false;  // 军营是否建造完成
int tmpFrame=(int)1e6;  // 临时帧计数
int house;  // 统计房子的数量
int id_Wrong_lastFrame = -1;

void UsrAI::processData()
{
    int nowState_Farmer;
    int SN_res;
    double dis , temp_dis;
    int order_Record;
    static int timers = 1;

    house = 0;
    myInfo = getInfo(); //每帧获取新的tagInfo

    if(id_Wrong_lastFrame >= 0 && timers > 0) //记录了上一帧下达命令的编号
    {
        order_Record = myInfo.ins_ret[id_Wrong_lastFrame];  //获取上一帧下达命令的结果

        DebugText( ("命令" + QString::number(id_Wrong_lastFrame) + "的返回为" + QString::number(order_Record)).toStdString());
        id_Wrong_lastFrame = -1;

        timers --;  //只获取并打印一次
    }

    // 遍历所有建筑（此处用了C++的快速遍历方式）
    for(tagBuilding building : getInfo().buildings)
    {
        if(building.Type==BUILDING_HOME && building.Percent==100)//如果建筑类型是房子且建造完成，增加房子数量计数
        {
            house++;
        }
        else if( building.Type==BUILDING_ARMYCAMP )// 如果建筑类型是军营
        {
            isCamp=true;    //标记有军营

            if(getInfo().GameFrame>tmpFrame+10) // 军营建好后过一会，执行创建棍棒兵的指令
            {
                //下达命令并获取命令号
               id_Wrong_lastFrame = BuildingAction(building.SN,BUILDING_ARMYCAMP_CREATE_CLUBMAN);
            }

            if(building.Percent==100&&!isCampFinish)// 如果军营建造完成且尚未标记完成
            {
                isCampFinish=true;  // 标记军营建造完成

                tmpFrame=tagUsrGame.getInfo().GameFrame;  // 记录下当前帧数

                DebugText("我有兵营啦"); //打印信息到debugText窗口
            }
        }
        else if( building.Type==BUILDING_CENTER && building.Project==ACT_NULL && getInfo().farmers.size()<8 )
        {       // 如果建筑类型是城镇中心且没有进行任何项目且农民数量少于8，创建农民
            BuildingAction(building.SN,BUILDING_CENTER_CREATEFARMER);
        }
    }

    // 遍历所有农民（此处即传统遍历方式）
    for(int i = 0 ; i<myInfo.farmers.size() ; i++)
    {
        nowState_Farmer = myInfo.farmers[i].NowState;

        if( i == 0 && ( nowState_Farmer==HUMAN_STATE_IDLE || nowState_Farmer==HUMAN_STATE_STOP ) )
        {   // 如果是第一个农民且处于空闲或停止状态，建造房子
            static int x_home = 20;
            static int y_home = 20;

            if( x_home < 75 && y_home < 75 && house < 2 )
            {
                HumanBuild(myInfo.farmers[i].SN , BUILDING_HOME , x_home , y_home);
            }
            x_home += 10;
            y_home += 10;

        }
        else if( i == 1 && ( nowState_Farmer==HUMAN_STATE_IDLE || nowState_Farmer==HUMAN_STATE_STOP ) )
        {   // 如果是第二个农民且处于空闲或停止状态，建造军营
            static int x_camp = 20;
            static int y_camp = 20;

            if( x_camp < 75 && y_camp < 75 && !isCamp)
            {
                HumanBuild(myInfo.farmers[i].SN , BUILDING_ARMYCAMP , 75 - x_camp , y_camp);
            }
            x_camp += 10;
            y_camp += 10;

        }
        else if(i < 5 && nowState_Farmer == HUMAN_STATE_IDLE)
        {   // 如果是前三个农民且处于空闲状态，采集浆果资源
            SN_res = -1;
            dis = 1e6;

            for(int j = 0 ; j<myInfo.resources.size() ; j++)
            {
                temp_dis = calDistance(mid , mid , myInfo.resources[j].DR , myInfo.resources[j].UR);
                if(myInfo.resources[j].Type==RESOURCE_BUSH && temp_dis < dis )
                {
                    SN_res = myInfo.resources[j].SN;
                    dis = temp_dis;
                }
            }
            HumanAction(myInfo.farmers[i].SN , SN_res);
        }
        else if( i >= 5 && nowState_Farmer == HUMAN_STATE_IDLE)
        {   // 如果是第五个及以上的农民且处于空闲状态，采集树木资源
            SN_res = -1;
            dis = 1e6;

            for(int j = 0 ; j<myInfo.resources.size() ; j++)
            {
                temp_dis = calDistance(mid , mid , myInfo.resources[j].DR , myInfo.resources[j].UR);

                if(myInfo.resources[j].Type == RESOURCE_TREE && temp_dis < dis)
                {
                    SN_res = myInfo.resources[j].SN;
                    dis = temp_dis;
                }
            }
            HumanAction(myInfo.farmers[i].SN , SN_res);
        }
    }

    // 遍历所有军队
    for(int i = 0 ; i<myInfo.armies.size(); i++)
    {
        temp_dis = calDistance(myInfo.armies[i].DR , myInfo.armies[i].UR , mid+100 , mid-100);

        if(temp_dis > 100 && myInfo.armies[i].NowState==HUMAN_STATE_IDLE)
        {   // 如果军队距离中间位置超过200且处于空闲状态，移动到中间位置
            HumanMove(myInfo.armies[i].SN , mid+100 , mid-100);
        }
    }
}

```
