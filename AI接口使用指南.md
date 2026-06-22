# AI接口使用指南（NewAOE 3.0）

本文只说明写 `UsrAI.cpp` 时需要使用的 AI 接口、状态结构和常量。建筑、兵种、科技的完整数值表以 `new-aoe1新版常量表.docx` 和 `config.h` 为准。

## 1. AI代码入口

用户 AI 的入口是 `UsrAI::processData()`。游戏每帧会尝试唤醒 AI 线程，因此该函数不是一次性执行，而是随游戏帧反复执行。

```cpp
#include "UsrAI.h"
tagGame tagUsrGame;
ins UsrIns;
/*##########DO NOT MODIFY THE CODE ABOVE##########*/

void UsrAI::processData()
{
    tagInfo info = getInfo();  // 每帧先获取当前游戏快照
    // 在这里写策略逻辑，并调用 HumanMove / HumanAction 等接口下令
}
```

注意事项：

- 只需要改 `UsrAI.cpp` 和 `UsrAI.h` 中允许修改的区域。
- 建议在 `processData()` 开头调用一次 `tagInfo info = getInfo();`，本帧后续逻辑都使用这个快照。
- 需要跨帧保存的策略状态，应使用全局变量、静态变量或 `UsrAI` 成员变量。普通局部变量每帧会重新初始化。
- `tagGame::update()` 会打乱 `buildings`、`farmers`、`armies`、`resources` 等列表顺序，策略不要依赖列表下标恒定。
- 同一帧内如果对同一个对象连续下多条指令，内核会按对象 SN 去重，只保留最后一条。

## 2. 获取游戏状态

`getInfo()` 是 `UsrAI` 的成员函数，返回当前帧的 `tagInfo` 快照。

```cpp
tagInfo info = getInfo();
```

### 2.1 tagInfo 总结构

```cpp
struct tagInfo
{
    using TerrainData = const vector<vector<tagTerrain>>;

    vector<tagBuilding> buildings;       // 我方建筑
    vector<tagFarmer> farmers;           // 我方农民、渔船、运输船
    vector<tagArmy> armies;              // 我方军队、战船、投石车等
    vector<tagBuilding> enemy_buildings; // 敌方建筑
    vector<tagFarmer> enemy_farmers;     // 敌方农民类单位
    vector<tagArmy> enemy_armies;        // 敌方军队
    vector<tagResource> resources;       // 可见资源、动物、尸体、鱼等

    map<int, int> ins_ret;               // 指令执行结果：map<指令id, 返回码>
    TerrainData* theMap;                 // 地形图，访问方式见下文
    vector<Point> exploredUpdate;        // 本轮新增探索的块坐标

    int GameFrame;                       // 当前游戏帧
    int civilizationStage;               // 当前文明阶段
    int Wood, Meat, Stone, Gold;         // 当前资源
    int Human_MaxNum;                    // 当前人口上限
};
```

地形访问示例：

```cpp
tagInfo info = getInfo();
int blockDR = 20;
int blockUR = 20;

if (info.theMap != nullptr) {
    const tagTerrain& terrain = (*info.theMap)[blockDR][blockUR];
    int height = terrain.height;
    int type = terrain.type;
}
```

### 2.2 基础对象字段

所有 `tagBuilding`、`tagResource`、`tagFarmer`、`tagArmy` 都包含对象编号和块坐标：

```cpp
struct tagObj
{
    int SN;        // 全局唯一编号，用于下指令
    int BlockDR;   // 块坐标 DR
    int BlockUR;   // 块坐标 UR
};
```

建筑信息：

```cpp
struct tagBuilding : tagObj
{
    int Type;            // 建筑类型，见 BUILDING_TYPE
    int Blood;           // 当前血量
    int MaxBlood;        // 最大血量
    int Percent;         // 建造完成百分比
    int Project;         // 当前执行项目，空闲通常为 ACT_NULL
    int ProjectPercent;  // 当前项目完成百分比
    int Cnt;             // 剩余资源量，仅农田等资源建筑有意义
};
```

资源信息：

