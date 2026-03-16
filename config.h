#ifndef CONFIG_H
#define CONFIG_H

#include <QWidget>
#include <QPainter>
#include <QTextBrowser>
#include <QElapsedTimer>
#include <QButtonGroup>
#include <QMessageBox>
#include <QThread>
#include <QPixmap>
#include <QString>
#include <QDebug>
#include <QDirIterator>
#include <QDir>
#include <QFileInfo>
#include <QtWidgets>
#include <QMouseEvent>
#include <QImage>
#include <QObject>
#include <QKeyEvent>
#include <qtimer.h>
#include <stack>
#include <queue>
#include <list>
#include <map>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <time.h>
#include<array>
/**********debug消息************/
#define REPRESENT_BOARDCAST_MESSAGE -1
/********** 游戏配置数据 **********/
#define gen5 sqrt(5)
#define GENERATE_L (MAP_L+8)
#define GENERATE_U (MAP_U+8)
#define GAMEWIDGET_MIDBLOCKL 22
#define GAMEWIDGET_MIDBLOCKU 0
#define BLOCK_COUNT 41     // Block种类计数，包括所有种类和样式的地图块数量
/********** 飞行物投掷判断 **********/
#define THROWMISSION_FARMER 25
#define THROWMISSION_ARCHER 4
#define THROWMISSION_IMPROVEDBOWMAN1 6
#define THROWMISSION_SHIP 4
#define THROWMISSION_SLINGER 5
#define THROWMISSION_STONE_THROWER 5
#define THROWMISSION_ARROWTOWN_TIMER 25
/********** 地图块种类 **********/
/* L0边为右上角，L0到L3顺时针排列 */
/* A0边为上方角，A0到A3顺时针排列 */
#define MAPTYPE_EMPTY 0         // 未定义种类
#define MAPTYPE_FLAT 1          // 平地
#define MAPTYPE_A2_UPTOU 2      // A2角（向上方凸起）
#define MAPTYPE_A0_DOWNTOD 3    // A0角（向下方凹陷）
#define MAPTYPE_L1_UPTOLU 4     // L1边（向左上凸起）
#define MAPTYPE_L3_DOWNTORD 4   // L3边（向右下凹陷）
#define MAPTYPE_L0_DOWNTOLD 5   // L0边（向左下凹陷）
#define MAPTYPE_L2_UPTORU 5     // L2边（向右上凸起）
#define MAPTYPE_A1_UPTOL 6      // A1角（向左方凸起）
#define MAPTYPE_A3_UPTOR 7      // A3角（向右方凸起）
#define MAPTYPE_A1_DOWNTOL 8    // A1角（向左方凹陷）
#define MAPTYPE_A3_DOWNTOR 9    // A3角（向右方凹陷）
#define MAPTYPE_L0_UPTOLD 10    // L0边（向左下凸起）
#define MAPTYPE_L2_DOWNTORU 10  // L2边（向右上凹陷）
#define MAPTYPE_L3_UPTORD 11    // L3边（向右下凸起）
#define MAPTYPE_L1_DOWNTOLU 11  // L1边（向左上凹陷）
#define MAPTYPE_A2_DOWNTOU 12   // A2角（向上方凹陷）
#define MAPTYPE_A0_UPTOD 13     // A0角（向下方凸起）
#define MAPTYPE_OCEAN    14     //海洋

/********** 地图块高度 **********/
#define MAPHEIGHT_FLAT 0        // 地形高度
#define MAPHEIGHT_MAX 5         // 最高地形高度
#define MAPHEIGHT_OCEAN 9       //海洋的高度
#define MAPHEIGHT_PERCENT 60    // 生成概率，范围0~100
#define MAPHEIGHT_OPTCOUNT 20   // 生成高度时的优化次数，范围要求>=5
#define CENTER_RADIUS   12      // 特判市镇中心附近平地的半径
#define CENTER_DEVIATION 3      // 市镇中心坐标偏移量


