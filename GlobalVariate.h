#ifndef GLOBALVARIATE_H
#define GLOBALVARIATE_H

#include <QMutex>
#include <QSoundEffect>
#include "EventFilter.h"
#include "config.h"
#include<QNetworkAccessManager>
#include"networkplugin.h"
#include<QNetworkReply>

using namespace std;
class Coordinate;
//
extern QString ResultLogFile;
extern bool OffScreen;
extern int DefaultCivilization;
extern bool MAP_EXPLORE;
extern bool MAP_VISIABLE;
extern int INITIAL_WOOD;
extern int INITIAL_MEAT;
extern int INITIAL_GOLD;
extern int INITIAL_STONE;
extern double CRASHBOX_MICRO;
extern double CRASHBOX_SINGLEBLOCK;
extern double CRASHBOX_SMALLBLOCK;
extern double CRASHBOX_SMALL;
extern double CRASHBOX_MIDDLE;
extern double CRASHBOX_BIG;
extern double CRASHBOX_SINGLEOB;
extern double CRASHBOX_SMALLOB;
extern double CRASHBOX_BIGOB;
extern int BLOOD_BUILD_CENTER;
extern int VISION_CENTER;
extern int BUILD_CENTER_WOOD;
extern int TIME_BUILD_CENTER;
extern int BUILDING_CENTER_CREATEFARMER_FOOD;
extern int TIME_BUILDING_CENTER_CREATEFARMER;
extern int BUILDING_CENTER_UPGRADE_TOOLAGE_FOOD;
extern int TIME_BUILDING_CENTER_UPGRADE;
extern int BUILDING_CENTER_UPGRADE_BRONZEAGE_FOOD;
extern int BUILDING_CENTER_UPGRADE_BRONZEAGE_GOLD;
extern int BLOOD_BUILD_HOUSE;
extern int VISION_HOME;
extern int BUILD_HOUSE_WOOD;
extern int TIME_BUILD_HOME;
extern int BLOOD_BUILD_STOCK;
extern int VISION_STOCK;
extern int BUILD_STOCK_WOOD;
extern int TIME_BUILD_STOCK;
extern int BUILDING_STOCK_UPGRADE_CLOSER_ATTACK_FOOD;
extern int TIME_BUILDING_STOCK_UPGRADE_CLOSER_ATTACK;
extern int BUILDING_STOCK_UPGRADE_CLOSER_ATTACK_ADDITION_ATTACK;
extern int BUILDING_STOCK_UPGRADE_CLOSER_ATTACK_2_FOOD;
extern int BUILDING_STOCK_UPGRADE_CLOSER_ATTACK_2_GOLD;
extern int TIME_BUILDING_STOCK_UPGRADE_CLOSER_ATTACK_2;
extern int BUILDING_STOCK_UPGRADE_CLOSER_ATTACK_2_ADDITION_ATTACK;
extern int BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY_FOOD;
extern int TIME_BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY;
extern int BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY_ADDITION_DEFENSE_INFANTRY;
extern int BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY_2_FOOD;
extern int BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY_2_GOLD;
extern int TIME_BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY_2;
extern int BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY_2_ADDITION_DEFENSE_INFANTRY;
extern int BUILDING_STOCK_UPGRADE_MISSILE_DEFENSE_INFANTRY_FOOD;
extern int BUILDING_STOCK_UPGRADE_MISSILE_DEFENSE_INFANTRY_GOLD;
extern int TIME_BUILDING_STOCK_UPGRADE_MISSILE_DEFENSE_INFANTRY;
extern int BUILDING_STOCK_UPGRADE_MISSILE_DEFENSE_INFANTRY_ADDITION_DEFENSE_INFANTRY;
extern int BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER_FOOD;
extern int TIME_BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER;
extern int BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER_ADDITION_DEFENSE_ARCHER;
extern int BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER_2_FOOD;
extern int BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER_2_GOLD;
extern int TIME_BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER_2;
extern int BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER_2_ADDITION_DEFENSE_ARCHER;
extern int BUILDING_STOCK_UPGRADE_DEFENSE_RIDER_FOOD;
extern int TIME_BUILDING_STOCK_UPGRADE_DEFENSE_RIDER;
extern int BUILDING_STOCK_UPGRADE_DEFENSE_RIDER_ADDITION_DEFENSE_RIDER;
extern int BUILDING_STOCK_UPGRADE_DEFENSE_RIDER_2_FOOD;
extern int BUILDING_STOCK_UPGRADE_DEFENSE_RIDER_2_GOLD;
extern int TIME_BUILDING_STOCK_UPGRADE_DEFENSE_RIDER_2;
extern int BUILDING_STOCK_UPGRADE_DEFENSE_RIDER_2_ADDITION_DEFENSE_RIDER;
extern int BUILDING_GRANARY_UPGRADE_ARROWTOWER_FOOD;
extern int BUILDING_GRANARY_UPGRADE_ARROWTOWER_STONE;
extern int TIME_BUILDING_GRANARY_UPGRADE_ARROWTOWER;
extern int BUILDING_GRANARY_UPGRADE_ARROWTOWER_ADDITION_ATK;
extern int BLOOD_BUILD_GRANARY;
extern int VISION_GRANARY;
extern int BUILD_GRANARY_WOOD;
extern int TIME_BUILD_GRANARY;
extern int BUILDING_GRANARY_ARROWTOWER_FOOD;
extern int TIME_BUILDING_GRANARY_RESEARCH_ARROWTOWER;
extern int BUILDING_GRANARY_RESEARCH_WALL_FOOD;
extern int TIME_BUILDING_GRANARY_RESEARCH_WALL;
extern int BLOOD_BUILD_ARMYCAMP;
extern int VISION_ARMYCAMP;
extern int BUILD_ARMYCAMP_WOOD;
extern int TIME_BUILD_ARMYCAMP;
extern int BUILDING_ARMYCAMP_CREATE_CLUBMAN_FOOD;
extern int TIME_BUILDING_ARMYCAMP_CREATE_CLUBMAN;
extern int BUILDING_ARMYCAMP_UPGRADE_CLUBMAN_FOOD;
extern int TIME_BUILDING_ARMYCAMP_UPGRADE_CLUBMAN;
extern int BUILDING_ARMYCAMP_CREATE_SLINGER_FOOD;
extern int BUILDING_ARMYCAMP_CREATE_SLINGER_STONE;
extern int TIME_BUILDING_ARMYCAMP_CREATE_SLINGER;
// 训练阔剑兵
extern int BUILDING_ARMYCAMP_CREATE_BROADSWORD_FOOD;
extern int BUILDING_ARMYCAMP_CREATE_BROADSWORD_GOLD;
extern int TIME_BUILDING_ARMYCAMP_CREATE_BROADSWORD;
// 升级阔剑兵科技
extern int BUILDING_ARMYCAMP_UPGRADE_BROADSWORD_FOOD;
extern int BUILDING_ARMYCAMP_UPGRADE_BROADSWORD_GOLD;
extern int TIME_BUILDING_ARMYCAMP_UPGRADE_BROADSWORD;
extern int BLOOD_BUILD_RANGE;
extern int VISION_RANGE;
extern int BUILD_RANGE_WOOD;
extern int TIME_BUILD_RANGE;
extern int BUILDING_RANGE_CREATE_BOWMAN_FOOD;
extern int BUILDING_RANGE_CREATE_BOWMAN_WOOD;
extern int TIME_BUILDING_RANGE_CREATE_BOWMAN;
// 训练复合弓兵
extern int BUILDING_RANGE_CREATE_COMPOSITE_BOWMAN_FOOD;
extern int BUILDING_RANGE_CREATE_COMPOSITE_BOWMAN_GOLD;
extern int TIME_BUILDING_RANGE_CREATE_COMPOSITE_BOWMAN;
// 升级复合弓科技
extern int BUILDING_RANGE_UPGRADE_COMPOSITE_BOW_FOOD;
extern int BUILDING_RANGE_UPGRADE_COMPOSITE_BOW_WOOD;
extern int TIME_BUILDING_RANGE_UPGRADE_COMPOSITE_BOW;
// 训练战车弓箭手
extern int BUILDING_RANGE_CREATE_CHARIOT_ARCHER_FOOD;
extern int BUILDING_RANGE_CREATE_CHARIOT_ARCHER_WOOD;
extern int TIME_BUILDING_RANGE_CREATE_CHARIOT_ARCHER;