```cpp
struct tagResource : tagObj
{
    double DR, UR;       // 细节坐标
    int Type;            // 资源类型，见 RESOURCE_TYPE
    int ProductSort;     // 采集产物类型，见 ProductSort / HUMAN_RESOURCE
    int Cnt;             // 剩余资源量
    int Blood;           // 当前血量，动物或树木等对象有意义
};
```

单位基础信息：

```cpp
struct tagHuman : tagObj
{
    double DR, UR;       // 当前细节坐标
    double DR0, UR0;     // 目的地细节坐标
    int NowState;        // 当前状态，见 HUMAN_STATE
    int WorkObjectSN;    // 当前工作或攻击目标
    int Blood;           // 当前血量
    int attack;          // 攻击力
    int rangedDefense;   // 远程防御
    int meleeDefense;    // 近战防御
};
```

农民类单位：

```cpp
struct tagFarmer : public tagHuman
{
    int ResourceSort;    // 手持资源类型
    int Resource;        // 手持资源数量；运输船中可理解为载员数量
    int FarmerSort;      // FARMERTYPE_FARMER / FARMERTYPE_WOOD_BOAT / FARMERTYPE_SAILING
};
```

军队类单位：

```cpp
struct tagArmy : public tagHuman
{
    int Sort;            // 兵种，见 AT_ARMY
    int status;          // AI或战斗状态标记
    int starttime;
    int finishtime;
    double startpointDR, startpointUR;
    double destinaDR, destinaUR;
    bool ifAttack;
    int timelock;
};
```

地形信息：

```cpp
struct tagTerrain
{
    int height;          // 陆地高度；海洋或特殊地形按地图实现给值
    int type;            // 地图块样式，见 MAPPATTERN
};
```

## 3. 发送控制命令

AI 对游戏的控制应通过 `AI` 成员函数完成。命令函数返回的是“指令 id”，不是最终执行结果。真实执行结果会在下一帧写入 `info.ins_ret[id]`。

| 函数 | 用途 | 参数说明 | 典型目标 |
|------|------|----------|----------|
| `HumanMove(SN, DR0, UR0)` | 命令单位移动 | `SN` 为农民、士兵、船等单位编号；`DR0/UR0` 是细节坐标 | 侦察、集合、撤退 |
| `HumanAction(SN, obSN)` | 命令单位对目标执行上下文动作 | `obSN` 是资源、敌人、建筑、运输船等对象编号 | 采集、攻击、修理、上交、登船 |
| `HumanBuild(SN, BuildingNum, BlockDR, BlockUR)` | 命令陆地村民建造建筑 | `BuildingNum` 取 `BUILDING_TYPE`；位置为块坐标 | 房屋、仓库、市场、马厩等 |
| `BuildingAction(SN, Action)` | 命令建筑生产单位或研发科技 | `Action` 取 `BuildingAction` 枚举 | 造村民、升级时代、训练兵、研发科技 |
| `PinPointStrike(SN, DR0, UR0)` | 命令投石车定点投射 | `SN` 必须是投石车；目标为细节坐标 | 攻城或攻击固定区域 |

示例：查询上一帧命令结果。

```cpp
static int lastOrder = -1;

void UsrAI::processData()
{
    tagInfo info = getInfo();

    if (lastOrder >= 0 && info.ins_ret.count(lastOrder)) {
        int ret = info.ins_ret[lastOrder];
        DebugText(QString("order %1 ret = %2").arg(lastOrder).arg(ret));
        lastOrder = -1;
    }

    if (!info.farmers.empty() && info.farmers[0].NowState == HUMAN_STATE_IDLE) {
        lastOrder = HumanMove(info.farmers[0].SN, 30 * BLOCKSIDELENGTH, 30 * BLOCKSIDELENGTH);
    }
}
```

## 4. 常用操作写法

### 4.1 建造建筑

```cpp
tagInfo info = getInfo();

for (const tagFarmer& farmer : info.farmers) {
    if (farmer.FarmerSort != FARMERTYPE_FARMER) continue;
    if (farmer.NowState != HUMAN_STATE_IDLE) continue;

    HumanBuild(farmer.SN, BUILDING_HOME, 20, 20);
    break;
}
```

