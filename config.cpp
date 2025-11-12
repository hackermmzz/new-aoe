#include"QJsonDocument"
#include"QJsonObject"
#include"QJsonArray"
#include"QJsonValue"
#include"QFile"
#include"config.h"
using namespace std;
int GAME_WIDTH; //总窗口宽度
int GAME_HEIGHT; //总窗口高度
string GAME_VERSION; //版本
string GAME_TITLE;//总窗口名称
//string MAPFILE_SUFFIX;  //地图文件后缀
int GOLD;
int STONE;
//double gen5;
int NOWPLAYERREPRESENT;
int INITIAL_FREQUENCY;
bool MAP_EXPLORE;
bool MAP_VISIABLE;
int MEMORYROW;
int MEMORYCOLUMN;
int GAMEWIDGET_WIDTH;
int GAMEWIDGET_HEIGHT;
int UNLOAD_RADIAN;
//int MAP_L;
//int MAP_U;
//int GENERATE_L;
//int GENERATE_U;
int GAMEWIDGET_MIDBLOCKL;
int GAMEWIDGET_MIDBLOCKU;
//QString RESPATH;
//int ForestMAX;
//int FOREST_GENERATE_L;
//int FOREST_GENERATE_U;
//int FOREST_GENERATE_PERCENT;
//int FOREST_GENERATE_OPTCOUNTER;
//int FOREST_COUNT_MIN;
//int TREEMAX;
int FRAMES_PER_SECOND;
int GENERATELANDFORMS_NUM;
int KEY_UP;
int KEY_LEFT;
int KEY_DOWN;
int KEY_RIGHT;
//int BLOCK_COUNT;     // Block种类计数，包括所有种类和样式的地图块数量
bool OPTION_MUSIC;  // 默认false，关闭
bool OPTION_SOUND; // 默认false，关闭
bool OPTION_SELECT;
bool OPTION_LINE;
bool OPTION_POS;
bool OPTION_OVERLAP;

vector<string>sound_work;

/********** 地图块种类 **********/
/* L0边为右上角，L0到L3顺时针排列 */
/* A0边为上方角，A0到A3顺时针排列 */
//int MAPTYPE_EMPTY;        // 未定义种类
//int MAPTYPE_FLAT;          // 平地

int MAPTYPE_A2_UPTOU;      // A2角（向上方凸起）
int MAPTYPE_A0_DOWNTOD;    // A0角（向下方凹陷）
int MAPTYPE_L1_UPTOLU;     // L1边（向左上凸起）
int MAPTYPE_L3_DOWNTORD;   // L3边（向右下凹陷）
int MAPTYPE_L0_DOWNTOLD;   // L0边（向左下凹陷）
int MAPTYPE_L2_UPTORU;    // L2边（向右上凸起）
int MAPTYPE_A1_UPTOL;      // A1角（向左方凸起）
int MAPTYPE_A3_UPTOR;      // A3角（向右方凸起）
int MAPTYPE_A1_DOWNTOL;    // A1角（向左方凹陷）
int MAPTYPE_A3_DOWNTOR;    // A3角（向右方凹陷）
int MAPTYPE_L0_UPTOLD;    // L0边（向左下凸起）
int MAPTYPE_L2_DOWNTORU;  // L2边（向右上凹陷）
int MAPTYPE_L3_UPTORD;    // L3边（向右下凸起）
int MAPTYPE_L1_DOWNTOLU;  // L1边（向左上凹陷）
int MAPTYPE_A2_DOWNTOU;   // A2角（向上方凹陷）
int MAPTYPE_A0_UPTOD;     // A0角（向下方凸起）
//int MAPTYPE_OCEAN;     //海洋

/********** 地图块高度 **********/
int MAPHEIGHT_FLAT;        // 地形高度
int MAPHEIGHT_MAX;        // 最高地形高度
int MAPHEIGHT_OCEAN;      //海洋的高度
int MAPHEIGHT_PERCENT;    // 生成概率，范围0~100
int MAPHEIGHT_OPTCOUNT;   // 生成高度时的优化次数，范围要求>=5
int CENTER_RADIUS;      // 特判市镇中心附近平地的半径
int CENTER_DEVIATION;    // 市镇中心坐标偏移量

/********** 地图块样式 **********/
int MAPPATTERN_UNKNOWN;      // 未定义样式
int MAPPATTERN_GRASS;      // 草原
int MAPPATTERN_DESERT;     // 沙漠
int MAPPATTERN_OCEAN;      // 海洋/河流
int MAPPATTERN_SHOAL;      // 浅滩（河流中可行走部分）

/********** 地图块绘制偏移量 **********/
int DRAW_OFFSET;

/********** DebugText栏颜色 **********/

///********** 建筑火焰种类 **********/
int BUILDING_FIRE_SMALL;
int BUILDING_FIRE_MIDDLE;
int BUILDING_FIRE_BIG;

double BUILDING_BLOOD_FIRE_SMALL;
double BUILDING_BLOOD_FIRE_MIDDLE;
double BUILDING_BLOOD_FIRE_BIG;


///********** 建筑种类 **********/
//int BUILDING_TYPE_MAXNUM;
//int BUILDING_HOME;
//int BUILDING_GRANARY;
//int BUILDING_CENTER;
//int BUILDING_STOCK;

//int BUILDING_MARKET;
//int BUILDING_ARROWTOWER;
//int BUILDING_ARROWTOWERPOSITION;

//int BUILDING_ARMYCAMP;
//int BUILDING_STABLE;
//int BUILDING_RANGE;
//int BUILDING_DOCK;
//int BUILDING_WALL;

////Building_Resource类（产资源建筑）
//int BUILDING_FARM;

///********** 建筑状态 **********/
//int BUILDING_FREE; // 建筑状态为空闲（无工作）时用0表示


///********** 建筑动作 **********/
//市镇中心
//int BUILDING_CENTER_CREATEFARMER;
//int BUILDING_CENTER_UPGRADE;
//////谷仓
//int BUILDING_GRANARY_ARROWTOWER;
//int BUILDING_GRANARY_WALL;
//市场
//int BUILDING_MARKET_WOOD_UPGRADE;
//int BUILDING_MARKET_STONE_UPGRADE;
//int BUILDING_MARKET_FARM_UPGRADE;
//int BUILDING_MARKET_GOLD_UPGRADE;

////仓库
//int BUILDING_STOCK_UPGRADE_USETOOL;
//int BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY;
//int BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER;
//int BUILDING_STOCK_UPGRADE_DEFENSE_RIDER;

////军营
//int BUILDING_ARMYCAMP_CREATE_CLUBMAN;
//int BUILDING_ARMYCAMP_CREATE_SLINGER;
//int BUILDING_ARMYCAMP_UPGRADE_CLUBMAN;

////靶场
//int BUILDING_RANGE_CREATE_BOWMAN;

////马厩
//int BUILDING_STABLE_CREATE_SCOUT;

//int BUILDING_CANCEL;
////船坞
//int BUILDING_DOCK_CREATE_SAILING;
//int BUILDING_DOCK_CREATE_WOOD_BOAT;
//int BUILDING_DOCK_CREATE_SHIP;

/********** 建筑动作命名 **********/
string BUILDING_GRANARY_ARROWTOWER_NAME;
string BUILDING_CENTER_CREATEFARMER_NAME;
string BUILDING_CENTER_UPGRADE_NAME;
string BUILDING_MARKET_WOOD_UPGRADE_NAME;
string BUILDING_MARKET_STONE_UPGRADE_NAME;
string BUILDING_MARKET_GOLD_UPGRADE_NAME;
string BUILDING_MARKET_FARM_UPGRADE_NAME;
string BUILDING_ACTION_CANCEL_NAME;


///********** Coordinate子类中 Num值实际指代种类 **********/
//资源
//int NUM_STATICRES_Bush;
//int NUM_STATICRES_Stone;
//int NUM_STATICRES_GoldOre;
//int NUM_STATICRES_Fish;
////动物
//int TREE;
//int GAZELLE;
//int ELEPHANT;
//int LION;
//int FOREST;

///********** 人物状态 **********/
///*
// * 0代表为空闲状态
// * 1代表为正在移动状态（无目标对象）
// * 2代表为正在工作状态
// * 3代表为正在攻击状态
// */
int HUMAN_STATE_IDLE;
int HUMAN_STATE_WALKING;
int HUMAN_STATE_WORKING;
int HUMAN_STATE_ATTACKING;