extern int BLOOD_BUILD_STABLE;
extern int VISION_STABLE;
extern int BUILD_STABLE_WOOD;
extern int TIME_BUILD_STABLE;
extern int BUILDING_STABLE_CREATE_SCOUT_FOOD;
extern int TIME_BUILDING_STABLE_CREATE_SCOUT;
//训练骑兵
extern int BUILDING_STABLE_CREATE_CAVALRY_FOOD;
extern int BUILDING_STABLE_CREATE_CAVALRY_GOLD;
extern int TIME_BUILDING_STABLE_CREATE_CAVALRY;

// 训练战车
extern int BUILDING_STABLE_CREATE_CHARIOT_FOOD;
extern int BUILDING_STABLE_CREATE_CHARIOT_WOOD;
extern int TIME_BUILDING_STABLE_CREATE_CHARIOT;
extern int BLOOD_BUILD_MARKET;
extern int VISION_MARKET;
extern int BUILD_MARKET_WOOD;
extern int TIME_BUILD_MARKET;
extern int BLOOD_BUILD_DOCK;
extern int VISION_DOCK;
extern int BUILD_DOCK_WOOD;
extern int TIME_BUILD_DOCK;
extern int BUILDING_DOCK_CREATE_SHIP_WOOD;
extern int TIME_BUILDING_DOCK_CREATE_SHIP;
extern int BUILDING_DOCK_CREATE_SAILING_WOOD;
extern int TIME_BUILDING_DOCK_CREATE_SAILING;
extern int BUILDING_DOCK_CREATE_WOOD_BOAT_WOOD;
extern int TIME_BUILDING_DOCK_CREATE_WOOD_BOAT;
extern int BUILDING_MARKET_WOOD_UPGRADE_FOOD;
extern int BUILDING_MARKET_WOOD_UPGRADE_WOOD;
extern int TIME_BUILDING_MARKET_UPGRADE_CUTTING;
extern int BUILDING_MARKET_WOOD_UPGRADE_ADDITION_CARRY;
extern int BUILDING_MARKET_WOOD_UPGRADE_ADDITION_GATHERRATE;
extern int BUILDING_MARKET_WOOD_UPGRADE_ADDITION_DISSHOOT;
extern int BLOOD_BUILD_SIEGE;
extern int VISION_SIEGE;
extern int BUILD_SIEGE_WOOD;
extern int TIME_BUILD_SIEGE;
extern int BLOOD_BUILD_COLLAGE;
extern int VISION_COLLAGE;
extern int BUILD_COLLAGE_WOOD;
extern int TIME_BUILD_COLLAGE;
extern int BUILDING_COLLAGE_CREATE_HOPLITE_FOOD;
extern int BUILDING_COLLAGE_CREATE_HOPLITE_GOLD;
extern int TIME_BUILDING_COLLAGE_CREATE_HOPLITE;
extern int BUILDING_SIEGE_CREATE_STONE_THROWER_WOOD;
extern int BUILDING_SIEGE_CREATE_STONE_THROWER_GOLD;
extern int TIME_BUILDING_SIEGE_CREATE_STONE_THROWER;
extern int BUILDING_MARKET_STONE_UPGRADE_FOOD;
extern int BUILDING_MARKET_STONE_UPGRADE_STONE;
extern int TIME_BUILDING_MARKET_UPGRADE_DIGGINGSOTNE;
extern int BUILDING_MARKET_STONE_UPGRADE_ADDITION_CARRY;
extern int BUILDING_MARKET_STONE_UPGRADE_ADDITION_GATHERRATE;
extern int BUILDING_MARKET_STONE_UPGRADE_ADDITION_SILNGERATK;
extern int BUILDING_MARKET_STONE_UPGRADE_ADDITION_SILNGERDIS;
extern int BUILDING_MARKET_GOLD_UPGRADE_FOOD;
extern int BUILDING_MARKET_GOLD_UPGRADE_WOOD;
extern int TIME_BUILDING_MARKET_UPGRADE_GOLD;
extern int BUILDING_MARKET_GOLD_UPGRADE_ADDITION_CARRY;
extern int BUILDING_MARKET_GOLD_UPGRADE_ADDITION_GATHERRATE;
extern int BUILDING_MARKET_FARM_UPGRADE_FOOD;
extern int BUILDING_MARKET_FARM_UPGRADE_WOOD;
extern int TIME_BUILDING_MARKET_UPGRADE_FARM;
extern int BUILDING_MARKET_FARM_UPGRADE_ADDITION_FOOD;
// 升级车轮
extern int BUILDING_MARKET_WHEEL_UPGRADE_FOOD;
extern int BUILDING_MARKET_WHEEL_UPGRADE_WOOD;
extern int TIME_BUILDING_MARKET_WHEEL_UPGRADE;
// 研发工艺（铜器时代，木材加工升级版）
extern int BUILDING_MARKET_CRAFT_UPGRADE_FOOD;
extern int BUILDING_MARKET_CRAFT_UPGRADE_WOOD;
extern int TIME_BUILDING_MARKET_CRAFT_UPGRADE;
extern int BUILDING_MARKET_CRAFT_UPGRADE_ADDITION_DISSHOOT;  // 投射武器攻击距离+1
extern int BUILDING_MARKET_CRAFT_UPGRADE_ADDITION_GATHERRATE;  // 伐木+2
// 研发犁（铜器时代，驯养动物升级版）
extern int BUILDING_MARKET_PLOW_UPGRADE_FOOD;
extern int BUILDING_MARKET_PLOW_UPGRADE_WOOD;
extern int TIME_BUILDING_MARKET_PLOW_UPGRADE;
extern int BUILDING_MARKET_PLOW_UPGRADE_ADDITION_FOOD;  // 农场食物产量+75
extern int BLOOD_BUILD_FARM;
extern int CNT_BUILD_FARM;
extern int VISION_FARM;
extern int BUILD_FARM_WOOD;
extern int TIME_BUILD_FARM;
extern int BLOOD_BUILD_ARROWTOWER;
extern double VISION_ARROWTOWER;
extern int DIS_BUILD_ARROWTOWER;
extern int ATK_BUILD_ARROWTOWER;
extern double DIS_ARROWTOWER;
extern int DEFSHOOT_BUILD_ARROWTOWER;
extern int BUILD_ARROWTOWER_STONE;
extern int TIME_BUILD_ARROWTOWER;
extern int BLOOD_BUILD_WALL;
extern int DEFSHOOT_BUILD_WALL;
extern int BUILD_WALL_STONE;
extern int TIME_BUILD_WALL;
extern double BLOCKSIDELENGTH;
extern double UNLOAD_RADIAN;
extern int MAP_L;
extern int MAP_U;
extern double HUMAN_SPEED;
extern double WOOD_BOAT_SPEED;
extern double ANIMAL_SPEED;
extern QString RESPATH;
extern int FRAMES_PER_SECOND;
extern bool OPTION_MUSIC;
extern bool OPTION_SOUND;
extern bool OPTION_SELECT;
extern bool OPTION_LINE;
extern bool OPTION_POS;
extern bool OPTION_OVERLAP;
extern int GAME_WIDTH;
extern int GAME_HEIGHT;
extern string GAME_VERSION;
extern QString GAME_TITLE;
extern string MAPFILE_SUFFIX;
extern int GAME_LOSE_SEC;
extern int MAXPLAYER;
extern int NOWPLAYER;
extern int NOWPLAYERREPRESENT;
extern int INITIAL_FREQUENCY;
extern int MEMORYROW;
extern int MEMORYCOLUMN;
extern int GAMEWIDGET_WIDTH;
extern int GAMEWIDGET_HEIGHT;
extern bool EditorMode;
extern EventFilter *eventFilter;
extern bool tryCaptured;
extern bool GlobalVision;
extern double BUILDING_BLOOD_FIRE_SMALL;
extern double BUILDING_BLOOD_FIRE_MIDDLE;
extern double BUILDING_BLOOD_FIRE_BIG;
extern int HOUSE_HUMAN_NUM;
extern int VISION_GAZELLE;
extern int BLOOD_GAZELLE;
extern int CNT_GAZELLE;
extern int VISION_LION;
extern int BLOOD_LION;
extern int CNT_LION;
extern int ANIMAL_ATTACKRANGE_LION;
extern int VISION_ELEPHANT;
extern int BLOOD_ELEPHANT;
extern double SPEED_ELEPHANT;
extern int CNT_ELEPHANT;
extern int ANIMAL_ATTACKRANGE_ELEPHANT;
extern int BLOOD_TREE;
extern int CNT_TREE;
extern int VISION_FARMER;
extern int BLOOD_FARMER;
extern int FARMER_CARRYLIMIT_WOOD;
extern int FARMER_CARRYLIMIT_FOOD;
extern int FARMER_CARRYLIMIT_GOLD;
extern int FARMER_CARRYLIMIT_STONE;
extern int FARMER_CARRYLIMIT_UPGRADEWOOD;
extern int FARMER_CARRYLIMIT_UPGRADEFOOD;
extern int FARMER_CARRYLIMIT_UPGRADEGOLD;
extern int FARMER_CARRYLIMIT_UPGRADESTONE;
extern double FARMER_GATHERSPEED_WOOD;
extern double FARMER_GATHERSPEED_FOOD;
extern double FARMER_GATHERSPEED_GOLD;
extern double FARMER_GATHERSPEED_STONE;
extern double FARMER_CONSTRUCTSPEED;
extern int CNT_UPGRADEFARM;
extern int CNT_BUSH;
extern int CNT_STONE;
extern int CNT_GOLDORE;
extern int CNT_FISH;
extern bool is_cheatAction;
extern double DISTANCE_Manhattan_MoveEndNEAR;
extern double DISTANCE_Manhattan_PathMove;
extern double DISTANCE_Manhattan_Unload;
extern double DISTANCE_Manhattan_Transport;
extern double DISTANCE_ATTACK_CLOSE;
extern double DISTANCE_HIT_TARGET;
extern double DISTANCE_ELEPHANT_ATTACK;
extern double SHIP_ACT_MAX_DISTANCE;
extern double SPEED_CLUBMAN1;
extern int VISION_CLUBMAN1;
extern int DIS_CLUBMAN1;
extern double INTERVAL_CLUBMAN1;
extern int ATK_CLUBMAN1;
extern int DEFCLOSE_CLUBMAN1;
extern int DEFSHOOT_CLUBMAN1;
extern int BLOOD_CLUBMAN1;
extern int BLOOD_CLUBMAN2;
extern double SPEED_CLUBMAN2;
extern int VISION_CLUBMAN2;
extern int DIS_CLUBMAN2;
extern double INTERVAL_CLUBMAN2;
extern int ATK_CLUBMAN2;
extern int DEFCLOSE_CLUBMAN2;
extern int DEFSHOOT_CLUBMAN2;
extern int BLOOD_SHORTSWORDSMAN1;
extern double SPEED_SHORTSWORDSMAN1;
extern int VISION_SHORTSWORDSMAN1;
extern int DIS_SHORTSWORDSMAN1;
extern double INTERVAL_SHORTSWORDSMAN1;
extern int ATK_SHORTSWORSMAN1;
extern int DEFCLOSE_SHORTSWORSMAN1;
extern int DEFSHOOT_SHORTSWORSMAN1;
extern int BLOOD_SHORTSWORDSMAN2;
extern double SPEED_SHORTSWORDSMAN2;
extern int VISION_SHORTSWORDSMAN2;
extern int DIS_SHORTSWORDSMAN2;
extern double INTERVAL_SHORTSWORDSMAN2;
extern int ATK_SHORTSWORSMAN2;
extern int DEFCLOSE_SHORTSWORSMAN2;
extern int DEFSHOOT_SHORTSWORSMAN2;
extern int BLOOD_SHORTSWORDSMAN3;
extern double SPEED_SHORTSWORDSMAN3;
extern int VISION_SHORTSWORDSMAN3;
extern int DIS_SHORTSWORDSMAN3;
extern double INTERVAL_SHORTSWORDSMAN3;
extern int ATK_SHORTSWORSMAN3;
extern int DEFCLOSE_SHORTSWORSMAN3;
extern int DEFSHOOT_SHORTSWORSMAN3;
extern int BLOOD_SHORTSWORDSMAN4;
extern double SPEED_SHORTSWORDSMAN4;
extern int VISION_SHORTSWORDSMAN4;
extern int DIS_SHORTSWORDSMAN4;
extern double INTERVAL_SHORTSWORDSMAN4;
extern int ATK_SHORTSWORSMAN4;
extern int DEFCLOSE_SHORTSWORSMAN4;
extern int DEFSHOOT_SHORTSWORSMAN4;
// 阔剑兵属性（使用SHORTSWORDSMAN2的属性）
extern int BLOOD_BROADSWORDSMAN;
extern double SPEED_BROADSWORDSMAN;
extern int VISION_BROADSWORDSMAN;
extern int ATK_BROADSWORDSMAN;
extern double DIS_BROADSWORDSMAN;
extern double INTERVAL_BROADSWORDSMAN;
extern int DEFCLOSE_BROADSWORDSMAN;
extern int DEFSHOOT_BROADSWORDSMAN;
extern int NOWRES_TIMER_BROADSWORDSMAN;