`HumanBuild` 会由内核检查资源、时代、前置建筑、探索、地形高度、边界和重叠。如果失败，下一帧从 `ins_ret` 读取错误码。

### 4.2 采集资源

```cpp
tagInfo info = getInfo();

for (const tagFarmer& farmer : info.farmers) {
    if (farmer.FarmerSort != FARMERTYPE_FARMER) continue;
    if (farmer.NowState != HUMAN_STATE_IDLE) continue;

    int targetSN = -1;
    double best = 1e18;
    for (const tagResource& res : info.resources) {
        if (res.Type != RESOURCE_TREE) continue;
        double d = calDistance(farmer.DR, farmer.UR, res.DR, res.UR);
        if (d < best) {
            best = d;
            targetSN = res.SN;
        }
    }

    if (targetSN >= 0) HumanAction(farmer.SN, targetSN);
}
```

### 4.3 建筑生产或研发

```cpp
tagInfo info = getInfo();

for (const tagBuilding& building : info.buildings) {
    if (building.Percent < 100) continue;
    if (building.Project != ACT_NULL) continue;

    if (building.Type == BUILDING_CENTER && info.farmers.size() < info.Human_MaxNum) {
        BuildingAction(building.SN, BUILDING_CENTER_CREATEFARMER);
    }

    if (building.Type == BUILDING_MARKET &&
        info.civilizationStage >= CIVILIZATION_TOOLAGE) {
        BuildingAction(building.SN, BUILDING_MARKET_WHEEL_UPGRADE);
    }
}
```

### 4.4 攻击目标

```cpp
tagInfo info = getInfo();

for (const tagArmy& army : info.armies) {
    if (army.NowState != HUMAN_STATE_IDLE) continue;

    if (!info.enemy_armies.empty()) {
        HumanAction(army.SN, info.enemy_armies[0].SN);
    } else if (!info.enemy_farmers.empty()) {
        HumanAction(army.SN, info.enemy_farmers[0].SN);
    } else if (!info.enemy_buildings.empty()) {
        HumanAction(army.SN, info.enemy_buildings[0].SN);
    }
}
```

### 4.5 投石车定点投射

```cpp
tagInfo info = getInfo();

for (const tagArmy& army : info.armies) {
    if (army.Sort != AT_STONE_THROWER) continue;
    if (army.NowState != HUMAN_STATE_IDLE) continue;

    PinPointStrike(army.SN, 80 * BLOCKSIDELENGTH, 80 * BLOCKSIDELENGTH);
    break;
}
```

取消定点投射不需要单独接口。对同一投石车下达新的 `HumanMove` 或 `HumanAction` 即可覆盖当前行动。

## 5. 常量表摘要

完整表格见 `new-aoe1新版常量表.docx`。下面列写 AI 时最常用的常量。

### 5.1 建筑类型 `BUILDING_TYPE`

| 常量 | 值 | 含义 |
|------|----|------|
| `BUILDING_HOME` | 0 | 房屋 |
| `BUILDING_GRANARY` | 1 | 谷仓 |
| `BUILDING_CENTER` | 2 | 市镇中心 |
| `BUILDING_STOCK` | 3 | 仓库 |
| `BUILDING_FARM` | 4 | 农田 |
| `BUILDING_MARKET` | 5 | 市场 |
| `BUILDING_ARROWTOWER` | 6 | 箭塔 |
| `BUILDING_ARMYCAMP` | 7 | 兵营 |
| `BUILDING_STABLE` | 8 | 马厩 |
| `BUILDING_RANGE` | 9 | 靶场 |
| `BUILDING_DOCK` | 10 | 船坞 |
| `BUILDING_SIEGE` | 11 | 攻城武器厂 |
| `BUILDING_COLLAGE` | 12 | 学院 |
| `BUILDING_TEMPLE` | 13 | 神庙，枚举保留 |
| `BUILDING_WALL` | 14 | 城墙，枚举保留 |

### 5.2 建筑行动 `BuildingAction`

