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

以下是您可以调用的获取游戏信息的接口，为tagUsrGame的成员函数

```c++
    tagInfo getInfo();
```

以下是上述函数返回的结构体说明

```c++
struct tagInfo
{
    vector<tagBuilding> buildings; // 我方建筑列表
    int buildings_n; // 我方建筑数量
    vector<tagFarmer> farmers; // 我方农民列表
    int farmers_n; // 我方农民数量
    vector<tagArmy> armies; // 我方军队列表
    int armies_n; // 我方军队数量
    vector<tagBuilding> enemy_buildings; // 敌方建筑列表
    int enemy_buildings_n; // 敌方建筑数量
    vector<tagFarmer> enemy_farmers; // 敌方农民列表
    int enemy_farmers_n; // 敌方农民数量
    vector<tagArmy> enemy_armies; // 敌方军队列表
    int enemy_armies_n; // 敌方军队数量
    vector<tagResource> resources; // 资源列表
    int resources_n; // 资源数量
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
    int Owner; // 所有者
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
    int Owner; // 所有者
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

