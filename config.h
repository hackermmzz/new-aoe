#ifndef CONFIG_H
#define CONFIG_H

#include <QWidget>
#include <QPainter>
#include <stack>
#include <qtimer.h>
#include <QTextBrowser>
#include <QElapsedTimer>
#include <stack>
#include <queue>
#include <QButtonGroup>
#include <QMessageBox>
#include <QThread>
#include <Windows.h>
#include <time.h>
#include <map>
#include <QPixmap>
#include <QString>
#include <list>
//#include <QSound>
#include <QDebug>
#include <QDirIterator>
#include <QDir>
#include <QFileInfo>
#include <QtWidgets>
#include <QMouseEvent>
#include <QImage>
#include <QObject>
#include <QKeyEvent>
//#include <QMediaPlayer>
#include <fstream>
#include <sstream>
#include <vector>

/********** 游戏配置数据 **********/
#define GAME_WIDTH 1920                 //总窗口宽度
#define GAME_HEIGHT 1000              //总窗口高度
#define GAME_VERSION "v1.0"
#define GAME_TITLE "Age of Empires"     //总窗口名称
#define GAME_LOSE_SEC 1500
#define GOLD 10                         //金块资源数量
#define STONE 10                        //石头资源数量
#define gen5 sqrt(5)
#define MAXPLAYER 8
#define MEMORYROW 400                  //列 行长度
#define MEMORYCOLUMN 200               //行 列长度
#define GAMEWIDGET_WIDTH 1440
#define GAMEWIDGET_HEIGHT 751
#define BLOCKSIDELENGTH (16*gen5)
#define MAP_L 72
#define MAP_U 72
#define GAMEWIDGET_MIDBLOCKL 22
#define GAMEWIDGET_MIDBLOCKU 0
#define HUMAN_SPEED sqrt(5)
#define ANIMAL_SPEED sqrt(10)
#define RESPATH ":/new/prefix1/res"
#define ForestMAX 2
#define FOREST_GENERATE_L 15
#define FOREST_GENERATE_U 30
#define FOREST_GENERATE_PERCENT 45  // 生成森林时出现空地的概率
#define FOREST_GENERATE_OPTCOUNTER 5
#define FOREST_COUNT_MIN 150
#define TREEMAX 40
#define BLOCKPIXEL_X (65 - showLine)
#define BLOCKPIXEL_Y (33 - showLine)
#define GENERATELANDFORMS_NUM 4
#define KEY_UP 87
#define KEY_LEFT 65
#define KEY_DOWN 83
#define KEY_RIGHT 68
#define BLOCK_COUNT 29      // Block种类计数，包括所有种类和样式的地图块数量
#define FRAMES_PER_SECOND 25    //每秒帧数

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

/********** 地图块高度 **********/
#define MAPHEIGHT_FLAT 0        // 地形高度
#define MAPHEIGHT_MAX 5         // 最高地形高度
#define MAPHEIGHT_PERCENT 60    // 生成概率，范围0~100
#define MAPHEIGHT_OPTCOUNT 20   // 生成高度时的优化次数，范围要求>=5

/********** 地图块样式 **********/
#define MAPPATTERN_EMPTY 0      // 未定义样式
#define MAPPATTERN_GRASS 0      // 草原
#define MAPPATTERN_DESERT 1     // 沙漠
#define MAPPATTERN_OCEAN 2      // 海洋/河流
#define MAPPATTERN_SHOAL 3      // 浅滩（河流中可行走部分）

/********** 地图块绘制偏移量 **********/
#define DRAW_OFFSET -15

/********** DebugText栏颜色 **********/
#define COLOR_RED(STRING) QString("<font color=red>%1</font><font color=black> </font>").arg(STRING)
#define COLOR_BLUE(STRING) QString("<font color=blue>%1</font><font color=black> </font>").arg(STRING)
#define COLOR_GREEN(STRING) QString("<font color=green>%1</font><font color=black> </font>").arg(STRING)

/********** 建筑种类 **********/
#define BUILDING_TYPE_MAXNUM 12
#define BUILDING_HOME 0
#define BUILDING_GRANARY 1
#define BUILDING_CENTER 2
#define BUILDING_STOCK 3

#define BUILDING_MARKET 5
#define BUILDING_ARROWTOWER 6
#define BUILDING_ARROWTOWERPOSITION -1

