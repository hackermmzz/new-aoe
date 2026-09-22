#ifndef USRAI_H
#define USRAI_H

#include <unordered_map>

#include "ai.h"

extern tagGame tagUsrGame;
extern ins UsrIns;
/*##########DO NOT MODIFY THE CODE ABOVE##########*/

class UsrAI : public AI
{
   public:
    UsrAI() { this->id = 0; }
    ~UsrAI() {}

   private:
    void processData() override;
    tagInfo getInfo() { return tagUsrGame.getInfo(); }
    int AddToIns(instruction ins) override
    {
        UsrIns.lock.lock();
        ins.id = UsrIns.g_id;
        UsrIns.g_id++;
        UsrIns.instructions.push(ins);
        UsrIns.lock.unlock();
        return ins.id;
    }
    void clearInsRet() override { tagUsrGame.clearInsRet(); }
    /*##########DO NOT MODIFY THE CODE IN THE CLASS##########*/
};

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <queue>
#include <set>
#include <unordered_set>
#include <vector>

const double EPS = 1e-5;

enum
{
    AT_FARMER = -1,
    AT_NONE = -2  // 科技, 不产出单位
};

// 建筑参数
const int PLACE_ADJACENT = 100;  // 紧贴其它建筑
const int PLACE_BONUS = -60;     // 落在该建筑理想距离带内
const int PLACE_FAILED = 400;    // 之前建造失败过的地基, 按次数累加
const int DEPOT_FAR = 9;         // 工作点离最近存放点超过这么多格产生智能仓储需求
const int CREW_BUILD = 1;        // 一个工地派几个人

// 侦察
const int SCOUT_VIEW = 12;                                 // 侦察视野
const int SCOUT_MIN_GAIN = 8;                              // 至少探明这么多格才有价值
const int SCOUT_HOME_RADIUS = 45;                          // 只在基地直线距离此范围内探图, 内含区域由防守机制保证无敌人
const int SCOUT_DONE = 2;                                  // 离路径点这么多格内就算站到了
const int SCOUT_HOME_DONE = 5;                             // 离集合点这么多格内就算回到了
const double SCOUT_DETOUR = 1.2;                           // 直线距离折算成实际路程的系数
const int SCOUT_HOME_STAY = 25 * 90;                       // 回避时间
const int SCOUT_WAVE[3] = {25 * 240, 25 * 540, 25 * 840};  // 波次
const int MOVE_RETRY = 25;                                 // 军队/祭司移动令连续这么多个 IDLE 帧无进展即判卡死
const double MOVE_GAIN = 0.5;                              // 两次 IDLE 之间至少靠近这么多格才算有进展

// 总攻
const int MAIN_FORCE = AT_BOWMAN;                 // 主力部队
const int ASSAULT_FRAME = 25 * 60 * 10.5;         // 发动进攻
const int ASSAULT_RUSH_PERIOD = 25 * 60 * 60;     // 总攻后每60秒放行一个主力直冲敌方基地
const double RETREAT_ARMY = 3.0;                  // 主力/投石车的后撤格距
const double RETREAT_PRIEST = 9.0;         // 祭司的后撤格距
const int RETREAT_STEP = 2;                // 单次后撤沿 nav 走这么多格
const int RETREAT_GROUP = 1;               // 触发后撤时, 周围这么多格内的己方一起走
const int HOME_KEEP = 0;                   // 出动前, 基地附近至少留这么多主力守家
const int HOME_RANGE = 40;                 // 算作"基地附近"的格距
const int BELONG_CORNER = 60;              // 分隔攻守判据
const int DEF_ALERT = 45;                  // 进到这个距离才算来袭波次
const int TOWER_ALERT = 55;                // 提前点名范围
const int FIX_TOWER_UNTIL = 25 * 60 * 11;  // 这之后不再修塔
const int WAIT_BAND_IN = 22;               // 待命部队散开到离基地此距离以外
const int WAIT_BAND_OUT = 26;              // 待命部队散开到离基地此距离以内