| 建筑 | 常用 Action |
|------|-------------|
| 市镇中心 | `BUILDING_CENTER_CREATEFARMER`、`BUILDING_CENTER_UPGRADE` |
| 谷仓 | `BUILDING_GRANARY_ARROWTOWER`、`BUILDING_GRANARY_WALL`、`BUILDING_GRANARY_ARROWTOWE_UPGRADE` |
| 市场 | `BUILDING_MARKET_WOOD_UPGRADE`、`BUILDING_MARKET_STONE_UPGRADE`、`BUILDING_MARKET_FARM_UPGRADE`、`BUILDING_MARKET_GOLD_UPGRADE`、`BUILDING_MARKET_WHEEL_UPGRADE`、`BUILDING_MARKET_CRAFT_UPGRADE`、`BUILDING_MARKET_PLOW_UPGRADE` |
| 仓库 | `BUILDING_STOCK_UPGRADE_USETOOL`、`BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY`、`BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER`、`BUILDING_STOCK_UPGRADE_DEFENSE_RIDER`、`BUILDING_STOCK_UPGRADE_MISSILE_DEFENSE_INFANTRY` |
| 兵营 | `BUILDING_ARMYCAMP_CREATE_CLUBMAN`、`BUILDING_ARMYCAMP_CREATE_SLINGER`、`BUILDING_ARMYCAMP_UPGRADE_CLUBMAN`、`BUILDING_ARMYCAMP_CREATE_BROADSWORD`、`BUILDING_ARMYCAMP_UPGRADE_BROADSWORD` |
| 靶场 | `BUILDING_RANGE_CREATE_BOWMAN`、`BUILDING_RANGE_CREATE_CHARIOT_ARCHER`、`BUILDING_RANGE_CREATE_COMPOSITE_BOWMAN`、`BUILDING_RANGE_UPGRADE_COMPOSITE_BOW` |
| 马厩 | `BUILDING_STABLE_CREATE_SCOUT`、`BUILDING_STABLE_CREATE_CHARIOT`、`BUILDING_STABLE_CREATE_CAVALRY` |
| 船坞 | `BUILDING_DOCK_CREATE_SAILING`、`BUILDING_DOCK_CREATE_WOOD_BOAT`、`BUILDING_DOCK_CREATE_SHIP` |
| 攻城武器厂 | `BUILDING_SIEGE_CREATE_STONE_THROWER` |
| 学院 | `BUILDING_COLLAGE_CREATE_HOPLITE` |

说明：

- `BUILDING_CENTER_UPGRADE` 会按当前时代推进时代升级。
- 仓库部分科技是两级链，同一个 Action 第一次研发一级，第二次研发铜器时代升级。
- 市场中 `BUILDING_MARKET_WOOD_UPGRADE` 和 `BUILDING_MARKET_FARM_UPGRADE` 在科技链中也有二级效果；新版常量表已经标出对应关系。

### 5.3 兵种 `AT_ARMY`

| 常量 | 值 | 含义 |
|------|----|------|
| `AT_CLUBMAN` | 0 | 棍棒兵 |
| `AT_SLINGER` | 1 | 投石兵 |
| `AT_BOWMAN` | 2 | 弓箭手 |
| `AT_SCOUT` | 3 | 侦察骑兵 |
| `AT_SWORDSMAN` | 4 | 战斧或敌方近战单位 |
| `AT_IMPROVED` | 5 | 改进型单位或敌方单位 |
| `AT_CAVALRY` | 6 | 骑兵 |
| `AT_SHIP` | 7 | 战船 |
| `AT_STONE_THROWER` | 8 | 投石车 |
| `AT_PRIEST` | 9 | 祭司 |
| `AT_HOPLITE` | 10 | 方阵兵 |
| `AT_CHARIOT` | 11 | 四马战车 |
| `AT_CHARIOT_ARCHER` | 12 | 战车弓兵 |
| `AT_BROADSWORDSMAN` | 13 | 阔剑兵 |
| `AT_COMPOSITE_BOWMAN` | 14 | 复合弓兵 |

### 5.4 资源、状态和时代