#define BUILDING_ARMYCAMP 7
#define BUILDING_STABLE 8
#define BUILDING_RANGE 9

#define BUILDING_WALL 10

//Building_Resource类（产资源建筑）
#define BUILDING_FARM 4

/********** 建筑状态 **********/
#define BUILDING_FREE 0 // 建筑状态为空闲（无工作）时用0表示

/********** 建筑属性相关 **********/
/********** 建筑动作耗时 **********/  //单位为秒
/********** 行动消耗资源 **********/
/**市政中心**/
#define BLOOD_BUILD_CENTER 600
#define VISION_CENTER 4
#define BUILD_CENTER_WOOD 200
#define TIME_BUILD_CENTER 60
//生产村民
#define BUILDING_CENTER_CREATEFARMER_FOOD 50
#define TIME_BUILDING_CENTER_CREATEFARMER 20
//升级时代至工具时代
#define BUILDING_CENTER_UPGRADE_FOOD 500
#define TIME_BUILDING_CENTER_UPGRADE 60

/**房子**/
#define BLOOD_BUILD_HOUSE 75
#define VISION_HOME 3
#define BUILD_HOUSE_WOOD 30
#define TIME_BUILD_HOME 20

/**仓库**/
#define BLOOD_BUILD_STOCK 350
#define VISION_STOCK 3
#define BUILD_STOCK_WOOD 120
#define TIME_BUILD_STOCK 30
//升级工具利用（1级）
#define BUILDING_STOCK_UPGRADE_USETOOL_FOOD 100
#define TIME_BUILDING_STOCK_UPGRADE_USETOOL 40
#define BUILDING_STOCK_UPGRADE_USETOOL_ADDITION_ATKCLOSE 2
//升级步兵护甲（1级）
#define BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY_FOOD 75
#define TIME_BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY 40
#define BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY_ADDITION_DEFENSE_INFANTRY 2
//升级弓兵护甲（1级）
#define BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER_FOOD 100
#define TIME_BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER 40
#define BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER_ADDITION_DEFENSE_ARCHER 2
//升级骑兵护甲（1级）
#define BUILDING_STOCK_UPGRADE_DEFENSE_RIDER_FOOD 125
#define TIME_BUILDING_STOCK_UPGRADE_DEFENSE_RIDER 30
#define BUILDING_STOCK_UPGRADE_DEFENSE_RIDER_ADDITION_DEFENSE_RIDER 2

/**谷仓**/
#define BLOOD_BUILD_GRANARY 350
#define VISION_GRANARY 4
#define BUILD_GRANARY_WOOD 120
#define TIME_BUILD_GRANARY 30
//研发箭塔
#define BUILDING_GRANARY_ARROWTOWER_FOOD 50
#define TIME_BUILDING_GRANARY_RESEARCH_ARROWTOWER 10
//研发城墙
#define BUILDING_GRANARY_RESEARCH_WALL_FOOD 50
#define TIME_BUILDING_GRANARY_RESEARCH_WALL 10

/**兵营**/
#define BLOOD_BUILD_ARMYCAMP 350
#define VISION_ARMYCAMP 4
#define BUILD_ARMYCAMP_WOOD 125
#define TIME_BUILD_ARMYCAMP 30
//生产棍棒兵
#define BUILDING_ARMYCAMP_CREATE_CLUBMAN_FOOD 50
#define TIME_BUILDING_ARMYCAMP_CREATE_CLUBMAN 26
//升级棍棒兵为斧头兵
#define BUILDING_ARMYCAMP_UPGRADE_CLUBMAN_FOOD 100
#define TIME_BUILDING_ARMYCAMP_UPGRADE_CLUBMAN 40
//生产投石者
#define BUILDING_ARMYCAMP_CREATE_SLINGER_FOOD 40
#define BUILDING_ARMYCAMP_CREATE_SLINGER_STONE 10
#define TIME_BUILDING_ARMYCAMP_CREATE_SLINGER 24

/**靶场**/
#define BLOOD_BUILD_RANGE 350
#define VISION_RANGE 3
#define BUILD_RANGE_WOOD 150
#define TIME_BUILD_RANGE 40
//生产弓箭手
#define BUILDING_RANGE_CREATE_BOWMAN_FOOD 40
#define BUILDING_RANGE_CREATE_BOWMAN_WOOD 20
#define TIME_BUILDING_RANGE_CREATE_BOWMAN 30