// /********** 人物手持资源种类 **********/
/*int HUMAN_WOOD;
int HUMAN_STOCKFOOD;
int HUMAN_STONE;
int HUMAN_GOLD;
int HUMAN_GRANARYFOOD;
int HUMAN_DOCKFOOD;*/ //也就是鱼肉
///********** AI函数 **********/
////函数编号
int FUC_BUILDINGACTION;
int FUC_HUMANMOVE;
int FUC_HUMANACTION;
int FUC_HUMANBUILD;

///********** 动作返回编号及action错误码 **********/
///*
// * 0是成功
// * -1是SN不存在
// * -2是Action不存在
// * -3是指定位置超界
// * -4是obSN不存在
// * -5是BuildingNum不存在
// * -6是资源不足
// */
//int ACTION_SUCCESS;
//int ACTION_INVALID_SN;
//int ACTION_INVALID_ACTION;
//int ACTION_INVALID_LOCATION;
//int ACTION_INVALID_OBSN;
//int ACTION_INVALID_BUILDINGNUM;
//int ACTION_INVALID_RESOURCE;
//int ACTION_INVALID_UPGRADE_TIME;
// //控制对象被删除
//int ACTION_INVALID_NULLWORKER;
////目标对象已被删除
//int ACTION_INVALID_NULLGOALOBJECT;

////对象已有必须手动取消的任务，不空闲
//int ACTION_INVALID_ISNTFREE;

////BuildingAction
////建筑还在建造过程中
//int ACTION_INVALID_BUILDACT_NEEDBUILT;

////建筑行动未解锁，或该行动只能进行有限次且已达上限
//int ACTION_INVALID_BUILDACT_LOCK;
////造人行动，已达人口上限
//int ACTION_INVALID_BUILDACT_MAXHUMAN;

////HumanMove

////HumanAction
////修理建筑，建筑不需要修理
//int ACTION_INVALID_HUMANACTION_BUILDNOTNEEDFIX;
//int ACTION_INVALID_HUMANACTION_BUILD2RESOURCENOMATCH;

////HumanBuild
////建筑位置有高度差
//int ACTION_INVALID_HUMANBUILD_DIFFERENTHIGH;
////建筑位置加上建筑宽度，超出边界
//int ACTION_INVALID_HUMANBUILD_OVERBORDER;
////建筑位置未被探索
//int ACTION_INVALID_HUMANBUILD_UNEXPLORE;
////建筑位置与其他物体有重叠冲突
//int ACTION_INVALID_HUMANBUILD_OVERLAP;
////建筑未解锁，未达成建筑条件
//int ACTION_INVALID_HUMANBUILD_LOCK;
////距离相距太远
//int ACTION_INVALID_DISTANCE_FAR;
////携带人物满了
//int ACTION_INVALID_FULLY_LOAD;
////建筑放置的位置不合适
//int ACTION_INVALID_POSITION_NOT_FIT;
///********** 资源种类 **********/
///*
// * 如表 十进制位代表大的分类 个位代表他在大类中的具体编号
// */
//int RESOURCE_EMPTY;
//int RESOURCE_BUSH;
//int RESOURCE_TREE;
//int RESOURCE_STONE;
//int RESOURCE_GAZELLE;
//int RESOURCE_ELEPHANT;
//int RESOURCE_LION;
//int RESOURCE_GOLD;
//int RESOURCE_FISH;

// /********** 时代编号 **********/
//int CIVILIZATION_STONEAGE;
//int CIVILIZATION_TOOLAGE;
//int CIVILIZATION_BRONZEAGE;
//int CIVILIZATION_IRONAGE;


//int ACT_STATUS_ENABLED;
//int ACT_STATUS_ANIME;
//int ACT_STATUS_DISABLED;


/////********** 人物动作命名 **********/
string ACT_CREATEFARMER_NAME;
string ACT_UPGRADE_AGE_NAME;
string ACT_UPGRADE_TOWERBUILD_NAME;
string ACT_UPGRADE_WOOD_NAME;
string ACT_UPGRADE_STONE_NAME;
string ACT_UPGRADE_GOLD_NAME;
string ACT_UPGRADE_FARM_NAME;
string ACT_STOP_NAME;
string ACT_BUILD_NAME;
string ACT_BUILD_HOUSE_NAME;
string ACT_BUILD_GRANARY_NAME;
string ACT_BUILD_STOCK_NAME;
string ACT_BUILD_CANCEL_NAME;
string ACT_BUILD_FARM_NAME;
string ACT_BUILD_MARKET_NAME;
string ACT_BUILD_ARROWTOWER_NAME;
string ACT_BUILD_DOCK_NAME;
string ACT_SHIP_LAY_NAME;

string ACT_ARMYCAMP_CREATE_CLUBMAN_NAME;
string ACT_ARMYCAMP_CREATE_SLINGER_NAME;
string ACT_ARMYCAMP_UPGRADE_CLUBMAN_NAME;
string ACT_BUILD_ARMYCAMP_NAME;
string ACT_BUILD_RANGE_NAME;
string ACT_BUILD_STABLE_NAME;
string ACT_RANGE_CREATE_BOWMAN_NAME;
string ACT_RESEARCH_WALL_NAME;
string ACT_STABLE_CREATE_SCOUT_NAME;
string ACT_STOCK_UPGRADE_DEFENSE_ARCHER_NAME;
string ACT_STOCK_UPGRADE_DEFENSE_INFANTRY_NAME;
string ACT_STOCK_UPGRADE_DEFENSE_RIDER_NAME;
string ACT_STOCK_UPGRADE_USETOOL_NAME;
string ACT_DOCK_CREATE_SAILING_NAME;
string ACT_DOCK_CREATE_WOOD_BOAT_NAME;
string ACT_DOCK_CREATE_SHIP_NAME;
string ACT_NULL_NAME;

//int ACT_WINDOW_NUM_FREE;

//int ACT_NULL;
//int ACT_CREATEFARMER;
//int ACT_UPGRADE_AGE;
//int ACT_UPGRADE_TOWERBUILD;
//int ACT_UPGRADE_WOOD;
//int ACT_UPGRADE_STONE;
//int ACT_UPGRADE_FARM;
//int ACT_UPGRADE_GOLD;
//int ACT_STOCK_UPGRADE_USETOOL;
//int ACT_STOCK_UPGRADE_DEFENSE_INFANTRY;
//int ACT_STOCK_UPGRADE_DEFENSE_ARCHER;
//int ACT_STOCK_UPGRADE_DEFENSE_RIDER;
//int ACT_ARMYCAMP_CREATE_CLUBMAN;
//int ACT_ARMYCAMP_CREATE_SLINGER;
//int ACT_ARMYCAMP_UPGRADE_CLUBMAN;
//int ACT_RANGE_CREATE_BOWMAN;
//int ACT_STABLE_CREATE_SCOUT;
//int ACT_RESEARCH_WALL;
//int ACT_DOCK_CREATE_SAILING;
//int ACT_DOCK_CREATE_WOOD_BOAT;
//int ACT_DOCK_CREATE_SHIP;

//int ACT_BUILD;
//int ACT_BUILD_HOUSE;
//int ACT_BUILD_GRANARY;
//int ACT_BUILD_STOCK;
//int ACT_BUILD_CANCEL;
//int ACT_BUILD_FARM;
//int ACT_BUILD_MARKET;
//int ACT_BUILD_ARROWTOWER;
//int ACT_BUILD_ARMYCAMP;
//int ACT_BUILD_RANGE;
//int ACT_BUILD_STABLE;
//int ACT_BUILD_DOCK;

//int ACT_SHIP_LAY;

//int ACT_STOP;


///********** 对象视野 **********/
////建筑的视野搬至建筑属性相关
//int VISION_FARMER;
//int VISION_GAZELLE;
//int VISION_LION;
//int VISION_ELEPHANT;


///********** 地基编号 **********/
//int FOUNDATION_SMALL;
//int FOUNDATION_MIDDLE;
//int FOUNDATION_BIG;
//int FOUNDATION_HOUSE;
//int FOUNDATION_BLOCK;

//int SORT_COORDINATE;
//int SORT_BUILDING;
//int SORT_STATICRES;
//int SORT_HUMAN;
//int SORT_FARMER;
//int SORT_ANIMAL;
//int SORT_TREEFOREST;
//int SORT_MISSILE;
//int SORT_Building_Resource;
//int SORT_ARMY;