// 经济参数
const int CARRY_LIMIT = 10;           // 村民荷载
const double BASE_RATE_BUSH = 0.5;    // 浆果, 个/秒
const double BASE_RATE_CORPSE = 1.0;  // 猎物尸体, 个/秒
const double BASE_RATE_FARM = 0.5;    // 农田, 个/秒
const double BASE_RATE_GOLD = 1.0;    // 金矿, 个/秒
const double BASE_RATE_WOOD = 1.0;    // 木材, 个/秒
const int FARM_PRIORITY = 90;         // 农田在建造队列里的优先级
const int BUILD_WAIT = 25;            // 等地基出现的帧数
const int POP_CAP = 50;               // 人口上限
const int FARMER_MAX = 20;            // 村民数上限
const int RES_RANGE = 40;             // 有效资源的范围
const int HUNT_CLUSTER = 10;          // 打猎聚类限制
const int CREW_HUNT = 2;              // 打猎人数
const int RES_BLACK = 25 * 40;        // 无效资源点,拉黑这么久
const int GATHER_PERIOD = 5;          // 采集池最多这么多帧重建一次, 输入状态一变立刻重建
const int GATHER_STUCK = 25 * 12;     // 村民命令连续这么多帧既没动也没产出就判定卡死
const double GATHER_MOVE = 0.3;       // 到资源的格距变化小于这个值视为没动
const double WORK_SWITCH_COST = 6.0;  // 抢在岗采集工相当于额外走这么多格
const double WORK_CARRY_COST = 6.0;   // 身上已有资源时再增加这么多格的打断代价

// 各阶段人员比例, 顺序 木 食 金
const int ECON_WEIGHT[3][3] = {{5, 5, 0}, {4, 6, 0}, {1, 5, 3}};

const int SURPLUS_WEIGHT = 1;          // 最低标准
const int SURPLUS_BAND = 100;          // 数量防抖
const int SURPLUS_HOLD = 25 * 60 * 2;  // 时间防抖

// 辅助结构
struct Pos
{
    int dr = -1, ur = -1;
    Pos() = default;
    Pos(int a, int b) : dr(a), ur(b) {}

    bool operator==(const Pos& b) const { return dr == b.dr && ur == b.ur; }
};

struct FloatPos
{
    double dr = -1.0, ur = -1.0;
    FloatPos() = default;
    FloatPos(const Pos& p) : dr((p.dr + 0.5) * (double)BLOCKSIDELENGTH), ur((p.ur + 0.5) * (double)BLOCKSIDELENGTH) {}
    FloatPos(double a, double b) : dr(a), ur(b) {}
};

struct Stock
{
    int wood = 0, meat = 0, stone = 0, gold = 0;

    Stock& operator+=(const Stock& o)
    {
        wood += o.wood;
        meat += o.meat;
        stone += o.stone;
        gold += o.gold;
        return *this;
    }
    Stock& operator-=(const Stock& o)
    {
        wood -= o.wood;
        meat -= o.meat;
        stone -= o.stone;
        gold -= o.gold;
        return *this;
    }
    friend Stock operator-(Stock a, const Stock& b) { return a -= b; }

    bool covers(const Stock& c) const { return wood >= c.wood && meat >= c.meat && stone >= c.stone && gold >= c.gold; }
};

// 资源种类
enum ResKind
{
    RK_WOOD,
    RK_GOLD,
    RK_BUSH,
    RK_GAZELLE,
    RK_FARM,  // 已完工农田
    RK_COUNT
};

// 人口分配
enum EconRes
{
    E_WOOD,
    E_FOOD,
    E_GOLD,
    E_COUNT
};

struct GatherSpot
{
    int sn = -1;
    Pos at = {-1, -1};  // 资源自身所在格, 不要求可站人; 农田取中心格
    double cost = 0;    // 资源到最近存放点的像素距离
    double rate = 0;    // 综合搬运距离后的每秒产出
};

struct GatherPool
{
    std::vector<GatherSpot> spots;  // 默认按运输距离；食物规划时重排
    int desired = 0;
};

struct BuildSite
{
    int type = -1;
    Pos site = {-1, -1};
    int sn = -1;   // 地基SN, 出现前为 -1
    int born = 0;  // 下达建造令的帧号, 用来给地基出现留宽限
};

struct Want  // 本帧建造/生产需求
{
    int priority;
    int id;  // 建造为建筑类型, 生产为 action
};

// 统一命令: 军队/祭司按 MOVE_* 判卡, 村民按 GATHER_* 判卡
struct Order
{
    int target = -1;     // >=0 为动作目标SN, 否则为移动令
    FloatPos at;         // 移动终点 / 动作目标当前位置
    bool back = false;   // 后撤令, 走完即销毁, 期间不接新令
    bool stuck = false;  // 已确认过不去; 军队移动令不再重发, 村民仅作标记
    double best = 0;     // 判定进展的参考格距
    int idle = 0;        // 连续无进展的计数
};