/**马厩**/
#define BLOOD_BUILD_STABLE 350
#define VISION_STABLE 3
#define BUILD_STABLE_WOOD 150
#define TIME_BUILD_STABLE 40
//生产侦察骑兵
#define BUILDING_STABLE_CREATE_SCOUT_FOOD 100
#define TIME_BUILDING_STABLE_CREATE_SCOUT 30

/**市场**/
#define BLOOD_BUILD_MARKET 350
#define VISION_MARKET 4
#define BUILD_MARKET_WOOD 150
#define TIME_BUILD_MARKET 40
//升级伐木（1级）
#define BUILDING_MARKET_WOOD_UPGRADE_FOOD 120
#define BUILDING_MARKET_WOOD_UPGRADE_WOOD 75
#define TIME_BUILDING_MARKET_UPGRADE_CUTTING 40
#define BUILDING_MARKET_WOOD_UPGRADE_ADDITION_CARRY 2
#define BUILDING_MARKET_WOOD_UPGRADE_ADDITION_DISSHOOT 1

//升级采集石头（1级）
#define BUILDING_MARKET_STONE_UPGRADE_FOOD 100
#define BUILDING_MARKET_STONE_UPGRADE_STONE 50
#define TIME_BUILDING_MARKET_UPGRADE_DIGGINGSOTNE 60
#define BUILDING_MARKET_STONE_UPGRADE_ADDITION_CARRY 3
#define BUILDING_MARKET_STONE_UPGRADE_ADDITION_SILNGERATK 1
#define BUILDING_MARKET_STONE_UPGRADE_ADDITION_SILNGERDIS 1

//升级金矿开采（1级）
#define BUILDING_MARKET_GOLD_UPGRADE_FOOD 120
#define BUILDING_MARKET_GOLD_UPGRADE_WOOD 100
#define TIME_BUILDING_MARKET_UPGRADE_GOLD 60
#define BUILDING_MARKET_GOLD_UPGRADE_ADDITION_CARRY 3

//升级农田（1级）
#define BUILDING_MARKET_FARM_UPGRADE_FOOD 200
#define BUILDING_MARKET_FARM_UPGRADE_WOOD 50
#define TIME_BUILDING_MARKET_UPGRADE_FARM 60
#define BUILDING_MARKET_FARM_UPGRADE_ADDITION_FOOD 75


/**农场**/
#define BLOOD_BUILD_FARM 50
#define CNT_BUILD_FARM 250
#define VISION_FARM 3
#define BUILD_FARM_WOOD 75
#define TIME_BUILD_FARM 30

/**箭塔**/
#define BLOOD_BUILD_ARROWTOWER 125
#define VISION_ARROWTOWER 10
#define DIS_BUILD_ARROWTOWER 5
#define ATK_BUILD_ARROWTOWER 3
#define DEFSHOOT_BUILD_ARROWTOWER 3
#define BUILD_ARROWTOWER_STONE 150
#define TIME_BUILD_ARROWTOWER 80

/**城墙**/
#define BLOOD_BUILD_WALL 200
#define DEFSHOOT_BUILD_WALL 3
#define BUILD_WALL_STONE 5
#define TIME_BUILD_WALL 10

/********** 建筑动作 **********/
//市镇中心
#define BUILDING_CENTER_CREATEFARMER 1
#define BUILDING_CENTER_UPGRADE 2
//谷仓
#define BUILDING_GRANARY_ARROWTOWER 3
#define BUILDING_GRANARY_WALL 18
//市场
#define BUILDING_MARKET_WOOD_UPGRADE 4
#define BUILDING_MARKET_STONE_UPGRADE 5
#define BUILDING_MARKET_FARM_UPGRADE 6
#define BUILDING_MARKET_GOLD_UPGRADE 7

//仓库
#define BUILDING_STOCK_UPGRADE_USETOOL 8
#define BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY 9
#define BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER 10
#define BUILDING_STOCK_UPGRADE_DEFENSE_RIDER 11

//军营
#define BUILDING_ARMYCAMP_CREATE_CLUBMAN 13
#define BUILDING_ARMYCAMP_CREATE_SLINGER 14
#define BUILDING_ARMYCAMP_UPGRADE_CLUBMAN 15

//靶场
#define BUILDING_RANGE_CREATE_BOWMAN 16

//马厩
#define BUILDING_STABLE_CREATE_SCOUT 17