extern int BLOOD_SLINGER;
extern double SPEED_SLINGER;
extern int VISION_SLINGER;
extern int DIS_SLINGER;
extern double INTERVAL_SLINGER;
extern int ATK_SLINGER;
extern int DEFCLOSE_SLINGER;
extern int DEFSHOOT_SLINGER;
extern int BLOOD_BOWMAN;
extern double SPEED_BOWMAN;
extern int VISION_BOWMAN;
extern int DIS_BOWMAN;
extern double INTERVAL_BOWMAN;
extern int ATK_BOWMAN;
extern int DEFCLOSE_BOWMAN;
extern int DEFSHOOT_BOWMAN;
// 复合弓兵属性（使用IMPROVEDBOWMAN2的属性）
extern int BLOOD_COMPOSITE_BOWMAN;
extern double SPEED_COMPOSITE_BOWMAN;
extern int VISION_COMPOSITE_BOWMAN;
extern int ATK_COMPOSITE_BOWMAN;
extern double DIS_COMPOSITE_BOWMAN;
extern double INTERVAL_COMPOSITE_BOWMAN;
extern int DEFCLOSE_COMPOSITE_BOWMAN;
extern int DEFSHOOT_COMPOSITE_BOWMAN;
extern int NOWRES_TIMER_COMPOSITE_BOWMAN;
// 战车弓箭手属性
extern int BLOOD_CHARIOT_ARCHER;
extern double SPEED_CHARIOT_ARCHER;
extern int VISION_CHARIOT_ARCHER;
extern int ATK_CHARIOT_ARCHER;
extern double DIS_CHARIOT_ARCHER;
extern double INTERVAL_CHARIOT_ARCHER;
extern int DEFCLOSE_CHARIOT_ARCHER;
extern int DEFSHOOT_CHARIOT_ARCHER;
extern int NOWRES_TIMER_CHARIOT_ARCHER;

