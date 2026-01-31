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
extern int GAME_WIDTH;                 //总窗口宽度
extern int GAME_HEIGHT;              //总窗口高度
extern QString GAME_VERSION;
extern QString GAME_TITLE;     //总窗口名称
extern QString MAPFILE_SUFFIX;           //地图文件后缀
extern int GAME_LOSE_SEC;
extern int GAME_WIN_GOLD;
extern int GOLD;                         //金块资源数量
extern int STONE;                        //石头资源数量
extern double gen5;
extern int MAXPLAYER;
extern int NOWPLAYER;
extern int NOWPLAYERREPRESENT;
extern int INITIAL_FREQUENCY;

extern int MEMORYROW;                  //列 行长度
extern int MEMORYCOLUMN;               //行 列长度
extern int GAMEWIDGET_WIDTH;
extern int GAMEWIDGET_HEIGHT;
extern double BLOCKSIDELENGTH;
extern int UNLOAD_RADIAN;  //卸货时候寻找合适的陆地的半径
extern int MAP_L;
extern int MAP_U;
extern int GENERATE_L;
extern int GENERATE_U;
extern int GAMEWIDGET_MIDBLOCKL;
extern int GAMEWIDGET_MIDBLOCKU;
extern double HUMAN_SPEED;
extern double WOOD_BOAT_SPEED;     //运输船的速度和骑士一样快
extern double ANIMAL_SPEED;
extern QString RESPATH;
extern int ForestMAX;
extern int FOREST_GENERATE_L;
extern int FOREST_GENERATE_U;
extern int FOREST_GENERATE_PERCENT;  // 生成森林时出现空地的概率
extern int FOREST_GENERATE_OPTCOUNTER;
extern int FOREST_COUNT_MIN;
extern int TREEMAX;
extern int BLOCKPIXEL_X;
extern int BLOCKPIXEL_Y;
extern int GENERATELANDFORMS_NUM;
extern int KEY_UP;
extern int KEY_LEFT;
extern int KEY_DOWN;
extern int KEY_RIGHT;
extern int BLOCK_COUNT;     // Block种类计数，包括所有种类和样式的地图块数量
extern int FRAMES_PER_SECOND;    //每秒帧数
extern bool OPTION_MUSIC;  // 默认false，关闭
extern bool OPTION_SOUND;  // 默认false，关闭
extern bool OPTION_SELECT;
extern bool OPTION_LINE;
extern bool OPTION_POS;
extern bool OPTION_OVERLAP;


/********** 地图块种类 **********/
/* L0边为右上角，L0到L3顺时针排列 */
/* A0边为上方角，A0到A3顺时针排列 */
extern int MAPTYPE_EMPTY;         // 未定义种类
extern int MAPTYPE_FLAT;          // 平地
extern int MAPTYPE_A2_UPTOU;      // A2角（向上方凸起）
extern int MAPTYPE_A0_DOWNTOD;    // A0角（向下方凹陷）
extern int MAPTYPE_L1_UPTOLU;     // L1边（向左上凸起）
extern int MAPTYPE_L3_DOWNTORD;   // L3边（向右下凹陷）
extern int MAPTYPE_L0_DOWNTOLD;   // L0边（向左下凹陷）
extern int MAPTYPE_L2_UPTORU;     // L2边（向右上凸起）
extern int MAPTYPE_A1_UPTOL;      // A1角（向左方凸起）
extern int MAPTYPE_A3_UPTOR;      // A3角（向右方凸起）
extern int MAPTYPE_A1_DOWNTOL;    // A1角（向左方凹陷）
extern int MAPTYPE_A3_DOWNTOR;    // A3角（向右方凹陷）
extern int MAPTYPE_L0_UPTOLD;     // L0边（向左下凸起）
extern int MAPTYPE_L2_DOWNTORU;   // L2边（向右上凹陷）
extern int MAPTYPE_L3_UPTORD;     // L3边（向右下凸起）
extern int MAPTYPE_L1_DOWNTOLU;   // L1边（向左上凹陷）
extern int MAPTYPE_A2_DOWNTOU;    // A2角（向上方凹陷）
extern int MAPTYPE_A0_UPTOD;      // A0角（向下方凸起）
extern int MAPTYPE_OCEAN;    //海洋