#define BUILDING_CANCEL 12

/********** 建筑动作命名 **********/
#define BUILDING_GRANARY_ARROWTOWER_NAME "研发:建造箭塔(花费:50食物)"
#define BUILDING_CENTER_CREATEFARMER_NAME "创造村民(花费:50食物)"
#define BUILDING_CENTER_UPGRADE_NAME "演进到工具时代(花费:500食物)"
#define BUILDING_MARKET_WOOD_UPGRADE_NAME "研发木材加工:远程攻击距离+1,伐木+2(花费:120食物,75木头)"
#define BUILDING_MARKET_STONE_UPGRADE_NAME "研发石矿开采:采石+3(花费:100食物,50石头)"
#define BUILDING_MARKET_FARM_UPGRADE_NAME "研发驯养动物:农场食物产量+75(花费:200食物,50木头)"
#define BUILDING_ACTION_CANCEL_NAME "中止"

/********** Coordinate子类中 Num值实际指代种类 **********/
//资源
#define NUM_STATICRES_Bush 0
#define NUM_STATICRES_Stone 1
#define NUM_STATICRES_GoldOre 2

//动物
#define ANIMAL_TREE 0
#define ANIMAL_GAZELLE 1
#define ANIMAL_ELEPHANT 2
#define ANIMAL_LION 3
#define ANIMAL_FOREST 4


/********** 人物状态 **********/
/*
 * 0代表为空闲状态
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
 * 12代表种地
 * 13代表正在采集肉的状态
 * 后续补充
 */
#define HUMAN_STATE_IDLE 0
#define HUMAN_STATE_GOTO_OBJECT 1
#define HUMAN_STATE_CUTTING 2
#define HUMAN_STATE_DIGGING_STONE 3
#define HUMAN_STATE_DIGGING_GOLD 4
#define HUMAN_STATE_GATHERING 5
#define HUMAN_STATE_BUILDING 6
#define HUMAN_STATE_FIXING 7
#define HUMAN_STATE_ATTACKING 8
#define HUMAN_STATE_GOTO_RESOURCE 9
#define HUMAN_STATE_GOTO_ATTACK 10
#define HUMAN_STATE_STOP 11
#define HUMAN_STATE_FARMING 12
#define HUMAN_STATE_BUTCHERING 13

/********** 人物手持资源种类 **********/
#define HUMAN_WOOD 1
#define HUMAN_STOCKFOOD 2
#define HUMAN_STONE 3
#define HUMAN_GOLD 4
#define HUMAN_GRANARYFOOD 5


/********** 动作返回编号 **********/
/*
 * 0是成功
 * -1是SN不存在
 * -2是Action不存在
 * -3是位置超界
 * -4是obSN不存在
 * -5是BuildingNum不存在
 * -6是资源不足
 */
#define ACTION_SUCCESS 0
#define ACTION_INVALID_SN -1
#define ACTION_INVALID_ACTION -2
#define ACTION_INVALID_LOCATION -3
#define ACTION_INVALID_OBSN -4
#define ACTION_INVALID_BUILDINGNUM -5
#define ACTION_INVALID_RESOURCE -6

/********** 资源种类 **********/
/*
 * 如表 十进制位代表大的分类 个位代表他在大类中的具体编号
 */
#define RESOURCE_EMPTY 0
#define RESOURCE_BUSH 20
#define RESOURCE_TREE 60
#define RESOURCE_STONE 21
#define RESOURCE_GAZELLE 71
#define RESOURCE_ELEPHANT 72
#define RESOURCE_LION 73


/********** 时代编号 **********/
#define CIVILIZATION_STONEAGE 1
#define CIVILIZATION_TOOLAGE 2
#define CIVILIZATION_BRONZEAGE 3
#define CIVILIZATION_IRONAGE 4


#define ACT_STATUS_ENABLED 0
#define ACT_STATUS_ANIME 1
#define ACT_STATUS_DISABLED 2