extern int BLOOD_IMPROVEDBOWMAN1;
extern double SPEED_IMPROVEDBOWMAN1;
extern int VISION_IMPROVEDBOWMAN1;
extern int DIS_IMPROVEDBOWMAN1;
extern double INTERVAL_IMPROVEDBOWMAN1;
extern int ATK_IMPROVEDBOWMAN1;
extern int DEFCLOSE_IMPROVEDBOWMAN1;
extern int DEFSHOOT_IMPROVEDBOWMAN1;
extern int BLOOD_IMPROVEDBOWMAN2;
extern double SPEED_IMPROVEDBOWMAN2;
extern int VISION_IMPROVEDBOWMAN2;
extern int DIS_IMPROVEDBOWMAN2;
extern double INTERVAL_IMPROVEDBOWMAN2;
extern int ATK_IMPROVEDBOWMAN2;
extern int DEFCLOSE_IMPROVEDBOWMAN2;
extern int DEFSHOOT_IMPROVEDBOWMAN2;
extern int BLOOD_SCOUT;
extern double SPEED_SCOUT;
extern int VISION_SCOUT;
extern int DIS_SCOUT;
extern double INTERVAL_SCOUT;
extern int ATK_SCOUT;
extern int DEFCLOSE_SCOUT;
extern int Boulder_Trail_Effect_Duration;
extern int DEFSHOOT_SCOUT;
// 战车属性
extern int BLOOD_CHARIOT;
extern double SPEED_CHARIOT;
extern int VISION_CHARIOT;
extern int ATK_CHARIOT;
extern double DIS_CHARIOT;
extern double INTERVAL_CHARIOT;
extern int DEFCLOSE_CHARIOT;
extern int DEFSHOOT_CHARIOT;
extern int NOWRES_TIMER_CHARIOT;
extern int BLOOD_CAVALRY;
extern double SPEED_CAVALRY;
extern int VISION_CAVALRY;
extern int DIS_CAVALRY;
extern double INTERVAL_CAVALRY;
extern int ATK_CAVALRY;
extern int DEFCLOSE_CAVALRY;
extern int DEFSHOOT_CAVALRY;
extern int BLOOD_SHIP;
extern double SPEED_SHIP;
extern int VISION_SHIP;
extern int ATK_SHIP;
extern double DIS_SHIP;
extern int INTERVAL_SHIP;
extern int DEFCLOSE_SHIP;
extern int DEFSHOOT_SHIP;
extern int BLOOD_STONE_THROWER;
extern double SPEED_STONE_THROWER;
extern int VISION_STONE_THROWER;
extern int ATK_STONE_THROWER;
extern double DIS_STONE_THROWER;
extern int INTERVAL_STONE_THROWER;
extern int DEFCLOSE_STONE_THROWER;
extern int DEFSHOOT_STONE_THROWER;
extern int BLOOD_PRIEST;
extern double SPEED_PRIEST;
extern int VISION_PRIEST;
extern int ATK_PRIEST;
extern double DIS_PRIEST;
extern int INTERVAL_PRIEST;
extern int DEFCLOSE_PRIEST;
extern int DEFSHOOT_PRIEST;
// 方阵兵属性
extern int BLOOD_HOPLITE;
extern double SPEED_HOPLITE;
extern int VISION_HOPLITE;
extern int ATK_HOPLITE;
extern double DIS_HOPLITE;
extern double INTERVAL_HOPLITE;
extern int DEFCLOSE_HOPLITE;
extern int DEFSHOOT_HOPLITE;
extern int NOWRES_TIMER_HOPLITE;

