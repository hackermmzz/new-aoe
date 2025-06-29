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

#include <Windows.h>
#include <time.h>
#include "digitalConfig.h"


/********** 游戏配置数据 **********/
#define GAME_WIDTH 1920                 //总窗口宽度
#define GAME_HEIGHT 1000              //总窗口高度
#define GAME_VERSION "v2.02g"
#define GAME_TITLE "Age of Empires"     //总窗口名称
#define GAME_LOSE_SEC 1500
#define GAME_WIN_GOLD 2500
#define GOLD 10                         //金块资源数量
#define STONE 10                        //石头资源数量
#define gen5 sqrt(5)
#define MAXPLAYER 8
#define NOWPLAYER 2
#define NOWPLAYERREPRESENT 0
#define INITIAL_FREQUENCY 1
#define EXAMINE_MODE false

#define MEMORYROW 400                  //列 行长度
#define MEMORYCOLUMN 200               //行 列长度
#define GAMEWIDGET_WIDTH 1440
#define GAMEWIDGET_HEIGHT 751
#define BLOCKSIDELENGTH (16*gen5)
#define UNLOAD_RADIAN  2                //卸货时候寻找合适的陆地的半径
#define MAP_L 128
#define MAP_U 128
#define GENERATE_L (MAP_L+8)
#define GENERATE_U (MAP_U+8)
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
#define BLOCK_COUNT 41     // Block种类计数，包括所有种类和样式的地图块数量
#define FRAMES_PER_SECOND 25    //每秒帧数
#define OPTION_MUSIC true  // 默认关闭
#define OPTION_SOUND true  // 默认关闭
#define OPTION_SELECT false
#define OPTION_LINE false
#define OPTION_POS false
#define OPTION_OVERLAP false


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
#define MAPPATTERN_UNKNOWN 0      // 未定义样式
#define MAPPATTERN_GRASS 1      // 草原
#define MAPPATTERN_DESERT 2     // 沙漠
#define MAPPATTERN_OCEAN 3      // 海洋/河流
#define MAPPATTERN_SHOAL 4      // 浅滩（河流中可行走部分）

/********** 地图块绘制偏移量 **********/
#define DRAW_OFFSET -15


/********** DebugText栏颜色 **********/
#define COLOR_RED(STRING) QString("<font color=red>%1</font><font color=black> </font>").arg(STRING)
#define COLOR_BLUE(STRING) QString("<font color=blue>%1</font><font color=black> </font>").arg(STRING)
#define COLOR_GREEN(STRING) QString("<font color=green>%1</font><font color=black> </font>").arg(STRING)
#define COLOR_BLACK(STRING) QString("<font color=black>%1</font><font color=black> </font>").arg(STRING)


/********** 建筑火焰种类 **********/
#define BUILDING_FIRE_SMALL 0
#define BUILDING_FIRE_MIDDLE 1
#define BUILDING_FIRE_BIG 2

#define BUILDING_BLOOD_FIRE_SMALL 0.75
#define BUILDING_BLOOD_FIRE_MIDDLE 0.5
#define BUILDING_BLOOD_FIRE_BIG 0.25


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
#define BUILDING_DOCK 10
#define BUILDING_WALL 11

//Building_Resource类（产资源建筑）
#define BUILDING_FARM 4

/********** 建筑状态 **********/
#define BUILDING_FREE 0 // 建筑状态为空闲（无工作）时用0表示


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
//船坞
#define BUILDING_DOCK_CREATE_SAILING 119
#define BUILDING_DOCK_CREATE_WOOD_BOAT 120
#define BUILDING_DOCK_CREATE_SHIP 515


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
#define NUM_STATICRES_Bush 0
#define NUM_STATICRES_Stone 1
#define NUM_STATICRES_GoldOre 2
#define NUM_STATICRES_Fish 3
//动物
#define ANIMAL_TREE 0
#define ANIMAL_GAZELLE 1
#define ANIMAL_ELEPHANT 2
#define ANIMAL_LION 3
#define ANIMAL_FOREST 4

/********** 人物状态 **********/
/*
 * 0代表为空闲状态
 * 1代表为正在移动状态（无目标对象）
 * 2代表为正在工作状态
 * 3代表为正在攻击状态
 */
#define HUMAN_STATE_IDLE 0
#define HUMAN_STATE_WALKING 1
#define HUMAN_STATE_WORKING 2
#define HUMAN_STATE_ATTACKING 3

 /********** 人物手持资源种类 **********/
#define HUMAN_WOOD 1
#define HUMAN_STOCKFOOD 2
#define HUMAN_STONE 3
#define HUMAN_GOLD 4
#define HUMAN_GRANARYFOOD 5
#define HUMAN_DOCKFOOD 6 //也就是鱼肉
/********** AI函数 **********/
//函数编号
#define FUC_BUILDINGACTION 1
#define FUC_HUMANMOVE 2
#define FUC_HUMANACTION 3
#define FUC_HUMANBUILD 4