/********** 人物动作命名 **********/
#define ACT_CREATEFARMER_NAME "创造村民(花费:50食物)"
#define ACT_UPGRADE_AGE_NAME "演进到工具时代(花费:500食物)"
#define ACT_UPGRADE_TOWERBUILD_NAME "研发:建造箭塔(花费:50食物)"
#define ACT_UPGRADE_WOOD_NAME "研发木材加工:远程攻击距离+1,伐木+2(花费:120食物,75木头)"
#define ACT_UPGRADE_STONE_NAME "研发石矿开采:采石+3(花费:100食物,50石头)"
#define ACT_UPGRADE_FARM_NAME "研发驯养动物:农场食物产量+75(花费:200食物,50木头)"
#define ACT_STOP_NAME "中止"
#define ACT_BUILD_NAME "建造"
#define ACT_BUILD_HOUSE_NAME "建造房屋(花费:30木头)(上限:12)"
#define ACT_BUILD_GRANARY_NAME "建造谷仓(花费:120木头)"
#define ACT_BUILD_STOCK_NAME "建造仓库(花费:120木头)"
#define ACT_BUILD_CANCEL_NAME "取消建造"
#define ACT_BUILD_FARM_NAME "建造农场(花费:75木头)(需要先建造市场)"
#define ACT_BUILD_MARKET_NAME "建造市场(花费:150木头)(需要先建造谷仓)"
#define ACT_BUILD_ARROWTOWER_NAME "建造箭塔(花费:150石头)(需要先在谷仓内升级科技)"
#define ACT_NULL_NAME ""


#define ACT_WINDOW_NUM_FREE 10

#define ACT_NULL 0
#define ACT_CREATEFARMER 1
#define ACT_UPGRADE_AGE 2
#define ACT_UPGRADE_TOWERBUILD 3
#define ACT_UPGRADE_WOOD 4
#define ACT_UPGRADE_STONE 5
#define ACT_UPGRADE_FARM 6
#define ACT_UPGRADE_GOLD 7
#define ACT_STOCK_UPGRADE_USETOOL 8
#define ACT_STOCK_UPGRADE_DEFENSE_INFANTRY 9
#define ACT_STOCK_UPGRADE_DEFENSE_ARCHER 10
#define ACT_STOCK_UPGRADE_DEFENSE_RIDER 11
#define ACT_ARMYCAMP_CREATE_CLUBMAN 13
#define ACT_ARMYCAMP_CREATE_SLINGER 14
#define ACT_ARMYCAMP_UPGRADE_CLUBMAN 15
#define ACT_RANGE_CREATE_BOWMAN 16
#define ACT_STABLE_CREATE_SCOUT 17
#define ACT_RESEARCH_WALL 18

#define ACT_BUILD 50
#define ACT_BUILD_HOUSE 51
#define ACT_BUILD_GRANARY 52
#define ACT_BUILD_STOCK 53
#define ACT_BUILD_CANCEL 54
#define ACT_BUILD_FARM 55
#define ACT_BUILD_MARKET 56
#define ACT_BUILD_ARROWTOWER 57
#define ACT_BUILD_ARMYCAMP 58
#define ACT_BUILD_RANGE 59
#define ACT_BUILD_STABLE 60

#define ACT_STOP 100

/********** 对象视野 **********/
//建筑的视野搬至建筑属性相关
#define VISION_FARMER 4
#define VISION_GAZELLE 2
#define VISION_LION 3
#define VISION_ELEPHANT 4

/********** 地基编号 **********/
#define FOUNDATION_SMALL 0
#define FOUNDATION_MIDDLE 1
#define FOUNDATION_BIG 2
#define FOUNDATION_HOUSE 3
#define FOUNDATION_BLOCK 4

#define SORT_COORDINATE 0
#define SORT_BUILDING 1
#define SORT_STATICRES 2
#define SORT_HUMAN 4
#define SORT_FARMER 5
#define SORT_ANIMAL 7
#define SORT_TREEFOREST 6
#define SORT_MISSILE 8
#define SORT_Building_Resource 9
#define SORT_ARMY 10

#define PRODUCTSORT_WOOD 1
#define PRODUCTSORT_GRANARYFOOD 2
#define PRODUCTSORT_STONE 3
#define PRODUCTSORT_GOLD 4
#define PRODUCTSORT_STOCKFOOD 5

#define OBJECTTYPE_BLOCK 0
#define OBJECTTYPE_COORDINATE 1

#define ANIMAL_STATE_IDLE 0
#define ANIMAL_STATE_ROAMING 1
#define ANIMAL_STATE_FLEEING 2
#define ANIMAL_STATE_CHASING 3
#define ANIMAL_STATE_ATTACKING 4