/********** 地图块样式 **********/
enum MAPPATTERN {
    MAPPATTERN_UNKNOWN = 0, // 未定义样式
    MAPPATTERN_GRASS = 1,// 草原
    MAPPATTERN_DESERT = 2,// 沙漠
    MAPPATTERN_OCEAN = 3,// 海洋/河流
    MAPPATTERN_SHOAL = 4// 浅滩（河流中可行走部分）
};

/********** 地图块绘制偏移量 **********/
#define DRAW_OFFSET -15

/********** DebugText栏颜色 **********/
#define COLOR_RED(STRING) QString("<font color=red>%1</font><font color=black> </font>").arg(STRING)
#define COLOR_BLUE(STRING) QString("<font color=blue>%1</font><font color=black> </font>").arg(STRING)
#define COLOR_GREEN(STRING) QString("<font color=green>%1</font><font color=black> </font>").arg(STRING)
#define COLOR_BLACK(STRING) QString("<font color=black>%1</font><font color=black> </font>").arg(STRING)


/********** 建筑火焰种类 **********/
enum BuildingFireType{
    BUILDING_FIRE_SMALL=0,
    BUILDING_FIRE_MIDDLE,
    BUILDING_FIRE_BIG
};

/********** 建筑种类 **********/
enum BUILDING_TYPE {
    BUILDING_HOME = 0,
    BUILDING_GRANARY,
    BUILDING_CENTER,
    BUILDING_STOCK,
    BUILDING_FARM,
    BUILDING_MARKET,
    BUILDING_ARROWTOWER,
    BUILDING_ARMYCAMP,
    BUILDING_STABLE,
    BUILDING_RANGE,
    BUILDING_DOCK,
    BUILDING_SIEGE,
    BUILDING_COLLAGE,
    BUILDING_TEMPLE,//这个还未增加，本次也不用增加
    BUILDING_WALL,//这个还未增加，本次也不用增加
    BUILDING_TYPE_MAXNUM,
};


/********** 建筑动作 **********/
enum BuildingAction{
    //市镇中心
    BUILDING_CENTER_CREATEFARMER=1,
    BUILDING_CENTER_UPGRADE,
    //谷仓
    BUILDING_GRANARY_ARROWTOWER,
    BUILDING_GRANARY_WALL,
    BUILDING_GRANARY_ARROWTOWE_UPGRADE,
    //市场
    BUILDING_MARKET_WOOD_UPGRADE,
    BUILDING_MARKET_STONE_UPGRADE,
    BUILDING_MARKET_FARM_UPGRADE,
    BUILDING_MARKET_GOLD_UPGRADE,
    BUILDING_MARKET_WHEEL_UPGRADE,
    BUILDING_MARKET_CRAFT_UPGRADE, // 研发工艺（铜器时代，木材加工升级版）
    BUILDING_MARKET_PLOW_UPGRADE,  // 研发犁（铜器时代，驯养动物升级版）

    //仓库
    BUILDING_STOCK_UPGRADE_USETOOL,
    BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY,
    BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER,
    BUILDING_STOCK_UPGRADE_DEFENSE_RIDER,
    BUILDING_STOCK_UPGRADE_MISSILE_DEFENSE_INFANTRY,
    //军营
    BUILDING_ARMYCAMP_CREATE_CLUBMAN,
    BUILDING_ARMYCAMP_CREATE_SLINGER,
    BUILDING_ARMYCAMP_UPGRADE_CLUBMAN,
    BUILDING_ARMYCAMP_CREATE_BROADSWORD,  // 训练阔剑兵（使用未使用的编号）
    BUILDING_ARMYCAMP_UPGRADE_BROADSWORD,  // 升级阔剑兵科技（使用未使用的编号）

    //靶场
    BUILDING_RANGE_CREATE_BOWMAN,
    BUILDING_RANGE_CREATE_CHARIOT_ARCHER,   // 训练战车弓箭手
    BUILDING_RANGE_CREATE_COMPOSITE_BOWMAN,   // 训练复合弓兵
    BUILDING_RANGE_UPGRADE_COMPOSITE_BOW,  // 升级复合弓科技

    //马厩
    BUILDING_STABLE_CREATE_SCOUT,
    BUILDING_STABLE_CREATE_CHARIOT,
    BUILDING_STABLE_CREATE_CAVALRY,

