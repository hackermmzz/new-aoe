#include "UsrAI.h"
#include<set>
#include <iostream>
#include<unordered_map>
#include<list>
#include <cstdlib>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>

using namespace std;
tagGame tagUsrGame;
ins UsrIns;
MyAiData gAiData;

// ============================================================
// 可调参数（改这些常量即可调整 AI 运营节奏）
// ============================================================
static const int kMaxFarmers = 22;     // 经济农民数量目标（文档建议 20~28）
static const int kAttackArmyMin = 26;  // 发起反攻所需的最少兵力

// ============================================================
// 工具函数
// ============================================================

/**
 * @brief 细节坐标欧氏距离
 */
static double disDetail(double dr1, double ur1, double dr2, double ur2)
{
  double dx = dr1 - dr2, dy = ur1 - ur2;
  return sqrt(dx * dx + dy * dy);
}

/**
 * @brief 块坐标欧氏距离
 */
static double blockDis(int b1, int u1, int b2, int u2)
{
  double dx = b1 - b2, dy = u1 - u2;
  return sqrt(dx * dx + dy * dy);
}

/**
 * @brief 块坐标 → 细节坐标（HumanMove 只接受细节坐标）
 */
static double blockToDetail(int block)
{
  return (block + 0.5) * BLOCKSIDELENGTH;
}

/**
 * @brief 统计某类型建筑总数量（含建造中的地基）
 */
static int countType(const vector<tagBuilding>& buildList, int type)
{
  int c = 0;
  for (const auto& b : buildList) if (b.Type == type) c++;
  return c;
}

/**
 * @brief 统计"已建成"（Percent>=100）的某类型建筑数量
 *
 * 建造完成判定和需求分析都用它，避免把"地基已创建"误当"已建成"，
 * 从而无限排队建房、其他建筑排不上队。
 */
static int countFinishedType(const tagInfo& info, int type)
{
  int c = 0;
  for (const auto& b : info.buildings) if (b.Type == type && b.Percent >= 100) c++;
  return c;
}

/**
 * @brief 当前帧是否存在指定类型资源点
 */
static bool hasType(const tagInfo& info, int type)
{
  for (const auto& r : info.resources) if (r.Type == type) return true;
  return false;
}

/**
 * @brief 找距指定坐标最近的某类型资源
 * @return 资源 SN；找不到返回 -1
 */
static int findNearestRes(const tagInfo& info, int type, double dr, double ur)
{
  int best = -1;
  double bestD = 1e18;
  for (const auto& r : info.resources) {
    if (r.Type != type) continue;
    double d = disDetail(dr, ur, r.DR, r.UR);
    if (d < bestD) { bestD = d; best = r.SN; }
  }
  return best;
}

/**
 * @brief 找距指定块坐标最近的"已建成农田"
 * @return 农田建筑 SN；找不到返回 -1
 */
static int findNearestFarm(const tagInfo& info, int bdr, int bur)
{
  int best = -1;
  double bestD = 1e18;
  for (const auto& b : info.buildings) {
    if (b.Type != BUILDING_FARM || b.Percent < 100) continue;
    double d = blockDis(b.BlockDR, b.BlockUR, bdr, bur);
    if (d < bestD) { bestD = d; best = b.SN; }
  }
  return best;
}

/**
 * @brief 判断兵种是否为远程（第一波策略：远程兵留给祭司转化，军队只打近战）
 */
static bool isRangedSort(int sort)
{
  return sort == AT_BOWMAN || sort == AT_IMPROVED ||
         sort == AT_COMPOSITE_BOWMAN || sort == AT_CHARIOT_ARCHER ||
         sort == AT_STONE_THROWER;
}

/**
 * @brief 敌方单位的"价值"（祭司转化优先级）
 * 第二波时祭司按价值从高到低依次转化最强单位；
 * 平时给弓箭手额外加成（消除远程威胁）。
 */
static int unitValue(int sort)
{
  switch (sort) {
    case AT_STONE_THROWER:    return 100;
    case AT_HOPLITE:          return 90;
    case AT_BROADSWORDSMAN:   return 80;
    case AT_COMPOSITE_BOWMAN: return 70;
    case AT_CHARIOT_ARCHER:   return 60;
    case AT_CHARIOT:          return 50;
    case AT_CAVALRY:          return 40;
    case AT_IMPROVED:         return 30;
    case AT_BOWMAN:           return 28;
    case AT_SWORDSMAN:        return 25;
    case AT_CLUBMAN:          return 20;
    case AT_SHIP:             return 10;
    default:                  return 1;
  }
}

/**
 * @brief 采集角色 → 资源类型
 *   0 木 / 1 食 / 2 石 / 3 金
 * 食物优先浆果（无需战斗），其次羚羊/大象（需击杀），最后鱼。
 * @return 对应资源类型；该类型当前不存在返回 -1
 */
static int roleToResType(int role, const tagInfo& info)
{
  switch (role) {
    case 0: return hasType(info, RESOURCE_TREE) ? RESOURCE_TREE : -1;
    case 1:
      if (hasType(info, RESOURCE_BUSH)) return RESOURCE_BUSH;
      if (hasType(info, RESOURCE_GAZELLE)) return RESOURCE_GAZELLE;
      if (hasType(info, RESOURCE_ELEPHANT)) return RESOURCE_ELEPHANT;
      if (hasType(info, RESOURCE_FISH)) return RESOURCE_FISH;
      return -1;
    case 2: return hasType(info, RESOURCE_STONE) ? RESOURCE_STONE : -1;
    case 3: return hasType(info, RESOURCE_GOLD) ? RESOURCE_GOLD : -1;
  }
  return -1;
}

/**
 * @brief 为新空闲农民选采集角色（按缺口补位）
 *
 * 策略：食物 > 木头 > 石头 > 金子。
 * 造农民/产兵/升时代都消耗食物，所以食物农民数量要足；
 * 铜器后兵种需要金子，采金农民目标提高。
 */
static int pickRole(const tagInfo& info, int cWood, int cFood, int cStone, int cGold, int farmerCount)
{
  int targetWood = 6, targetFood = 9, targetStone = 3, targetGold = 3;
  if (farmerCount <= 10) { targetWood = 4; targetFood = 5; }      // 开局：木4 食5
  if (info.civilizationStage >= CIVILIZATION_BRONZEAGE) {
    targetFood = 7;   // 铜器后食物需求仍高（造兵）
    targetGold = 4;   // 铜器兵种需要金子（阔剑/复合弓/方阵/骑兵）
  }
  if (cFood < targetFood) return 1;
  if (cWood < targetWood) return 0;
  if (cGold < targetGold) return 3;
  if (cStone < targetStone) return 2;
  return 0;
}

/**
 * @brief 建筑造价预检（避免 HumanBuild 被"资源不足"拒绝）
 */