#define BLOOD_TREE 25
#define BLOOD_GAZELLE 8
#define BLOOD_ELEPHANT 45
#define BLOOD_LION 20
#define BLOOD_FARMER 25
#define BLOOD_FOREST 100


#define CNT_TREE 75
#define CNT_GAZELLE 150
#define CNT_ELEPHANT 300
#define CNT_LION 100
//#define CNT_FARM 250
#define CNT_UPGRADEFARM 325
#define CNT_BUSH 150
#define CNT_STONE 250
#define CNT_GOLDORE 200
#define CNT_FOREST 300

#define ANIMAL_ATTACKRANGE_LION 10
#define ANIMAL_ATTACKRANGE_ELEPHANT 10

#define FARMER_VILLAGER 0
#define FARMER_LUMBER 1
#define FARMER_GATHERER 3
#define FARMER_MINER 2
#define FARMER_HUNTER 4
#define FARMER_FARMER 5
#define FARMER_WORKER 6

#define FARMER_CARRYLIMIT_WOOD 10
#define FARMER_CARRYLIMIT_FOOD 10
#define FARMER_CARRYLIMIT_STONE 10
#define FARMER_CARRYLIMIT_GOLD 10
#define FARMER_CARRYLIMIT_UPGRADEWOOD 12
#define FARMER_CARRYLIMIT_UPGRADEFOOD 13
#define FARMER_CARRYLIMIT_UPGRADESTONE 13
#define FARMER_CARRYLIMIT_UPGRADEGOLD 10

#define FARMER_HUNTRANGE 3
#define FARMER_UPGRADEHUNTRANGE 4

#define FARMER_GATHERSPEED_WOOD 0.02
#define FARMER_GATHERSPEED_FOOD 0.02
#define FARMER_GATHERSPEED_STONE 0.02
#define FARMER_GATHERSPEED_GOLD 0.02
#define FARMER_CONSTRUCTSPEED 0.02

#define MOVEOBJECT_STATE_STAND 0
#define MOVEOBJECT_STATE_WALK 1
#define MOVEOBJECT_STATE_ATTACK 2
#define MOVEOBJECT_STATE_DIE 3

#define MOVEOBJECT_STATE_WORK 5
#define MOVEOBJECT_STATE_RUN 6

#define ATTACKVALUE_FARMER 3

#define HOUSE_HUMAN_NUM 4
#define MAX_HUMAN_NUM 50

//鼠标结构体中对应鼠标点击事件
#define NULL_MOUSEEVENT 0
#define LEFT_PRESS 1
#define RIGHT_PRESS 2

/********** 同Class中图像资源种类数 **********/
#define NUMBER_MISSILE 2

/********** Core静态表 **********/
//####关系事件名称
#define CoreEven_JustMoveTo 0
#define CoreEven_CreatBuilding 1
#define CoreEven_Gather 2
#define CoreEven_Attacking 4
#define CoreEven_FixBuilding 5
#define CoreEven_BuildingAct 6
#define CoreEven_MissileAttack 7


//####对一个关系事件，细节关系的最大数量
#define CoreDetailLinkMaxNum 15
//####细节环节名称
#define CoreDetail_NormalEnd -1
#define CoreDetail_AbsoluteEnd -2
#define CoreDetail_JumpPhase -3
#define CoreDetail_Move 0
#define CoreDetail_Attack 1
#define CoreDetail_Gather 2
#define CoreDetail_ResourceIn 3
//#define CoreDetail_FindNextGoal 4
#define CoreDetail_UpdateRatio 5

/********** Core关系函数的可变操作指令 **********/
#define OPERATECON_DEFAULT 11111
#define OPERATECHANGE 100

//####距离判定
#define OPERATECON_NEAR_ABSOLUTE OPERATECON_DEFAULT
#define OPERATECON_MOVEALTER 200
#define OPERATECON_NEAR_ATTACK 10001
#define OPERATECON_NEAR_WORK 10002
#define OPERATECON_NEAR_MISSILE 10003
#define OPERATECON_NEARALTER_ABSOLUTE 20000
#define OPERATECON_NEARALTER_WORK 20002

//####指定对象
#define OPERATECON_OBJECT1 10011
#define OPERATECON_OBJECT2 10012

#define OPERATECON_TIMES 00001
#define OPERATE_TIMEMAX 10

#define OPERATECON_TIMES_USELESSACT_MOVE 250