    //船坞
    BUILDING_DOCK_CREATE_SAILING,
    BUILDING_DOCK_CREATE_WOOD_BOAT,
    BUILDING_DOCK_CREATE_SHIP,
    //攻城武器厂
    BUILDING_SIEGE_CREATE_STONE_THROWER,
    //学院
    BUILDING_COLLAGE_CREATE_HOPLITE,
};


/********** 建筑动作命名 **********/
#define BUILDING_GRANARY_ARROWTOWER_NAME "研发:建造箭塔(花费:50食物)"
#define BUILDING_CENTER_CREATEFARMER_NAME "创造村民(花费:50食物)"
#define BUILDING_CENTER_UPGRADE_NAME "演进到工具时代(花费:500食物)"
#define BUILDING_MARKET_WOOD_UPGRADE_NAME "研发木材加工:远程攻击距离+1,伐木+2(花费:120食物,75木头)"
#define BUILDING_MARKET_STONE_UPGRADE_NAME "研发石矿开采:采石+3(花费:100食物,50石头)"
#define BUILDING_MARKET_GOLD_UPGRADE_NAME "研发金矿开采:采金+3(花费:120食物,100木头)"
#define BUILDING_MARKET_FARM_UPGRADE_NAME "研发驯养动物:农场食物产量+75(花费:200食物,50木头)"
#define BUILDING_ACTION_CANCEL_NAME "中止"


/********** Coordinate子类中 Num值实际指代种类 **********/
//资源
enum STATICRES {
    NUM_STATICRES_Bush = 0,
    NUM_STATICRES_Stone,
    NUM_STATICRES_GoldOre,
    NUM_STATICRES_Fish
};
//动物
enum ANIMAL {
    ANIMAL_TREE = 0,
    ANIMAL_GAZELLE,
    ANIMAL_ELEPHANT,
    ANIMAL_LION,
    ANIMAL_FOREST
};
/********** 人物状态 **********/
/*
 * 0代表为空闲状态
 * 1代表为正在移动状态（无目标对象）
 * 2代表为正在工作状态
 * 3代表为正在攻击状态
 */
enum HUMAN_STATE {
    HUMAN_STATE_IDLE = 0,
    HUMAN_STATE_WALKING,
    HUMAN_STATE_WORKING,
    HUMAN_STATE_ATTACKING
};

/********** 人物手持资源种类 **********/
enum HUMAN_RESOURCE {
    HUMAN_UNKNOWN = 0,
    HUMAN_WOOD,
    HUMAN_STOCKFOOD,
    HUMAN_STONE,
    HUMAN_GOLD,
    HUMAN_GRANARYFOOD,
    HUMAN_DOCKFOOD //也就是鱼肉
};
/********** AI函数 **********/
/********** 动作返回编号及action错误码 **********/
/*
 * 0是成功
 */
enum ACTION_STATUS_CODE {
    ACTION_SUCCESS = 0,
    ACTION_INVALID_HUMANBUILD_DIFFERENTHIGH,//建筑位置有高度差
    ACTION_INVALID_HUMANBUILD_OVERBORDER,//建筑位置加上建筑宽度，超出边界
    ACTION_INVALID_HUMANBUILD_UNEXPLORE,//建筑位置未被探索
    ACTION_INVALID_HUMANBUILD_OVERLAP,//建筑位置与其他物体有重叠冲突
    ACTION_INVALID_HUMANBUILD_LOCK,//建筑未解锁，未达成建筑条件
    ACTION_INVALID_DISTANCE_FAR,//距离相距太远
    ACTION_INVALID_FULLY_LOAD,//携带人物满了
    ACTION_INVALID_POSITION_NOT_FIT,//建筑放置的位置不合适
    /*********修理建筑，建筑不需要修理********/
    ACTION_INVALID_HUMANACTION_BUILD2RESOURCENOMATCH,
    ACTION_INVALID_HUMANACTION_BUILDNOTNEEDFIX,
    ///////////////////////
    ACTION_INVALID_BUILDACT_MAXHUMAN,//造人行动，已达人口上限
    ACTION_INVALID_BUILDACT_LOCK,//建筑行动未解锁，或该行动只能进行有限次且已达上限
    ACTION_INVALID_BUILDACT_NEEDBUILT,//建筑还在建造过程中
    ACTION_INVALID_ISNTFREE,//对象已有必须手动取消的任务，不空闲
    ACTION_INVALID_NULLGOALOBJECT,//目标对象已被删除
    ACTION_INVALID_NULLWORKER,//控制对象被删除
    ACTION_INVALID_PINPOINT_NOT_FIT,//定点打击地点不合适
    ACTION_INVALID_PRIEST_TARGET_ERROR,
    ACTION_INVALID_UPGRADE_TIME,
    ACTION_INVALID_RESOURCE,
    ACTION_INVALID_BUILDINGNUM,
    ACTION_INVALID_OBSN,
    ACTION_INVALID_LOCATION,
    ACTION_INVALID_ACTION,
    ACTION_INVALID_SN

};
/********** 资源种类 **********/
/*
 * 如表 十进制位代表大的分类 个位代表他在大类中的具体编号
 */