//int PRODUCTSORT_WOOD;
//int PRODUCTSORT_GRANARYFOOD;
//int PRODUCTSORT_STONE;
//int PRODUCTSORT_GOLD;
//int PRODUCTSORT_STOCKFOOD;

//int OBJECTTYPE_BLOCK;
//int OBJECTTYPE_COORDINATE;

//int ANIMAL_STATE_IDLE;
//int ANIMAL_STATE_ROAMING;
//int ANIMAL_STATE_FLEEING;
//int ANIMAL_STATE_CHASING;
//int ANIMAL_STATE_ATTACKING;

//int BLOOD_TREE;
//int BLOOD_GAZELLE;
//int BLOOD_ELEPHANT;
//int BLOOD_LION;
//int BLOOD_FARMER;
//int BLOOD_FOREST;

//double SPEED_ELEPHANT;

//int CNT_TREE;
//int CNT_GAZELLE;
//int CNT_ELEPHANT;
//int CNT_LION;
//int CNT_FARM;
//int CNT_UPGRADEFARM;
//int CNT_BUSH;
//int CNT_STONE;
//int CNT_GOLDORE;
//int CNT_FOREST;
//int CNT_FISH;

//int ANIMAL_ATTACKRANGE_LION;
//int ANIMAL_ATTACKRANGE_ELEPHANT;

//int FARMER_VILLAGER;
//int FARMER_LUMBER;
//int FARMER_GATHERER;
//int FARMER_MINER;
//int FARMER_HUNTER;
//int FARMER_FARMER;
//int FARMER_WORKER;
//int FARMER_FISHER;

//int FARMER_CARRYLIMIT_WOOD;
//int FARMER_CARRYLIMIT_FOOD;
//int FARMER_CARRYLIMIT_STONE;
//int FARMER_CARRYLIMIT_GOLD;
//int FARMER_CARRYLIMIT_UPGRADEWOOD;
//int FARMER_CARRYLIMIT_UPGRADEFOOD;
//int FARMER_CARRYLIMIT_UPGRADESTONE;
//int FARMER_CARRYLIMIT_UPGRADEGOLD;

//int FARMER_HUNTRANGE;
//int FARMER_UPGRADEHUNTRANGE;

//double FARMER_GATHERSPEED_WOOD;
//double FARMER_GATHERSPEED_FOOD;
//double FARMER_GATHERSPEED_STONE;
//double FARMER_GATHERSPEED_GOLD;
//double FARMER_CONSTRUCTSPEED;

//int MOVEOBJECT_STATE_STAND;
//int MOVEOBJECT_STATE_WALK;
//int MOVEOBJECT_STATE_ATTACK;
//int MOVEOBJECT_STATE_DIE;

//int MOVEOBJECT_STATE_WORK;
//int MOVEOBJECT_STATE_RUN;
//int ATTACKVALUE_FARMER;

//int HOUSE_HUMAN_NUM;
//int MAX_HUMAN_NUM;
////鼠标结构体中对应鼠标点击事件
//int NULL_MOUSEEVENT;
//int LEFT_PRESS;
//int RIGHT_PRESS;


///********** 同Class中图像资源种类数 **********/
//int NUMBER_MISSILE;


///********** Core静态表 **********/
////####关系事件名称
//int CoreEven_JustMoveTo;
//int CoreEven_CreatBuilding;
//int CoreEven_Gather;
//int CoreEven_Attacking;
//int CoreEven_FixBuilding;
//int CoreEven_BuildingAct;
//int CoreEven_MissileAttack;
//int CoreEven_Transport;
//int CoreEven_UnLoad;
////####对一个关系事件，细节关系的最大数量
//int CoreDetailLinkMaxNum;
////####细节环节名称
//int CoreDetail_NormalEnd;
//int CoreDetail_AbsoluteEnd;
//int CoreDetail_JumpPhase;
//int CoreDetail_Move;
//int CoreDetail_Attack;
//int CoreDetail_Gather;
//int CoreDetail_ResourceIn;
//int CoreDetail_Transport;
//int CoreDetail_UpdateRatio;
//int CoreDetail_Unload;

///********** Core关系函数的可变操作指令 **********/
//int OPERATECON_DEFAULT;
//int OPERATECHANGE;

////####距离判定
//int OPERATECON_NEAR_ABSOLUTE;
//int OPERATECON_MOVEALTER;
//int OPERATECON_NEAR_ATTACK;
//int OPERATECON_NEAR_WORK;
//int OPERATECON_NEAR_MISSILE;
//int OPERATECON_NEAR_ATTACK_MOVE;
//int OPERATECON_NEAR_UNLOAD;
//int OPERATECON_NEAR_TRANSPORT;

//int OPERATECON_NEARALTER_ABSOLUTE;
//int OPERATECON_NEARALTER_WORK;

////####指定对象
//int OPERATECON_OBJECT1;
//int OPERATECON_OBJECT2;

//int OPERATECON_TIMES;
//int OPERATE_TIMEMAX;

//int OPERATECON_TIMES_USELESSACT_MOVE;


/********** 占地边长-块坐标常量 **********/
//int SIZELEN_SINGEL;
//int SIZELEN_SMALL;
//int SIZELEN_MIDDLE;
//int SIZELEN_BIG;


///********** animal友好度 **********/
//int FRIENDLY_NULL;
//int FRIENDLY_FRI;
//int FRIENDLY_ENEMY;
//int FRIENDLY_FENCY;

///********** 兵种状态 **********/
//int ARMY_STATE_DEFAULT;
//int ARMY_STATE_AROUND;
//int ARMY_STATE_DEFENSE;
//int ARMY_STATE_ATTACK;

/********** 巡逻状态 **********/
//int PATROL_STATE_IDLE;
//int PATROL_STATE_PATROLLING;
//int PATROL_STATE_CHASING;
//int PATROL_STATE_RETURNING;

//初始化配置函数
void configInit(){
    //
    QFile configFile("config.json");
    if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "无法打开文件：" << configFile.errorString();
            exit(0);
        }
    QByteArray jsonData = configFile.readAll();
    configFile.close(); // 读取完成后关闭文件
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "JSON解析失败：" << parseError.errorString();
        exit(0);
    }
    //
    QJsonObject  root=jsonDoc.object();
    GAME_WIDTH=root["GAME_WIDTH"].toInt();
    GAME_HEIGHT=root["GAME_HEIGHT"].toInt();
    GAME_VERSION=root["GAME_VERSION"].toString().toStdString();
    GAME_TITLE=root["GAME_TITLE"].toString().toStdString();
    //MAPFILE_SUFFIX=root["MAPFILE_SUFFIX"].toString().toStdString();
    GOLD=root["GOLD"].toInt();
    STONE=root["STONE"].toInt();
    //gen5=root["gen5"].toDouble();
    NOWPLAYERREPRESENT=root["NOWPLAYERREPRESENT"].toInt();
    INITIAL_FREQUENCY=root["INITIAL_FREQUENCY"].toInt();
    MAP_EXPLORE=root["MAP_EXPLORE"].toBool();
    MAP_VISIABLE=root["MAP_VISIABLE"].toBool();
    MEMORYROW=root["MEMORYROW"].toInt();
    MEMORYCOLUMN=root["MEMORYCOLUMN"].toInt();
    GAMEWIDGET_WIDTH=root["GAMEWIDGET_WIDTH"].toInt();
    GAMEWIDGET_HEIGHT=root["GAMEWIDGET_HEIGHT"].toInt();
    UNLOAD_RADIAN=root["UNLOAD_RADIAN"].toInt();
    //MAP_L=root["MAP_L"].toInt();
    //MAP_U=root["MAP_U"].toInt();
    //GENERATE_L=root["GENERATE_L"].toInt();
    //GENERATE_U=root["GENERATE_U"].toInt();
    GAMEWIDGET_MIDBLOCKL=root["GAMEWIDGET_MIDBLOCKL"].toInt();
    GAMEWIDGET_MIDBLOCKU=root["GAMEWIDGET_MIDBLOCKU"].toInt();
    FRAMES_PER_SECOND=root["FRAMES_PER_SECOND"].toInt();
    GENERATELANDFORMS_NUM=root["GENERATELANDFORMS_NUM"].toInt();
    KEY_UP=root["KEY_UP"].toInt();
    KEY_LEFT=root["KEY_LEFT"].toInt();
    KEY_DOWN=root["KEY_DOWN"].toInt();
    KEY_RIGHT=root["KEY_RIGHT"].toInt();
    //BLOCK_COUNT=root["BLOCK_COUNT"].toInt();
    OPTION_MUSIC=root["OPTION_MUSIC"].toBool();
    OPTION_SOUND=root["OPTION_SOUND"].toBool();
    OPTION_SELECT=root["OPTION_SELECT"].toBool();
    OPTION_LINE=root["OPTION_LINE"].toBool();
    OPTION_POS=root["OPTION_POS"].toBool();
    OPTION_OVERLAP=root["OPTION_OVERLAP"].toBool();
    for(auto ele:root["sound_work"].toArray()){
        sound_work.push_back(ele.toString().toStdString());
    }

    /********** 地图块种类 **********/
    /* L0边为右上角，L0到L3顺时针排列 */
    /* A0边为上方角，A0到A3顺时针排列 */