extern double Missile_Speed_Spear;
extern double Missile_Speed_Arrow;
extern double Missile_Speed_Cobblestone;
extern double Missile_Speed_Boulders;
extern int Missile_Boulders_Range;
extern int NOWRES_TIMER_FARMER;
extern int NOWRES_TIMER_CLUBMAN;
extern int NOWRES_TIMER_BOWMAN;
extern int NOWRES_TIMER_IMPROVEDBOWMAN1;
extern int NOWRES_TIMER_SCOUT;
extern int NOWRES_TIMER_CAVALRY;
extern int NOWRES_TIMER_SLINGER;
extern int NOWRES_TIMER_SWORSMAN;
extern int NOWRES_TIMER_LION;
extern int NOWRES_TIMER_ELEPHANT;
extern int NOWRES_TIMER_SHIP;
extern int NOWRES_TIMER_STONE_THROWER;
extern int NOWRES_TIMER_PRIEST;
//
extern bool IsExamining;
extern int TimePerFrame;
extern NetworkPlugin*NetworkManager;
extern QString GameServerAddr;
//
extern bool AIfinished;
extern bool INSfinshed;
extern int g_globalNum;
extern int mapmoveFrequency;
extern std::map<int, Coordinate*> g_Object;
extern int** memorymap;
extern int MidX;
extern int MidY;
extern int MAP_LSide[2];
extern int MAP_USide[2];
extern int Forest[3][15][15];
extern int Food[5][5][5];
extern int Stone[5][5][5];
extern int g_frame;
extern QTextBrowser* g_DebugText;
extern map<string, list<QPixmap>> resMap;
extern map<string, QSoundEffect*> SoundMap;
extern std::queue<string> soundQueue;