enum RESOURCE_TYPE {
    RESOURCE_EMPTY = 0,
    RESOURCE_BUSH,
    RESOURCE_TREE,
    RESOURCE_STONE,
    RESOURCE_GAZELLE,
    RESOURCE_ELEPHANT,
    RESOURCE_LION,
    RESOURCE_GOLD,
    RESOURCE_FISH
};

/********** 时代编号 **********/
enum CIVILIZATION {
    CIVILIZATION_UNKNOWN = 0,
    CIVILIZATION_STONEAGE,
    CIVILIZATION_TOOLAGE,
    CIVILIZATION_BRONZEAGE,
    CIVILIZATION_IRONAGE
};


#define ACT_STATUS_ENABLED 0
#define ACT_STATUS_ANIME 1
#define ACT_STATUS_DISABLED 2


/********** 人物动作命名 **********/
#define ACT_CREATEFARMER_NAME "创造村民(花费:50食物)"
#define ACT_UPGRADE_AGE_NAME "演进到工具时代(花费:500食物)"
#define ACT_UPGRADE_TOWERBUILD_NAME "研发:建造箭塔(花费:50食物)"
#define ACT_UPGRADE_WOOD_NAME "研发木材加工:远程攻击距离+1,伐木+2(花费:120食物,75木头)"
#define ACT_UPGRADE_STONE_NAME "研发石矿开采:采石+3(花费:100食物,50石头)"
#define ACT_UPGRADE_GOLD_NAME "研发金矿开采:采金+3(花费:120食物,100木头)"
#define ACT_UPGRADE_FARM_NAME "研发驯养动物:农场食物产量+75(花费:200食物,50木头)"
#define ACT_UPGRADE_WHEEL_NAME "研发车轮:解锁战车单位(花费:150食物,100木头)"
#define ACT_UPGRADE_CRAFT_NAME "研发工艺:投射武器攻击距离+1 & 伐木+2(花费:170食物,150木材)"
#define ACT_UPGRADE_PLOW_NAME "研发犁:农场食物产量+75(花费:250食物,75木材)"
#define ACT_STOP_NAME "中止"
#define ACT_BUILD_NAME "建造"
#define ACT_BUILD_HOUSE_NAME "建造房屋(花费:30木头)(上限:12)"
#define ACT_BUILD_GRANARY_NAME "建造谷仓(花费:120木头)"
#define ACT_BUILD_STOCK_NAME "建造仓库(花费:120木头)"
#define ACT_BUILD_CANCEL_NAME "取消建造"
#define ACT_BUILD_FARM_NAME "建造农场(花费:75木头)(需要先建造市场)"
#define ACT_BUILD_MARKET_NAME "建造市场(花费:150木头)(需要先建造谷仓)"
#define ACT_BUILD_ARROWTOWER_NAME "建造箭塔(花费:150石头)(需要先在谷仓内升级科技)"
#define ACT_BUILD_DOCK_NAME "建造船坞(花费:120木头)"
#define ACT_BUILD_COLLAGE_NAME "建造学院"
#define ACT_BUILD_SIEGE_NAME "建造攻城武器厂"
#define ACT_SHIP_LAY_NAME "卸货"