/********** 地图块高度 **********/
extern int MAPHEIGHT_FLAT;        // 地形高度
extern int MAPHEIGHT_MAX;         // 最高地形高度
extern int MAPHEIGHT_OCEAN;       //海洋的高度
extern int MAPHEIGHT_PERCENT;    // 生成概率，范围0~100
extern int MAPHEIGHT_OPTCOUNT;  // 生成高度时的优化次数，范围要求>=5
extern int CENTER_RADIUS;   // 特判市镇中心附近平地的半径
extern int CENTER_DEVIATION;      // 市镇中心坐标偏移量


/********** 地图块样式 **********/
extern int MAPPATTERN_UNKNOWN;      // 未定义样式
extern int MAPPATTERN_GRASS;      // 草原
extern int MAPPATTERN_DESERT;     // 沙漠
extern int MAPPATTERN_OCEAN;      // 海洋/河流
extern int MAPPATTERN_SHOAL;      // 浅滩（河流中可行走部分）

/********** 地图块绘制偏移量 **********/
extern int DRAW_OFFSET;


/********** DebugText栏颜色 **********/
extern QString (*COLOR_RED)(QString);
extern QString (*COLOR_BLUE)(QString);
extern QString (*COLOR_GREEN)(QString);
extern QString (*COLOR_BLACK)(QString);


/********** 建筑火焰种类 **********/
extern int BUILDING_FIRE_SMALL;
extern int BUILDING_FIRE_MIDDLE;
extern int BUILDING_FIRE_BIG;

extern double BUILDING_BLOOD_FIRE_SMALL;
extern double BUILDING_BLOOD_FIRE_MIDDLE;
extern double BUILDING_BLOOD_FIRE_BIG;


/********** 建筑种类 **********/
extern int BUILDING_TYPE_MAXNUM;
extern int BUILDING_HOME;
extern int BUILDING_GRANARY;
extern int BUILDING_CENTER;
extern int BUILDING_STOCK;

extern int BUILDING_MARKET;
extern int BUILDING_ARROWTOWER;
extern int BUILDING_ARROWTOWERPOSITION;

extern int BUILDING_ARMYCAMP;
extern int BUILDING_STABLE;
extern int BUILDING_RANGE;
extern int BUILDING_DOCK;
extern int BUILDING_WALL;

//Building_Resource类（产资源建筑）
extern int BUILDING_FARM;

/********** 建筑状态 **********/
extern int BUILDING_FREE; // 建筑状态为空闲（无工作）时用0表示


/********** 建筑动作 **********/
//市镇中心
extern int BUILDING_CENTER_CREATEFARMER;
extern int BUILDING_CENTER_UPGRADE;
//谷仓
extern int BUILDING_GRANARY_ARROWTOWER;
extern int BUILDING_GRANARY_WALL;
//市场
extern int BUILDING_MARKET_WOOD_UPGRADE;
extern int BUILDING_MARKET_STONE_UPGRADE;
extern int BUILDING_MARKET_FARM_UPGRADE;
extern int BUILDING_MARKET_GOLD_UPGRADE;

//仓库
extern int BUILDING_STOCK_UPGRADE_USETOOL;
extern int BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY;
extern int BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER;
extern int BUILDING_STOCK_UPGRADE_DEFENSE_RIDER;

//军营
extern int BUILDING_ARMYCAMP_CREATE_CLUBMAN;
extern int BUILDING_ARMYCAMP_CREATE_SLINGER;
extern int BUILDING_ARMYCAMP_UPGRADE_CLUBMAN;

//靶场
extern int BUILDING_RANGE_CREATE_BOWMAN;

//马厩
extern int BUILDING_STABLE_CREATE_SCOUT;

extern int BUILDING_CANCEL;
//船坞
extern int BUILDING_DOCK_CREATE_SAILING;
extern int BUILDING_DOCK_CREATE_WOOD_BOAT;
extern int BUILDING_DOCK_CREATE_SHIP;