enum DutyKind
{
    D_GATHER,  // 可被抢
    D_FARM,
    D_BUILD,
    D_FIX,
    D_HUNT
};

struct Duty
{
    int kind;
    int target;  // 采集/农田为目标SN, 工地为 siteKey, 修塔/打猎为 -1
};

struct WorkerCand  // 选址时反复取用的村民快照, 免去每次哈希查表
{
    int sn;
    FloatPos at;
    double extra;  // 打断代价
};

inline bool inMap(int dr, int ur) { return dr >= 0 && ur >= 0 && dr < MAP_L && ur < MAP_U; }
inline int cellIdx(int dr, int ur) { return dr * MAP_U + ur; }
inline Pos cellPos(int idx) { return Pos(idx / MAP_U, idx % MAP_U); }

template <typename T>
inline double dis(const T& a, const T& b)
{
    const double ddr = a.dr - b.dr, dur = a.ur - b.ur;
    return std::sqrt(ddr * ddr + dur * dur);
}

inline double gatherRate(ResKind k, double dropDis)
{
    static const double rate[RK_COUNT] = {BASE_RATE_WOOD, BASE_RATE_GOLD, BASE_RATE_BUSH, BASE_RATE_CORPSE,
                                          BASE_RATE_FARM};

    double gatherSec = CARRY_LIMIT / rate[k];
    double walkSec = 2.0 * dropDis / ((double)HUMAN_SPEED * 25.0);
    return CARRY_LIMIT / (gatherSec + walkSec);
}

int buildingSize(int type);
int resourceSize(int type);
int buildWoodCost(int type);
ResKind kindOf(int resourceType);

struct ActionInfo
{
    int action;
    int host;  // 执行该 action 的建筑类型, 未登记为 -1
    int unit;  // 产出的单位类型, 科技为 AT_NONE
    Stock cost;
};
ActionInfo actionInfo(int action);    // 每次调用现取参数
ActionInfo unitAction(int unitType);  // 生产该单位的 action

inline FloatPos centerOf(const Pos& p, int buildingType)  // 建筑的几何中心
{
    const double half = buildingSize(buildingType) * 0.5;
    return FloatPos((p.dr + half) * (double)BLOCKSIDELENGTH, (p.ur + half) * (double)BLOCKSIDELENGTH);
}

inline Pos resourceCell(const tagResource* r)  // 资源格点
{
    if (resourceSize(r->Type) == 1) return Pos(r->BlockDR, r->BlockUR);
    return Pos((int)(r->DR / (double)BLOCKSIDELENGTH + 0.5) - 1, (int)(r->UR / (double)BLOCKSIDELENGTH + 0.5) - 1);
}

class Mgr : public UsrAI
{
   public:
    virtual ~Mgr() = default;
    void update(const tagInfo& info);

   private:
    // 每帧信息
    void makeFrame(const tagInfo& info);
    void mark(const tagBuilding& b);  // 构建建筑的 blockcell
    void terrainFrame();              // 重建地形派生表: 可走 / 可建 / 高度

    std::unordered_map<int, const tagFarmer*> farmerMap;
    std::unordered_map<int, const tagArmy*> armyMap;
    std::unordered_map<int, const tagBuilding*> buildingMap;
    std::unordered_map<int, const tagResource*> resourceMap;
    std::unordered_map<int, const tagArmy*> eArmyMap;
    std::unordered_map<int, const tagBuilding*> eBuildingMap;

    std::unordered_map<int, std::vector<int>> byType;  // 建筑类型 -> SN 列表(默认顺序)
    std::vector<int> unitCnt, bldCnt, bldDoneCnt;

    template <class T>
    static const T* get(const std::unordered_map<int, const T*>& m, int sn)
    {
        auto it = m.find(sn);
        return it == m.end() ? nullptr : it->second;
    }

    const tagFarmer* farmer(int sn) const { return get(farmerMap, sn); }
    const tagArmy* army(int sn) const { return get(armyMap, sn); }
    const tagBuilding* building(int sn) const { return get(buildingMap, sn); }
    const tagResource* resource(int sn) const { return get(resourceMap, sn); }
    const tagArmy* enemyArmy(int sn) const { return get(eArmyMap, sn); }
    const tagBuilding* enemyBuilding(int sn) const { return get(eBuildingMap, sn); }
    bool locate(int sn, FloatPos* at = nullptr) const;  // 任意SN的当前位置(建筑取中心), 不存在返回 false