static bool enoughForBuild(const tagInfo& info, int buildType)
{
  switch (buildType) {
    case BUILDING_HOME:      return info.Wood >= 30;
    case BUILDING_STOCK:     return info.Wood >= 120;
    case BUILDING_GRANARY:   return info.Wood >= 120;
    case BUILDING_MARKET:    return info.Wood >= 150;
    case BUILDING_ARMYCAMP:  return info.Wood >= 125;
    case BUILDING_RANGE:     return info.Wood >= 150;
    case BUILDING_STABLE:    return info.Wood >= 150;
    case BUILDING_COLLAGE:   return info.Wood >= 180;
    case BUILDING_FARM:      return info.Wood >= 75;
    case BUILDING_ARROWTOWER:return info.Stone >= 150;
    default: return false;
  }
}

/**
 * @brief 判断某个块是否可被建筑占用（无静态资源、无建筑、无单位）
 *
 * 树/浆果/矿/鱼静止，挡住建造，视为障碍；动物会走动，不算永久障碍。
 * 单位（农民/军队/敌方单位）站着的格也算占用，避免"与其他物体重叠"。
 */
static bool isBlockFree(const tagInfo& info, int x, int y)
{
  for (const auto& r : info.resources) {
    if (r.BlockDR != x || r.BlockUR != y) continue;
    if (r.Type == RESOURCE_TREE || r.Type == RESOURCE_BUSH ||
        r.Type == RESOURCE_STONE || r.Type == RESOURCE_GOLD ||
        r.Type == RESOURCE_FISH) return false;
  }
  for (const auto& b : info.buildings) {
    if (b.BlockDR == x && b.BlockUR == y) return false;
  }
  for (const auto& b : info.enemy_buildings) {
    if (b.BlockDR == x && b.BlockUR == y) return false;
  }
  for (const auto& h : info.farmers)       if (h.BlockDR == x && h.BlockUR == y) return false;
  for (const auto& a : info.armies)        if (a.BlockDR == x && a.BlockUR == y) return false;
  for (const auto& h : info.enemy_farmers) if (h.BlockDR == x && h.BlockUR == y) return false;
  for (const auto& a : info.enemy_armies)  if (a.BlockDR == x && a.BlockUR == y) return false;
  return true;
}

/**
 * @brief 在市镇中心周围找一块平坦、无遮挡的空地（按 2x2 占地检查）
 *
 * @param attempt 尝试序号：第 0 次从半径 3 开始；失败后从更大半径重新搜，
 *                避免每次都找到同一个位置死磕。
 * @param outDR, outUR 输出建造位置（块坐标）
 */
static bool findBuildSpot(const tagInfo& info, int baseDR, int baseUR, int attempt, int& outDR, int& outUR)
{
  if (info.theMap == nullptr) return false;
  int rMin = 3 + attempt;
  if (rMin > 11) rMin = 11;
  for (int r = rMin; r <= 11; r++)
  {
    for (int dx = -r; dx <= r; dx++)
    {
      for (int dy = -r; dy <= r; dy++)
      {
        if (abs(dx) != r && abs(dy) != r) continue;   // 只扫当前半径的环
        int x = baseDR + dx, y = baseUR + dy;
        if (x < 1 || x >= MAP_L - 2 || y < 1 || y >= MAP_U - 2) continue;
        bool ok = true;
        for (int ox = 0; ox <= 1; ox++)
        {
          for (int oy = 0; oy <= 1; oy++)
           {
            int cx = x + ox, cy = y + oy;
            const tagTerrain& ct = (*info.theMap)[cx][cy];
            if (ct.type != MAPPATTERN_GRASS && ct.type != MAPPATTERN_DESERT) { ok = false; break; }
            if (ct.height != 0) { ok = false; break; }
            if (!isBlockFree(info, cx, cy)) { ok = false; break; }
          }
          if (!ok) break;
        }
        if (!ok) continue;
        outDR = x; outUR = y;
        return true;
      }
    }
  }
  return false;
}

// ============================================================
// AI 每帧入口
// ============================================================

/**
 * @brief AI 每帧执行入口
 *
 * 按课程设计任务的 4 个阶段组织策略：
 *   阶段0（经济期）：祭司探路 + 采集 + 造农民 + 建房/兵营/市场/靶场，
 *                    食物 850 + 市场/靶场/马厩中2种后升铜器
 *   阶段1（爆兵期）：挖金 + 研发阔剑/复合弓 + 学院方阵 + 马厩骑兵，
 *                    防御第二、三波，种田补充食物
 *   阶段2（反攻期）：军队推进敌方基地清场，祭司转化攻城武器厂获胜
 */