//    MAPTYPE_EMPTY=root["MAPTYPE_EMPTY"].toInt();
//    MAPTYPE_FLAT=root["MAPTYPE_FLAT"].toInt();
    MAPTYPE_A2_UPTOU=root["MAPTYPE_A2_UPTOU"].toInt();
    MAPTYPE_A0_DOWNTOD=root["MAPTYPE_A0_DOWNTOD"].toInt();
    MAPTYPE_L1_UPTOLU=root["MAPTYPE_L1_UPTOLU"].toInt();
    MAPTYPE_L3_DOWNTORD=root["MAPTYPE_L3_DOWNTORD"].toInt();
    MAPTYPE_L0_DOWNTOLD=root["MAPTYPE_L0_DOWNTOLD"].toInt();
    MAPTYPE_L2_UPTORU=root["MAPTYPE_L2_UPTORU"].toInt();
    MAPTYPE_A1_UPTOL=root["MAPTYPE_A1_UPTOL"].toInt();
    MAPTYPE_A3_UPTOR=root["MAPTYPE_A3_UPTOR"].toInt();
    MAPTYPE_A1_DOWNTOL=root["MAPTYPE_A1_DOWNTOL"].toInt();
    MAPTYPE_A3_DOWNTOR=root["MAPTYPE_A3_DOWNTOR"].toInt();
    MAPTYPE_L0_UPTOLD=root["MAPTYPE_L0_UPTOLD"].toInt();
    MAPTYPE_L2_DOWNTORU=root["MAPTYPE_L2_DOWNTORU"].toInt();
    MAPTYPE_L3_UPTORD=root["MAPTYPE_L3_UPTORD"].toInt();
    MAPTYPE_L1_DOWNTOLU=root["MAPTYPE_L1_DOWNTOLU"].toInt();
    MAPTYPE_A2_DOWNTOU=root["MAPTYPE_A2_DOWNTOU"].toInt();
    MAPTYPE_A0_UPTOD=root["MAPTYPE_A0_UPTOD"].toInt();
//    MAPTYPE_OCEAN=root["MAPTYPE_OCEAN"].toInt();

    /********** 地图块高度 **********/
    MAPHEIGHT_FLAT=root["MAPHEIGHT_FLAT"].toInt();
    MAPHEIGHT_MAX=root["MAPHEIGHT_MAX"].toInt();
    MAPHEIGHT_OCEAN=root["MAPHEIGHT_OCEAN"].toInt();
    MAPHEIGHT_PERCENT=root["MAPHEIGHT_PERCENT"].toInt();
    MAPHEIGHT_OPTCOUNT=root["MAPHEIGHT_OPTCOUNT"].toInt();
    CENTER_RADIUS=root["CENTER_RADIUS"].toInt();
    CENTER_DEVIATION=root["int CENTER_DEVIATION"].toInt();

    /********** 地图块样式 **********/
    MAPPATTERN_UNKNOWN=root["MAPPATTERN_UNKNOWN"].toInt();
    MAPPATTERN_GRASS=root["MAPPATTERN_GRASS"].toInt();
    MAPPATTERN_DESERT=root["MAPPATTERN_DESERT"].toInt();
    MAPPATTERN_OCEAN=root["MAPPATTERN_OCEAN"].toInt();
    MAPPATTERN_SHOAL=root["MAPPATTERN_SHOAL"].toInt();

    /********** 地图块绘制偏移量 **********/
    DRAW_OFFSET=root["DRAW_OFFSEF"].toInt();

    /********** DebugText栏颜色 **********/


    /********** 建筑火焰种类 **********/
    BUILDING_FIRE_SMALL=root["BUILDING_FIRE_SMALL"].toInt();
    BUILDING_FIRE_MIDDLE=root["BUILDING_FIRE_MIDDLE"].toInt();
    BUILDING_FIRE_BIG=root["BUILDING_FIRE_BIG"].toInt();

    BUILDING_BLOOD_FIRE_SMALL=root["BUILDING_BLOOD_FIRE_SMALL"].toDouble();
    BUILDING_BLOOD_FIRE_MIDDLE=root["BUILDING_BLOOD_FIRE_MIDDLE"].toDouble();
    BUILDING_BLOOD_FIRE_BIG=root["BUILDING_BLOOD_FIRE_BIG"].toDouble();


    /********** 建筑种类 **********/
    //BUILDING_TYPE_MAXNUM=root["BUILDING_TYPE_MAXNUM"].toInt();
//    BUILDING_HOME=root["BUILDING_HOME"].toInt();
//    BUILDING_GRANARY=root["BUILDING_GRANARY"].toInt();
//    BUILDING_CENTER=root["BUILDING_CENTER"].toInt();
//    BUILDING_STOCK=root["BUILDING_STOCK"].toInt();

//    BUILDING_MARKET=root["BUILDING_MARKET"].toInt();
//    BUILDING_ARROWTOWER=root["BUILDING_ARROWTOWER"].toInt();
//    BUILDING_ARROWTOWERPOSITION=root["BUILDING_ARROWTOWERPOSITION"].toInt();

//    BUILDING_ARMYCAMP=root["BUILDING_ARMYCAMP"].toInt();
//    BUILDING_STABLE=root["BUILDING_STABLE"].toInt();
//    BUILDING_RANGE=root["BUILDING_RANGE"].toInt();
//    BUILDING_DOCK=root["BUILDING_DOCK"].toInt();
//    BUILDING_WALL=root["BUILDING_WALL"].toInt();

    //Building_Resource类（产资源建筑）
//    BUILDING_FARM=root["BUILDING_FARM"].toInt();

//    /********** 建筑状态 **********/
//    BUILDING_FREE=root["BUILDING_FREE"].toInt(); // 建筑状态为空闲（无工作）时用0表示


    /********** 建筑动作 **********/
    //市镇中心
//    BUILDING_CENTER_CREATEFARMER=root["BUILDING_CENTER_CREATEFARMER"].toInt();
//    BUILDING_CENTER_UPGRADE=root["BUILDING_CENTER_UPGRADE"].toInt();
//    //谷仓
//    BUILDING_GRANARY_ARROWTOWER=root["BUILDING_GRANARY_ARROWTOWER"].toInt();
//    BUILDING_GRANARY_WALL=root["BUILDING_GRANARY_WALL"].toInt();
//    //市场
//    BUILDING_MARKET_WOOD_UPGRADE=root["BUILDING_MARKET_WOOD_UPGRADE"].toInt();
//    BUILDING_MARKET_STONE_UPGRADE=root["BUILDING_MARKET_STONE_UPGRADE"].toInt();
//    BUILDING_MARKET_FARM_UPGRADE=root["BUILDING_MARKET_FARM_UPGRADE"].toInt();
//    BUILDING_MARKET_GOLD_UPGRADE=root["BUILDING_MARKET_GOLD_UPGRADE"].toInt();

//    //仓库
//    BUILDING_STOCK_UPGRADE_USETOOL=root["BUILDING_STOCK_UPGRADE_USETOOL"].toInt();
//    BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY=root["BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY"].toInt();
//    BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER=root["BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER"].toInt();
//    BUILDING_STOCK_UPGRADE_DEFENSE_RIDER=root["BUILDING_STOCK_UPGRADE_DEFENSE_RIDER"].toInt();

//    //军营
//    BUILDING_ARMYCAMP_CREATE_CLUBMAN=root["BUILDING_ARMYCAMP_CREATE_CLUBMAN"].toInt();
//    BUILDING_ARMYCAMP_CREATE_SLINGER=root["BUILDING_ARMYCAMP_CREATE_SLINGER"].toInt();
//    BUILDING_ARMYCAMP_UPGRADE_CLUBMAN=root["BUILDING_ARMYCAMP_UPGRADE_CLUBMAN"].toInt();