/********** 建筑动作命名 **********/
extern QString BUILDING_GRANARY_ARROWTOWER_NAME;
extern QString BUILDING_CENTER_CREATEFARMER_NAME;
extern QString BUILDING_CENTER_UPGRADE_NAME;
extern QString BUILDING_MARKET_WOOD_UPGRADE_NAME;
extern QString BUILDING_MARKET_STONE_UPGRADE_NAME;
extern QString BUILDING_MARKET_GOLD_UPGRADE_NAME;
extern QString BUILDING_MARKET_FARM_UPGRADE_NAME;
extern QString BUILDING_ACTION_CANCEL_NAME;


/********** Coordinate子类中 Num值实际指代种类 **********/
//资源
extern int NUM_STATICRES_Bush;
extern int NUM_STATICRES_Stone;
extern int NUM_STATICRES_GoldOre;
extern int NUM_STATICRES_Fish;
//动物
extern int ANIMAL_TREE;
extern int ANIMAL_GAZELLE;
extern int ANIMAL_ELEPHANT;
extern int ANIMAL_LION;
extern int ANIMAL_FOREST;

/********** 人物状态 **********/
/*
 * 0代表为空闲状态
 * 1代表为正在移动状态（无目标对象）
 * 2代表为正在工作状态
 * 3代表为正在攻击状态
 */
extern int HUMAN_STATE_IDLE;
extern int HUMAN_STATE_WALKING;
extern int HUMAN_STATE_WORKING;
extern int HUMAN_STATE_ATTACKING;

 /********** 人物手持资源种类 **********/
extern int HUMAN_WOOD;
extern int HUMAN_STOCKFOOD;
extern int HUMAN_STONE;
extern int HUMAN_GOLD;
extern int HUMAN_GRANARYFOOD;
extern int HUMAN_DOCKFOOD; //也就是鱼肉
/********** AI函数 **********/
//函数编号
extern int FUC_BUILDINGACTION;
extern int FUC_HUMANMOVE;
extern int FUC_HUMANACTION;
extern int FUC_HUMANBUILD;

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
extern int ACTION_SUCCESS;
extern int ACTION_INVALID_SN;
extern int ACTION_INVALID_ACTION;
extern int ACTION_INVALID_LOCATION;
extern int ACTION_INVALID_OBSN;
extern int ACTION_INVALID_BUILDINGNUM;
extern int ACTION_INVALID_RESOURCE;
extern int ACTION_INVALID_UPGRADE_TIME;
 //控制对象被删除
extern int ACTION_INVALID_NULLWORKER;
//目标对象已被删除
extern int ACTION_INVALID_NULLGOALOBJECT;

//对象已有必须手动取消的任务，不空闲
extern int ACTION_INVALID_ISNTFREE;

//BuildingAction
//建筑还在建造过程中
extern int ACTION_INVALID_BUILDACT_NEEDBUILT;

//建筑行动未解锁，或该行动只能进行有限次且已达上限
extern int ACTION_INVALID_BUILDACT_LOCK;
//造人行动，已达人口上限
extern int ACTION_INVALID_BUILDACT_MAXHUMAN;

//HumanMove

//HumanAction
//修理建筑，建筑不需要修理
extern int ACTION_INVALID_HUMANACTION_BUILDNOTNEEDFIX;
extern int ACTION_INVALID_HUMANACTION_BUILD2RESOURCENOMATCH;

//HumanBuild
//建筑位置有高度差
extern int ACTION_INVALID_HUMANBUILD_DIFFERENTHIGH;
//建筑位置加上建筑宽度，超出边界
extern int ACTION_INVALID_HUMANBUILD_OVERBORDER;
//建筑位置未被探索
extern int ACTION_INVALID_HUMANBUILD_UNEXPLORE;
//建筑位置与其他物体有重叠冲突
extern int ACTION_INVALID_HUMANBUILD_OVERLAP;
//建筑未解锁，未达成建筑条件
extern int ACTION_INVALID_HUMANBUILD_LOCK;
//距离相距太远
extern int ACTION_INVALID_DISTANCE_FAR;
//携带人物满了
extern int ACTION_INVALID_FULLY_LOAD;
//建筑放置的位置不合适
extern int ACTION_INVALID_POSITION_NOT_FIT;
/********** 资源种类 **********/
/*
 * 如表 十进制位代表大的分类 个位代表他在大类中的具体编号
 */