    const std::vector<int>& buildingsOf(int type) const;
    int unitCount(int type) const { return unitCnt[type + 1]; }  // AT_FARMER = -1, 偏移 1
    int buildingCount(int type, bool doneOnly = false) const { return doneOnly ? bldDoneCnt[type] : bldCnt[type]; }

    // 地形与位置判定
    const tagTerrain& cell(int dr, int ur) const { return (*theMap)[dr][ur]; }
    bool blocked(int dr, int ur) const { return blockCell[cellIdx(dr, ur)] != 0; }
    bool valid(int dr, int ur) const;  // 地形是否允许建造
    bool walkable(int dr, int ur) const { return inMap(dr, ur) && walkCell[cellIdx(dr, ur)] != 0; }
    bool canPlace(int dr, int ur, int size) const;  // size*size 的地基是否放得下
    int lockOf(int enemySN) const;                  // 该敌人锁着的我方SN, 没锁到我方返回 -1, 锁到祭司返回 -1

    // 库存
    Stock available() const { return res - held; }
    bool afford(const Stock& c) const { return available().covers(c); }

    // 距离场与代价图
    void fieldBuild(std::vector<int>& out, const Pos& src, int size);                                // 可走格的 bfs
    void ringAdd(std::vector<int>& g, const Pos& around, int size, int cost, int inner, int outer);  // bfs环带变体

    std::vector<Pos> bfsQueue;   // 两个 bfs 共用的队列缓冲, 免去每次分配
    std::vector<int> ringStamp;  // ringAdd 的访问标记, 用版本号代替整图清零
    int ringTick = 0;

    // 统一命令层
    void orderFrame();                                              // 清理失效命令, 更新进展与卡死标记
    void orderMove(int sn, const FloatPos& at, bool back = false);  // 同目标不重发, 仅在 IDLE 且未卡死时补发
    void orderAction(int sn, int target);                           // 已在执行同一目标则不重发; 建筑按 Project 去重
    bool orderStuck(int sn) const;

    std::unordered_map<int, Order> orders;  // 单位SN -> 当前命令

    // 村民调度, duty 是岗位的唯一数据源
    void dutyFrame();   // 清理阵亡村民的岗位
    void laborFrame();  // 重建空闲池
    void laborRelease();
    double workerCost(int sn, const FloatPos& at, bool steal) const;  // 距离 + 打断代价, 单位为格
    int pickWorker(const FloatPos& at, bool steal, double* cost = nullptr) const;
    void workerCands(std::vector<WorkerCand>& out) const;  // 可抢的村民快照, 与 pickWorker(steal) 同序
    int pickCand(const std::vector<WorkerCand>& cand, const FloatPos& at, double* cost) const;
    void claimWorker(int sn);                             // 解绑并移出空闲池
    void setDuty(int sn, int kind, int target);           // 取走该村民并登记岗位
    void dropDuty(int sn, bool toFree);                   // 解绑并撤销命令, toFree 时交还空闲池
    void freeWorker(int sn);                              // 交还空闲池(已阵亡或仍有岗位则忽略)
    std::vector<int> crewOf(int kind, int target) const;  // 某岗位上的村民, 按 SN 升序
    bool workerReserved(int sn) const;                    // 在专职岗位上(农田/工地/修塔/打猎), 不许被抢

    std::unordered_map<int, Duty> duty;   // 村民SN -> 岗位
    std::unordered_map<int, int> holder;  // 采集点/农田SN -> 村民SN, 与 duty 同步维护

    // 全局帧状态
    std::vector<unsigned char> blockCell;  // 被资源或建筑占住的格子, cellIdx 索引
    std::vector<unsigned char> walkCell;   // 本帧可走格
    std::vector<unsigned char> buildCell;  // 本帧可建格: 地形合法且没被占
    std::vector<int> heightCell;           // 高度, 免去每次二维查表
    std::vector<unsigned char> walkPrev;   // 上次重算距离场时的可走格
    Pos navBase = {-1, -1};                // 当前距离场对应的基地
    int navVer = 0;                        // 距离场版本, 供派生缓存失效
    int frameTick = 0;                     // 内部帧号, 给各类每帧缓存打戳
    const std::vector<std::vector<tagTerrain>>* theMap = nullptr;