#define ACT_ARMYCAMP_CREATE_CLUBMAN_NAME "训练棍棒兵(花费:50食物)"
#define ACT_ARMYCAMP_CREATE_SLINGER_NAME "训练投石兵(花费:40食物,10石头)"
#define ACT_ARMYCAMP_UPGRADE_CLUBMAN_NAME "升级为战斧(花费:100食物)"
#define ACT_ARMYCAMP_CREATE_BROADSWORD_NAME "训练阔剑兵(花费:35食物,15黄金)(需要阔剑科技)"
#define ACT_ARMYCAMP_UPGRADE_BROADSWORD_NAME "升级为阔剑(花费:140食物,50黄金)"
#define ACT_BUILD_ARMYCAMP_NAME "建造兵营(花费:125木头)"
#define ACT_BUILD_RANGE_NAME "建造靶场(花费:150木头)"
#define ACT_BUILD_STABLE_NAME "建造马厩(花费:150木头)"
#define ACT_RANGE_CREATE_BOWMAN_NAME "训练弓箭手(花费:40食物,20木头)"
#define ACT_RANGE_CREATE_CHARIOT_ARCHER_NAME "训练战车弓兵(花费:40食物,70木材)(需要车轮科技)"
#define ACT_RANGE_CREATE_COMPOSITE_BOWMAN_NAME "训练复合弓兵(花费:40食物,20黄金)(需要复合弓科技)"
#define ACT_RANGE_UPGRADE_COMPOSITE_BOW_NAME "升级为复合弓(花费:180食物,100木材)"
#define ACT_RESEARCH_WALL_NAME "研发:建造低级城墙"
#define ACT_STABLE_CREATE_SCOUT_NAME "训练侦察骑兵(花费:60食物)"
#define ACT_STABLE_CREATE_CAVALRY_NAME "训练骑兵(花费:70食物,80黄金)"
#define ACT_STABLE_CREATE_CHARIOT_NAME "训练四马战车(花费:40食物,60木材)(需要车轮科技)"
#define ACT_STOCK_UPGRADE_DEFENSE_ARCHER_NAME "研发弓兵护甲:弓箭手近战防御+2(花费:100食物)"
#define ACT_STOCK_UPGRADE_DEFENSE_INFANTRY_NAME "研发步兵护甲:近战单位近战防御+2(花费:75食物)"
#define ACT_STOCK_UPGRADE_DEFENSE_RIDER_NAME "研发骑兵护甲:骑兵近战防御+2(花费:125食物)"
#define ACT_STOCK_UPGRADE_USETOOL_NAME "研发工具使用:近战单位攻击+2(花费:100食物)"
// 铜器时代升级名称
#define ACT_STOCK_UPGRADE_METALWORKING_NAME "研发金属加工:近战部队攻击力+2(花费:200食物,120黄金)"
#define ACT_STOCK_UPGRADE_DEFENSE_INFANTRY_SCALE_NAME "研发步兵鳞甲:步兵护甲+2(花费:100食物,50黄金)"
#define ACT_STOCK_UPGRADE_DEFENSE_ARCHER_SCALE_NAME "研发弓兵鳞甲:弓兵护甲+2(花费:125食物,50黄金)"
#define ACT_STOCK_UPGRADE_DEFENSE_RIDER_SCALE_NAME "研发骑兵鳞甲:骑兵护甲+2(花费:150食物,50黄金)"
#define ACT_STOCK_UPGRADE_MISSILE_DEFENSE_INFANTRY_NAME "研究青铜盾:步兵对投射武器防御力+1(花费:150食物,180黄金)"
#define ACT_DOCK_CREATE_SAILING_NAME "建造渔船"
#define ACT_DOCK_CREATE_WOOD_BOAT_NAME "建造运输船"
#define ACT_DOCK_CREATE_SHIP_NAME "建造战船"
#define ACT_COLLAGE_CREATE_HOPLITE_NAME "训练方阵兵"
#define ACT_SIEGE_CREATE_STONE_THROWER_NAME "建造投石车"
#define ACT_STONE_THROWER_PINPOINT_STRIKE_NAME "定点投射"
#define ACT_STONE_THROWER_CANCEL_PINPOINT_STRIKE_NAME "取消定点投射"
#define ACT_NULL_NAME ""