/********** 距离常量 **********/
#define DISTANCE_Manhattan_MoveEndNEAR 0.0001
#define DISTANCE_ATTACK_CLOSE (8*gen5)
#define DISTANCE_HIT_TARGET 4

//箭塔攻击距离
#define DIS_ARROWTOWER 7

/********** 占地边长-块坐标常量 **********/
#define SIZELEN_SINGEL 1
#define SIZELEN_SMALL 2
#define SIZELEN_MIDDLE 3
#define SIZELEN_BIG 4

/********** 攻击方式 **********/
#define ATTACKTYPE_CANTATTACK -1
#define ATTACKTYPE_ANIMAL 0
#define ATTACKTYPE_CLOSE 1
#define ATTACKTYPE_CLOSE_TOTREE 11
#define ATTACKTYPE_SHOOT 2
//转化
#define ATTACKTYPE_CHANGE 3
//投石车
#define ATTACKTYPE_CATAPULT 4
//弩炮
#define ATTACKTYPE_BALISTA 5

/********** 军队类别 **********/
//步兵
#define ARMY_INFANTRY 1
//弓兵
#define ARMY_ARCHER 3
//骑兵
#define ARMY_RIDER 4
//祭
#define ARMY_FLAMEN 5
//攻城兵器
#define ARMY_SIEGE 6
//船
#define ARMY_SHIP 7

/********** 兵种类别 **********/
#define AT_CLUBMAN 0
//#define AT_SWORDSMAN 1
#define AT_SLINGER 1

//#define AT_HOPLITE 3

#define AT_BOWMAN 2
//#define AT_IMPROVED 5
//#define AT_CHARIOTARCHER 6
//#define AT_HORSE 7
//#define AT_ELEPHENTARCHER 8

#define AT_SCOUT 3
//#define AT_CAVALRY 10
//#define AT_CAMEL 11
//#define AT_CHARIOT 12
//#define AT_ELEPHENT 13

/********** 祭祀 **********/
#define AT_PRIEST 51

/********** 攻城工具 **********/
#define AT_THROWER 61
#define AT_BALLISTA 62

/********** 船类别 **********/
#define SHIP_FISHING 71
#define SHIP_TRADE 73
#define SHIP_TRANSPORT 72
#define SHIP_SCOUT 74
#define SHIP_FIRE 75
#define SHIP_CATAPUL 76
#define SHIP_JUGGERNAUT 77

/************士兵属性**************/
//棍棒手
#define BLOOD_CLUBMAN1 40
//村民表格中速度为1.1,设置为gen(5)
#define SPEED_CLUBMAN1 (1.2/1.1*HUMAN_SPEED)
#define VISION_CLUBMAN1 4
#define DIS_CLUBMAN1 0
#define INTERVAL_CLUBMAN1 1.5
#define ATK_CLUBMAN1 3
#define DEFCLOSE_CLUBMAN1 0
#define DEFSHOOT_CLUBMAN1 0

//刀斧手
#define BLOOD_CLUBMAN2 50
#define SPEED_CLUBMAN2 (1.2/1.1*HUMAN_SPEED)
#define VISION_CLUBMAN2 4
#define DIS_CLUBMAN2 0
#define INTERVAL_CLUBMAN2 1.5
#define ATK_CLUBMAN2 5
#define DEFCLOSE_CLUBMAN2 0
#define DEFSHOOT_CLUBMAN2 0

//短剑手
#define BLOOD_SHORTSWORDSMAN1 60
#define SPEED_SHORTSWORDSMAN1 (1.2/1.1*HUMAN_SPEED)
#define VISION_SHORTSWORDSMAN1 4
#define DIS_SHORTSWORDSMAN1 0
#define INTERVAL_SHORTSWORDSMAN1 1.5
#define ATK_SHORTSWORSMAN1 7
#define DEFCLOSE_SHORTSWORSMAN1 1
#define DEFSHOOT_SHORTSWORSMAN1 0

//阔剑手
#define BLOOD_SHORTSWORDSMAN2 70
#define SPEED_SHORTSWORDSMAN2 (1.2/1.1*HUMAN_SPEED)
#define VISION_SHORTSWORDSMAN2 4
#define DIS_SHORTSWORDSMAN2 0
#define INTERVAL_SHORTSWORDSMAN2 1.5
#define ATK_SHORTSWORSMAN2 9
#define DEFCLOSE_SHORTSWORSMAN2 1
#define DEFSHOOT_SHORTSWORSMAN2 0

