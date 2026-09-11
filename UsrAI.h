#ifndef USRAI_H
#define USRAI_H

#include "ai.h"
#include <unordered_map>

extern tagGame tagUsrGame;
extern ins UsrIns;
/*##########DO NOT MODIFY THE CODE ABOVE##########*/

class UsrAI:public AI
{
public:
    UsrAI(){this->id=0;}
    ~UsrAI(){}

private:
    void processData() override;
    tagInfo getInfo(){return tagUsrGame.getInfo();}
    int AddToIns(instruction ins) override
    {
        UsrIns.lock.lock();
        ins.id=UsrIns.g_id;
        UsrIns.g_id++;
        UsrIns.instructions.push(ins);
        UsrIns.lock.unlock();
        return ins.id;
    }
    void clearInsRet() override
    {
        tagUsrGame.clearInsRet();
    }
    /*##########DO NOT MODIFY THE CODE IN THE CLASS##########*/



};

/*##########YOUR CODE BEGINS HERE##########*/

#include <set>
#include <utility>

/**
 * @brief 跨帧保存的 AI 状态
 *
 * processData() 每帧被唤醒一次，局部变量无法跨帧保留，
 * 因此所有需要跨帧记住的策略状态都放在这里（全局实例 gAiData）。
 *
 * 注意：getInfo() 返回的列表每帧会被随机打乱，策略不要依赖下标，
 *      应依赖 SN（全局唯一编号）。
 */
struct MyAiData
{
  // ---------- 对局检测 ----------
  // gAiData 是全局变量，对局间会残留上一局的跨帧状态（任务、锚点、血量记录等），
  // 导致新开局祭司不探索/行为异常。用帧号回退检测新对局并整体重置。
  int lastFrame = -1;

  // ---------- 已探索区域 ----------
  // 未探索的格子在内核寻路中被视为障碍（Map.cpp:946），
  // 所以探路/移动的目标必须限制在已探索区域。
  // 用每帧的 exploredUpdate 累积维护（块坐标集合）。
  std::set<std::pair<int,int>> exploredSet;

  // ---------- 阶段状态机（对应课程设计任务的 4 个阶段） ----------
  // stage 0 = 经济期：探路 + 采集 + 建房/兵营/市场 + 造农民（升铜器前）
  // stage 1 = 爆兵期：升铜器后，挖金 + 研发强力兵种 + 防御第二、三波
  // stage 2 = 反攻期：军队推进敌方基地 + 祭司转化攻城武器厂（最终目标）
  int stage = 0;

  // ---------- 农民分工 ----------
  // key   : 农民 SN；value : 采集角色（0 木 / 1 食 / 2 石 / 3 金）
  std::map<int, int> farmerRole;

  // ---------- 建造队列 ----------
  // 待建造建筑类型队列（BUILDING_TYPE），一次只派一个专职农民建队首建筑
  std::vector<int> buildQueue;

  // ---------- 科技研究标记（避免对同一级科技反复下令） ----------
  bool techWoodDone = false;      // 市场"伐木"科技
  bool techWheelDone = false;     // 市场"车轮"科技
  bool techCraftDone = false;     // 市场"工艺"科技（伐木二级，铜器）
  bool techStockDone = false;     // 仓库"工具使用"科技（近战攻击）
  bool arrowTowerResearched = false; // 谷仓"箭塔"科技
  bool arrowTowerUpgraded = false;   // 谷仓"箭塔升级"科技（铜器）
  bool broadswordUpgraded = false;   // 兵营"阔剑兵"科技
  bool compositeUpgraded = false;    // 靶场"复合弓"科技

  // ---------- 祭司行为管理（状态机） ----------
  // 0=探索（去地图中间） 1=回塔 2=守塔 3=转化目标 4=绕塔转圈躲避
  int  priestTask = 0;          // 当前任务
  int  priestOrbitAngle = 0;    // 绕塔转圈的角度（度，递增）
  int  priestHurtFrame = -1;    // 最后受伤的帧号（长时间无伤则结束转圈）
  int  scoutNextFrame = -1;     // 移动指令节流帧（移动距离/速度）
  int  scoutPrevBlood = -1;     // 上一帧血量（用于检测是否受伤）
  int  scoutAnchorX = -1, scoutAnchorY = -1; // 探索锚点（块坐标，-1=未初始化，逐步向地图中间前进）
  double scoutPosX = 0, scoutPosY = 0;  // 上次位置（细节坐标，用于"3秒未移动=卡住"检测）
  int    scoutPosFrame = -1;            // 上次位置检查帧
  int    scoutStall = 0;                // 探索"无法推进"连续帧数（超过阈值则回塔结束探索）

  // ---------- 单位自保 / 治疗 ----------
  std::map<int, int> archerPrevBlood;  // 远程单位血量记录（检测受伤逃离）
  int lastEnemyVisibleFrame = -1;      // 最后看到敌人的帧号（判断波次间隙，触发治疗）

  // ---------- 反攻阶段 ----------
  bool attacking = false;        // 是否已进入反攻阶段（stage 2）
  int  attackFrame = -1;         // 进入反攻的帧号（日志用）
  int  siegeDR = -1, siegeUR = -1; // 最近观测到的敌方攻城武器厂位置（块坐标）

  // ---------- 专职建造管理 ----------
  // 指定一个农民专职建造，采集逻辑跳过它，避免"没空闲农民"导致建造卡死
  int  builderSn = -1;           // 专职建造农民 SN（-1 表示无）
  int  buildTarget = -1;         // 正在建造的建筑类型
  int  buildNeedCount = 0;       // 目标：该类型建筑应达到的数量
  int  buildDR = -1, buildUR = -1; // 建造位置（块坐标）
  int  buildTries = 0;           // 当前任务重试次数
};

// 全局唯一实例，定义在 UsrAI.cpp
extern MyAiData gAiData;

/*##########YOUR CODE ENDS HERE##########*/
#endif // USRAI_H