extern std::list<Coordinate*> drawlist;

extern Coordinate* nowobject;
extern Coordinate* LeftMouseObjCapture;
extern Coordinate*RightMouseObjCaptrue;
extern bool GenerateHumanLock;//
//每一帧传输到输出日志的结构体
struct ResultLogInfo{
    bool win;
    int wood;
    int food;
    int gold;
    int stone;
    int score;
    string msg;
    ResultLogInfo(bool win_,int score_,int wood_,int food_,int gold_,int stone_,string msg_="");
    void LogOut();
    QString ToString();
};
//当前选中对象
//出于gamewidget和core均需要获取当前访问对象

struct st_DebugMassage {
    QString color;
    QString content;
    st_DebugMassage() {}
    st_DebugMassage(QString color, QString content);
};
void call_debugText(QString color, QString content, int playerID);

extern std::queue<st_DebugMassage>debugMassagePackage;
extern bool only_debug_Player0;
extern bool filterRepetitionMessage;
extern std::map<QString, int>debugMessageRecord;

struct Score {
private:
    int id;
    int score;
    int scoreTypes[SCORE_TYPE_COUNT] = { 0 };

    void addScore(int points, const QString& message);

public:
    Score(int id);
    int getScore();
    void update(int type, int num = 1);
};
//
struct tagObj{
    int SN;// 序列号
    int BlockDR, BlockUR; //区块坐标
    bool operator <(const tagObj&obj)const;
};
struct tagBuilding:tagObj
{
    int Type; // 建筑类型
    int Blood; // 当前血量
    int MaxBlood; // 最大血量
    int Percent; // 完成百分比
    int Project; // 当前项目
    int ProjectPercent; // 项目完成百分比
    int Cnt; // 剩余资源量（仅农田）
    tagBuilding toEnemy();
};

struct tagResource:tagObj
{
    double DR, UR; //细节坐标
    int Type; // 资源类型
    int ProductSort; // 产品种类
    int Cnt; // 剩余资源数量
    int Blood; // 当前血量
};