void UsrAI::processData()
{
  // 第一步永远先取当前帧快照
  tagInfo info = getInfo();
  if (info.theMap == nullptr) return;

  const int frame = info.GameFrame;

  // ---------- 新对局检测：帧号回退说明开了新局，重置所有跨帧状态 ----------
  // （gAiData 是全局变量，上一局的任务/锚点/血量记录等会残留，必须清空）
  if (gAiData.lastFrame >= 0 && frame < gAiData.lastFrame) {
    gAiData = MyAiData();
  }
  gAiData.lastFrame = frame;

  // ---------- 基地（我方市镇中心）定位 ----------
  int baseDR = 50, baseUR = 50;
  const tagBuilding* center = nullptr;
  for (const auto& b : info.buildings)
  {
    if (b.Type == BUILDING_CENTER) { center = &b; baseDR = b.BlockDR; baseUR = b.BlockUR; break; }
  }

  // ---------- 箭塔位置（供祭司探路逃跑 / 第一波防御 / 保护祭司共用） ----------
  int towerDR = -1, towerUR = -1;
  for (const auto& b : info.buildings)
  {
    if (b.Type == BUILDING_ARROWTOWER && b.Percent >= 100) { towerDR = b.BlockDR; towerUR = b.BlockUR; break; }
  }

  // ---------- 累积已探索区域（未探索格=寻路障碍，移动目标必须限制在已知区域） ----------
  for (const auto& p : info.exploredUpdate) {
    gAiData.exploredSet.insert(std::make_pair(p.x, p.y));
  }

  // ---------- 阶段推进 ----------
  // 升铜器后从经济期进入爆兵期
  if (gAiData.stage == 0 && info.civilizationStage >= CIVILIZATION_BRONZEAGE)
  {
    gAiData.stage = 1;
  }
  // 反攻（stage 2）在下方按兵力/时间开启

  // ---------- 1. 时代升级（优先于造农民，避免市中心被占用） ----------
  if (center && center->Percent >= 100 && center->Project == ACT_NULL)
  {
    // 升铜器：食物足够 + 市场/靶场/马厩中至少 2 种已建成（时代前置条件）
    if (info.civilizationStage < CIVILIZATION_BRONZEAGE && info.Meat >= 850)
    {
      int toolB = 0;
      if (countFinishedType(info, BUILDING_MARKET) > 0) toolB++;
      if (countFinishedType(info, BUILDING_RANGE) > 0) toolB++;
      if (countFinishedType(info, BUILDING_STABLE) > 0) toolB++;
      if (toolB >= 2) {
        BuildingAction(center->SN, BUILDING_CENTER_UPGRADE);  // 升铜器
      }
    }
    else if (info.civilizationStage == CIVILIZATION_BRONZEAGE && info.Meat >= 1300 && info.Gold >= 300)
    {
      BuildingAction(center->SN, BUILDING_CENTER_UPGRADE);  // 升铁器（资源富余时再升）
    }
  }

  // ---------- 2. 市镇中心造农民（到 kMaxFarmers 之前持续补） ----------
  if (info.Human_Num < info.Human_MaxNum && info.Human_Num < kMaxFarmers)
  {
    int farmerCount = 0;
    for (const auto& f : info.farmers) if (f.FarmerSort == FARMERTYPE_FARMER)
        farmerCount++;
    bool wantFarmer = false;
    if (farmerCount < 14)
        wantFarmer = (info.Meat >= 70);        // 前期：优先补经济
    else
        wantFarmer = (info.Meat >= 150);                        // 后期：有富余食物才补（省食物产兵）
    if (wantFarmer && center && center->Percent >= 100 && center->Project == ACT_NULL)
    {
      BuildingAction(center->SN, BUILDING_CENTER_CREATEFARMER);
    }
  }

  // ---------- 3. 祭司行为管理（状态机） ----------
  // 策略：
  //   1) 开局先移动到地图中间探索视野（目标点是陆地，避免卡在海里）
  //   2) 视野内出现敌方单位 或 时间到3分钟(4500帧) → 立即赶回箭塔
  //   3) 守塔时视野内出现转化目标（平时优先弓箭手，第二波优先最强单位）→ 去转化
  //   4) 转化距离控制：目标在 12 格(祭司射程)内直接转化，否则移动到 12 格处
  //      "遥控转化"，与敌人保持最大距离，保护祭司
  //   5) 转化后回到箭塔周围守塔，冷却结束自动继续转化下一个
  //   6) 受到伤害 → 围着箭塔一直转圈躲避
  // 任务：0探索 1回塔 2守塔 3转化 4绕塔转圈
  if (!gAiData.attacking) {
    // 更新"最后看到敌人"的帧（波次间隙治疗用）
    if (!info.enemy_armies.empty()) gAiData.lastEnemyVisibleFrame = frame;

    for (const auto& a : info.armies) {
      if (a.Sort != AT_PRIEST) continue;
      double pDr = blockToDetail(a.BlockDR), pUr = blockToDetail(a.BlockUR);
      // 防守中心：优先箭塔，无箭塔用市中心
      int cDR = (towerDR >= 0) ? towerDR : baseDR;
      int cUR = (towerUR >= 0) ? towerUR : baseUR;
      double cDr = blockToDetail(cDR), cUr = blockToDetail(cUR);

      // 受伤检测：血量下降
      bool hurt = (gAiData.scoutPrevBlood >= 0 && a.Blood < gAiData.scoutPrevBlood);
      if (hurt) gAiData.priestHurtFrame = frame;
      gAiData.scoutPrevBlood = a.Blood;

      // 视野内是否有敌人
      bool enemyVisible = false;
      for (const auto& e : info.enemy_armies) {
        if (disDetail(pDr, pUr, e.DR, e.UR) < 14 * BLOCKSIDELENGTH) { enemyVisible = true; break; }
      }

      // 转化目标选择：第二波按价值选最强单位；平时优先弓箭手（价值+500）
      // enemy_armies 本身就是"玩家可见"的敌人（内核已按视野过滤），
      // 无需再按距离过滤；距离只用于 task3 的"遥控转化"（12格内直接转，外则保持最远距离）
      bool secondWave = (frame > 12500 && frame < 18000);
      int convertTarget = -1;
      double cBest = -1e18;
      for (const auto& e : info.enemy_armies) {
        if (e.Sort == AT_SHIP) continue;
        if (e.Blood <= 0) continue;   // 不转化已死亡的目标
        double val = unitValue(e.Sort);
        if (!secondWave && (e.Sort == AT_BOWMAN || e.Sort == AT_IMPROVED)) val += 500;
        if (val > cBest) { cBest = val; convertTarget = e.SN; }
      }

      // 转化优先级最高：冷却好（能转化）时不转圈，优先转化视野内最强单位；
      // 只在转化冷却期间（无法转化）且血量下降时才绕塔转圈躲避；血<10 仍强制回塔
      if (hurt && a.ConvertCooldown > 0) gAiData.priestTask = 4;

      // 血线保命（最高优先）：血 < 10 才强制回塔（用户要求：转化优先级最高，除非血降到 10 以下）
      if (a.Blood < 10) gAiData.priestTask = 1;

      int task = gAiData.priestTask;

      if (task == 0) {
        // 探索：逐步向地图中间前进，每次前进一个祭司视野（12块）
        // 若目标点不可达（海洋），改为该点周围一圈（半径12块）找最近的陆地
        if (enemyVisible || frame >= 4500) {
          gAiData.priestTask = 1;
        } else {
          // 卡住检测：3秒（75帧）内位置几乎没变（<4块）→ 卡住，锚点跳过当前目标点
          if (gAiData.scoutPosFrame < 0) {
            gAiData.scoutPosX = a.DR;
            gAiData.scoutPosY = a.UR;
            gAiData.scoutPosFrame = frame;
          } else if (frame - gAiData.scoutPosFrame >= 75) {
            double moved = disDetail(a.DR, a.UR, gAiData.scoutPosX, gAiData.scoutPosY);
            if (moved < 4 * BLOCKSIDELENGTH) {
              gAiData.scoutStall += 60;   // 卡住：累计，加速触发"探索到头回塔"
            } else {
              gAiData.scoutStall = 0;
            }
            gAiData.scoutPosX = a.DR;
            gAiData.scoutPosY = a.UR;
            gAiData.scoutPosFrame = frame;
          }
          // 移动分支：只有空闲且到节流时间才发指令
          if (a.NowState == HUMAN_STATE_IDLE && frame >= gAiData.scoutNextFrame) {
            // 目标：祭司真实视野（12块内）的已探索陆地中，"向地图中间方向投影最大"的点。
            // 关键1：目标距祭司 <= 12 块（真实视野内）→ 路径必然已探索，A* 可达，不会卡住
            //        （旧的 ±12 偏移搜索范围对角线可达 17 块，会选到不连通的远处格导致卡住）
            // 关键2：朝地图中间（另一侧/敌基方向）推进，而不是在地图边缘打转
            int px = a.BlockDR, py = a.BlockUR;
            double pax2 = blockToDetail(px), pay2 = blockToDetail(py);
            double mdx = blockToDetail(MAP_L / 2) - pax2;   // 指向地图中间
            double mdy = blockToDetail(MAP_U / 2) - pay2;
            double ml = sqrt(mdx * mdx + mdy * mdy);
            if (ml < 1.0) ml = 1.0;
            mdx /= ml; mdy /= ml;
            int bestX = px, bestY = py;
            double bestProj = -1e18;
            bool targetOk = false;
            for (int r = 0; r <= 12; r++) {
              for (int ox = -r; ox <= r; ox++) {
                for (int oy = -r; oy <= r; oy++) {
                  if (abs(ox) != r && abs(oy) != r) continue;   // 只扫当前半径的环
                  int cx = px + ox, cy = py + oy;
                  if (cx < 1 || cx >= MAP_L - 1 || cy < 1 || cy >= MAP_U - 1) continue;
                  // 真实视野限制：目标必须在祭司 12 块内（保证路径已探索、可达）
                  double pd = sqrt((double)((cx - px) * (cx - px) + (cy - py) * (cy - py)));
                  if (pd > 12.0) continue;
                  if (!gAiData.exploredSet.count(std::make_pair(cx, cy))) continue;
                  const tagTerrain& ct = (*info.theMap)[cx][cy];
                  if (ct.type == MAPPATTERN_OCEAN || ct.type == MAPPATTERN_SHOAL) continue;
                  double vx = blockToDetail(cx) - pax2, vy = blockToDetail(cy) - pay2;
                  double proj = vx * mdx + vy * mdy;   // 向中间方向投影越大越好
                  if (proj > bestProj) { bestProj = proj; bestX = cx; bestY = cy; targetOk = true; }
                }
              }
            }
            if (targetOk) {
              double mDr = blockToDetail(bestX), mUr = blockToDetail(bestY);
              double dMid = disDetail(pDr, pUr, mDr, mUr);
              if (dMid > 2 * BLOCKSIDELENGTH) {
                HumanMove(a.SN, mDr, mUr);
                gAiData.scoutNextFrame = frame + (int)(dMid / SPEED_PRIEST) + 20;
                gAiData.scoutStall = 0;
              } else {
                gAiData.scoutStall++;   // 目标太近（前方被海/边缘挡）
              }
            }
            // 兜底：向中间方向无可用目标（被海/地图边缘挡）时，
            // 改找"视野内距祭司最远"的已探索陆地，换方向继续探索，不因此回塔。
            // 只有"时间到3分钟"或"视野出现敌人"才结束探索（见 task0 开头）。
            if (!targetOk) {
              double farD = -1.0;
              for (int r = 0; r <= 12; r++) {
                for (int ox = -r; ox <= r; ox++) {
                  for (int oy = -r; oy <= r; oy++) {
                    if (abs(ox) != r && abs(oy) != r) continue;
                    int cx = px + ox, cy = py + oy;
                    if (cx < 1 || cx >= MAP_L - 1 || cy < 1 || cy >= MAP_U - 1) continue;
                    double pd = sqrt((double)((cx - px) * (cx - px) + (cy - py) * (cy - py)));
                    if (pd > 12.0) continue;
                    if (!gAiData.exploredSet.count(std::make_pair(cx, cy))) continue;
                    const tagTerrain& ct = (*info.theMap)[cx][cy];
                    if (ct.type == MAPPATTERN_OCEAN || ct.type == MAPPATTERN_SHOAL) continue;
                    double dd = disDetail(blockToDetail(cx), blockToDetail(cy), pax2, pay2);
                    if (dd > farD) { farD = dd; bestX = cx; bestY = cy; targetOk = true; }
                  }
                }
              }
              if (targetOk) {
                double mDr = blockToDetail(bestX), mUr = blockToDetail(bestY);
                double dMid = disDetail(pDr, pUr, mDr, mUr);
                if (dMid > 2 * BLOCKSIDELENGTH) {
                  HumanMove(a.SN, mDr, mUr);
                  gAiData.scoutNextFrame = frame + (int)(dMid / SPEED_PRIEST) + 20;
                } else {
                  gAiData.scoutStall++;
                }
              } else {
                gAiData.scoutStall++;   // 视野内真的无可探索陆地，等待视野扩展
              }
            }
            // 已到目标 → 待命（节流到后走下一步）
          }
        }
      }
      else if (task == 1) {
        // 回塔
        if (disDetail(pDr, pUr, cDr, cUr) > 3 * BLOCKSIDELENGTH) {
          if (a.NowState == HUMAN_STATE_IDLE) HumanMove(a.SN, cDr, cUr);
        } else {
          gAiData.priestTask = 2;   // 到塔 → 守塔
        }
      }
      else if (task == 2) {
        // 守塔：视野出现转化目标 → 转化；否则待在塔旁
        if (convertTarget >= 0) {
          gAiData.priestTask = 3;
        } else if (a.NowState == HUMAN_STATE_IDLE && disDetail(pDr, pUr, cDr, cUr) > 4 * BLOCKSIDELENGTH) {
          HumanMove(a.SN, cDr, cUr);   // 回到塔旁
        }
      }
      else if (task == 3) {
        // 转化目标（距离控制：12 格内直接转，外则移动到"距目标12格"处遥控转化）
        // 转化优先级最高：冷却好且血 >= 10 就转化（用户要求，除非血降到 10 以下）
        if (a.ConvertCooldown > 0 || a.Blood < 10) {
          gAiData.priestTask = 2;      // 冷却中 / 血低于10 → 回守塔
        } else if (convertTarget >= 0) {
          if (a.NowState == HUMAN_STATE_IDLE) {
            // 确认目标仍存活且在敌人列表（可能已被军队/箭塔打死或被转化）
            double eDr2 = 0, eUr2 = 0;
            bool targetAlive = false;
            for (const auto& e : info.enemy_armies) {
              if (e.SN == convertTarget) { eDr2 = e.DR; eUr2 = e.UR; targetAlive = true; break; }
            }
            if (!targetAlive) {
              gAiData.priestTask = 2;   // 目标已死亡/消失：回守塔，避免读条转化死目标
            } else {
              double d = disDetail(pDr, pUr, eDr2, eUr2);
              double maxR = 12 * BLOCKSIDELENGTH;   // 祭司有效转化距离（DIS_PRIEST）
              if (d <= maxR) {
                HumanAction(a.SN, convertTarget);   // 距离内：直接转化
              } else {
                // 距离外：移动到"距目标 maxR"处（最远距离遥控转化，保护祭司）
                double dx = eDr2 - pDr, dy = eUr2 - pUr;
                double len = sqrt(dx * dx + dy * dy);
                if (len > 1.0) {
                  double bl = BLOCKSIDELENGTH;
                  double tx = eDr2 - dx / len * maxR;
                  double ty = eUr2 - dy / len * maxR;
                  tx = std::max(bl, std::min(tx, (MAP_L - 1.5) * bl));
                  ty = std::max(bl, std::min(ty, (MAP_U - 1.5) * bl));
                  HumanMove(a.SN, tx, ty);
                }
              }
            }
          }
        } else {
          gAiData.priestTask = 2;      // 目标没了 → 守塔
        }
      }
      else if (task == 4) {
        // 绕塔转圈躲避（只在转化冷却期间受伤触发）
        // 冷却结束（能转化）→ 立即退出转圈，去转化/守塔（修复"冷却后不转化"）
        if (a.ConvertCooldown <= 0) {
          gAiData.priestTask = (convertTarget >= 0) ? 3 : 2;
        } else if (a.NowState == HUMAN_STATE_IDLE) {
          gAiData.priestOrbitAngle += 30;
          double rad = gAiData.priestOrbitAngle * 3.14159265 / 180.0;
          double bl = BLOCKSIDELENGTH;
          double ox = cDr + cos(rad) * 6 * bl;
          double oy = cUr + sin(rad) * 6 * bl;
          ox = std::max(bl, std::min(ox, (MAP_L - 1.5) * bl));
          oy = std::max(bl, std::min(oy, (MAP_U - 1.5) * bl));
          HumanMove(a.SN, ox, oy);
        }
        // 长时间无新伤害（900帧≈36秒）→ 结束转圈，回守塔/转化
        if (gAiData.priestHurtFrame >= 0 && frame - gAiData.priestHurtFrame > 900) {
          gAiData.priestTask = (convertTarget >= 0) ? 3 : 2;
        }
      }

      // 诊断日志：每 150 帧输出祭司状态（便于定位"不移动/暴毙"问题）
      if (frame % 150 == 0) {
        DebugText(QString("[祭司] task=%1 血=%2 状态=%3 冷却=%4 敌人=%5 卡帧=%6 pos=(%7,%8)")
          .arg(gAiData.priestTask)
          .arg(a.Blood)
          .arg(a.NowState)
          .arg(a.ConvertCooldown)
          .arg((int)info.enemy_armies.size())
          .arg(gAiData.scoutStall)
          .arg(a.BlockDR).arg(a.BlockUR));
      }
      break;
    }
  }

  // ---------- 4. 建造管理（需求分析 → 任务推进 → 指派建造者） ----------
  // 4.1 需求分析：按阶段规划建筑
  if (gAiData.buildQueue.empty() && gAiData.buildTarget < 0)
  {
    const int nHome = countFinishedType(info, BUILDING_HOME);
    const int nCamp = countFinishedType(info, BUILDING_ARMYCAMP);
    const int nMarket = countFinishedType(info, BUILDING_MARKET);
    const int nRange = countFinishedType(info, BUILDING_RANGE);
    const int nStable = countFinishedType(info, BUILDING_STABLE);
    const int nTower = countFinishedType(info, BUILDING_ARROWTOWER);
    const int nCollage = countFinishedType(info, BUILDING_COLLAGE);
    const int nFarm = countFinishedType(info, BUILDING_FARM);
    int toolB = 0;
    if (nMarket > 0) toolB++;
    if (nRange > 0) toolB++;
    if (nStable > 0) toolB++;

    if (gAiData.stage == 0)
    {
      // ---- 经济期：房 → 兵营 → 市场 → 靶场(凑升铜前置) → 箭塔 → 补房 ----
      if (info.Human_Num >= info.Human_MaxNum - 2 && nHome < 10)
      {
        gAiData.buildQueue.push_back(BUILDING_HOME);        // 人口快满补房
      }
      else if (nCamp < 3 && info.Wood >= 125 && frame > 100)
      {
        gAiData.buildQueue.push_back(BUILDING_ARMYCAMP);   // 兵营×3：第一波前多产防守兵
      }
      else if (nMarket == 0 && info.Wood >= 150 && frame > 1500)
      {
        gAiData.buildQueue.push_back(BUILDING_MARKET);     // 市场：升铜前置 + 农田前置
      }
      else if (toolB < 2 && info.Meat >= 850 && info.Wood >= 150)
      {
        gAiData.buildQueue.push_back(BUILDING_RANGE);      // 补靶场凑齐升铜前置
      }
      else if (frame > 3500 && nTower < 2 && gAiData.arrowTowerResearched && info.Stone >= 150)
      {
        gAiData.buildQueue.push_back(BUILDING_ARROWTOWER); // 防御箭塔（第一波拉怪防守）
      }
      else if (nHome < 10 && info.Wood >= 30)
      {
        gAiData.buildQueue.push_back(BUILDING_HOME);       // 平稳期补房到10
      }
    }
    else
    {
      // ---- 爆兵/反攻期：学院 → 马厩 → 农田 → 更多房/箭塔 ----
      if (info.Human_Num >= info.Human_MaxNum - 2 && nHome < 12)
      {
        gAiData.buildQueue.push_back(BUILDING_HOME);       // 人口快满补房（人口上限50）
      }
      else if (nCollage == 0 && info.Wood >= 180 && info.Meat >= 100)
      {
        gAiData.buildQueue.push_back(BUILDING_COLLAGE);    // 学院：强力兵种方阵兵
      }
      else if (nStable == 0 && info.Wood >= 150)
      {
        gAiData.buildQueue.push_back(BUILDING_STABLE);     // 马厩：骑兵（机动冲击）
      }
      else if (nFarm < 6 && info.Wood >= 75 && frame > 7000)
      {
        gAiData.buildQueue.push_back(BUILDING_FARM);       // 农田：野外食物不足时种田
      }
      else if (nTower < 3 && gAiData.arrowTowerResearched && info.Stone >= 150)
      {
        gAiData.buildQueue.push_back(BUILDING_ARROWTOWER); // 补箭塔防御
      }
      else if (nCamp < 3 && info.Wood >= 125)
      {
        gAiData.buildQueue.push_back(BUILDING_ARMYCAMP);   // 补兵营
      }
      else if (nHome < 12 && info.Wood >= 30)
      {
        gAiData.buildQueue.push_back(BUILDING_HOME);       // 补房到12
      }
    }
  }

  // 4.2 任务推进：检查完成，失败则换位置重试
  if (gAiData.buildTarget >= 0)
  {
    bool done = countFinishedType(info, gAiData.buildTarget) >= gAiData.buildNeedCount;
    bool builderAlive = false;
    const tagFarmer* builder = nullptr;
    for (const auto& f : info.farmers)
    {
      if (f.SN == gAiData.builderSn) { builderAlive = true; builder = &f; break; }
    }
    if (done || !builderAlive)
    {
      gAiData.buildTarget = -1;
      gAiData.builderSn = -1;
      gAiData.buildNeedCount = 0;
      gAiData.buildTries = 0;
    } else if (builder->NowState == HUMAN_STATE_IDLE)
    {
      // 空闲但未建成：换新位置重试（最多15次）
      if (gAiData.buildTries < 15)
      {
        int nDR = 0, nUR = 0;
        if (findBuildSpot(info, baseDR, baseUR, gAiData.buildTries + 1, nDR, nUR))
        {
          gAiData.buildDR = nDR;
          gAiData.buildUR = nUR;
          HumanBuild(builder->SN, gAiData.buildTarget, nDR, nUR);
        }
        gAiData.buildTries++;
      } else
      {
        gAiData.buildTarget = -1;
        gAiData.builderSn = -1;
        gAiData.buildNeedCount = 0;
        gAiData.buildTries = 0;
      }
    }
  }

  // 4.3 任务启动：指派一个空闲农民专职建造
  if (!gAiData.buildQueue.empty() && gAiData.buildTarget < 0)
  {
    int buildType = gAiData.buildQueue.front();
    if (enoughForBuild(info, buildType))
    {
      int workerSn = -1;
      for (const auto& f : info.farmers)
      {
        if (f.FarmerSort == FARMERTYPE_FARMER && f.NowState == HUMAN_STATE_IDLE) { workerSn = f.SN; break; }
      }
      if (workerSn >= 0) {
        int bDR = 0, bUR = 0;
        if (findBuildSpot(info, baseDR, baseUR, 0, bDR, bUR))
        {
          gAiData.builderSn = workerSn;
          gAiData.buildTarget = buildType;
          gAiData.buildNeedCount = countFinishedType(info, buildType) + 1;
          gAiData.buildDR = bDR;
          gAiData.buildUR = bUR;
          gAiData.buildTries = 0;
          HumanBuild(workerSn, buildType, bDR, bUR);
          gAiData.buildQueue.erase(gAiData.buildQueue.begin());
        }
        else
        {
          gAiData.buildQueue.erase(gAiData.buildQueue.begin());
        }
      }
    }
  }

  // ---------- 5. 农民分工采集 ----------
  // 5.1 清理已死亡农民记录，统计各角色人数
  int cWood = 0, cFood = 0, cStone = 0, cGold = 0;
  for (auto it = gAiData.farmerRole.begin(); it != gAiData.farmerRole.end();)
  {
    bool alive = false;
    for (const auto& f : info.farmers)
    {
      if (f.SN == it->first) { alive = true; break; }
    }
    if (!alive) { it = gAiData.farmerRole.erase(it); continue; }
    if (it->second == 0) cWood++;
    else if (it->second == 1) cFood++;
    else if (it->second == 2) cStone++;
    else cGold++;
    ++it;
  }

  // 有建造任务时预留一个空闲农民待命
  const bool needReserve = !gAiData.buildQueue.empty();
  bool reserved = false;
  // 5.2 给所有空闲农民分配采集任务
  for (const auto& f : info.farmers)
  {
    if (f.FarmerSort != FARMERTYPE_FARMER) continue;
    if (f.SN == gAiData.builderSn) continue;               // 专职建造农民不采集
    if (f.NowState != HUMAN_STATE_IDLE) continue;          // 忙的不打断
    if (needReserve && !reserved) { reserved = true; continue; }  // 保留一个空闲给建造

    int role = -1;
    auto it = gAiData.farmerRole.find(f.SN);
    if (it != gAiData.farmerRole.end())
    {
      role = it->second;
    } else
    {
      role = pickRole(info, cWood, cFood, cStone, cGold, (int)info.farmers.size());
      gAiData.farmerRole[f.SN] = role;
      if (role == 0) cWood++; else if (role == 1) cFood++;
      else if (role == 2) cStone++; else cGold++;
    }

    int resType = roleToResType(role, info);
    // 野外食物耗尽：食物农民转去种田（文档策略：浆果采完后开始种田）
    if (resType < 0 && role == 1)
    {
      int farmSn = findNearestFarm(info, f.BlockDR, f.BlockUR);
      if (farmSn >= 0) { HumanAction(f.SN, farmSn); continue; }
    }
    if (resType < 0)
    {
      // 该角色没有对应资源：重选角色
      role = pickRole(info, cWood, cFood, cStone, cGold, (int)info.farmers.size());
      gAiData.farmerRole[f.SN] = role;
      resType = roleToResType(role, info);
      if (resType < 0) continue;
    }
    int resSn = findNearestRes(info, resType, f.DR, f.UR);
    if (resSn >= 0) HumanAction(f.SN, resSn);
  }

  // ---------- 6. 科技研究 ----------
  for (const auto& b : info.buildings)
  {
    // 市场：伐木 → 车轮 → 工艺（铜器）
    if (b.Type == BUILDING_MARKET && b.Percent >= 100 && b.Project == ACT_NULL)
    {
      if (!gAiData.techWoodDone && info.Meat >= 120 && info.Wood >= 75)
      {
        BuildingAction(b.SN, BUILDING_MARKET_WOOD_UPGRADE);  // 伐木：采木效率
        gAiData.techWoodDone = true;
        break;
      }
      if (gAiData.techWoodDone && !gAiData.techWheelDone && info.Meat >= 150 && info.Wood >= 100)
      {
        BuildingAction(b.SN, BUILDING_MARKET_WHEEL_UPGRADE); // 车轮：移速（骑兵/战车前置）
        gAiData.techWheelDone = true;
        break;
      }
      if (gAiData.techWoodDone && gAiData.techWheelDone && !gAiData.techCraftDone
          && info.civilizationStage >= CIVILIZATION_BRONZEAGE && info.Meat >= 170 && info.Wood >= 150)
      {
        BuildingAction(b.SN, BUILDING_MARKET_CRAFT_UPGRADE); // 工艺：伐木二级 + 射程
        gAiData.techCraftDone = true;
        break;
      }
    }
    // 仓库：工具使用（近战攻击+2）
    if (b.Type == BUILDING_STOCK && b.Percent >= 100 && b.Project == ACT_NULL
        && !gAiData.techStockDone && info.Meat >= 100)
    {
      BuildingAction(b.SN, BUILDING_STOCK_UPGRADE_USETOOL);
      gAiData.techStockDone = true;
      break;
    }
    // 谷仓：箭塔技术（一级）→ 箭塔升级（铜器）
    if (b.Type == BUILDING_GRANARY && b.Percent >= 100 && b.Project == ACT_NULL)
    {
      if (!gAiData.arrowTowerResearched && info.Meat >= 50)
      {
        BuildingAction(b.SN, BUILDING_GRANARY_ARROWTOWER);
        gAiData.arrowTowerResearched = true;
        break;
      }
      if (gAiData.arrowTowerResearched && !gAiData.arrowTowerUpgraded
          && info.civilizationStage >= CIVILIZATION_BRONZEAGE && info.Meat >= 120 && info.Stone >= 50) {
        BuildingAction(b.SN, BUILDING_GRANARY_ARROWTOWE_UPGRADE);
        gAiData.arrowTowerUpgraded = true;
        break;
      }
    }
  }

  // ---------- 7. 军事生产（按时代分兵种） ----------
  if (info.civilizationStage < CIVILIZATION_BRONZEAGE) {
    // 工具时代：只靠兵营出棍棒兵（便宜，第一波防守）
    for (const auto& b : info.buildings) {
      if (b.Type != BUILDING_ARMYCAMP) continue;
      if (b.Percent < 100 || b.Project != ACT_NULL) continue;
      if (info.Meat >= 60) BuildingAction(b.SN, BUILDING_ARMYCAMP_CREATE_CLUBMAN);
    }
  } else {
    // 铜器时代：混合兵种（文档：阔剑兵 / 复合弓 / 方阵兵 / 骑兵）
    for (const auto& b : info.buildings) {
      if (b.Percent < 100 || b.Project != ACT_NULL) continue;
      if (b.Type == BUILDING_ARMYCAMP) {
        if (!gAiData.broadswordUpgraded && info.Meat >= 140 && info.Gold >= 50) {
          BuildingAction(b.SN, BUILDING_ARMYCAMP_UPGRADE_BROADSWORD);  // 研究阔剑兵
          gAiData.broadswordUpgraded = true;
        } else if (gAiData.broadswordUpgraded && info.Meat >= 40 && info.Gold >= 20) {
          BuildingAction(b.SN, BUILDING_ARMYCAMP_CREATE_BROADSWORD);   // 阔剑兵（35食15金）
        } else if (info.Meat >= 60) {
          BuildingAction(b.SN, BUILDING_ARMYCAMP_CREATE_CLUBMAN);      // 保底：棍棒兵
        }
      } else if (b.Type == BUILDING_RANGE) {
        if (!gAiData.compositeUpgraded && info.Meat >= 180 && info.Wood >= 100) {
          BuildingAction(b.SN, BUILDING_RANGE_UPGRADE_COMPOSITE_BOW);  // 研究复合弓
          gAiData.compositeUpgraded = true;
        } else if (gAiData.compositeUpgraded && info.Meat >= 45 && info.Gold >= 25) {
          BuildingAction(b.SN, BUILDING_RANGE_CREATE_COMPOSITE_BOWMAN);// 复合弓（40食20金）
        } else if (info.Meat >= 45 && info.Wood >= 25) {
          BuildingAction(b.SN, BUILDING_RANGE_CREATE_BOWMAN);          // 保底：弓兵（40食20木）
        }
      } else if (b.Type == BUILDING_COLLAGE) {
        if (info.Meat >= 65 && info.Gold >= 45) {
          BuildingAction(b.SN, BUILDING_COLLAGE_CREATE_HOPLITE);       // 方阵兵（60食40金）
        }
      } else if (b.Type == BUILDING_STABLE) {
        if (info.Meat >= 75 && info.Gold >= 85) {
          BuildingAction(b.SN, BUILDING_STABLE_CREATE_CAVALRY);        // 骑兵（70食80金）
        }
      }
    }
  }

  // ---------- 8. 远程单位自保：受伤害逃离锁定，让近战吸收伤害 ----------
  // 我方远程单位（弓箭手/复合弓/战车弓，含被祭司转化的弓箭手）一旦
  // 被敌人锁定攻击或血量下降，立即向远离攻击者方向移动脱离，避免被集火；
  // 由近战单位在前线吸收伤害。
  for (const auto& a : info.armies) {
    if (a.Sort == AT_PRIEST || a.Sort == AT_SHIP) continue;
    if (!isRangedSort(a.Sort)) continue;             // 只处理远程单位
    if (a.NowState != HUMAN_STATE_IDLE) continue;    // 忙的不打断
    // 是否被敌人锁定攻击
    const tagArmy* attacker = nullptr;
    for (const auto& e : info.enemy_armies) {
      if (e.WorkObjectSN == a.SN) { attacker = &e; break; }
    }
    // 血量下降检测
    bool hurt = false;
    auto prevIt = gAiData.archerPrevBlood.find(a.SN);
    if (prevIt != gAiData.archerPrevBlood.end() && a.Blood < prevIt->second) hurt = true;
    gAiData.archerPrevBlood[a.SN] = a.Blood;

    if (attacker || hurt) {
      // 逃向"远离攻击者"的方向（无明确攻击者则远离最近敌人）
      double ex = a.DR, ey = a.UR;
      if (attacker) { ex -= attacker->DR; ey -= attacker->UR; }
      else {
        double bestD = 1e18;
        for (const auto& e : info.enemy_armies) {
          double d = disDetail(a.DR, a.UR, e.DR, e.UR);
          if (d < bestD) { bestD = d; ex = a.DR - e.DR; ey = a.UR - e.UR; }
        }
        if (bestD >= 1e18) ex = ey = 0;   // 无敌人，不逃
      }
      double len = sqrt(ex * ex + ey * ey);
      if (len > 1.0) {
        double bl = BLOCKSIDELENGTH;
        double tx = a.DR + ex / len * 12 * bl;
        double ty = a.UR + ey / len * 12 * bl;
        tx = std::max(bl, std::min(tx, (MAP_L - 1.5) * bl));
        ty = std::max(bl, std::min(ty, (MAP_U - 1.5) * bl));
        HumanMove(a.SN, tx, ty);
      }
    }
  }

  // ---------- 8. 第一波箭塔防御（只限第一波，贯彻文档的箭塔拉怪方案） ----------
  // 第一波：6000帧(4分钟)到达 2 斧头兵 + 1 弓箭手，攻防高、造兵来不及。
  // 方案：军队集结到防守中心（箭塔/市中心）旁拖住近战斧兵，
  //       靠箭塔 80% 伤害减免输出；远程弓箭手由祭司转化（见第3步祭司状态机）。
  // 注意：不依赖时代，只要在第一波时间窗口内就生效。
  const bool firstWave = frame > 4000 && frame < 9500;

  // ---------- 9. 军队进攻（视野内出现敌人就去攻击，无需呆在箭塔旁） ----------
  // enemy_armies 本身就是"玩家可见"的敌人（内核已按视野过滤）。
  // 每个士兵攻击"离自己最近"的敌人，分散火力；打完一个变空闲后
  // 下一帧自动选剩余敌人继续打，不会集体空闲。
  if (!info.enemy_armies.empty()) {
    for (const auto& a : info.armies) {
      if (a.Sort == AT_PRIEST || a.Sort == AT_SHIP) continue;
      if (a.NowState != HUMAN_STATE_IDLE) continue;
      int myTarget = -1;
      double bestD = 1e18;
      for (const auto& e : info.enemy_armies) {
        if (firstWave && isRangedSort(e.Sort)) continue;   // 第一波：远程留给祭司转化
        if (e.Blood <= 0) continue;                        // 不攻击已死的目标
        double d = disDetail(a.DR, a.UR, e.DR, e.UR);
        if (d < bestD) { bestD = d; myTarget = e.SN; }
      }
      if (myTarget >= 0) HumanAction(a.SN, myTarget);   // 攻击离自己最近的敌人
    }
  }

  // ---------- 10. 治疗：视野内无敌方单位时，祭司治疗己方单位 ----------
  // 优先级：祭司自己 > 按单位强度从高到低；
  // 受伤单位在无敌人时自行移动到祭司相邻一格内（按 SN 分 8 个方向，避免重叠）。
  if (info.enemy_armies.empty() && !gAiData.attacking) {
    const tagArmy* priest = nullptr;
    for (const auto& a : info.armies) if (a.Sort == AT_PRIEST) { priest = &a; break; }
    if (priest) {
      double pDr2 = blockToDetail(priest->BlockDR), pUr2 = blockToDetail(priest->BlockUR);
      // 受伤单位 → 自行移动到祭司相邻一格内
      for (const auto& a : info.armies) {
        if (a.Sort == AT_PRIEST || a.Sort == AT_SHIP) continue;
        if (a.NowState != HUMAN_STATE_IDLE) continue;
        if (a.Blood >= a.MaxBlood) continue;                       // 满血不需要
        if (disDetail(a.DR, a.UR, pDr2, pUr2) > 1.8 * BLOCKSIDELENGTH) {
          int dir = (a.SN > 0) ? (a.SN % 8) : 0;
          double ang = dir * 45.0 * 3.14159265 / 180.0;
          double bl = BLOCKSIDELENGTH;
          double tx = pDr2 + cos(ang) * 1.0 * bl;
          double ty = pUr2 + sin(ang) * 1.0 * bl;
          tx = std::max(bl, std::min(tx, (MAP_L - 1.5) * bl));
          ty = std::max(bl, std::min(ty, (MAP_U - 1.5) * bl));
          HumanMove(a.SN, tx, ty);
        }
      }
      // 祭司治疗友军：按单位强度从高到低
      // 注意：不对祭司自己发 HumanAction(祭司,祭司)——object1==object2
      // 会触发内核"自我攻击/自我转化"边界异常，导致祭司瞬间暴毙；
      // 祭司自己的血量靠血线保命（血<50 回塔）处理。
      if (priest->NowState == HUMAN_STATE_IDLE) {
        int patient = -1;
        int bestVal = -1;
        for (const auto& a : info.armies) {
          if (a.Sort == AT_PRIEST) continue;             // 不治疗祭司自己
          if (a.Blood >= a.MaxBlood) continue;
          double d = disDetail(priest->DR, priest->UR, a.DR, a.UR);
          if (d > 12 * BLOCKSIDELENGTH) continue;        // 治疗距离内
          int v = unitValue(a.Sort);
          if (v > bestVal) { bestVal = v; patient = a.SN; }
        }
        if (patient >= 0) HumanAction(priest->SN, patient);   // 治疗
      }
    }
  }

  // ---------- 11. 反攻（最终目标：祭司转化敌方攻城武器厂获胜） ----------
  // 11.1 记录攻城武器厂位置（可见时更新；开局在迷雾中不可见）
  const tagBuilding* siege = nullptr;
  for (const auto& b : info.enemy_buildings) {
    if (b.Type == BUILDING_SIEGE) { siege = &b; break; }
  }
  if (siege) {
    gAiData.siegeDR = siege->BlockDR;
    gAiData.siegeUR = siege->BlockUR;
  }

  // 11.2 反攻目标点：已知攻城厂 → 可见敌建筑 → 可见敌单位 → 地图对角
  int targetDR = gAiData.siegeDR, targetUR = gAiData.siegeUR;
  if (targetDR < 0) {
    for (const auto& b : info.enemy_buildings) { targetDR = b.BlockDR; targetUR = b.BlockUR; break; }
  }
  if (targetDR < 0) {
    for (const auto& e : info.enemy_armies) { targetDR = e.BlockDR; targetUR = e.BlockUR; break; }
  }
  if (targetDR < 0) {
    targetDR = (baseDR < MAP_L / 2) ? MAP_L - 12 : 12;
    targetUR = (baseUR < MAP_U / 2) ? MAP_U - 12 : 12;
  }

  // 11.3 反攻开启：第三波之后（21000帧） + 兵力足够 + 已升铜器
  if (!gAiData.attacking) {
    int armyCount = 0;
    for (const auto& a : info.armies) if (a.Sort != AT_PRIEST && a.Sort != AT_SHIP) armyCount++;
    if (armyCount >= kAttackArmyMin && frame > 21000 && info.civilizationStage >= CIVILIZATION_BRONZEAGE) {
      gAiData.attacking = true;
      gAiData.stage = 2;
      gAiData.attackFrame = frame;
      DebugText(QString("进入反攻阶段: 兵力=%1 时代=%2 目标=(%3,%4)")
        .arg(armyCount).arg(info.civilizationStage).arg(targetDR).arg(targetUR));
    }
  }

  // 11.4 反攻执行：军队推进清场 + 祭司护送转化
  if (gAiData.attacking) {
    double tDr = blockToDetail(targetDR), tUr = blockToDetail(targetUR);
    const tagArmy* tA = nullptr;
    for (const auto& e : info.enemy_armies) {
      if (!tA || disDetail(e.DR, e.UR, tDr, tUr) < disDetail(tA->DR, tA->UR, tDr, tUr)) tA = &e;
    }
    int tSn = -1;
    if (tA) tSn = tA->SN;
    else {
      for (const auto& b : info.enemy_buildings) {
        if (b.Type == BUILDING_SIEGE) continue;
        tSn = b.SN; break;
      }
    }
    // 空闲军队：有目标打目标，没目标朝目标点移动
    for (const auto& a : info.armies) {
      if (a.Sort == AT_PRIEST || a.Sort == AT_SHIP) continue;
      if (a.NowState != HUMAN_STATE_IDLE) continue;
      if (tSn >= 0) HumanAction(a.SN, tSn);
      else HumanMove(a.SN, tDr, tUr);
    }
    // 祭司：靠近攻城厂后转化；否则跟随推进（保持在后，避免被杀）
    for (const auto& a : info.armies) {
      if (a.Sort != AT_PRIEST) continue;
      double pDr = blockToDetail(a.BlockDR), pUr = blockToDetail(a.BlockUR);
      if (a.NowState != HUMAN_STATE_IDLE) break;
      if (siege) {
        double dSiege = disDetail(pDr, pUr, blockToDetail(siege->BlockDR), blockToDetail(siege->BlockUR));
        if (dSiege < 7 * BLOCKSIDELENGTH) HumanAction(a.SN, siege->SN);  // 转化！
        else HumanMove(a.SN, tDr, tUr);
      } else {
        HumanMove(a.SN, tDr, tUr);
      }
      break;
    }
  }

  // ---------- 状态日志（每 300 帧输出一次） ----------
  if (frame % 300 == 0) {
    DebugText(QString("帧=%1 阶段=%2 时代=%3 人口=%4/%5 农民=%6 军队=%7 木=%8 肉=%9 石=%10 金=%11")
      .arg(frame)
      .arg(gAiData.stage)
      .arg(info.civilizationStage)
      .arg(info.Human_Num).arg(info.Human_MaxNum)
      .arg((int)info.farmers.size())
      .arg((int)info.armies.size())
      .arg(info.Wood).arg(info.Meat).arg(info.Stone).arg(info.Gold));
  }
}