//    //靶场
//    BUILDING_RANGE_CREATE_BOWMAN=root["BUILDING_RANGE_CREATE_BOWMAN"].toInt();

//    //马厩
//    BUILDING_STABLE_CREATE_SCOUT=root["BUILDING_STABLE_CREATE_SCOUT"].toInt();

//    BUILDING_CANCEL=root["BUILDING_CANCEL"].toInt();
//    //船坞
//    BUILDING_DOCK_CREATE_SAILING=root["BUILDING_DOCK_CREATE_SAILING"].toInt();
//    BUILDING_DOCK_CREATE_WOOD_BOAT=root["BUILDING_DOCK_CREATE_WOOD_BOAT"].toInt();
//    BUILDING_DOCK_CREATE_SHIP=root["BUILDING_DOCK_CREATE_SHIP"].toInt();
    /********** 建筑动作命名 **********/
    BUILDING_GRANARY_ARROWTOWER_NAME=root["BUILDING_GRANARY_ARROWTOWER_NAME"].toString().toStdString();
    BUILDING_CENTER_CREATEFARMER_NAME=root["BUILDING_CENTER_CREATEFARMER_NAME"].toString().toStdString();
    BUILDING_CENTER_UPGRADE_NAME=root["BUILDING_CENTER_UPGRADE_NAME"].toString().toStdString();
    BUILDING_MARKET_WOOD_UPGRADE_NAME=root["BUILDING_MARKET_WOOD_UPGRADE_NAME"].toString().toStdString();
    BUILDING_MARKET_STONE_UPGRADE_NAME=root["BUILDING_MARKET_STONE_UPGRADE_NAME"].toString().toStdString();
    BUILDING_MARKET_GOLD_UPGRADE_NAME=root["BUILDING_MARKET_GOLD_UPGRADE_NAME"].toString().toStdString();
    BUILDING_MARKET_FARM_UPGRADE_NAME=root["BUILDING_MARKET_FARM_UPGRADE_NAME"].toString().toStdString();
    BUILDING_ACTION_CANCEL_NAME=root["BUILDING_ACTION_CANCEL_NAME"].toString().toStdString();

    ///********** Coordinate子类中 Num值实际指代种类 **********/
//    ////资源
//    NUM_STATICRES_Bush=root["NUM_STATICRES_Bush"].toInt();
//    NUM_STATICRES_Stone=root["NUM_STATICRES_Stone"].toInt();
//    NUM_STATICRES_GoldOre=root["NUM_STATICRES_GoldOre"].toInt();
//    NUM_STATICRES_Fish=root["NUM_STATICRES_Fish"].toInt();
//    ////动物
//    TREE=root["TREE"].toInt();
//    GAZELLE=root["GAZELLE"].toInt();
//    ELEPHANT=root["ELEPHANT"].toInt();
//    LION=root["LION"].toInt();
//    FOREST=root["FOREST"].toInt();

//    ///********** 人物状态 **********/
//    ///*
//    // * 0代表为空闲状态
//    // * 1代表为正在移动状态（无目标对象）
//    // * 2代表为正在工作状态
//    // * 3代表为正在攻击状态
//    // */
    HUMAN_STATE_IDLE=root["HUMAN_STATE_IDLE"].toInt();
    HUMAN_STATE_WALKING=root["HUMAN_STATE_WALKING"].toInt();
    HUMAN_STATE_WORKING=root["HUMAN_STATE_WORKING"].toInt();
    HUMAN_STATE_ATTACKING=root["HUMAN_STATE_ATTACKING"].toInt();

//    // /********** 人物手持资源种类 **********/
    /*HUMAN_WOOD=root["HUMAN_WOOD"].toInt();
    HUMAN_STOCKFOOD=root["HUMAN_STOCKFOOD"].toInt();
    HUMAN_STONE=root["HUMAN_STONE"].toInt();
    HUMAN_GOLD=root["HUMAN_GOLD"].toInt();
    HUMAN_GRANARYFOOD=root["HUMAN_GRANARYFOOD"].toInt();
    HUMAN_DOCKFOOD=root["HUMAN_DOCKFOOD"].toInt();*/ //也就是鱼肉
//    ///********** AI函数 **********/
    ////函数编号
    FUC_BUILDINGACTION=root["FUC_BUILDINGACTION"].toInt();
    FUC_HUMANMOVE=root["FUC_HUMANMOVE"].toInt();
    FUC_HUMANACTION=root["FUC_HUMANACTION"].toInt();
    FUC_HUMANBUILD=root["FUC_HUMANBUILD"].toInt();

//    ///********** 动作返回编号及action错误码 **********/
//    ///*
//    // * 0是成功
//    // * -1是SN不存在
//    // * -2是Action不存在
//    // * -3是指定位置超界
//    // * -4是obSN不存在
//    // * -5是BuildingNum不存在
//    // * -6是资源不足
//    // */
//    ACTION_SUCCESS=root["ACTION_SUCCESS"].toInt();
//    ACTION_INVALID_SN=root["ACTION_INVALID_SN"].toInt();
//    ACTION_INVALID_ACTION=root["ACTION_INVALID_ACTION"].toInt();
//    ACTION_INVALID_LOCATION=root["ACTION_INVALID_LOCATION"].toInt();
//    ACTION_INVALID_OBSN=root["ACTION_INVALID_OBSN"].toInt();
//    ACTION_INVALID_BUILDINGNUM=root["ACTION_INVALID_BUILDINGNUM"].toInt();
//    ACTION_INVALID_RESOURCE=root["ACTION_INVALID_RESOURCE"].toInt();
//    ACTION_INVALID_UPGRADE_TIME=root["ACTION_INVALID_UPGRADE_TIME"].toInt();
//     //控制对象被删除
//    ACTION_INVALID_NULLWORKER=root["ACTION_INVALID_NULLWORKER"].toInt();
//    ////目标对象已被删除
//    ACTION_INVALID_NULLGOALOBJECT=root["ACTION_INVALID_NULLGOALOBJECT"].toInt();

//    ////对象已有必须手动取消的任务，不空闲
//    ACTION_INVALID_ISNTFREE=root["ACTION_INVALID_ISNTFREE"].toInt();

//    ////BuildingAction
//    ////建筑还在建造过程中
//    ACTION_INVALID_BUILDACT_NEEDBUILT=root["ACTION_INVALID_BUILDACT_NEEDBUILT"].toInt();

//    ////建筑行动未解锁，或该行动只能进行有限次且已达上限
//    ACTION_INVALID_BUILDACT_LOCK=root["ACTION_INVALID_BUILDACT_LOCK"].toInt();
//    ////造人行动，已达人口上限
//    ACTION_INVALID_BUILDACT_MAXHUMAN=root["ACTION_INVALID_BUILDACT_MAXHUMAN"].toInt();

//    ////HumanMove

//    ////HumanAction
//    ////修理建筑，建筑不需要修理
//    ACTION_INVALID_HUMANACTION_BUILDNOTNEEDFIX=root["ACTION_INVALID_HUMANACTION_BUILDNOTNEEDFIX"].toInt();
//    ACTION_INVALID_HUMANACTION_BUILD2RESOURCENOMATCH=root["ACTION_INVALID_HUMANACTION_BUILD2RESOURCENOMATCH"].toInt();