extern int RESOURCE_EMPTY;
extern int RESOURCE_BUSH;
extern int RESOURCE_TREE;
extern int RESOURCE_STONE;
extern int RESOURCE_GAZELLE;
extern int RESOURCE_ELEPHANT;
extern int RESOURCE_LION;
extern int RESOURCE_GOLD;
extern int RESOURCE_FISH;

 /********** 时代编号 **********/
extern int CIVILIZATION_STONEAGE;
extern int CIVILIZATION_TOOLAGE;
extern int CIVILIZATION_BRONZEAGE;
extern int CIVILIZATION_IRONAGE;


extern int ACT_STATUS_ENABLED;
extern int ACT_STATUS_ANIME;
extern int ACT_STATUS_DISABLED;


/********** 人物动作命名 **********/
extern QString ACT_CREATEFARMER_NAME;
extern QString ACT_UPGRADE_AGE_NAME;
extern QString ACT_UPGRADE_TOWERBUILD_NAME;
extern QString ACT_UPGRADE_WOOD_NAME;
extern QString ACT_UPGRADE_STONE_NAME;
extern QString ACT_UPGRADE_GOLD_NAME;
extern QString ACT_UPGRADE_FARM_NAME;
extern QString ACT_STOP_NAME;
extern QString ACT_BUILD_NAME;
extern QString ACT_BUILD_HOUSE_NAME;
extern QString ACT_BUILD_GRANARY_NAME;
extern QString ACT_BUILD_STOCK_NAME;
extern QString ACT_BUILD_CANCEL_NAME;
extern QString ACT_BUILD_FARM_NAME;
extern QString ACT_BUILD_MARKET_NAME;
extern QString ACT_BUILD_ARROWTOWER_NAME;
extern QString ACT_BUILD_DOCK_NAME;
extern QString ACT_SHIP_LAY_NAME;

extern QString ACT_ARMYCAMP_CREATE_CLUBMAN_NAME;
extern QString ACT_ARMYCAMP_CREATE_SLINGER_NAME;
extern QString ACT_ARMYCAMP_UPGRADE_CLUBMAN_NAME;
extern QString ACT_BUILD_ARMYCAMP_NAME;
extern QString ACT_BUILD_RANGE_NAME;
extern QString ACT_BUILD_STABLE_NAME;
extern QString ACT_RANGE_CREATE_BOWMAN_NAME;
extern QString ACT_RESEARCH_WALL_NAME;
extern QString ACT_STABLE_CREATE_SCOUT_NAME;
extern QString ACT_STOCK_UPGRADE_DEFENSE_ARCHER_NAME;
extern QString ACT_STOCK_UPGRADE_DEFENSE_INFANTRY_NAME;
extern QString ACT_STOCK_UPGRADE_DEFENSE_RIDER_NAME;
extern QString ACT_STOCK_UPGRADE_USETOOL_NAME;
extern QString ACT_DOCK_CREATE_SAILING_NAME;
extern QString ACT_DOCK_CREATE_WOOD_BOAT_NAME;
extern QString ACT_DOCK_CREATE_SHIP_NAME;
extern QString ACT_NULL_NAME;

extern int ACT_WINDOW_NUM_FREE;

extern int ACT_NULL;
extern int ACT_CREATEFARMER;
extern int ACT_UPGRADE_AGE;
extern int ACT_UPGRADE_TOWERBUILD;
extern int ACT_UPGRADE_WOOD;
extern int ACT_UPGRADE_STONE;
extern int ACT_UPGRADE_FARM;
extern int ACT_UPGRADE_GOLD;
extern int ACT_STOCK_UPGRADE_USETOOL;
extern int ACT_STOCK_UPGRADE_DEFENSE_INFANTRY;
extern int ACT_STOCK_UPGRADE_DEFENSE_ARCHER;
extern int ACT_STOCK_UPGRADE_DEFENSE_RIDER;
extern int ACT_ARMYCAMP_CREATE_CLUBMAN;
extern int ACT_ARMYCAMP_CREATE_SLINGER;
extern int ACT_ARMYCAMP_UPGRADE_CLUBMAN;
extern int ACT_RANGE_CREATE_BOWMAN;
extern int ACT_STABLE_CREATE_SCOUT;
extern int ACT_RESEARCH_WALL;
extern int ACT_DOCK_CREATE_SAILING;
extern int ACT_DOCK_CREATE_WOOD_BOAT;
extern int ACT_DOCK_CREATE_SHIP;