//长剑手
#define BLOOD_SHORTSWORDSMAN3 80
#define SPEED_SHORTSWORDSMAN3 (1.2/1.1*HUMAN_SPEED)
#define VISION_SHORTSWORDSMAN3 4
#define DIS_SHORTSWORDSMAN3 0
#define INTERVAL_SHORTSWORDSMAN3 1.5
#define ATK_SHORTSWORSMAN3 11
#define DEFCLOSE_SHORTSWORSMAN3 2
#define DEFSHOOT_SHORTSWORSMAN3 0

//铁甲步兵
#define BLOOD_SHORTSWORDSMAN4 160
#define SPEED_SHORTSWORDSMAN4 (1.2/1.1*HUMAN_SPEED)
#define VISION_SHORTSWORDSMAN4 4
#define DIS_SHORTSWORDSMAN4 0
#define INTERVAL_SHORTSWORDSMAN4 1.5
#define ATK_SHORTSWORSMAN4 13
#define DEFCLOSE_SHORTSWORSMAN4 2
#define DEFSHOOT_SHORTSWORSMAN4 0

//投石者
#define BLOOD_SLINGER 25
#define SPEED_SLINGER (1.2/1.1*HUMAN_SPEED)
#define VISION_SLINGER 5
#define DIS_SLINGER 4
#define INTERVAL_SLINGER 1.5
#define ATK_SLINGER 2
#define DEFCLOSE_SLINGER 0
#define DEFSHOOT_SLINGER 2

//弓箭手
#define BLOOD_BOWMAN 35
#define SPEED_BOWMAN (1.2/1.1*HUMAN_SPEED)
#define VISION_BOWMAN 7
#define DIS_BOWMAN 5
#define INTERVAL_BOWMAN 1.4
#define ATK_BOWMAN 3
#define DEFCLOSE_BOWMAN 0
#define DEFSHOOT_BOWMAN 0

//长弓手
#define BLOOD_IMPROVEDBOWMAN1 40
#define SPEED_IMPROVEDBOWMAN1 (1.2/1.1*HUMAN_SPEED)
#define VISION_IMPROVEDBOWMAN1 8
#define DIS_IMPROVEDBOWMAN1 6
#define INTERVAL_IMPROVEDBOWMAN1 1.4
#define ATK_IMPROVEDBOWMAN1 4
#define DEFCLOSE_IMPROVEDBOWMAN1 0
#define DEFSHOOT_IMPROVEDBOWMAN1 0

//十字弓手
#define BLOOD_IMPROVEDBOWMAN2 45
#define SPEED_IMPROVEDBOWMAN2 (1.2/1.1*HUMAN_SPEED)
#define VISION_IMPROVEDBOWMAN2 9
#define DIS_IMPROVEDBOWMAN2 7
#define INTERVAL_IMPROVEDBOWMAN2 1.4
#define ATK_IMPROVEDBOWMAN2 5
#define DEFCLOSE_IMPROVEDBOWMAN2 0
#define DEFSHOOT_IMPROVEDBOWMAN2 0

//侦察骑兵
#define BLOOD_SCOUT 60
#define SPEED_SCOUT (2/1.1*HUMAN_SPEED)
#define VISION_SCOUT 8
#define DIS_SCOUT 0
#define INTERVAL_SCOUT 1.5
#define ATK_SCOUT 3
#define DEFCLOSE_SCOUT 0
#define DEFSHOOT_SCOUT 0

/********** animal友好度 **********/
#define FRIENDLY_NULL 0
#define FRIENDLY_FRI 1
#define FRIENDLY_ENEMY 2
#define FRIENDLY_FENCY 3


/********** 飞行物投掷判断 **********/
#define THROWMISSION_FARMER 25
#define THROWMISSION_ARCHER 4

#define THROWMISSION_ARROWTOWN_TIMER 30

/********** 飞行物类别 **********/
#define Missile_Spear 0
#define Missile_Arrow 1

/********** 飞行物属性 **********/
#define Missile_Speed_Spear (4*HUMAN_SPEED)
#define Missile_Speed_Arrow (4*HUMAN_SPEED)

/********** 设置多少帧切换一次nowres **********/
#define NOWRES_TIMER_FARMER 1
#define NOWRES_TIMER_LION 1
#define NOWRES_TIMER_ELEPHANT 1

#endif // CONFIG_H