//    ////HumanBuild
//    ////建筑位置有高度差
//    ACTION_INVALID_HUMANBUILD_DIFFERENTHIGH=root["ACTION_INVALID_HUMANBUILD_DIFFERENTHIGH"].toInt();
//    ////建筑位置加上建筑宽度，超出边界
//    ACTION_INVALID_HUMANBUILD_OVERBORDER=root["ACTION_INVALID_HUMANBUILD_OVERBORDER"].toInt();
//    ////建筑位置未被探索
//    ACTION_INVALID_HUMANBUILD_UNEXPLORE=root["ACTION_INVALID_HUMANBUILD_UNEXPLORE"].toInt();
//    ////建筑位置与其他物体有重叠冲突
//    ACTION_INVALID_HUMANBUILD_OVERLAP=root["ACTION_INVALID_HUMANBUILD_OVERLAP"].toInt();
//    ////建筑未解锁，未达成建筑条件
//    ACTION_INVALID_HUMANBUILD_LOCK=root["ACTION_INVALID_HUMANBUILD_LOCK"].toInt();
//    ////距离相距太远
//    ACTION_INVALID_DISTANCE_FAR=root["ACTION_INVALID_DISTANCE_FAR"].toInt();
//    ////携带人物满了
//    ACTION_INVALID_FULLY_LOAD=root["ACTION_INVALID_FULLY_LOAD"].toInt();
//    ////建筑放置的位置不合适
//    ACTION_INVALID_POSITION_NOT_FIT=root["ACTION_INVALID_POSITION_NOT_FIT"].toInt();
//    ///********** 资源种类 **********/
//    ///*
//    // * 如表 十进制位代表大的分类 个位代表他在大类中的具体编号
//    // */
//    RESOURCE_EMPTY=root["RESOURCE_EMPTY"].toInt();
//    RESOURCE_BUSH=root["RESOURCE_BUSH"].toInt();
//    RESOURCE_TREE=root["RESOURCE_TREE"].toInt();
//    RESOURCE_STONE=root["RESOURCE_STONE"].toInt();
//    RESOURCE_GAZELLE=root["RESOURCE_GAZELLE"].toInt();
//    RESOURCE_ELEPHANT=root["RESOURCE_ELEPHANT"].toInt();
//    RESOURCE_LION=root["RESOURCE_LION"].toInt();
//    RESOURCE_GOLD=root["RESOURCE_GOLD"].toInt();
//    RESOURCE_FISH=root["RESOURCE_FISH"].toInt();

//    // /********** 时代编号 **********/
//    CIVILIZATION_STONEAGE=root["CIVILIZATION_STONEAGE"].toInt();
//    CIVILIZATION_TOOLAGE=root["CIVILIZATION_TOOLAGE"].toInt();
//    CIVILIZATION_BRONZEAGE=root["CIVILIZATION_BRONZEAGE"].toInt();
//    CIVILIZATION_IRONAGE=root["CIVILIZATION_IRONAGE"].toInt();


//    ACT_STATUS_ENABLED=root["ACT_STATUS_ENABLED"].toInt();
//    ACT_STATUS_ANIME=root["ACT_STATUS_ANIME"].toInt();
//    ACT_STATUS_DISABLED=root["ACT_STATUS_DISABLED"].toInt();


    ///********** 人物动作命名 **********/
    ACT_CREATEFARMER_NAME=root["ACT_CREATEFARMER_NAME"].toString().toStdString();
    ACT_UPGRADE_AGE_NAME=root["ACT_UPGRADE_AGE_NAME"].toString().toStdString();
    ACT_UPGRADE_TOWERBUILD_NAME=root["ACT_UPGRADE_TOWERBUILD_NAME"].toString().toStdString();
    ACT_UPGRADE_WOOD_NAME=root["ACT_UPGRADE_WOOD_NAME"].toString().toStdString();
    ACT_UPGRADE_STONE_NAME=root["ACT_UPGRADE_STONE_NAME"].toString().toStdString();
    ACT_UPGRADE_GOLD_NAME=root["ACT_UPGRADE_GOLD_NAME"].toString().toStdString();
    ACT_UPGRADE_FARM_NAME=root["ACT_UPGRADE_FARM_NAME"].toString().toStdString();
    ACT_STOP_NAME=root["ACT_STOP_NAME"].toString().toStdString();
    ACT_BUILD_NAME=root["ACT_BUILD_NAME"].toString().toStdString();
    ACT_BUILD_HOUSE_NAME=root["ACT_BUILD_HOUSE_NAME"].toString().toStdString();
    ACT_BUILD_GRANARY_NAME=root["ACT_BUILD_GRANARY_NAME"].toString().toStdString();
    ACT_BUILD_STOCK_NAME=root["ACT_BUILD_STOCK_NAME"].toString().toStdString();
    ACT_BUILD_CANCEL_NAME=root["ACT_BUILD_CANCEL_NAME"].toString().toStdString();
    ACT_BUILD_FARM_NAME=root["ACT_BUILD_FARM_NAME"].toString().toStdString();
    ACT_BUILD_MARKET_NAME=root["ACT_BUILD_MARKET_NAME"].toString().toStdString();
    ACT_BUILD_ARROWTOWER_NAME=root["ACT_BUILD_ARROWTOWER_NAME"].toString().toStdString();
    ACT_BUILD_DOCK_NAME=root["ACT_BUILD_DOCK_NAME"].toString().toStdString();
    ACT_SHIP_LAY_NAME=root["ACT_SHIP_LAY_NAME"].toString().toStdString();

    ACT_ARMYCAMP_CREATE_CLUBMAN_NAME=root["ACT_ARMYCAMP_CREATE_CLUBMAN_NAME"].toString().toStdString();
    ACT_ARMYCAMP_CREATE_SLINGER_NAME=root["ACT_ARMYCAMP_CREATE_SLINGER_NAME"].toString().toStdString();
    ACT_ARMYCAMP_UPGRADE_CLUBMAN_NAME=root["ACT_ARMYCAMP_UPGRADE_CLUBMAN_NAME"].toString().toStdString();
    ACT_BUILD_ARMYCAMP_NAME=root["ACT_BUILD_ARMYCAMP_NAME"].toString().toStdString();
    ACT_BUILD_RANGE_NAME=root["ACT_BUILD_RANGE_NAME"].toString().toStdString();
    ACT_BUILD_STABLE_NAME=root["ACT_BUILD_STABLE_NAME"].toString().toStdString();
    ACT_RANGE_CREATE_BOWMAN_NAME=root["ACT_RANGE_CREATE_BOWMAN_NAME"].toString().toStdString();
    ACT_RESEARCH_WALL_NAME=root["ACT_RESEARCH_WALL_NAME"].toString().toStdString();
    ACT_STABLE_CREATE_SCOUT_NAME=root["ACT_STABLE_CREATE_SCOUT_NAME"].toString().toStdString();
    ACT_STOCK_UPGRADE_DEFENSE_ARCHER_NAME=root["ACT_STOCK_UPGRADE_DEFENSE_ARCHER_NAME"].toString().toStdString();
    ACT_STOCK_UPGRADE_DEFENSE_INFANTRY_NAME=root["ACT_STOCK_UPGRADE_DEFENSE_INFANTRY_NAME"].toString().toStdString();
    ACT_STOCK_UPGRADE_DEFENSE_RIDER_NAME=root["ACT_STOCK_UPGRADE_DEFENSE_RIDER_NAME"].toString().toStdString();
    ACT_STOCK_UPGRADE_USETOOL_NAME=root["ACT_STOCK_UPGRADE_USETOOL_NAME"].toString().toStdString();
    ACT_DOCK_CREATE_SAILING_NAME=root["ACT_DOCK_CREATE_SAILING_NAME"].toString().toStdString();
    ACT_DOCK_CREATE_WOOD_BOAT_NAME=root["ACT_DOCK_CREATE_WOOD_BOAT_NAME"].toString().toStdString();
    ACT_DOCK_CREATE_SHIP_NAME=root["ACT_DOCK_CREATE_SHIP_NAME"].toString().toStdString();
    ACT_NULL_NAME=root["ACT_NULL_NAME"].toString().toStdString();

//    ACT_WINDOW_NUM_FREE=root["ACT_WINDOW_NUM_FREE"].toInt();

