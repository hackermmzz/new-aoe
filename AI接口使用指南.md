# AI接口使用指南

所有对游戏信息的获取都**必须**通过tagUsrGame/tagEnemyGame

在AI的一次运行中，tagUsrGame/tagEnemyGame存储的游戏信息可能发生改变（每个游戏帧都会刷新tagUsrGame/tagEnemyGame）

以下是您可以调用的控制游戏的接口，所有接口都是AI的成员函数

```c++
    //以下函数为控制函数，若命令下达成功，则返回该指令对应的唯一id
	//通过指令id可以在下一帧查询该指令的执行结果
	//所有的指令一旦下达，无论ai是否结束，都会在下一帧执行
	int HumanMove(int SN, double L0, double U0);

    int HumanAction(int SN,int obSN);

    int HumanBuild(int SN,int BuildingNum,int BlockL,int BlockU);

    int BuildingAction(int SN,int Action);
	//辅助函数，打印对应指令id的执行信息
    void printInsRet(int id);
```

以下是您可以调用的获取游戏信息的接口，所有接口都是tagUsrGame/tagEnemyGame的成员函数

```c++
	//获取食物数量
    int getFood();
	//获取木材数量
    int getWood();
	//获取石材数量
    int getStone();
	//获取金子数量
    int getGold();
	//获取当前游戏帧
    int getGameFrame();
	//获取当前文明进程
    int getCivilizationStage();
	//获取当前的人口上限
    int getMaxHumanNum();
	//获取对应坐标位置的单格地图信息
    tagMap getMap(int L,int U);
	//获取己方农民数组
    vector<tagFarmer> getFarmers();
	//获取己方农民数
    int getFarmers_n();
	//获取敌方农民数组
    vector<tagFarmer> getEnemyFarmers();
	//获取敌方农民数
    int getEnemyFarmers_n();
	//获取己方军队数组
    vector<tagArmy> getArmies();
	//获取己方军队数
    int getArmies_n();
	//获取敌方军队数组
    vector<tagArmy> getEnemyArmies();
	//获取敌方军队数
    int getEnemyArmies_n();
	//获取己方建筑数组
    vector<tagBuilding> getBuildings();
	//获取己方建筑数
    int getBuildings_n();
	//获取敌方建筑数组
    vector<tagBuilding> getEnemyBuildings();
	//获取敌方建筑数
    int getEnemyBuildings_n();
	//获取地图资源数组
    vector<tagResource> getResource();
	//获取地图资源数
    int getResource_n();
	//获取对应指令id的执行信息，若没有该id对应的信息，则返回一个type为-1的非法instruction
	instruction getInsRet(int ins_id);
	//清空记录的指令
    void clearInsRet();
```

以下是上述函数返回的结构体说明

```c++
struct tagMap
{
    bool explore;
    int high;
    int type;       //资源种类（浆果、树等）
    int ResType;    //采集获得的资源种类（食物、木头等） human_WOOD...
    int fundation;  //该资源占地图大小
    int SN;
    int remain;     //剩余资源量
}

```

```c++
struct tagBuilding
{
    int BlockL,BlockU;
    int Type;
    int SN;
    int Blood;
    int MaxBlood;
    int Percent;
    int Project;
    int ProjectPercent;
    int Cnt;
    int Owner;	//归属玩家  0是Usr，1是Enemy
};
```

```c++
struct tagResource
{
    double L,U;
    int BlockL,BlockU;
    int Type;
    int SN;
    int ProductSort;
    int Cnt;
    int Blood;
};
```

```c++
struct tagHuman
{
    double L,U;
    int BlockL,BlockU;
    double L0,U0;   // 目的地
    int NowState;
    int WorkObjectSN;
    int Blood;
    int SN;
    int Owner;
};
```

```C++
struct tagFarmer:public tagHuman
{
    int ResourceSort;
    int Resource;
};
```

```C++
struct tagArmy:public tagHuman
{
    int Sort;   //军队种类
};
```

```C++
struct instruction{
    ///用于存储ai发出的指令信息
    /// @param type 指令类型
    /// @param option 对应类型下的操作
    /// type 0:终止对象self的动作
    /// type 1:命令村民self走向指定坐标L0，U0
    /// type 2:将obj对象设定为村民self的工作对象，村民会自动走向对象并工作
    /// type 3:命令村民self在块坐标BlockL,BlockU处建造类型为option的新建筑
    /// type 4:对建筑self发出命令option
    int ret=-1;
    int type;
    int id;
    Coordinate* self;
    Coordinate* obj;
    int option;
    int BL,BU;
    double L,U;
    bool isExist();
};
```