#define ACT_WINDOW_NUM_FREE 12


enum ACTION {
    ACT_NULL = 0,
    ACT_CREATEFARMER,
    ACT_UPGRADE_AGE,
    ACT_UPGRADE_TOWERBUILD,
    ACT_UPGRADE_WOOD,
    ACT_UPGRADE_STONE,
    ACT_UPGRADE_FARM,
    ACT_UPGRADE_GOLD,
    ACT_UPGRADE_WHEEL,  // 升级车轮
    ACT_UPGRADE_CRAFT,  // 研发工艺
    ACT_UPGRADE_PLOW,   // 研发犁
    ACT_STOCK_UPGRADE_USETOOL,
    ACT_STOCK_UPGRADE_DEFENSE_INFANTRY,
    ACT_STOCK_UPGRADE_DEFENSE_ARCHER,
    ACT_STOCK_UPGRADE_DEFENSE_RIDER,
    ACT_STOCK_UPGRADE_MISSILE_DEFENSE_INFANTRY,  // 研究青铜盾
    ACT_STOCK_UPGRADE_METALWORKING,  // 研发金属加工（铜器时代）
    ACT_STOCK_UPGRADE_DEFENSE_INFANTRY_SCALE,  // 研发步兵鳞甲（铜器时代）
    ACT_STOCK_UPGRADE_DEFENSE_ARCHER_SCALE,  // 研发弓兵鳞甲（铜器时代）
    ACT_STOCK_UPGRADE_DEFENSE_RIDER_SCALE,  // 研发骑兵鳞甲（铜器时代)

    ACT_ARMYCAMP_CREATE_CLUBMAN,
    ACT_ARMYCAMP_CREATE_SLINGER,
    ACT_ARMYCAMP_UPGRADE_CLUBMAN,
    ACT_ARMYCAMP_CREATE_BROADSWORD,  // 训练阔剑兵
    ACT_ARMYCAMP_UPGRADE_BROADSWORD,  // 升级阔剑兵科技
    ACT_RANGE_CREATE_BOWMAN,
    ACT_RANGE_CREATE_CHARIOT_ARCHER,  // 训练战车弓箭手
    ACT_RANGE_CREATE_COMPOSITE_BOWMAN,  // 训练复合弓兵
    ACT_RANGE_UPGRADE_COMPOSITE_BOW,    // 升级复合弓科技
    ACT_STABLE_CREATE_SCOUT,
    ACT_STABLE_CREATE_CAVALRY,
    ACT_STABLE_CREATE_CHARIOT,  // 训练战车
    ACT_RESEARCH_WALL,
    ACT_DOCK_CREATE_SAILING,
    ACT_DOCK_CREATE_WOOD_BOAT,
    ACT_DOCK_CREATE_SHIP,
    ACT_SIEGE_CREATE_STONE_THROWER,
    ACT_STONE_THROWER_PINPOINT_STRIKE,
    ACT_STONE_THROWER_CANCEL_PINPOINT_STRIKE,  // 取消定点投射
    ACT_COLLAGE_CREATE_HOPLITE,
    ACT_BUILD,
    ACT_BUILD_HOUSE,
    ACT_BUILD_GRANARY,
    ACT_BUILD_STOCK,
    ACT_BUILD_CANCEL,
    ACT_BUILD_FARM,
    ACT_BUILD_MARKET,
    ACT_BUILD_ARROWTOWER,
    ACT_BUILD_ARMYCAMP,
    ACT_BUILD_RANGE,
    ACT_BUILD_STABLE,
    ACT_BUILD_DOCK,
    ACT_BUILD_COLLAGE,
    ACT_BUILD_SIEGE,
    ACT_SHIP_LAY,
    ACT_STOP
};
/********** 地基编号 **********/
enum FOUNDATION {
    FOUNDATION_SMALL = 0,
    FOUNDATION_MIDDLE,
    FOUNDATION_BIG,
    FOUNDATION_HOUSE,
    FOUNDATION_BLOCK
};