//    ACT_NULL=root["ACT_NULL"].toInt();
//    ACT_CREATEFARMER=root["ACT_CREATEFARMER"].toInt();
//    ACT_UPGRADE_AGE=root["ACT_UPGRADE_AGE"].toInt();
//    ACT_UPGRADE_TOWERBUILD=root["ACT_UPGRADE_TOWERBUILD"].toInt();
//    ACT_UPGRADE_WOOD=root["ACT_UPGRADE_WOOD"].toInt();
//    ACT_UPGRADE_STONE=root["ACT_UPGRADE_STONE"].toInt();
//    ACT_UPGRADE_FARM=root["ACT_UPGRADE_FARM"].toInt();
//    ACT_UPGRADE_GOLD=root["ACT_UPGRADE_GOLD"].toInt();
//    ACT_STOCK_UPGRADE_USETOOL=root["ACT_STOCK_UPGRADE_USETOOL"].toInt();
//    ACT_STOCK_UPGRADE_DEFENSE_INFANTRY=root["ACT_STOCK_UPGRADE_DEFENSE_INFANTRY"].toInt();
//    ACT_STOCK_UPGRADE_DEFENSE_ARCHER=root["ACT_STOCK_UPGRADE_DEFENSE_ARCHER"].toInt();
//    ACT_STOCK_UPGRADE_DEFENSE_RIDER=root["ACT_STOCK_UPGRADE_DEFENSE_RIDER"].toInt();
//    ACT_ARMYCAMP_CREATE_CLUBMAN=root["ACT_ARMYCAMP_CREATE_CLUBMAN"].toInt();
//    ACT_ARMYCAMP_CREATE_SLINGER=root["ACT_ARMYCAMP_CREATE_SLINGER"].toInt();
//    ACT_ARMYCAMP_UPGRADE_CLUBMAN=root["ACT_ARMYCAMP_UPGRADE_CLUBMAN"].toInt();
//    ACT_RANGE_CREATE_BOWMAN=root["ACT_RANGE_CREATE_BOWMAN"].toInt();
//    ACT_STABLE_CREATE_SCOUT=root["ACT_STABLE_CREATE_SCOUT"].toInt();
//    ACT_RESEARCH_WALL=root["ACT_RESEARCH_WALL"].toInt();
//    ACT_DOCK_CREATE_SAILING=root["ACT_DOCK_CREATE_SAILING"].toInt();
//    ACT_DOCK_CREATE_WOOD_BOAT=root["ACT_DOCK_CREATE_WOOD_BOAT"].toInt();
//    ACT_DOCK_CREATE_SHIP=root["ACT_DOCK_CREATE_SHIP"].toInt();

//    ACT_BUILD=root["ACT_BUILD"].toInt();
//    ACT_BUILD_HOUSE=root["ACT_BUILD_HOUSE"].toInt();
//    ACT_BUILD_GRANARY=root["ACT_BUILD_GRANARY"].toInt();
//    ACT_BUILD_STOCK=root["ACT_BUILD_STOCK"].toInt();
//    ACT_BUILD_CANCEL=root["ACT_BUILD_CANCEL"].toInt();
//    ACT_BUILD_FARM=root["ACT_BUILD_FARM"].toInt();
//    ACT_BUILD_MARKET=root["ACT_BUILD_MARKET"].toInt();
//    ACT_BUILD_ARROWTOWER=root["ACT_BUILD_ARROWTOWER"].toInt();
//    ACT_BUILD_ARMYCAMP=root["ACT_BUILD_ARMYCAMP"].toInt();
//    ACT_BUILD_RANGE=root["ACT_BUILD_RANGE"].toInt();
//    ACT_BUILD_STABLE=root["ACT_BUILD_STABLE"].toInt();
//    ACT_BUILD_DOCK=root["ACT_BUILD_DOCK"].toInt();

//    ACT_SHIP_LAY=root["ACT_SHIP_LAY"].toInt();

//    ACT_STOP=root["ACT_STOP"].toInt();


//    ///********** 对象视野 **********/
//    ////建筑的视野搬至建筑属性相关
//    VISION_FARMER=root["VISION_FARMER"].toInt();
//    VISION_GAZELLE=root["VISION_GAZELLE"].toInt();
//    VISION_LION=root["VISION_LION"].toInt();
//    VISION_ELEPHANT=root["VISION_ELEPHANT"].toInt();


//    ///********** 地基编号 **********/
//    FOUNDATION_SMALL=root["FOUNDATION_SMALL"].toInt();
//    FOUNDATION_MIDDLE=root["FOUNDATION_MIDDLE"].toInt();
//    FOUNDATION_BIG=root["FOUNDATION_BIG"].toInt();
//    FOUNDATION_HOUSE=root["FOUNDATION_HOUSE"].toInt();
//    FOUNDATION_BLOCK=root["FOUNDATION_BLOCK"].toInt();

//    SORT_COORDINATE=root["SORT_COORDINATE"].toInt();
//    SORT_BUILDING=root["SORT_BUILDING"].toInt();
//    SORT_STATICRES=root["SORT_STATICRES"].toInt();
//    SORT_HUMAN=root["SORT_HUMAN"].toInt();
//    SORT_FARMER=root["SORT_FARMER"].toInt();
//    SORT_ANIMAL=root["SORT_ANIMAL"].toInt();
//    SORT_TREEFOREST=root["SORT_TREEFOREST"].toInt();
//    SORT_MISSILE=root["SORT_MISSILE"].toInt();
//    SORT_Building_Resource=root["SORT_Building_Resource"].toInt();
//    SORT_ARMY=root["SORT_ARMY"].toInt();

//    PRODUCTSORT_WOOD=root["PRODUCTSORT_WOOD"].toInt();
//    PRODUCTSORT_GRANARYFOOD=root["PRODUCTSORT_GRANARYFOOD"].toInt();
//    PRODUCTSORT_STONE=root["PRODUCTSORT_STONE"].toInt();
//    PRODUCTSORT_GOLD=root["PRODUCTSORT_GOLD"].toInt();
//    PRODUCTSORT_STOCKFOOD=root["PRODUCTSORT_STOCKFOOD"].toInt();

//    OBJECTTYPE_BLOCK=root["OBJECTTYPE_BLOCK"].toInt();
//    OBJECTTYPE_COORDINATE=root["OBJECTTYPE_COORDINATE"].toInt();

//    ANIMAL_STATE_IDLE=root["ANIMAL_STATE_IDLE"].toInt();
//    ANIMAL_STATE_ROAMING=root["ANIMAL_STATE_ROAMING"].toInt();
//    ANIMAL_STATE_FLEEING=root["ANIMAL_STATE_FLEEING"].toInt();
//    ANIMAL_STATE_CHASING=root["ANIMAL_STATE_CHASING"].toInt();
//    ANIMAL_STATE_ATTACKING=root["ANIMAL_STATE_ATTACKING"].toInt();

//    BLOOD_TREE=root["BLOOD_TREE"].toInt();
//    BLOOD_GAZELLE=root["BLOOD_GAZELLE"].toInt();
//    BLOOD_ELEPHANT=root["BLOOD_ELEPHANT"].toInt();
//    BLOOD_LION=root["BLOOD_LION"].toInt();
//    BLOOD_FARMER=root["BLOOD_FARMER"].toInt();
//    BLOOD_FOREST=root["BLOOD_FOREST"].toInt();

//    SPEED_ELEPHANT=root["SPEED_ELEPHANT"].toDouble();

//    CNT_TREE=root["CNT_TREE"].toInt();
//    CNT_GAZELLE=root["CNT_GAZELLE"].toInt();
//    CNT_ELEPHANT=root["CNT_ELEPHANT"].toInt();
//    CNT_LION=root["CNT_LION"].toInt();
//    CNT_FARM=root["CNT_FARM"].toInt();
//    CNT_UPGRADEFARM=root["CNT_UPGRADEFARM"].toInt();
//    CNT_BUSH=root["CNT_BUSH"].toInt();
//    CNT_STONE=root["CNT_STONE"].toInt();
//    CNT_GOLDORE=root["CNT_GOLDORE"].toInt();
//    CNT_FOREST=root["CNT_FOREST"].toInt();
//    CNT_FISH=root["CNT_FISH"].toInt();

//    ANIMAL_ATTACKRANGE_LION=root["ANIMAL_ATTACKRANGE_LION"].toInt();
//    ANIMAL_ATTACKRANGE_ELEPHANT=root["ANIMAL_ATTACKRANGE_ELEPHANT"].toInt();

//    FARMER_VILLAGER=root["FARMER_VILLAGER"].toInt();
//    FARMER_LUMBER=root["FARMER_LUMBER"].toInt();
//    FARMER_GATHERER=root["FARMER_GATHERER"].toInt();
//    FARMER_MINER=root["FARMER_MINER"].toInt();
//    FARMER_HUNTER=root["FARMER_HUNTER"].toInt();
//    FARMER_FARMER=root["FARMER_FARMER"].toInt();
//    FARMER_WORKER=root["FARMER_WORKER"].toInt();
//    FARMER_FISHER=root["FARMER_FISHER"].toInt();