extern int ACT_BUILD;
extern int ACT_BUILD_HOUSE;
extern int ACT_BUILD_GRANARY;
extern int ACT_BUILD_STOCK;
extern int ACT_BUILD_CANCEL;
extern int ACT_BUILD_FARM;
extern int ACT_BUILD_MARKET;
extern int ACT_BUILD_ARROWTOWER;
extern int ACT_BUILD_ARMYCAMP;
extern int ACT_BUILD_RANGE;
extern int ACT_BUILD_STABLE;
extern int ACT_BUILD_DOCK;

extern int ACT_SHIP_LAY;

extern int ACT_STOP;


/********** 对象视野 **********/
//建筑的视野搬至建筑属性相关
extern int VISION_FARMER;
extern int VISION_GAZELLE;
extern int VISION_LION;
extern int VISION_ELEPHANT;


/********** 地基编号 **********/
extern int FOUNDATION_SMALL;
extern int FOUNDATION_MIDDLE;
extern int FOUNDATION_BIG;
extern int FOUNDATION_HOUSE;
extern int FOUNDATION_BLOCK;

extern int SORT_COORDINATE;
extern int SORT_BUILDING;
extern int SORT_STATICRES;
extern int SORT_HUMAN;
extern int SORT_FARMER;
extern int SORT_ANIMAL;
extern int SORT_TREEFOREST;
extern int SORT_MISSILE;
extern int SORT_Building_Resource;
extern int SORT_ARMY;

extern int PRODUCTSORT_WOOD;
extern int PRODUCTSORT_GRANARYFOOD;
extern int PRODUCTSORT_STONE;
extern int PRODUCTSORT_GOLD;
extern int PRODUCTSORT_STOCKFOOD;

extern int OBJECTTYPE_BLOCK;
extern int OBJECTTYPE_COORDINATE;

extern int ANIMAL_STATE_IDLE;
extern int ANIMAL_STATE_ROAMING;
extern int ANIMAL_STATE_FLEEING;
extern int ANIMAL_STATE_CHASING;
extern int ANIMAL_STATE_ATTACKING;

extern int BLOOD_TREE;
extern int BLOOD_GAZELLE;
extern int BLOOD_ELEPHANT;
extern int BLOOD_LION;
extern int BLOOD_FARMER;
extern int BLOOD_FOREST;

extern double SPEED_ELEPHANT;

extern int CNT_TREE;
extern int CNT_GAZELLE;
extern int CNT_ELEPHANT;
extern int CNT_LION;
//#define CNT_FARM 250
extern int CNT_UPGRADEFARM;
extern int CNT_BUSH;
extern int CNT_STONE;
extern int CNT_GOLDORE;
extern int CNT_FOREST;
extern int CNT_FISH;

extern int ANIMAL_ATTACKRANGE_LION;
extern int ANIMAL_ATTACKRANGE_ELEPHANT;

extern int FARMER_VILLAGER;
extern int FARMER_LUMBER;
extern int FARMER_GATHERER;
extern int FARMER_MINER;
extern int FARMER_HUNTER;
extern int FARMER_FARMER;
extern int FARMER_WORKER;
extern int FARMER_FISHER;

extern int FARMER_CARRYLIMIT_WOOD;
extern int FARMER_CARRYLIMIT_FOOD;
extern int FARMER_CARRYLIMIT_STONE;
extern int FARMER_CARRYLIMIT_GOLD;
extern int FARMER_CARRYLIMIT_UPGRADEWOOD;
extern int FARMER_CARRYLIMIT_UPGRADEFOOD;
extern int FARMER_CARRYLIMIT_UPGRADESTONE;
extern int FARMER_CARRYLIMIT_UPGRADEGOLD;