enum ElementSort {
    SORT_COORDINATE = 0,
    SORT_BUILDING,
    SORT_STATICRES,
    SORT_HUMAN,
    SORT_FARMER,
    SORT_ANIMAL,
    SORT_TREEFOREST,
    SORT_MISSILE,
    SORT_Building_Resource,
    SORT_ARMY
};

enum ProductSort {
    PRODUCTSORT_UNKNOWN = 0,
    PRODUCTSORT_WOOD,
    PRODUCTSORT_GRANARYFOOD,
    PRODUCTSORT_STONE,
    PRODUCTSORT_GOLD,
    PRODUCTSORT_STOCKFOOD
};

#define OBJECTTYPE_BLOCK 0
#define OBJECTTYPE_COORDINATE 1

enum AnimalState {
    ANIMAL_STATE_IDLE = 0,
    ANIMAL_STATE_ROAMING,
    ANIMAL_STATE_FLEEING,
    ANIMAL_STATE_CHASING,
    ANIMAL_STATE_ATTACKING
};

enum FarmerRole {
    FARMER_VILLAGER = 0,
    FARMER_LUMBER,
    FARMER_GATHERER,
    FARMER_MINER,
    FARMER_HUNTER,
    FARMER_FARMER,
    FARMER_WORKER,
    FARMER_FISHER
};


enum MoveObjectState {
    MOVEOBJECT_STATE_STAND = 0,
    MOVEOBJECT_STATE_WALK,
    MOVEOBJECT_STATE_ATTACK,
    MOVEOBJECT_STATE_DIE,
    MOVEOBJECT_STATE_WORK,
    MOVEOBJECT_STATE_RUN
};



//鼠标结构体中对应鼠标点击事件
#define NULL_MOUSEEVENT 0
#define LEFT_PRESS 1
#define RIGHT_PRESS 2
/********** Core静态表 **********/
//####关系事件名称
enum CoreEvenType {
    CoreEven_JustMoveTo = 0,
    CoreEven_CreatBuilding,
    CoreEven_Gather,
    CoreEven_Attacking,
    CoreEven_PinPoint_Attacking,
    CoreEven_FixBuilding,
    CoreEven_BuildingAct,
    CoreEven_MissileAttack,
    CoreEven_Transport,
    CoreEven_UnLoad
};
//####对一个关系事件，细节关系的最大数量
enum CoreDetailType {
    CoreDetail_JumpPhase = -3,
    CoreDetail_AbsoluteEnd,
    CoreDetail_NormalEnd,
    CoreDetail_Move,
    CoreDetail_Attack,
    CoreDetail_Gather,
    CoreDetail_ResourceIn,
    CoreDetail_Transport,
    CoreDetail_UpdateRatio,
    CoreDetail_Unload,
    CoreDetail_PinPoint_Attack,
   // CoreDetailLinkMaxNum
};
#define CoreDetailLinkMaxNum 15
/********** Core关系函数的可变操作指令 **********/
#define OPERATECON_DEFAULT 11111
#define OPERATECHANGE 100
//####距离判定
#define OPERATECON_NEAR_ABSOLUTE OPERATECON_DEFAULT
#define OPERATECON_MOVEALTER 200
#define OPERATECON_NEAR_ATTACK 10001
#define OPERATECON_NEAR_WORK 10002
#define OPERATECON_NEAR_MISSILE 10003
#define OPERATECON_NEAR_ATTACK_MOVE 10004
#define OPERATECON_NEAR_UNLOAD 10005
#define OPERATECON_NEAR_TRANSPORT 10006
#define OPERATECON_NEARALTER_ABSOLUTE 20000
#define OPERATECON_NEARALTER_WORK 20002
//####指定对象
#define OPERATECON_OBJECT1 10011
#define OPERATECON_OBJECT2 10012
#define OPERATECON_TIMES 00001
#define OPERATE_TIMEMAX 10
#define OPERATECON_TIMES_USELESSACT_MOVE 250
/********** 占地边长-块坐标常量 **********/
enum SizeLenType {
    SIZELEN_SINGEL = 1,
    SIZELEN_SMALL,
    SIZELEN_MIDDLE,
    SIZELEN_BIG
};
/********** animal友好度 **********/
enum FriendlyType {
    FRIENDLY_NULL = 0,
    FRIENDLY_FRI,
    FRIENDLY_ENEMY,
    FRIENDLY_FENCY
};
/********** 兵种类别 **********/
enum AT_ARMY {
    AT_CLUBMAN = 0,
    AT_SLINGER,
    AT_BOWMAN,
    AT_SCOUT,
    AT_SWORDSMAN,
    AT_IMPROVED,
    AT_CAVALRY,
    AT_SHIP,
    AT_STONE_THROWER,
    AT_PRIEST,
    AT_HOPLITE,
    AT_CHARIOT,             // 战车（四马战车）
    AT_CHARIOT_ARCHER,      // 战车弓箭手
    AT_BROADSWORDSMAN,      // 阔剑兵
    AT_COMPOSITE_BOWMAN,    // 复合弓兵
    AT_ARMY_MAX_NUM
};