struct tagHuman:tagObj
{
    double DR, UR; //细节坐标
    double DR0, UR0; // 目的地坐标
    int NowState; // 当前状态
    int WorkObjectSN; // 工作对象序列号
    int Blood; // 当前血量
    int attack; // 攻击力
    int rangedDefense; // 远程防御
    int meleeDefense; // 近战防御
    void cast_from(tagHuman taghuman);
};

struct tagFarmer : public tagHuman
{
    int ResourceSort; // 手持资源种类
    int Resource; // 手持资源数量
    int FarmerSort;//农民的类型
    tagFarmer toEnemy();
};

struct tagArmy : public tagHuman
{
    int Sort; // 军队种类
    int status;
    int starttime;
    int finishtime;
    double startpointDR;
    double startpointUR;
    double destinaDR;
    double destinaUR;
    bool ifAttack;
    int timelock;
    tagArmy toEnemy();
};
//struct tagBlock{
//    bool explored=false;
//    int height=0;
//};

struct instruction {
    ///用于存储ai发出的指令信息
    /// @param type 指令类型
    /// @param option 对应类型下的操作
    /// type 0:终止对象self的动作
    /// type 1:命令村民self走向指定坐标L0，U0
    /// type 2:将obj对象设定为村民self的工作对象，村民会自动走向对象并工作
    /// type 3:命令村民self在块坐标BlockL,BlockU处建造类型为option的新建筑
    /// type 4:对建筑self发出命令option
    int ret = -1;
    int type;
    int id;
    Coordinate* self;
    Coordinate* obj;
    int option;
    int BlockDR, BlockUR;
    int SN = -1, obSN = -1;
    double DR, UR;
    bool isExist();
    instruction() { type = -1; }
    instruction(int type, int SN, int obSN, bool twoCoredinate);
    instruction(int type, int SN, int BlockDR, int BlockUR, int option);
    instruction(int type, int SN, double DR, double UR);
    instruction(int type, int SN, int option);
};

struct ins {
    int g_id = 0;
    std::queue<instruction> instructions;
    QMutex lock;
};
struct tagMap
{
    bool explore;
    int high;

    //该位置资源信息
    int type;       //资源种类（浆果、树等）
    int ResType;    //采集获得的资源种类（食物、木头等） human_WOOD...
    int fundation;  //该资源占地图大小
    int SN;
    int remain;     //剩余资源量

    //*********************************
    tagMap();
    void clear();

    void clear_r();
};
struct tagTerrain {
    int height;
    int type;
};
// 定义二维点的结构体
struct Point {
    int x;
    int y;

    Point();
    Point(int x, int y);
    Point(const Point& board);

    Point operator +(const Point& ps);
    Point operator -(const Point& ps);
    bool operator ==(const Point& ps)const;
    bool operator < (const Point& ps)const;
};

struct tagInfo
{
    using TerrainData=const vector<vector<tagTerrain>>;
    vector<tagBuilding> buildings; // 我方建筑列表
    vector<tagFarmer> farmers; // 我方农民列表
    vector<tagArmy> armies; // 我方军队列表
    vector<tagBuilding> enemy_buildings; // 敌方建筑列表
    vector<tagFarmer> enemy_farmers; // 敌方农民列表
    vector<tagArmy> enemy_armies; // 敌方军队列表
    vector<tagResource> resources; // 资源列表
    map<int, int> ins_ret; // 指令返回值，map<id, ret>
    TerrainData*theMap;// 高程图
    //tagTerrain theMap[MAP_L][MAP_U]; //弃用这种形式
    vector<Point>exploredUpdate;//多探索的区域
    int GameFrame; // 游戏帧数
    int civilizationStage; // 文明阶段
    int Wood; // 木材数量
    int Meat; // 肉类数量
    int Stone; // 石头数量
    int Gold; // 黄金数量
    int Human_MaxNum; // 最大人口数量
    // Assignment operator
    tagInfo& operator=(const tagInfo& other);

    void clear();
};



struct tagGame
{
private:
    tagInfo* Info;
    QMutex Locker;
public:
    template<class T>
    void WLHHunYao(vector<T>& v) {
        vector<T> res = v;
        static auto randint = [](int a, int b)->int {return rand() % (b - a + 1) + a;};
        for (int i = 0;i < v.size();++i) {
            int idx = randint(0, res.size() - 1);
            swap(res.back(), res[idx]);
            v[i] = res.back();
            res.pop_back();
        }
    }
    void update(tagInfo* newinfo);
    void insertInsRet(int id, instruction ins);
    tagInfo getInfo();
    void clearInsRet();
};

struct MouseEvent
{
private:
    int memoryMapX;
    int memoryMapY;
    double DR;
    double UR;
    int mouseEventType;
public:
    //鼠标点击类型 自定义对应关系 左键点击 左键拉框 右键点击等
    MouseEvent();
    int GetMouseEventType();
    void SetMouseEventType(int tp);
    bool HaveEvent();
    int GetMemoryMapX();
    int GetMemoryMapY();
    void SetMemoeyMapX(int v);
    void SetMemoryMapY(int v);
    double GetDR();
    double GetUR();
    void SetDR(double v);
    void SetUR(double v);
    void Reset();
};