extern int FARMER_HUNTRANGE;
extern int FARMER_UPGRADEHUNTRANGE;

extern double FARMER_GATHERSPEED_WOOD;
extern double FARMER_GATHERSPEED_FOOD;
extern double FARMER_GATHERSPEED_STONE;
extern double FARMER_GATHERSPEED_GOLD;
extern double FARMER_CONSTRUCTSPEED;

extern int MOVEOBJECT_STATE_STAND;
extern int MOVEOBJECT_STATE_WALK;
extern int MOVEOBJECT_STATE_ATTACK;
extern int MOVEOBJECT_STATE_DIE;

extern int MOVEOBJECT_STATE_WORK;
extern int MOVEOBJECT_STATE_RUN;
extern int ATTACKVALUE_FARMER;

extern int HOUSE_HUMAN_NUM;
extern int MAX_HUMAN_NUM;

//鼠标结构体中对应鼠标点击事件
extern int NULL_MOUSEEVENT;
extern int LEFT_PRESS;
extern int RIGHT_PRESS;


/********** 同Class中图像资源种类数 **********/
extern int NUMBER_MISSILE;


/********** Core静态表 **********/
//####关系事件名称
extern int CoreEven_JustMoveTo;
extern int CoreEven_CreatBuilding;
extern int CoreEven_Gather;
extern int CoreEven_Attacking;
extern int CoreEven_FixBuilding;
extern int CoreEven_BuildingAct;
extern int CoreEven_MissileAttack;
extern int CoreEven_Transport;
extern int CoreEven_UnLoad;
//####对一个关系事件，细节关系的最大数量
extern int CoreDetailLinkMaxNum;
//####细节环节名称
extern int CoreDetail_NormalEnd;
extern int CoreDetail_AbsoluteEnd;
extern int CoreDetail_JumpPhase;
extern int CoreDetail_Move;
extern int CoreDetail_Attack;
extern int CoreDetail_Gather;
extern int CoreDetail_ResourceIn;
extern int CoreDetail_Transport;
extern int CoreDetail_UpdateRatio;
extern int CoreDetail_Unload;

/********** Core关系函数的可变操作指令 **********/
extern int OPERATECON_DEFAULT;
extern int OPERATECHANGE;

//####距离判定
extern int OPERATECON_NEAR_ABSOLUTE;
extern int OPERATECON_MOVEALTER;
extern int OPERATECON_NEAR_ATTACK;
extern int OPERATECON_NEAR_WORK;
extern int OPERATECON_NEAR_MISSILE;
extern int OPERATECON_NEAR_ATTACK_MOVE;
extern int OPERATECON_NEAR_UNLOAD;
extern int OPERATECON_NEAR_TRANSPORT;

extern int OPERATECON_NEARALTER_ABSOLUTE;
extern int OPERATECON_NEARALTER_WORK;

//####指定对象
extern int OPERATECON_OBJECT1;
extern int OPERATECON_OBJECT2;

extern int OPERATECON_TIMES;
extern int OPERATE_TIMEMAX;

extern int OPERATECON_TIMES_USELESSACT_MOVE;


/********** 占地边长-块坐标常量 **********/
extern int SIZELEN_SINGEL;
extern int SIZELEN_SMALL;
extern int SIZELEN_MIDDLE;
extern int SIZELEN_BIG;


/********** animal友好度 **********/
extern int FRIENDLY_NULL;
extern int FRIENDLY_FRI;
extern int FRIENDLY_ENEMY;
extern int FRIENDLY_FENCY;

/********** 兵种状态 **********/
extern int ARMY_STATE_DEFAULT;
extern int ARMY_STATE_AROUND;
extern int ARMY_STATE_DEFENSE;
extern int ARMY_STATE_ATTACK;

/********** 巡逻状态 **********/
extern int PATROL_STATE_IDLE;
extern int PATROL_STATE_PATROLLING;
extern int PATROL_STATE_CHASING;
extern int PATROL_STATE_RETURNING;

#endif // CONFIG_H