/********** 军队类别 **********/
enum ARMY_TYPE {
    ARMY_UNKNOWN = 0,
    ARMY_INFANTRY,//步兵
    ARMY_ARCHER,//弓兵
    ARMY_RIDER,//骑兵
    ARMY_FLAMEN,//祭
    ARMY_SIEGE,//攻城兵器
    ARMY_SHIP//船
};
/********** 攻击方式 **********/
enum AttackType {
    ATTACKTYPE_CANTATTACK = -1,
    ATTACKTYPE_ANIMAL,
    ATTACKTYPE_CLOSE,
    ATTACKTYPE_CLOSE_TOTREE,
    ATTACKTYPE_SHOOT,
    ATTACKTYPE_CHANGE,
};
/***********农民类型**********/
enum FarmerType {
    FARMERTYPE_FARMER = 0,
    FARMERTYPE_WOOD_BOAT,
    FARMERTYPE_SAILING
};
/********** 飞行物类别 **********/
enum MissileType {
    Missile_Spear = 0,
    Missile_Arrow,
    Missile_Cobblestone,
    Missile_Boulders,
    NUMBER_MISSILE
};
/********** 兵种状态 **********/
enum ArmyState {
    ARMY_STATE_DEFAULT = 0,
    ARMY_STATE_AROUND,
    ARMY_STATE_DEFENSE,
    ARMY_STATE_ATTACK
};
/********** 巡逻状态 **********/
enum PatrolState {
    PATROL_STATE_IDLE = 0,
    PATROL_STATE_PATROLLING,
    PATROL_STATE_CHASING,
    PATROL_STATE_RETURNING
};
/************ AI指令枚举************/
enum AiInstructionType{
    INS_CANCEL=0,    //取消当前的行动
    INS_HUMANMOVE,   //人类移动
    INS_HUMANACTION, //人类进行一定动作
    INS_HUMANBUILD, //人类建筑
    INS_BUILDINGACTION, //建筑动作
    INS_PINPOINT_STRIKE//定点打击
};

/******************得分的类型********************/
enum ScoreType {
    _WOOD = 0,
    _STONE,
    _GOLD,
    _MEAT,
    _BERRY,
    _GAZELLE,
    _ELEPHANT,
    _FARM,
    _FISH,
    _ISWOOD,
    _ISGOLD,
    _ISSTONE,
    _TECH,
    _BUILDING1,
    _BUILDING2,
    _HUMAN1,
    _HUMAN2,
    _KILL2,
    _KILL10,
    _DESTORY2,
    _DESTORY4,
    _DESTORY5,
    _DESTORY10,
    _FINDENEMYLAND,
    SCORE_TYPE_COUNT
};
#endif // CONFIG_H