| 类别 | 常量 |
|------|------|
| 资源类型 | `RESOURCE_BUSH`、`RESOURCE_TREE`、`RESOURCE_STONE`、`RESOURCE_GAZELLE`、`RESOURCE_ELEPHANT`、`RESOURCE_LION`、`RESOURCE_GOLD`、`RESOURCE_FISH` |
| 农民手持资源 | `HUMAN_WOOD`、`HUMAN_STOCKFOOD`、`HUMAN_STONE`、`HUMAN_GOLD`、`HUMAN_GRANARYFOOD`、`HUMAN_DOCKFOOD` |
| 农民类型 | `FARMERTYPE_FARMER`、`FARMERTYPE_WOOD_BOAT`、`FARMERTYPE_SAILING` |
| 文明阶段 | `CIVILIZATION_STONEAGE`、`CIVILIZATION_TOOLAGE`、`CIVILIZATION_BRONZEAGE`、`CIVILIZATION_IRONAGE` |
| 单位状态 | `HUMAN_STATE_IDLE`、`HUMAN_STATE_WALKING`、`HUMAN_STATE_WORKING`、`HUMAN_STATE_ATTACKING` |
| 指令类型 | `INS_CANCEL`、`INS_HUMANMOVE`、`INS_HUMANACTION`、`INS_HUMANBUILD`、`INS_BUILDINGACTION`、`INS_PINPOINT_STRIKE` |

## 6. 常见返回码

`info.ins_ret[id]` 中 `0` 表示 `ACTION_SUCCESS`。非 0 表示失败或非法动作，常见返回码如下。

| 返回码常量 | 含义 |
|------------|------|
| `ACTION_INVALID_SN` | 控制对象不存在、死亡，或 SN 类型不适合该命令 |
| `ACTION_INVALID_ACTION` | 对象不能执行该动作 |
| `ACTION_INVALID_OBSN` | 目标对象不存在或目标类型非法 |
| `ACTION_INVALID_LOCATION` | 目标坐标超出地图或不合法 |
| `ACTION_INVALID_BUILDINGNUM` | 建筑类型非法 |
| `ACTION_INVALID_RESOURCE` | 资源不足 |
| `ACTION_INVALID_HUMANBUILD_DIFFERENTHIGH` | 建造区域高度不一致 |
| `ACTION_INVALID_HUMANBUILD_OVERBORDER` | 建筑区域超出地图边界 |
| `ACTION_INVALID_HUMANBUILD_UNEXPLORE` | 建造位置未探索 |
| `ACTION_INVALID_HUMANBUILD_OVERLAP` | 建造位置与已有对象冲突 |
| `ACTION_INVALID_HUMANBUILD_LOCK` | 建筑未解锁或被配置禁用 |
| `ACTION_INVALID_DISTANCE_FAR` | 距离过远 |
| `ACTION_INVALID_ISNTFREE` | 对象已有不能自动覆盖的任务 |
| `ACTION_INVALID_NULLGOALOBJECT` | 目标对象已被删除 |
| `ACTION_INVALID_NULLWORKER` | 执行动作的对象已被删除 |
| `ACTION_INVALID_BUILDACT_MAXHUMAN` | 造人或造兵时人口已满 |
| `ACTION_INVALID_BUILDACT_LOCK` | 建筑行动未解锁，或次数已达上限 |
| `ACTION_INVALID_BUILDACT_NEEDBUILT` | 建筑尚未建造完成 |
| `ACTION_INVALID_PINPOINT_NOT_FIT` | 定点投射目标位置不合法 |
| `ACTION_INVALID_PRIEST_TARGET_ERROR` | 祭司目标不合法 |
| `ACTION_INVALID_UPGRADE_TIME` | 科技升级时机不合法 |

写策略时应尽量通过资源、时代、建筑完成度、单位空闲状态等条件提前避免失败；返回码主要用于调试和定位问题。

## 7. 调试和作弊接口

调试输出：

```cpp
DebugText("hello");
DebugText(QString("frame=%1").arg(info.GameFrame));
DebugText(123);
DebugText(3.14);
```

辅助函数：

```cpp
double d = calDistance(dr1, ur1, dr2, ur2);
```

调试作弊：

```cpp
cheatAction(); // 后续命令不消耗时间，考试模式下失效
cheatRes();    // 增加资源，考试模式下失效
```

正式评测时不要依赖作弊接口。