//    FARMER_CARRYLIMIT_WOOD=root["FARMER_CARRYLIMIT_WOOD"].toInt();
//    FARMER_CARRYLIMIT_FOOD=root["FARMER_CARRYLIMIT_FOOD"].toInt();
//    FARMER_CARRYLIMIT_STONE=root["FARMER_CARRYLIMIT_STONE"].toInt();
//    FARMER_CARRYLIMIT_GOLD=root["FARMER_CARRYLIMIT_GOLD"].toInt();
//    FARMER_CARRYLIMIT_UPGRADEWOOD=root["FARMER_CARRYLIMIT_UPGRADEWOOD"].toInt();
//    FARMER_CARRYLIMIT_UPGRADEFOOD=root["FARMER_CARRYLIMIT_UPGRADEFOOD"].toInt();
//    FARMER_CARRYLIMIT_UPGRADESTONE=root["FARMER_CARRYLIMIT_UPGRADESTONE"].toInt();
//    FARMER_CARRYLIMIT_UPGRADEGOLD=root["FARMER_CARRYLIMIT_UPGRADEGOLD"].toInt();

//    FARMER_HUNTRANGE=root["FARMER_HUNTRANGE"].toInt();
//    FARMER_UPGRADEHUNTRANGE=root["FARMER_UPGRADEHUNTRANGE"].toInt();

//    FARMER_GATHERSPEED_WOOD=root["FARMER_GATHERSPEED_WOOD"].toDouble();
//    FARMER_GATHERSPEED_FOOD=root["FARMER_GATHERSPEED_FOOD"].toDouble();
//    FARMER_GATHERSPEED_STONE=root["FARMER_GATHERSPEED_STONE"].toDouble();
//    FARMER_GATHERSPEED_GOLD=root["FARMER_GATHERSPEED_GOLD"].toDouble();
//    FARMER_CONSTRUCTSPEED=root["FARMER_CONSTRUCTSPEED"].toDouble();

//    MOVEOBJECT_STATE_STAND=root["MOVEOBJECT_STATE_STAND"].toInt();
//    MOVEOBJECT_STATE_WALK=root["MOVEOBJECT_STATE_WALK"].toInt();
//    MOVEOBJECT_STATE_ATTACK=root["MOVEOBJECT_STATE_ATTACK"].toInt();
//    MOVEOBJECT_STATE_DIE=root["MOVEOBJECT_STATE_DIE"].toInt();

//    MOVEOBJECT_STATE_WORK=root["MOVEOBJECT_STATE_WORK"].toInt();
//    MOVEOBJECT_STATE_RUN=root["MOVEOBJECT_STATE_RUN"].toInt();
//    ATTACKVALUE_FARMER=root["ATTACKVALUE_FARMER"].toInt();

//    HOUSE_HUMAN_NUM=root["HOUSE_HUMAN_NUM"].toInt();
//    MAX_HUMAN_NUM=root["MAX_HUMAN_NUM"].toInt();
//    ////鼠标结构体中对应鼠标点击事件
//    NULL_MOUSEEVENT=root["NULL_MOUSEEVENT"].toInt();
//    LEFT_PRESS=root["LEFT_PRESS"].toInt();
//    RIGHT_PRESS=root["RIGHT_PRESS"].toInt();


//    ///********** 同Class中图像资源种类数 **********/
//    NUMBER_MISSILE=root["NUMBER_MISSILE"].toInt();


//    ///********** Core静态表 **********/
//    ////####关系事件名称
//    CoreEven_JustMoveTo=root["CoreEven_JustMoveTo"].toInt();
//    CoreEven_CreatBuilding=root["CoreEven_CreatBuilding"].toInt();
//    CoreEven_Gather=root["CoreEven_Gather"].toInt();
//    CoreEven_Attacking=root["CoreEven_Attacking"].toInt();
//    CoreEven_FixBuilding=root["CoreEven_FixBuilding"].toInt();
//    CoreEven_BuildingAct=root["CoreEven_BuildingAct"].toInt();
//    CoreEven_MissileAttack=root["CoreEven_MissileAttack"].toInt();
//    CoreEven_Transport=root["CoreEven_Transport"].toInt();
//    CoreEven_UnLoad=root["CoreEven_UnLoad"].toInt();
//    ////####对一个关系事件，细节关系的最大数量
//    CoreDetailLinkMaxNum=root["CoreDetailLinkMaxNum"].toInt();
//    ////####细节环节名称
//    CoreDetail_NormalEnd=root["CoreDetail_NormalEnd"].toInt();
//    CoreDetail_AbsoluteEnd=root["CoreDetail_AbsoluteEnd"].toInt();
//    CoreDetail_JumpPhase=root["CoreDetail_JumpPhase"].toInt();
//    CoreDetail_Move=root["CoreDetail_Move"].toInt();
//    CoreDetail_Attack=root["CoreDetail_Attack"].toInt();
//    CoreDetail_Gather=root["CoreDetail_Gather"].toInt();
//    CoreDetail_ResourceIn=root["CoreDetail_ResourceIn"].toInt();
//    CoreDetail_Transport=root["CoreDetail_Transport"].toInt();
//    CoreDetail_UpdateRatio=root["CoreDetail_UpdateRatio"].toInt();
//    CoreDetail_Unload=root["CoreDetail_Unload"].toInt();

//    ///********** Core关系函数的可变操作指令 **********/
//    OPERATECON_DEFAULT=root["OPERATECON_DEFAULT"].toInt();
//    OPERATECHANGE=root["OPERATECHANGE"].toInt();

//    ////####距离判定
//    OPERATECON_NEAR_ABSOLUTE=root["OPERATECON_NEAR_ABSOLUTE"].toInt();
//    OPERATECON_MOVEALTER=root["OPERATECON_MOVEALTER"].toInt();
//    OPERATECON_NEAR_ATTACK=root["OPERATECON_NEAR_ATTACK"].toInt();
//    OPERATECON_NEAR_WORK=root["OPERATECON_NEAR_WORK"].toInt();
//    OPERATECON_NEAR_MISSILE=root["OPERATECON_NEAR_MISSILE"].toInt();
//    OPERATECON_NEAR_ATTACK_MOVE=root["OPERATECON_NEAR_ATTACK_MOVE"].toInt();
//    OPERATECON_NEAR_UNLOAD=root["OPERATECON_NEAR_UNLOAD"].toInt();
//    OPERATECON_NEAR_TRANSPORT=root["OPERATECON_NEAR_TRANSPORT"].toInt();

//    OPERATECON_NEARALTER_ABSOLUTE=root["OPERATECON_NEARALTER_ABSOLUTE"].toInt();
//    OPERATECON_NEARALTER_WORK=root["OPERATECON_NEARALTER_WORK"].toInt();

//    ////####指定对象
//    OPERATECON_OBJECT1=root["OPERATECON_OBJECT1"].toInt();
//    OPERATECON_OBJECT2=root["OPERATECON_OBJECT2"].toInt();

//    OPERATECON_TIMES=root["OPERATECON_TIMES"].toInt();
//    OPERATE_TIMEMAX=root["OPERATE_TIMEMAX"].toInt();

//    OPERATECON_TIMES_USELESSACT_MOVE=root["OPERATECON_TIMES_USELESSACT_MOVE"].toInt();


    /********** 占地边长-块坐标常量 **********/
//    SIZELEN_SINGEL=root["SIZELEN_SINGEL"].toInt();
//    SIZELEN_SMALL=root["SIZELEN_SMALL"].toInt();
//    SIZELEN_MIDDLE=root["SIZELEN_MIDDLE"].toInt();
//    SIZELEN_BIG=root["SIZELEN_BIG"].toInt();


//    ///********** animal友好度 **********/
//    FRIENDLY_NULL=root["FRIENDLY_NULL"].toInt();
//    FRIENDLY_FRI=root["FRIENDLY_FRI"].toInt();
//    FRIENDLY_ENEMY=root["FRIENDLY_ENEMY"].toInt();
//    FRIENDLY_FENCY=root["FRIENDLY_FENCY"].toInt();

//    ///********** 兵种状态 **********/
//    ARMY_STATE_DEFAULT=root["ARMY_STATE_DEFAULT"].toInt();
//    ARMY_STATE_AROUND=root["ARMY_STATE_AROUND"].toInt();
//    ARMY_STATE_DEFENSE=root["ARMY_STATE_DEFENSE"].toInt();
//    ARMY_STATE_ATTACK=root["ARMY_STATE_ATTACK"].toInt();

    /********** 巡逻状态 **********/
//    PATROL_STATE_IDLE=root["PATROL_STATE_IDLE"].toInt();
//    PATROL_STATE_PATROLLING=root["PATROL_STATE_PATROLLING"].toInt();
//    PATROL_STATE_CHASING=root["PATROL_STATE_CHASING"].toInt();
//    PATROL_STATE_RETURNING=root["PATROL_STATE_RETURNING"].toInt();
}