extern std::string direction[5];

struct pixMemoryMap
{
    vector<char> MemoryMap;
    int width;
    int height;

    // 构造函数
    pixMemoryMap(int w, int h);

    pixMemoryMap();

    pixMemoryMap(const pixMemoryMap& other);

    pixMemoryMap& operator=(const pixMemoryMap& other);
    void setMemoryMap(int i, int j);

    char getMemoryMap(int i, int j);

    void fillBlockMemoryMap();
};

struct ImageResource
{
    QPixmap pix;
    pixMemoryMap memorymap;

    ImageResource(QPixmap pix);

    ImageResource();
};


struct conditionDevelop
{
    int civilization;
    int sort_building;  //所属建筑

    double times_second;

    int acttimes = 0;    //表示执行的此数

    bool isCreatObjectAction = false; //行动结束后是否需要创建对象
    int creatObjectSort = -1;   //需要创建对象的类sort
    int creatObjectNum = -1;    //需要创建对象的类中的Num

    //记录前置条件
    list<conditionDevelop*> preCondition;

    //本行动对应的下一项行动
    conditionDevelop* nextDevAction = NULL;

    //需要的物资
    int need_Wood, need_Food, need_Stone, need_Gold;

    //**********************************************************
    //构造器 ， 创建conditionDevelop实例相关
    conditionDevelop();
    conditionDevelop(int civilization, int sort_building, double needTimes, int need_Wood = 0, int need_Food = 0, int need_Stone = 0, int need_Gold = 0);

    //添加该行动的前置行动
    void addPreCondition(conditionDevelop* con_need);

    //添加行动结束后创建对象
    void setCreatObjectAfterAction(int creatSort);
    void setCreatObjectAfterAction(int creatSort, int creatNum);

    //**********************************************************
    //判断行动是否能进行
    //判断资源是否满足行动需要
    bool executable(int wood, int food, int stone, int gold);
    void get_needResource(int& wood, int& food, int& stone, int& gold);

    //判断当前时代、已完成的建筑行动，是否解锁当前行动
    bool isShowable(int nowcivilization);

    //**********************************************************
    //行动结束后相关操作
    void finishAct();

    //获取是否需要创建对象
    bool isNeedCreatObject(int& creatSort, int& creatNum);

    bool isNeedCreatObject();

    int getActTimes();
};

struct st_upgradeLab {
    conditionDevelop* headAct = NULL, * nowExecuteNode = NULL, * endNode = NULL;
    int haveFinishedPhaseNum = 0;
    bool nowExecuting = false;

    st_upgradeLab();
    ~st_upgradeLab();

    void setHead(conditionDevelop* head);

    void push_back(conditionDevelop* node);

    //设置建筑的行动没有下一个行动，但该行动可以重复地执行
    void endNodeAsOver();

    //切换
    void shift();

    //当前行动是否可在SelectWidget中显示
    bool isShowAble(int nowcivilization);

    //当前行动是否可执行
    bool executable(int nowcivilization, int wood, int food, int stone, int gold);

    void beginExecute();
    void overExecute();

    //获取当前行动列表执行过几轮（一个链node算一轮）
    int getPhaseTimes();
    //获取需要的资源
    void get_needResource(int& wood, int& food, int& stone, int& gold);
    /**
    *考虑加入错误码，以判断错误类型
    */

    bool isNeedCreatObject();
};

struct st_buildAction
{
    //建筑的建造条件
    conditionDevelop* buildCon = NULL;

    //存储该建筑拥有哪些行动。第一键值为行动标号，第二键值为对应的行动表，行动表为链表，存储了执行条件
    map<int, st_upgradeLab> actCon;

    st_buildAction();
    ~st_buildAction();

    void finishBuild();
    void finishAction(int actNum);
};




/*
 * 0是成功
 * -1是SN不存在
 * -2是Action不存在
 * -3是位置超界
 * -4是obSN不存在
 * -5是BuildingNum不存在
 * -6是资源不足
 */

int InitImageResMap(QString path);
int InitSoundResMap(QString path);
QPixmap applyTransparencyEffect(const QPixmap& originalPixmap, qreal opacity);
void loadResource(std::string name, std::list<ImageResource>* targetlist);
void loadGrayRes(std::list<ImageResource>* res, std::list<ImageResource>* grayres);
void loadBlackRes(std::list<ImageResource>* res, std::list<ImageResource>* blackres);

void flipResource(std::list<ImageResource>* currentlist, std::list<ImageResource>* targetlist);
void initMemory(ImageResource* res);
double countdistance(double L, double U, double L0, double U0);
bool isNear_Manhattan(double dr, double ur, double dr1, double ur1, double distance);

int calculateManhattanDistance(int x1, int y1, int x2, int y2);
double calculateManhattanDistance(double x1, double y1, double x2, double y2);

void calMirrorPoint(double& dr, double& ur, double dr_mirror, double ur_mirror, double dis);

double trans_BlockPointToDetailCenter(int p);
QString JsonMap(const QMap<QString, QVariant>&data);

int sgn(double __x);
void ParseArguments(const QApplication&app);
void ReadConfig();

#endif // GLOBALVARIATE_H