    int gameFrame = 0;
    Stock res;   // 当前库存
    Stock held;  // 本帧已被生产预定
    int stage = 0;

    // 固定信息
    Pos base = {-1, -1};
    FloatPos baseF = {-1, -1};
    int priest = -1;

    std::vector<int> nav;        // 基地距离, -1 表示不可达
    std::vector<int> laborPool;  // 空闲人口

    // 采集
    void gatherFrame();                          // 按需重建采集池, 每帧保证绑定有效
    void gatherRebuild();                        // 真正重建全部采集池(含农田)
    void gatherWatch();                          // 解除过期拉黑, 把卡死的采集点拉黑
    bool reachable(const tagResource* r) const;  // 周围一圈有没有 nav 可达的落脚格
    double depotCost(const FloatPos& at, int depotType) const;

    std::vector<FloatPos> depotDone[2];  // [0]谷仓系 [1]仓库系: 已完工的中心与存放点中心
    std::vector<FloatPos> depotAll[2];   // 同上但含未完工, 供覆盖判定
    std::unordered_set<int> gatherSel;   // 当前池里登记的采集点SN
    long long gatherSig = -1;            // 采集池输入状态的签名
    int gatherAt = -1000;                // 上次重建的帧号

    // 打猎
    void huntFrame();                                 // 维护当前猎物与已经稳定下来的尸体批次
    void runHunt();                                   // 两名猎人集火当前猎物
    void huntDepotWant(std::vector<Pos>& out) const;  // 整群打完后再把尸体交给仓库规划

    std::vector<int> huntTargets;               // 当前羚羊 SN
    std::vector<std::vector<int>> huntBatches;  // 尸体仍存在的历史批次

    // 人口分配
    int econPick(const int weight[E_COUNT], const int count[E_COUNT], const int cap[E_COUNT]) const;
    Stock phaseNeed() const;  // 已排进队列但还没花出去的资源
    void econPlan(int phase);
    void runEconomy();  // 对岗位缺口做贪心匹配

    GatherPool pools[RK_COUNT];             // 各池按运输距离升序
    std::unordered_map<int, int> resBlack;  // 资源SN -> 拉黑到期帧

    int wantFarm = 0;                   // 本帧规划新建几块农田
    bool econSurplus[E_COUNT] = {};     // 该项资源当前是否判定为够用
    int econSwitchAfter[E_COUNT] = {};  // 非富余状态至少保持到这个帧

    // 建造
    void buildFrame();                                             // 清空排队, 重算仓库收益图
    void runBuild();                                               // 维护建造
    void wantBuilding(int buildingType, int total, int priority);  // 该类总数补到 total
    void wantDepot(int depotType, int priority);                   // 有远端需求时补一座(首座谷仓无条件)

    void depotWant(ResKind k, std::vector<Pos>& out) const;  // 远端有人采集的点触发需求, 取最远的当锚点
    bool depotCovered(int depotType, const Pos& c) const;    // 是否已覆盖
    double depotBenefit(int depotType, const Pos& site) const;
    bool depotRoom(const Pos& c) const;  // 该点附近放得下一座存放点
    int queuedBuild(int type) const;
    bool buildAvailable(int type) const;
    Pos findSpot(int type, int& firstWorker);  // 选址与首个施工者联合决策

    static int costVariant(int type);                       // 建筑类型 -> 附加代价层的编号
    void costPrepare(int variant);                          // 代价图与其二维前缀和, 每帧每变体只算一次
    const std::vector<unsigned char>& placeMask(int size);  // 可落地掩码, 每帧每尺寸只算一次

    std::vector<int> costBaseMap;          // 与建筑类型无关的代价层
    std::vector<int> costVarMap[3];        // 叠加附加层之后的代价图
    std::vector<long long> costVarSat[3];  // 上面各自的二维前缀和
    int costBaseAt = -1;
    int costVarAt[3] = {-1, -1, -1};
    std::vector<unsigned char> maskMap[8];  // 下标为地基边长, buildingSize 最大为 3
    int maskAt[8] = {-1, -1, -1, -1, -1, -1, -1, -1};