/********** 动作返回编号及action错误码 **********/
/*
 * 0是成功
 * -1是SN不存在
 * -2是Action不存在
 * -3是指定位置超界
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

 //控制对象被删除
#define ACTION_INVALID_NULLWORKER -80
//目标对象已被删除
#define ACTION_INVALID_NULLGOALOBJECT -81

//对象已有必须手动取消的任务，不空闲
#define ACTION_INVALID_ISNTFREE -82

//BuildingAction
//建筑还在建造过程中
#define ACTION_INVALID_BUILDACT_NEEDBUILT -11

//建筑行动未解锁，或该行动只能进行有限次且已达上限
#define ACTION_INVALID_BUILDACT_LOCK -13
//造人行动，已达人口上限
#define ACTION_INVALID_BUILDACT_MAXHUMAN -14

//HumanMove

//HumanAction
//修理建筑，建筑不需要修理
#define ACTION_INVALID_HUMANACTION_BUILDNOTNEEDFIX -21
#define ACTION_INVALID_HUMANACTION_BUILD2RESOURCENOMATCH -22

//HumanBuild
//建筑位置有高度差
#define ACTION_INVALID_HUMANBUILD_DIFFERENTHIGH -41
//建筑位置加上建筑宽度，超出边界
#define ACTION_INVALID_HUMANBUILD_OVERBORDER -42
//建筑位置未被探索
#define ACTION_INVALID_HUMANBUILD_UNEXPLORE -43
//建筑位置与其他物体有重叠冲突
#define ACTION_INVALID_HUMANBUILD_OVERLAP -44
//建筑未解锁，未达成建筑条件
#define ACTION_INVALID_HUMANBUILD_LOCK -45
//距离相距太远
#define ACTION_INVALID_DISTANCE_FAR -99
//携带人物满了
#define ACTION_INVALID_FULLY_LOAD -119
//建筑放置的位置不合适
#define ACTION_INVALID_POSITION_NOT_FIT -520
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
#define RESOURCE_GOLD 99
#define RESOURCE_FISH 119

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
#define ACT_UPGRADE_GOLD_NAME "研发金矿开采:采金+3(花费:120食物,100木头)"
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
#define ACT_BUILD_DOCK_NAME "建造船坞(花费:120木头)"
#define ACT_SHIP_LAY_NAME "卸货"

#define ACT_ARMYCAMP_CREATE_CLUBMAN_NAME "训练棍棒兵(花费:50食物)"
#define ACT_ARMYCAMP_CREATE_SLINGER_NAME "训练投石兵(花费:40食物,10石头)"
#define ACT_ARMYCAMP_UPGRADE_CLUBMAN_NAME "升级为战斧(花费:100食物)"
#define ACT_BUILD_ARMYCAMP_NAME "建造兵营(花费:125木头)"
#define ACT_BUILD_RANGE_NAME "建造靶场(花费:150木头)"
#define ACT_BUILD_STABLE_NAME "建造马厩(花费:150木头)"
#define ACT_RANGE_CREATE_BOWMAN_NAME "训练弓箭手(花费:40食物,20木头)"
#define ACT_RESEARCH_WALL_NAME "研发:建造低级城墙"
#define ACT_STABLE_CREATE_SCOUT_NAME "训练侦察骑兵(花费:60食物)"
#define ACT_STOCK_UPGRADE_DEFENSE_ARCHER_NAME "研发弓兵护甲:弓箭手近战防御+2(花费:100食物)"
#define ACT_STOCK_UPGRADE_DEFENSE_INFANTRY_NAME "研发步兵护甲:近战单位近战防御+2(花费:75食物)"
#define ACT_STOCK_UPGRADE_DEFENSE_RIDER_NAME "研发骑兵护甲:骑兵近战防御+2(花费:125食物)"
#define ACT_STOCK_UPGRADE_USETOOL_NAME "研发工具使用:近战单位攻击+2(花费:100食物)"
#define ACT_DOCK_CREATE_SAILING_NAME "建造渔船"
#define ACT_DOCK_CREATE_WOOD_BOAT_NAME "建造运输船"
#define ACT_DOCK_CREATE_SHIP_NAME "建造战船"
#define ACT_NULL_NAME ""

#define ACT_WINDOW_NUM_FREE 12

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
#define ACT_DOCK_CREATE_SAILING 19
#define ACT_DOCK_CREATE_WOOD_BOAT 20
#define ACT_DOCK_CREATE_SHIP 21

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
#define ACT_BUILD_DOCK 61

#define ACT_SHIP_LAY 62

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

#define SPEED_ELEPHANT (0.8 * ANIMAL_SPEED)

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
#define CNT_FISH 200

#define ANIMAL_ATTACKRANGE_LION 10
#define ANIMAL_ATTACKRANGE_ELEPHANT 10

#define FARMER_VILLAGER 0
#define FARMER_LUMBER 1
#define FARMER_GATHERER 3
#define FARMER_MINER 2
#define FARMER_HUNTER 4
#define FARMER_FARMER 5
#define FARMER_WORKER 6
#define FARMER_FISHER 7

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
#define NUMBER_MISSILE 3


/********** Core静态表 **********/
//####关系事件名称
#define CoreEven_JustMoveTo 0
#define CoreEven_CreatBuilding 1
#define CoreEven_Gather 2
#define CoreEven_Attacking 4
#define CoreEven_FixBuilding 5
#define CoreEven_BuildingAct 6
#define CoreEven_MissileAttack 7
#define CoreEven_Transport 8
#define CoreEven_UnLoad 9
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
#define CoreDetail_Transport 4
#define CoreDetail_UpdateRatio 5
#define CoreDetail_Unload 6

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
#define SIZELEN_SINGEL 1
#define SIZELEN_SMALL 2
#define SIZELEN_MIDDLE 3
#define SIZELEN_BIG 4


/********** animal友好度 **********/
#define FRIENDLY_NULL 0
#define FRIENDLY_FRI 1
#define FRIENDLY_ENEMY 2
#define FRIENDLY_FENCY 3

/********** 兵种状态 **********/
#define ARMY_STATE_DEFAULT 0
#define ARMY_STATE_AROUND 1
#define ARMY_STATE_DEFENSE 2
#define ARMY_STATE_ATTACK 3

#endif // CONFIG_H