    // 生产
    void prodFrame();                                  // 清空本帧队列
    void runProd();                                    // 处理生产
    void runDestroy();                                 // 人口超编时拆村民(先解绑再自毁)
    void wantUnit(int type, int total, int priority);  // 该类总数补到 total
    void wantTech(int action, int priority);           // 一次性科技
    int queuedProd(int action) const;
    int projectCount(int action) const;  // 当前有多少建筑正在执行该 action
    bool hasTech(int action) const { return doneTech.count(action) > 0; }
    bool techAvailable(int action) const;
    int idleHost(int buildingType, const std::set<int>& busy) const;

    std::vector<Want> builds;  // 本帧建造需求
    std::vector<BuildSite> sites;
    std::unordered_map<long long, int> failedSpots;  // (buildingType, cell) -> fail count

    std::vector<Pos> granaryPendings;  // 谷仓选址的加权点: 远端浆果与远端农田
    std::vector<Pos> stockPendings;    // 仓库选址的加权点: 远端猎物与远端金矿

    std::vector<Want> prods;              // 本帧生产/科技需求
    std::unordered_set<int> runningTech;  // 已经下令且尚未完成
    std::unordered_set<int> doneTech;     // 仅在 Project 结束后进入

    // 侦察
    void runScout();
    int wpGain(const Pos& c) const;                       // c 为圆心半径 SCOUT_VIEW 内的未知格数
    int pickWaypoint(const Pos& here, Pos& stand) const;  // 最近的还有收益的路径点, 返回其下标
    int homeETA(const Pos& here);                         // 回家还要几帧(顺带更新 home)

    // 防守
    void defence();  // 处理来袭波次, 置 combat
    void fixTower();
    void runTower();
    int defenceSelector(const tagArmy& u) const;
    void runDefenders();

    bool combat = false;        // 本帧祭司不探图
    std::vector<int> hostiles;  // 本帧要处理的敌人SN
    int fixer = -1;             // 修塔村民SN, 固定一人

    // 进攻
    void offense();                    // 进攻总调度: 定位对角与攻城厂, 派兵
    void offenseUpdate();              // 更新 tars
    int siegeDis(const Pos& p) const;  // 到攻城厂的格距, 未定位返回角落运算

    int attackSelector(const tagArmy& u) const;
    void vanguardPick();
    bool inVanguard(int sn) const { return vanguard.count(sn) > 0; }
    void runAssault();
    void runTowerBreak();
    void runFarmerAssault();
    void clearRoad();  // 借过一下

    double enemyGap(const FloatPos& at) const;          // 到最近敌军的格距, 没有敌军返回INF
    FloatPos marchGoal() const;                         // 攻城厂, 没定位就是对角
    Pos retreatCell(const Pos& from, int steps) const;  // 沿 nav 下坡走 steps 格, 中途卡住就停

    std::vector<Pos> bandPoints;  // 待命散开点, 只随距离场重算
    int bandVer = -1;

    Pos corner = {-1, -1};  // 与基地对角的地图角
    int siegeSN = -1;
    Pos siegePos = {-1, -1};

    bool assaultOn = false;
    bool towerBreakOn = false;                 // 箭塔突破
    bool priestRushOn = false;                 // 转化
    bool farmerAssaultOn = false;              // 村民一旦投入进攻, 永久退出经济
    int nextAssaultRush = -1;                  // 下一个单兵直冲时刻
    std::unordered_map<int, int> towerShield;  // 主力/投石车SN -> 箭塔SN
    std::unordered_set<int> vanguard;          // 提前出动的主力; assaultOn 之后清空并入大部队
    std::unordered_set<int> assaultRush;       // 已单独放行直冲基地的主力, 后续不再控制

    std::vector<int> tars;  // offense 只登记目标象限的敌军

    // 探图
    // 每轴 MAP_L / SCOUT_VIEW + 1 个点, 下标 idx = i * 每轴点数 + j 对应 Pos(i, j) * SCOUT_VIEW
    std::vector<unsigned char> wpDone;  // 已经站到过或已经看光的路径点
    int goalWp = -1;                    // 目标路径点下标, -1 表示还没选
    Pos goalStand = {-1, -1};           // 当前路径点本身, 允许落在迷雾里
    int scoutHomeUntil = 0;             // 一旦决定回家, 锁定到该波次避险结束
    Pos home = {-1, -1};                // 实际可站立的回家格

    // 策略
    void strategy();
};

#endif  // USRAI_H
