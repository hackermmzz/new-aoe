#include "GlobalVariate.h"
#include "config.h"
#include <QDirIterator>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QtWidgets>
#include<iostream>

using namespace std;

/*************************配置读取量***********************/
QString ResultLogFile;//实时信息输出日志
bool OffScreen;//是否关闭渲染游戏渲染
int DefaultCivilization;//初始文明
bool IsExamining;
int TimePerFrame;//一帧要耗的时间
bool EditorMode;//是否开启编辑器模式
bool GlobalVision;//是否开启全局视野
bool only_debug_Player0;
bool filterRepetitionMessage;
double MUSIC_VOLUME;//音乐响度
int GAME_WIDTH;//总窗口宽度
int GAME_HEIGHT;//总窗口高度
string GAME_VERSION;//当前版本名称
QString GAME_TITLE;//总窗口名称
string MAPFILE_SUFFIX;//地图文件后缀名
int GAME_LOSE_SEC;//游戏失败的终止时间
int MAXPLAYER;//最大玩家数量
int NOWPLAYER;//当前游戏实现的玩家的数量
int NOWPLAYERREPRESENT;//当前游戏操控的玩家ID
int Boulder_Trail_Effect_Duration;//巨石拖尾特效存在时间
int INITIAL_FREQUENCY;//地图移动的速度
int MEMORYROW;//内存图的行数
int MEMORYCOLUMN;//内存图的列数
int GAMEWIDGET_WIDTH;//游戏界面绘制窗口宽
int GAMEWIDGET_HEIGHT;//游戏界面绘制窗口高
double BLOCKSIDELENGTH;//一个区块的大小
double UNLOAD_RADIAN;//船卸货的半径
int MAP_L;//地图长
int MAP_U;//地图宽
double HUMAN_SPEED;//人的速度
double WOOD_BOAT_SPEED;//木船速度
double ANIMAL_SPEED;//动物速度
QString RESPATH;//资源文件的路径
bool OPTION_MUSIC;//是否开启音乐
bool OPTION_SOUND;//是否开启音效
bool OPTION_SELECT;//
bool OPTION_LINE;//
bool OPTION_POS;//
bool OPTION_OVERLAP;//
int HOUSE_HUMAN_NUM;//一个房屋可以带来人数增加的数量
int VISION_GAZELLE;//瞪羚的视野范围
int BLOOD_GAZELLE;//瞪羚的血量
int CNT_GAZELLE;//瞪羚死亡可以获取的肉数量
int VISION_LION;//狮子的视野
int BLOOD_LION;//狮子的血量
int CNT_LION;//狮子死亡可以掉落的肉
int ANIMAL_ATTACKRANGE_LION;//狮子攻击范围
int VISION_ELEPHANT;//大象视野
int BLOOD_ELEPHANT;//大象血量
double SPEED_ELEPHANT;//大象速度
int CNT_ELEPHANT;//大象死亡掉落的肉数量
int ANIMAL_ATTACKRANGE_ELEPHANT;//大象攻击范围
int BLOOD_TREE;//树的血量
int CNT_TREE;//树死亡掉落的数量
int VISION_FARMER;//农民的视野范围
int BLOOD_FARMER;//农民的血量
/*农民一次性可以携带的资源上限*/
int FARMER_CARRYLIMIT_WOOD;
int FARMER_CARRYLIMIT_FOOD;
int FARMER_CARRYLIMIT_GOLD;
int FARMER_CARRYLIMIT_STONE;
int FARMER_CARRYLIMIT_UPGRADEWOOD;
int FARMER_CARRYLIMIT_UPGRADEFOOD;
int FARMER_CARRYLIMIT_UPGRADEGOLD;
int FARMER_CARRYLIMIT_UPGRADESTONE;
/*农民采摘和建造速度*/
double FARMER_GATHERSPEED_WOOD;
double FARMER_GATHERSPEED_FOOD;
double FARMER_GATHERSPEED_GOLD;
double FARMER_GATHERSPEED_STONE;
double FARMER_CONSTRUCTSPEED;
/*静态资源一堆的数量*/
int CNT_UPGRADEFARM;
int CNT_BUSH;
int CNT_STONE;
int CNT_GOLDORE;
int CNT_FISH;
//
bool MAP_EXPLORE;//设置地图是否已经全部探索
bool MAP_VISIABLE;//开启全局视野
/*初始的资源数量*/
int INITIAL_WOOD;
int INITIAL_MEAT;
int INITIAL_GOLD;
int INITIAL_STONE;
/*碰撞盒大小*/
double CRASHBOX_MICRO;
double CRASHBOX_SINGLEBLOCK;
double CRASHBOX_SMALLBLOCK;
double CRASHBOX_SMALL;
double CRASHBOX_MIDDLE;
double CRASHBOX_BIG;
double CRASHBOX_SINGLEOB;
double CRASHBOX_SMALLOB;
double CRASHBOX_BIGOB;
/**市政中心**/
int BLOOD_BUILD_CENTER;
int VISION_CENTER;
int BUILD_CENTER_WOOD;
int TIME_BUILD_CENTER;
//生产村民
int BUILDING_CENTER_CREATEFARMER_FOOD;
int TIME_BUILDING_CENTER_CREATEFARMER;
//升级时代至工具时代
int BUILDING_CENTER_UPGRADE_TOOLAGE_FOOD;
int TIME_BUILDING_CENTER_UPGRADE;
//升级至铜器时代
int BUILDING_CENTER_UPGRADE_BRONZEAGE_FOOD;
int BUILDING_CENTER_UPGRADE_BRONZEAGE_GOLD;
/**房子**/
int BLOOD_BUILD_HOUSE;
int VISION_HOME;
int BUILD_HOUSE_WOOD;
int TIME_BUILD_HOME;
/**仓库**/
int BLOOD_BUILD_STOCK;
int VISION_STOCK;
int BUILD_STOCK_WOOD;
int TIME_BUILD_STOCK;
//升级近战攻击（1级）
int BUILDING_STOCK_UPGRADE_CLOSER_ATTACK_FOOD;
int TIME_BUILDING_STOCK_UPGRADE_CLOSER_ATTACK;
int BUILDING_STOCK_UPGRADE_CLOSER_ATTACK_ADDITION_ATTACK;
//升级近战攻击 (2级）
int BUILDING_STOCK_UPGRADE_CLOSER_ATTACK_2_FOOD;
int BUILDING_STOCK_UPGRADE_CLOSER_ATTACK_2_GOLD;
int TIME_BUILDING_STOCK_UPGRADE_CLOSER_ATTACK_2;
int BUILDING_STOCK_UPGRADE_CLOSER_ATTACK_2_ADDITION_ATTACK;
//升级步兵护甲（1级）
int BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY_ADDITION_DEFENSE_INFANTRY;
int BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY_FOOD;
int TIME_BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY;
//升级步兵护甲 (2级)
int BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY_2_FOOD;
int BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY_2_GOLD;
int TIME_BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY_2;
int BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY_2_ADDITION_DEFENSE_INFANTRY;
//升级步兵对投射武器防御力（1级）
int BUILDING_STOCK_UPGRADE_MISSILE_DEFENSE_INFANTRY_FOOD;
int BUILDING_STOCK_UPGRADE_MISSILE_DEFENSE_INFANTRY_GOLD;
int TIME_BUILDING_STOCK_UPGRADE_MISSILE_DEFENSE_INFANTRY;
int BUILDING_STOCK_UPGRADE_MISSILE_DEFENSE_INFANTRY_ADDITION_DEFENSE_INFANTRY;
//升级弓兵护甲（1级）
int BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER_FOOD;
int TIME_BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER;
int BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER_ADDITION_DEFENSE_ARCHER;
//升级弓兵护甲（2级）
int BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER_2_FOOD;
int BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER_2_GOLD;
int TIME_BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER_2;
int BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER_2_ADDITION_DEFENSE_ARCHER;
//升级骑兵护甲（1级）
int BUILDING_STOCK_UPGRADE_DEFENSE_RIDER_FOOD;
int TIME_BUILDING_STOCK_UPGRADE_DEFENSE_RIDER;
int BUILDING_STOCK_UPGRADE_DEFENSE_RIDER_ADDITION_DEFENSE_RIDER;
//升级骑兵护甲（2级）
int BUILDING_STOCK_UPGRADE_DEFENSE_RIDER_2_FOOD;
int BUILDING_STOCK_UPGRADE_DEFENSE_RIDER_2_GOLD;
int TIME_BUILDING_STOCK_UPGRADE_DEFENSE_RIDER_2;
int BUILDING_STOCK_UPGRADE_DEFENSE_RIDER_2_ADDITION_DEFENSE_RIDER;
//升级箭塔 (1级)
int BUILDING_GRANARY_UPGRADE_ARROWTOWER_FOOD;
int BUILDING_GRANARY_UPGRADE_ARROWTOWER_STONE;
int TIME_BUILDING_GRANARY_UPGRADE_ARROWTOWER;
int BUILDING_GRANARY_UPGRADE_ARROWTOWER_ADDITION_ATK;
int BUILDING_GRANARY_UPGRADE_ARROWTOWER_ADDITION_DIS;
int THROWMISSION_ARROWTOWER_UPGRADED;
/**谷仓**/
int BLOOD_BUILD_GRANARY;
int VISION_GRANARY;
int BUILD_GRANARY_WOOD;
int TIME_BUILD_GRANARY;
//研发箭塔
int BUILDING_GRANARY_ARROWTOWER_FOOD;
int TIME_BUILDING_GRANARY_RESEARCH_ARROWTOWER;
//研发城墙
int BUILDING_GRANARY_RESEARCH_WALL_FOOD;
int TIME_BUILDING_GRANARY_RESEARCH_WALL;
/**兵营**/
int BLOOD_BUILD_ARMYCAMP;
int VISION_ARMYCAMP;
int BUILD_ARMYCAMP_WOOD;
int TIME_BUILD_ARMYCAMP;
//生产棍棒兵
int BUILDING_ARMYCAMP_CREATE_CLUBMAN_FOOD;
int TIME_BUILDING_ARMYCAMP_CREATE_CLUBMAN;
//升级棍棒兵为斧头兵
int BUILDING_ARMYCAMP_UPGRADE_CLUBMAN_FOOD;
int TIME_BUILDING_ARMYCAMP_UPGRADE_CLUBMAN;
// 训练阔剑兵
int BUILDING_ARMYCAMP_CREATE_BROADSWORD_FOOD;
int BUILDING_ARMYCAMP_CREATE_BROADSWORD_GOLD;
int TIME_BUILDING_ARMYCAMP_CREATE_BROADSWORD;
// 升级阔剑兵科技
int BUILDING_ARMYCAMP_UPGRADE_BROADSWORD_FOOD;
int BUILDING_ARMYCAMP_UPGRADE_BROADSWORD_GOLD;
int TIME_BUILDING_ARMYCAMP_UPGRADE_BROADSWORD;
//生产投石者
int BUILDING_ARMYCAMP_CREATE_SLINGER_FOOD;
int BUILDING_ARMYCAMP_CREATE_SLINGER_STONE;
int TIME_BUILDING_ARMYCAMP_CREATE_SLINGER;
/**靶场**/
int BLOOD_BUILD_RANGE;
int VISION_RANGE;
int BUILD_RANGE_WOOD;
int TIME_BUILD_RANGE;
//生产弓箭手
int BUILDING_RANGE_CREATE_BOWMAN_FOOD;
int BUILDING_RANGE_CREATE_BOWMAN_WOOD;
int TIME_BUILDING_RANGE_CREATE_BOWMAN;
// 训练战车弓箭手
int BUILDING_RANGE_CREATE_CHARIOT_ARCHER_FOOD;
int BUILDING_RANGE_CREATE_CHARIOT_ARCHER_WOOD;
int TIME_BUILDING_RANGE_CREATE_CHARIOT_ARCHER;
// 训练复合弓兵
int BUILDING_RANGE_CREATE_COMPOSITE_BOWMAN_FOOD;
int BUILDING_RANGE_CREATE_COMPOSITE_BOWMAN_GOLD;
int TIME_BUILDING_RANGE_CREATE_COMPOSITE_BOWMAN;
// 升级复合弓科技
int BUILDING_RANGE_UPGRADE_COMPOSITE_BOW_FOOD;
int BUILDING_RANGE_UPGRADE_COMPOSITE_BOW_WOOD;
int TIME_BUILDING_RANGE_UPGRADE_COMPOSITE_BOW;
/**马厩**/
int BLOOD_BUILD_STABLE;
int VISION_STABLE;
int BUILD_STABLE_WOOD;
int TIME_BUILD_STABLE;
//生产侦察骑兵
int BUILDING_STABLE_CREATE_SCOUT_FOOD;
int TIME_BUILDING_STABLE_CREATE_SCOUT;
// 训练战车
int BUILDING_STABLE_CREATE_CHARIOT_FOOD;
int BUILDING_STABLE_CREATE_CHARIOT_WOOD;
int TIME_BUILDING_STABLE_CREATE_CHARIOT;
//训练骑兵
int BUILDING_STABLE_CREATE_CAVALRY_FOOD;
int BUILDING_STABLE_CREATE_CAVALRY_GOLD;
int TIME_BUILDING_STABLE_CREATE_CAVALRY;

/**市场**/
int BLOOD_BUILD_MARKET;
int VISION_MARKET;
int BUILD_MARKET_WOOD;
int TIME_BUILD_MARKET;
/**船坞**/
int BLOOD_BUILD_DOCK;
int VISION_DOCK;
int BUILD_DOCK_WOOD;
int TIME_BUILD_DOCK;
/**生产战船**/
int BUILDING_DOCK_CREATE_SHIP_WOOD;
int TIME_BUILDING_DOCK_CREATE_SHIP;
/**生产渔船**/
int BUILDING_DOCK_CREATE_SAILING_WOOD;
int TIME_BUILDING_DOCK_CREATE_SAILING;
/**生产运输船**/
int BUILDING_DOCK_CREATE_WOOD_BOAT_WOOD;
int TIME_BUILDING_DOCK_CREATE_WOOD_BOAT;
//升级伐木（1级）
int BUILDING_MARKET_WOOD_UPGRADE_FOOD;
int BUILDING_MARKET_WOOD_UPGRADE_WOOD;
int TIME_BUILDING_MARKET_UPGRADE_CUTTING;
int BUILDING_MARKET_WOOD_UPGRADE_ADDITION_CARRY;
int BUILDING_MARKET_WOOD_UPGRADE_ADDITION_GATHERRATE;
int BUILDING_MARKET_WOOD_UPGRADE_ADDITION_DISSHOOT;
/**攻城武器厂**/
int BLOOD_BUILD_SIEGE;
int VISION_SIEGE;
int BUILD_SIEGE_WOOD;
int TIME_BUILD_SIEGE;
//生产投石车
int BUILDING_SIEGE_CREATE_STONE_THROWER_WOOD;
int BUILDING_SIEGE_CREATE_STONE_THROWER_GOLD;
int TIME_BUILDING_SIEGE_CREATE_STONE_THROWER;
/**学院**/
int BLOOD_BUILD_COLLAGE;
int VISION_COLLAGE;
int BUILD_COLLAGE_WOOD;
int TIME_BUILD_COLLAGE;
// 训练方阵兵
int BUILDING_COLLAGE_CREATE_HOPLITE_FOOD;
int BUILDING_COLLAGE_CREATE_HOPLITE_GOLD;
int TIME_BUILDING_COLLAGE_CREATE_HOPLITE;
//升级采集石头（1级）
int BUILDING_MARKET_STONE_UPGRADE_FOOD;
int BUILDING_MARKET_STONE_UPGRADE_STONE;
int TIME_BUILDING_MARKET_UPGRADE_DIGGINGSOTNE;
int BUILDING_MARKET_STONE_UPGRADE_ADDITION_CARRY;
int BUILDING_MARKET_STONE_UPGRADE_ADDITION_GATHERRATE;
int BUILDING_MARKET_STONE_UPGRADE_ADDITION_SILNGERATK;
int BUILDING_MARKET_STONE_UPGRADE_ADDITION_SILNGERDIS;
//升级金矿开采（1级）
int BUILDING_MARKET_GOLD_UPGRADE_FOOD;
int BUILDING_MARKET_GOLD_UPGRADE_WOOD;
int TIME_BUILDING_MARKET_UPGRADE_GOLD;
int BUILDING_MARKET_GOLD_UPGRADE_ADDITION_CARRY;
int BUILDING_MARKET_GOLD_UPGRADE_ADDITION_GATHERRATE;
//升级农田（1级）
int BUILDING_MARKET_FARM_UPGRADE_FOOD;
int BUILDING_MARKET_FARM_UPGRADE_WOOD;
int TIME_BUILDING_MARKET_UPGRADE_FARM;
int BUILDING_MARKET_FARM_UPGRADE_ADDITION_FOOD;
// 升级车轮
int BUILDING_MARKET_WHEEL_UPGRADE_FOOD;
int BUILDING_MARKET_WHEEL_UPGRADE_WOOD;
int TIME_BUILDING_MARKET_WHEEL_UPGRADE;
// 研发工艺（铜器时代，木材加工升级版）
int BUILDING_MARKET_CRAFT_UPGRADE_FOOD;
int BUILDING_MARKET_CRAFT_UPGRADE_WOOD;
int TIME_BUILDING_MARKET_CRAFT_UPGRADE;
int BUILDING_MARKET_CRAFT_UPGRADE_ADDITION_DISSHOOT;
int BUILDING_MARKET_CRAFT_UPGRADE_ADDITION_GATHERRATE;
// 研发犁（铜器时代，驯养动物升级版）
int BUILDING_MARKET_PLOW_UPGRADE_FOOD;
int BUILDING_MARKET_PLOW_UPGRADE_WOOD;
int TIME_BUILDING_MARKET_PLOW_UPGRADE;
int BUILDING_MARKET_PLOW_UPGRADE_ADDITION_FOOD;
/**农场**/
int BLOOD_BUILD_FARM;
int CNT_BUILD_FARM;
int VISION_FARM;
int BUILD_FARM_WOOD;
int TIME_BUILD_FARM;
/**箭塔**/
int BLOOD_BUILD_ARROWTOWER;
double VISION_ARROWTOWER;
int DIS_BUILD_ARROWTOWER;
int ATK_BUILD_ARROWTOWER;
int DEFSHOOT_BUILD_ARROWTOWER;
int BUILD_ARROWTOWER_STONE;
int TIME_BUILD_ARROWTOWER;
double DIS_ARROWTOWER;//箭塔攻击距离
/**城墙**/
int BLOOD_BUILD_WALL;
int DEFSHOOT_BUILD_WALL;
int BUILD_WALL_STONE;
int TIME_BUILD_WALL;
/********** 距离常量 **********/
double DISTANCE_Manhattan_MoveEndNEAR;
double DISTANCE_Manhattan_PathMove;
double DISTANCE_Manhattan_Unload;
double DISTANCE_Manhattan_Transport;
double DISTANCE_ATTACK_CLOSE;
double DISTANCE_HIT_TARGET;
double DISTANCE_ELEPHANT_ATTACK;
double SHIP_ACT_MAX_DISTANCE;//船可以和人互动的最大距离(平方)
/************士兵属性**************/
//棍棒兵
int BLOOD_CLUBMAN1;
double SPEED_CLUBMAN1;
int VISION_CLUBMAN1;
int DIS_CLUBMAN1;
double INTERVAL_CLUBMAN1;
int ATK_CLUBMAN1;
int DEFCLOSE_CLUBMAN1;
int DEFSHOOT_CLUBMAN1;
//刀斧手
int BLOOD_CLUBMAN2;
double SPEED_CLUBMAN2;
int VISION_CLUBMAN2;
int DIS_CLUBMAN2;
double INTERVAL_CLUBMAN2;
int ATK_CLUBMAN2;
int DEFCLOSE_CLUBMAN2;
int DEFSHOOT_CLUBMAN2;
//短剑手
int BLOOD_SHORTSWORDSMAN1;
double SPEED_SHORTSWORDSMAN1;
int VISION_SHORTSWORDSMAN1;
int DIS_SHORTSWORDSMAN1;
double INTERVAL_SHORTSWORDSMAN1;
int ATK_SHORTSWORSMAN1;
int DEFCLOSE_SHORTSWORSMAN1;
int DEFSHOOT_SHORTSWORSMAN1;
//阔剑手
int BLOOD_SHORTSWORDSMAN2;
double SPEED_SHORTSWORDSMAN2;
int VISION_SHORTSWORDSMAN2;
int DIS_SHORTSWORDSMAN2;
double INTERVAL_SHORTSWORDSMAN2;
int ATK_SHORTSWORSMAN2;
int DEFCLOSE_SHORTSWORSMAN2;
int DEFSHOOT_SHORTSWORSMAN2;
//长剑手
int BLOOD_SHORTSWORDSMAN3;
double SPEED_SHORTSWORDSMAN3;
int VISION_SHORTSWORDSMAN3;
int DIS_SHORTSWORDSMAN3;
double INTERVAL_SHORTSWORDSMAN3;
int ATK_SHORTSWORSMAN3;
int DEFCLOSE_SHORTSWORSMAN3;
int DEFSHOOT_SHORTSWORSMAN3;
//铁甲步兵
int BLOOD_SHORTSWORDSMAN4;
double SPEED_SHORTSWORDSMAN4;
int VISION_SHORTSWORDSMAN4;
int DIS_SHORTSWORDSMAN4;
double INTERVAL_SHORTSWORDSMAN4;
int ATK_SHORTSWORSMAN4;
int DEFCLOSE_SHORTSWORSMAN4;
int DEFSHOOT_SHORTSWORSMAN4;
//投石者
int BLOOD_SLINGER;
double SPEED_SLINGER;
int VISION_SLINGER;
int DIS_SLINGER;
double INTERVAL_SLINGER;
int ATK_SLINGER;
int DEFCLOSE_SLINGER;
int DEFSHOOT_SLINGER;
// 阔剑兵属性（使用SHORTSWORDSMAN2的属性）
int BLOOD_BROADSWORDSMAN;
double SPEED_BROADSWORDSMAN;
int VISION_BROADSWORDSMAN;
int ATK_BROADSWORDSMAN;
double DIS_BROADSWORDSMAN;
double INTERVAL_BROADSWORDSMAN;
int DEFCLOSE_BROADSWORDSMAN;
int DEFSHOOT_BROADSWORDSMAN;
int NOWRES_TIMER_BROADSWORDSMAN;
//弓箭手
int BLOOD_BOWMAN;
double SPEED_BOWMAN;
int VISION_BOWMAN;
int DIS_BOWMAN;
double INTERVAL_BOWMAN;
int ATK_BOWMAN;
int DEFCLOSE_BOWMAN;
int DEFSHOOT_BOWMAN;
// 战车弓箭手属性
int BLOOD_CHARIOT_ARCHER;
double SPEED_CHARIOT_ARCHER;
int VISION_CHARIOT_ARCHER;
int ATK_CHARIOT_ARCHER;
double DIS_CHARIOT_ARCHER;
double INTERVAL_CHARIOT_ARCHER;
int DEFCLOSE_CHARIOT_ARCHER;
int DEFSHOOT_CHARIOT_ARCHER;
int NOWRES_TIMER_CHARIOT_ARCHER;
// 复合弓兵属性（使用IMPROVEDBOWMAN2的属性）
int BLOOD_COMPOSITE_BOWMAN;
double SPEED_COMPOSITE_BOWMAN;
int VISION_COMPOSITE_BOWMAN;
int ATK_COMPOSITE_BOWMAN;
double DIS_COMPOSITE_BOWMAN;
double INTERVAL_COMPOSITE_BOWMAN;
int DEFCLOSE_COMPOSITE_BOWMAN;
int DEFSHOOT_COMPOSITE_BOWMAN;
int NOWRES_TIMER_COMPOSITE_BOWMAN;
//长弓手
int BLOOD_IMPROVEDBOWMAN1;
double SPEED_IMPROVEDBOWMAN1;
int VISION_IMPROVEDBOWMAN1;
int DIS_IMPROVEDBOWMAN1;
double INTERVAL_IMPROVEDBOWMAN1;
int ATK_IMPROVEDBOWMAN1;
int DEFCLOSE_IMPROVEDBOWMAN1;
int DEFSHOOT_IMPROVEDBOWMAN1;
//十字弓手
int BLOOD_IMPROVEDBOWMAN2;
double SPEED_IMPROVEDBOWMAN2;
int VISION_IMPROVEDBOWMAN2;
int DIS_IMPROVEDBOWMAN2;
double INTERVAL_IMPROVEDBOWMAN2;
int ATK_IMPROVEDBOWMAN2;
int DEFCLOSE_IMPROVEDBOWMAN2;
int DEFSHOOT_IMPROVEDBOWMAN2;
//侦察骑兵
int BLOOD_SCOUT;
double SPEED_SCOUT;
int VISION_SCOUT;
int DIS_SCOUT;
double INTERVAL_SCOUT;
int ATK_SCOUT;
int DEFCLOSE_SCOUT;
int DEFSHOOT_SCOUT;
// 战车属性
int BLOOD_CHARIOT;
double SPEED_CHARIOT;
int VISION_CHARIOT;
int ATK_CHARIOT;
double DIS_CHARIOT;
double INTERVAL_CHARIOT;
int DEFCLOSE_CHARIOT;
int DEFSHOOT_CHARIOT;
int NOWRES_TIMER_CHARIOT;
//骑兵
int BLOOD_CAVALRY;
double SPEED_CAVALRY;
int VISION_CAVALRY;
int DIS_CAVALRY;
double INTERVAL_CAVALRY;
int ATK_CAVALRY;
int DEFCLOSE_CAVALRY;
int DEFSHOOT_CAVALRY;
//战船
int BLOOD_SHIP;
double SPEED_SHIP;
int VISION_SHIP;
int ATK_SHIP;
double DIS_SHIP;
int INTERVAL_SHIP;
int DEFCLOSE_SHIP;
int DEFSHOOT_SHIP;
//投石车
int BLOOD_STONE_THROWER;
double SPEED_STONE_THROWER;
int VISION_STONE_THROWER;
int ATK_STONE_THROWER;
double DIS_STONE_THROWER;
int INTERVAL_STONE_THROWER;
int DEFCLOSE_STONE_THROWER;
int DEFSHOOT_STONE_THROWER;
//祭司
int BLOOD_PRIEST;
double SPEED_PRIEST;
int VISION_PRIEST;
int ATK_PRIEST;
double DIS_PRIEST;
int INTERVAL_PRIEST;
int DEFCLOSE_PRIEST;
int DEFSHOOT_PRIEST;
// 方阵兵属性
int BLOOD_HOPLITE;
double SPEED_HOPLITE;
int VISION_HOPLITE;
int ATK_HOPLITE;
double DIS_HOPLITE;
double INTERVAL_HOPLITE;
int DEFCLOSE_HOPLITE;
int DEFSHOOT_HOPLITE;
int NOWRES_TIMER_HOPLITE;
/********** 飞行物投掷速度 **********/
double Missile_Speed_Spear;
double Missile_Speed_Arrow;
double Missile_Speed_Cobblestone;
double Missile_Speed_Boulders;
/***********范围飞行物的溅射伤害范围(曼哈顿块距离)********/
int Missile_Boulders_Range;
/********** 设置多少帧切换一次nowres **********/
int NOWRES_TIMER_FARMER;
int NOWRES_TIMER_CLUBMAN;
int NOWRES_TIMER_BOWMAN;
int NOWRES_TIMER_IMPROVEDBOWMAN1;
int NOWRES_TIMER_SCOUT;
int NOWRES_TIMER_CAVALRY;
int NOWRES_TIMER_SLINGER;
int NOWRES_TIMER_SWORSMAN;
int NOWRES_TIMER_LION;
int NOWRES_TIMER_ELEPHANT;
int NOWRES_TIMER_SHIP;
int NOWRES_TIMER_STONE_THROWER;
int NOWRES_TIMER_PRIEST;
/*******建筑着火配置***********/
double BUILDING_BLOOD_FIRE_SMALL;
double BUILDING_BLOOD_FIRE_MIDDLE;
double BUILDING_BLOOD_FIRE_BIG;
/************************全局初始量************************/
int mapmoveFrequency;//地图移动速度
bool is_cheatAction = false;
EventFilter *eventFilter;
NetworkPlugin*NetworkManager;
map<std::string, std::list<QPixmap>> resMap;
map<string, QSoundEffect*> SoundMap;
std::queue<string> soundQueue;
Score usrScore=Score(0);
Score enemyScore=Score(1);
Coordinate *nowobject=NULL;
bool tryCaptured=0;//尝试捕获点击对象后置1
Coordinate* LeftMouseObjCapture=0;
Coordinate* RightMouseObjCaptrue=0;
std::queue<st_DebugMassage>debugMassagePackage;
std::map<QString , int>debugMessageRecord;
int** memorymap;    //记录出现在当前画面上的object,用于g_Object[]中访问
std::string direction[5]={"Down","LeftDown","Left","LeftUp","Up"};
bool GenerateHumanLock=0;//每一帧保证只有一个人可以诞生
///////////////////////////////////////////////////////////////////////////////
int InitImageResMap(QString path)
{
    //判断路径是否存在
    QDir dir(path);
    if(!dir.exists())
    {
        qDebug()<<"Error: A path that does not exist. path: "<<path;
        return -1;
    }

    QStringList filters;
    filters<<QString("*.png")<<QString("*.gif");
    //文件类型过滤器（去除符号链接symlink）
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    //文件名称过滤器（去除其他后缀的文件）
    dir.setNameFilters(filters);
    int dirCount = dir.count();
    //
    if(dirCount <= 0)
    {
        qDebug()<<"路径内无png和gif文件";
        return -1;
    }
    //遍历所有文件

    for(int i=0; i<dirCount; i++)
    {
        //文件名称
        QString fileName = dir[i];
        //文件全路径
        QString filePath = path + "/" + fileName;
        //获取文件后缀
        int index = fileName.lastIndexOf("_");
        QString imageMapName;
        imageMapName = fileName.left(index);
        //
        std::string tmpListName = imageMapName.toStdString();
        //获取图片
        QPixmap img;
        /*
         * 鉴于项目耦合度太高了，汪立洪根本不可能去花时间去专门解耦，
         * 特此为了满足oj的低内存运行，必须把图片整体给砍掉,
         * 特此提醒，建议别动这行代码
         *
        */
        if(!OffScreen)img=QPixmap(filePath);
        /*
         * 正所谓，
         * 项目越大，代码越屎。
         * 项目越小，神人越神。
         * 我已经无力回天。
        */

        //存储全局资源
        resMap[tmpListName].push_back(img);
    }
    ////////////////////////////////对资源进行额外操作
    //对船的帧数进行调整
    for(auto&ele:resMap){
        string name=ele.first;
        if(name.find("Ship")!=string::npos){
            auto&list=ele.second;
            auto tmp=list.front();
            for(int i=0;i<10;++i)list.push_back(tmp);
        }
        else if(name.find("Sailing")!=string::npos){
            auto&list=ele.second;
            auto tmp=list.front();
            for(int i=0;i<10;++i)list.push_back(tmp);
        }
    }
    //投石车巨石就使用普通投石兵石头，但是我们对其进行放大
    {
        auto&stone=resMap["Cobblestone"];
        auto&boulders=resMap["Boulders"];
        for(QPixmap&pix:stone){
            QPixmap scaledPix = pix.scaled(
                pix.size() * 3.0,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
            );
            boulders.push_back(scaledPix);
        }
    }

    ////////////////////////////////
    return -1;
}
int InitSoundResMap(QString path)
{
    //判断路径是否存在
    QDir dir(path);
    if(!dir.exists())
    {
        qDebug()<<"路径不存在";
        return -1;
    }

    QStringList filters;
    filters<<QString("*.wav");

    //文件类型过滤器（去除符号链接symlink）
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    //文件名称过滤器（去除其他后缀的文件）
    dir.setNameFilters(filters);
    int dirCount = dir.count();
    //
    if(dirCount <= 0)
    {
        qDebug()<<"路径内无wav文件";
        return -1;
    }

    //获取分隔符
    QChar separator = QChar('/');

    if(!path.contains(separator))
    {
        separator = QChar('\\');
    }
    QChar lastChar = path.at(path.length()-1);
    if(lastChar == separator)
    {
        separator = QChar();
    }

    for(int i=0; i<dirCount; i++)
    {
        //文件名称
        QString fileName = dir[i];
        //获取文件后缀
        int index = fileName.lastIndexOf(".");
        QString SoundMapName;
        SoundMapName = fileName.left(index);
        std::string tmpMapName = SoundMapName.toStdString();
        //
        QSoundEffect* qSoundEffect = new QSoundEffect();
        QString filePath ="qrc:///"+fileName;
        //
        qSoundEffect->setSource(QUrl(filePath));
        qSoundEffect->setVolume(MUSIC_VOLUME*100);
        SoundMap.insert(map<string, QSoundEffect*>::value_type(tmpMapName, qSoundEffect));
    }

    return -1;
}

QPixmap applyTransparencyEffect(const QPixmap& originalPixmap, qreal opacity)
{
    // 创建一个新的 QPixmap，大小与原始图片相同
    QPixmap modifiedPixmap(originalPixmap.size());
    modifiedPixmap.fill(Qt::transparent);

    // 使用 QPainter 在新 QPixmap 上绘制原始图片
    QPainter painter(&modifiedPixmap);
    painter.setOpacity(1.0);  // 将绘制原始图片的不透明度设置为1.0
    painter.drawPixmap(0, 0, originalPixmap);


    // 创建原始图片的掩码
    QBitmap mask = originalPixmap.createMaskFromColor(Qt::transparent);

    // 创建一个剪切区域，仅包含原始图片的不透明部分
    QRegion opaqueRegion = QRegion(originalPixmap.rect()).subtracted(QRegion(mask));
    QPainterPath clipPath;
    clipPath.addRect(originalPixmap.rect());
    clipPath.addRegion(opaqueRegion);
    painter.setClipPath(clipPath);

    //    // 创建一个剪切区域，仅包含原始图片的透明部分
    //    QPainterPath clipPath;
    //    clipPath.addRect(originalPixmap.rect());
    //    clipPath.addRegion(QRegion(originalPixmap.mask().inverted()));
    //    painter.setClipPath(clipPath);

    // 绘制半透明黑色矩形，不影响原始图片的透明部分
    painter.setOpacity(opacity);
    painter.fillRect(modifiedPixmap.rect(), QColor(0, 0, 0));
    painter.end();

    return modifiedPixmap;
}

/*
种类：
0为空地；
1为树木；
2为浆果；
3为瞪羚；
4为石头；
5为金矿；
6为狮子；
7为大象；
9为主营；
10为箭塔废墟。
*/

/*
  下面是预设的资源样式
*/
int Forest[3][15][15] =
{
    {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0},
     {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
     {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
     {0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
     {1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
     {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
     {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
     {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
     {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
     {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
     {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
     {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
     {0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0}},

    {{0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
     {0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1},
     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
     {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
     {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
     {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
     {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
     {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
     {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
     {0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0}},

    {{0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0},
     {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
     {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
     {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
     {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
     {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
     {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
     {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
     {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
     {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
     {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
     {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
     {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1}}
};

int Food[5][5][5] =
{
    {{1, 0, 1, 0, 0},
     {0, 1, 0, 0, 1},
     {0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0},
     {1, 1, 0, 0, 0}},

    {{0, 0, 0, 1, 0},
     {0, 0, 0, 0, 0},
     {0, 0, 0, 1, 1},
     {0, 0, 0, 1, 0},
     {0, 0, 1, 0, 1}},

    {{0, 0, 0, 1, 0},
     {1, 0, 1, 1, 0},
     {0, 0, 0, 0, 0},
     {0, 1, 0, 0, 0},
     {1, 0, 0, 0, 0}},

    {{0, 1, 0, 1, 0},
     {0, 0, 1, 0, 1},
     {0, 0, 0, 0, 0},
     {0, 0, 0, 0, 1},
     {0, 0, 0, 1, 0}},

    {{1, 1, 0, 1, 0},
     {1, 0, 0, 0, 0},
     {0, 0, 0, 0, 1},
     {0, 0, 0, 0, 0},
     {0, 0, 0, 1, 0}}
};

int Stone[5][5][5] =
{
    {{1, 0, 0, 1, 1},
     {0, 0, 1, 1, 1},
     {0, 0, 1, 1, 0},
     {0, 1, 0, 0, 0},
     {0, 0, 0, 1, 1}},

    {{1, 0, 0, 1, 0},
     {0, 0, 0, 1, 0},
     {0, 1, 1, 1, 0},
     {0, 0, 1, 0, 0},
     {1, 0, 1, 1, 0}},

    {{1, 1, 1, 1, 1},
     {1, 0, 0, 0, 1},
     {1, 0, 1, 0, 0},
     {0, 1, 0, 0, 1},
     {0, 0, 0, 1, 0}},

    {{0, 1, 0, 0, 1},
     {0, 1, 0, 1, 1},
     {0, 0, 1, 0, 1},
     {0, 1, 1, 1, 0},
     {0, 0, 0, 1, 0}},

    {{0, 0, 0, 1, 1},
     {0, 1, 0, 1, 0},
     {1, 0, 1, 0, 1},
     {0, 0, 0, 1, 0},
     {0, 1, 0, 1, 1}}
};


void loadResource(std::string name, std::list<ImageResource> *targetlist)
{

    //
    targetlist->clear();
    auto temp=&resMap[name];
    auto index=temp->begin();
    while(index!=temp->end())
    {

        ImageResource *res=new ImageResource();
        //赋值pix
        res->pix=(*index);
        initMemory(res);
        if(name=="Center1")
        {
            res->memorymap.fillBlockMemoryMap();
        }
        targetlist->push_back(*res);
        delete res;
        index++;
    }
}

void initMemory(ImageResource *res)
{
    //赋值内存图
    QImage piximage=res->pix.toImage();
    res->memorymap = pixMemoryMap(res->pix.width(),res->pix.height());
    for(int i=0;i<res->pix.width();i++)
    {
        for(int j=0;j<res->pix.height();j++)
        {
            if((piximage.pixel(i,j))&(0xff000000)!=0)
            {
                res->memorymap.setMemoryMap(i,j);
                if(i>0&&j>0)
                {
                    res->memorymap.setMemoryMap(i-1,j-1);
                    res->memorymap.setMemoryMap(i-1,j);
                    res->memorymap.setMemoryMap(i,j-1);
                }
                else if(i>0)
                {
                    res->memorymap.setMemoryMap(i-1,j);
                }
                else if(j>0)
                {
                    res->memorymap.setMemoryMap(i,j-1);
                }
                if(i<res->pix.width()-1&&j<res->pix.height()-1)
                {
                    res->memorymap.setMemoryMap(i+1,j+1);
                    res->memorymap.setMemoryMap(i+1,j);
                    res->memorymap.setMemoryMap(i,j+1);
                }
                else if(i<res->pix.width()-1)
                {
                    res->memorymap.setMemoryMap(i+1,j);
                }
                else if(j<res->pix.height()-1)
                {
                    res->memorymap.setMemoryMap(i,j+1);
                }
            }
        }
    }
}

double countdistance(double L, double U, double L0, double U0)
{
    return sqrt((L-L0)*(L-L0)+(U-U0)*(U-U0));
}
bool isNear_Manhattan( double dr , double ur , double dr1  , double ur1 , double distance )
{
    return fabs(dr - dr1)<=distance && fabs(ur - ur1)<=distance;
}


void flipResource(std::list<ImageResource> *currentlist, std::list<ImageResource> *targetlist)
{
    if(currentlist==0)return;
    targetlist->clear();
    std::list<ImageResource>::iterator iter = currentlist->begin();
    while (iter != currentlist->end())
    {
        QImage image = (*iter).pix.toImage();
        image = image.mirrored(true, false);
        ImageResource *res = new ImageResource(QPixmap::fromImage(image));
        initMemory(res);
        targetlist->push_back(*res);
        delete res;
        iter++;
    }
}

void loadGrayRes(std::list<ImageResource> *res, std::list<ImageResource> *grayres)
{
    auto graypointer = res->begin();
    while (graypointer != res->end())
    {
        ImageResource* res = new ImageResource(applyTransparencyEffect((*graypointer).pix, 0.5));
        res->memorymap = (*graypointer).memorymap;
        grayres->push_back(*res);
        delete res;
        graypointer++;
    }
}

void loadBlackRes(std::list<ImageResource> *res, std::list<ImageResource> *blackres)
{
    auto blackpointer = res->begin();
    while (blackpointer != res->end())
    {
        ImageResource* res = new ImageResource(applyTransparencyEffect((*blackpointer).pix, 1));
        res->memorymap = (*blackpointer).memorymap;
        blackres->push_back(*res);
        delete res;
        blackpointer++;
    }
}

int calculateManhattanDistance(int x1, int y1, int x2, int y2)
{
        int distance = abs(x1 - x2) + abs(y1 - y2);
        return distance;
}

double calculateManhattanDistance(double x1, double y1, double x2, double y2)
{
    return fabs(x1-x2)+fabs(y1-y2);
}

void calMirrorPoint( double& dr , double &ur , double dr_mirror, double ur_mirror , double dis)
{
    double dr_deta = dr_mirror-dr, ur_deta = ur_mirror - ur;
    double total = fabs(dr_deta)+fabs(ur_deta);

    dr = dr_mirror+dr_deta/total*dis;
    ur = ur_mirror+ur_deta/total*dis;

}

double trans_BlockPointToDetailCenter( int p )
{
    return (p+0.5)*BLOCKSIDELENGTH;
}

void call_debugText(QString color, QString content,int playerID)
{
    if( !IsExamining && (!only_debug_Player0 || playerID==NOWPLAYERREPRESENT || playerID == REPRESENT_BOARDCAST_MESSAGE) )
    {
        if(  !filterRepetitionMessage || debugMessageRecord[content] == 0 || color == "black"|| color == "green" )
        {
            debugMassagePackage.push(st_DebugMassage(color, content));
            debugMessageRecord[content] = g_frame;
        }
    }
}
//*************************************************************

bool instruction::isExist() {
    return type != -1;
}

instruction::instruction(int type,int SN, int obSN , bool twoCoredinate){
    this->SN = SN;
    this->obSN = obSN;
    this->type=type;
    this->self=g_Object[SN];
    this->obj=g_Object[obSN];
}
instruction::instruction(int type,int SN,int BL,int BU,int option){
    this->SN = SN;
    this->type=type;
    this->self=g_Object[SN];
    this->BlockDR=BL;
    this->BlockUR=BU;
    this->option=option;
}
instruction::instruction(int type,int SN,double L,double U){
    this->SN = SN;
    this->type=type;
    this->self=g_Object[SN];
    this->DR=L;
    this->UR=U;
}
instruction::instruction(int type,int SN,int option){
    this->SN = SN;
    this->type=type;
    this->self=g_Object[SN];
    this->option=option;
}

int sgn(double __x)
{
    if(__x > 0) return 1;
    else if(__x < 0) return -1;
    else return 0;
}

MouseEvent::MouseEvent()
{
    Reset();
}

int MouseEvent::GetMouseEventType()
{
    return mouseEventType;
}

void MouseEvent::SetMouseEventType(int tp)
{
    mouseEventType=tp;
}

bool MouseEvent::HaveEvent()
{
    return mouseEventType!=NULL_MOUSEEVENT;
}

int MouseEvent::GetMemoryMapX()
{
    return memoryMapX;
}

int MouseEvent::GetMemoryMapY()
{
    return memoryMapY;
}

void MouseEvent::SetMemoeyMapX(int v)
{
    memoryMapX=v;
}

void MouseEvent::SetMemoryMapY(int v)
{
    memoryMapY=v;
}

double MouseEvent::GetDR()
{
    return DR;
}

double MouseEvent::GetUR()
{
    return UR;
}

void MouseEvent::SetDR(double v)
{
    DR=v;
}

void MouseEvent::SetUR(double v)
{
    UR=v;
}

void MouseEvent::Reset()
{
    memoryMapX=0;
    memoryMapY=0;
    DR=UR=0;
    mouseEventType=NULL_MOUSEEVENT;
}
/**********************************工具函数**********************************************/
//配置参数
void ParseArguments(const QApplication&app){
    ////////////////////////////////解析参数
    QCommandLineParser parser;
    // 添加帮助选项（自动处理--help/-h）
    parser.addHelpOption();
    QCommandLineOption option0(
        QStringList()<<"exam",
         "开启考试模式"
       );
    QCommandLineOption option1(
        QStringList()<<"offscreen",
        "关闭图像渲染"
       );
    QCommandLineOption option2(
        QStringList()<<"ResultLogFile",
         "实时数据输出存储日志",
         "a.txt"
       );
    QCommandLineOption option3(
        QStringList()<<"freq",
         "开启几倍速",
         "1|2|4|8|MAX"
       );
    QList<QCommandLineOption>options={option0,option1,option2,option3};
    parser.addOptions(options);
    parser.process(app);
    //
    if(parser.isSet("exam")){
        IsExamining=true;
    }
    //
    if(parser.isSet("offscreen")){
        OffScreen=true;
    }
    //
    if(parser.isSet("ResultLogFile")){
        auto value=parser.value("ResultLogFile");
        ResultLogFile=value;
    }
    //
    if(parser.isSet("freq")){
        auto value=parser.value("freq");
        if(value=="MAX")INITIAL_FREQUENCY=INT_MAX;
        else INITIAL_FREQUENCY=value.toInt();
    }
}
//Json化一个Map
QString JsonMap(const QMap<QString, QVariant>&data){
    QJsonObject obj;
    for(auto itr=data.begin();itr!=data.end();++itr){
        obj.insert(itr.key(),QJsonValue::fromVariant(itr.value()));
    }
    QJsonDocument jsonDoc(obj);
    QString ret= jsonDoc.toJson(QJsonDocument::Indented);
    return ret;
}



ResultLogInfo::ResultLogInfo(bool win_, int score_,int wood_, int food_, int gold_, int stone_, string msg_){
    win=win_;
    wood=wood_;
    food=food_;
    gold=gold_;
    stone=stone_;
    msg=msg_;
    score=score_;
}




void ResultLogInfo::LogOut()
{

   static QTextStream*out=0;
   if(!out){
       //打开文件：WriteOnly 只写模式
       QFile*file=new QFile(ResultLogFile);
       if(!file->open(QIODevice::WriteOnly | QIODevice::Text))
       {
           qDebug() << "文件打开失败：" << file->errorString();
           return;
       }
       //创建文本流
       out=new QTextStream(file);
   }
   //////////////////////////////////////写入信息
   (*out)<<ToString()<<"\n";
   out->flush();
}

QString ResultLogInfo::ToString()
{
    QJsonObject obj;
    obj["win"]=win;
    obj["time"]=g_frame*TimePerFrame;
    obj["frame"]=g_frame;
    obj["score"]=score;
    obj["wood"]=wood;
    obj["food"]=food;
    obj["gold"]=gold;
    obj["stone"]=stone;
    if(msg!="")obj["msg"]=QString::fromStdString(msg);
    QJsonDocument doc(obj);
    QString txt=doc.toJson(QJsonDocument::Compact);//确保每次输出只占一行
    return txt;
}

st_DebugMassage::st_DebugMassage(QString color, QString content)
{
    this->color = color;
    this->content = content;
}

void Score::addScore(int points, const QString &message) {
    score += points;
    if (id == 0)
        call_debugText("blue", " 玩家" + message, REPRESENT_BOARDCAST_MESSAGE);
    else
        call_debugText("red", " 敌方" + message, REPRESENT_BOARDCAST_MESSAGE);
}

Score::Score(int id) : id(id), score(0) {}

int Score::getScore() {
    return score;
}

void Score::update(int type, int num) {
    if(type==_FINDENEMYLAND){
        addScore(10,"登录地方大陆,分数+10");
        return;
    }
    if (type <= _ISSTONE && scoreTypes[type] == 0 && type > _MEAT) {
        addScore(5, " 采集到新资源，分数+5");
        if (type == _ISGOLD) {
            addScore(10, " 采集到黄金，分数+10");
        }
    }

    if (type > _MEAT && type <= _ISSTONE) {
        scoreTypes[type] = scoreTypes[type] | 1;
        return;
    }

    int before = scoreTypes[type] / 100;
    scoreTypes[type] += num;

    if (type <= _MEAT) {
        int after = scoreTypes[type] / 100;
        int change = after - before;
        while (change > 0) {
            addScore(1, " 单种资源收集满100个，分数+1");
            change--;
        }
    }

    switch (type) {
    case _TECH:
        addScore(2, " 解锁新科技，分数+2");
        break;
    case _HUMAN1:
        addScore(1, " 生产普通单位，分数+1");
        break;
    case _HUMAN2:
        addScore(2, " 生产特殊单位，分数+2");
        break;
    case _BUILDING1:
        addScore(1, " 建造住房或农田，分数+1");
        break;
    case _BUILDING2:
        addScore(2, " 建造一般建筑，分数+2");
        break;
    case _KILL2:
        addScore(2, " 击杀一般敌人，分数+2");
        break;
    case _DESTORY2:
        addScore(2, " 摧毁房屋或农田，分数+2");
        break;
    case _DESTORY4:
        addScore(4, " 摧毁一般建筑，分数+4");
        break;
    case _DESTORY5:
        addScore(5, " 摧毁箭塔，分数+5");
        break;
    case _DESTORY10:
        addScore(10, " 摧毁主营，分数+10");
        break;
    default:
        break;
    }
}

bool tagObj::operator <(const tagObj &obj) const{
    return SN<obj.SN;
}

tagBuilding tagBuilding::toEnemy() {
    this->Cnt = -1;
    this->Project = -1;
    this->ProjectPercent = -1;
    return *this;
}

void tagHuman::cast_from(tagHuman taghuman) {
    this->DR = taghuman.DR;
    this->UR = taghuman.UR;
    this->BlockDR = taghuman.BlockDR;
    this->BlockUR = taghuman.BlockUR;
    this->DR0 = taghuman.DR0;
    this->UR0 = taghuman.UR0;
    this->NowState = taghuman.NowState;
    this->WorkObjectSN = taghuman.WorkObjectSN;
    this->Blood = taghuman.Blood;
    this->SN = taghuman.SN;
}

tagFarmer tagFarmer::toEnemy() {
    Resource = -1;
    DR0 = -1.0;
    UR0 = -1.0;
    return *this;
}

tagArmy tagArmy::toEnemy() {
    DR0 = -1.0;
    UR0 = -1.0;
    return *this;
}



Point::Point() {}

Point::Point(int x, int y) { this->x = x, this->y = y; }

Point::Point(const Point &board) { x = board.x, y = board.y; }

Point Point::operator +(const Point &ps) { return Point(x + ps.x, y + ps.y); }

Point Point::operator -(const Point &ps) { return Point(x - ps.x, y - ps.y); }

bool Point::operator ==(const Point &ps) const { return ps.x == x && ps.y == y; }

bool Point::operator <(const Point &ps) const { return x < ps.x && y < ps.y; }

tagInfo &tagInfo::operator=(const tagInfo &other) {
    if (this != &other) { // Check for self-assignment
        buildings = other.buildings;
        farmers = other.farmers;
        armies = other.armies;
        enemy_buildings = other.enemy_buildings;
        enemy_farmers = other.enemy_farmers;
        enemy_armies = other.enemy_armies;
        resources = other.resources;
        ins_ret = other.ins_ret;

        // Deep copy theMap array
        theMap=other.theMap;
        /*
            for (int i = 0; i < MAP_L; ++i) {
                for (int j = 0; j < MAP_U; ++j) {
                    theMap[i][j] = other.theMap[i][j];
                }
            }
            */
        GameFrame = other.GameFrame;
        civilizationStage = other.civilizationStage;
        Wood = other.Wood;
        Meat = other.Meat;
        Stone = other.Stone;
        Gold = other.Gold;
        Human_MaxNum = other.Human_MaxNum;
    }
    return *this;
}

void tagInfo::clear() {
    buildings.clear();
    farmers.clear();
    armies.clear();
    enemy_buildings.clear();
    enemy_farmers.clear();
    enemy_armies.clear();
    resources.clear();
    ins_ret.clear();
}

void tagGame::update(tagInfo *newinfo) {
    //控制ins_ret的大小小于100，若大于100，则优先删除旧值
   // QMutexLocker locker(&Locker);//之前是严格的帧同步，现在改成严格的非帧同步
    if (this->Info != NULL) {
        while (Info->ins_ret.size() > 100) {
            Info->ins_ret.erase(Info->ins_ret.begin());
        }
    }
    if (this->Info != NULL)
        newinfo->ins_ret = this->Info->ins_ret;
    Info = newinfo;
    //对内部打乱
    static const bool openHunYao = 1;
    //
    if (openHunYao) {
        WLHHunYao(Info->buildings);
        WLHHunYao(Info->farmers);
        WLHHunYao(Info->armies);
        WLHHunYao(Info->enemy_buildings);
        WLHHunYao(Info->enemy_farmers);
        WLHHunYao(Info->enemy_armies);
        WLHHunYao(Info->resources);
    }
}

bool tagGame::tryLock()
{
    return Locker.tryLock();
}

void tagGame::release()
{
    Locker.unlock();
}

void tagGame::insertInsRet(int id, instruction ins) {
    QMutexLocker locker(&Locker);
    this->Info->ins_ret.insert(make_pair(id, ins.ret));
}

tagInfo tagGame::getInfo() {
    QMutexLocker locker(&Locker);
    return *Info;
}

void tagGame::clearInsRet() {
    QMutexLocker locker(&Locker);
    Info->ins_ret.clear();
}

pixMemoryMap::pixMemoryMap(int w, int h) : width(w), height(h) {
    // 分配内存图空间
    MemoryMap.resize(width * height);
}

pixMemoryMap::pixMemoryMap() : width(0), height(0) {}

pixMemoryMap::pixMemoryMap(const pixMemoryMap &other) : width(other.width), height(other.height)
{

    MemoryMap = other.MemoryMap;
}

pixMemoryMap &pixMemoryMap::operator=(const pixMemoryMap &other)
{
    width = other.width;
    height = other.height;

    MemoryMap = other.MemoryMap;

    return *this;
}

void pixMemoryMap::setMemoryMap(int i, int j) {
    int index = i * height + j;
    MemoryMap[index] = 1;
}

char pixMemoryMap::getMemoryMap(int i, int j) {
    int index = i * height + j;
    return MemoryMap[index];
}

void pixMemoryMap::fillBlockMemoryMap()
{
    for (int i = 0;i < width / 2;i++)
    {
        for (int j = 0;j < height / 2;j++)
        {
            if (j * width >= height * (width / 2 - i))
            {
                setMemoryMap(i, j);
            }
        }
    }
    for (int i = width / 2;i < width;i++)
    {
        for (int j = 0;j < height / 2;j++)
        {
            if (j * width >= height * (i - width / 2))
            {
                setMemoryMap(i, j);
            }
        }
    }
    for (int i = 0;i < width / 2;i++)
    {
        for (int j = height / 2;j < height;j++)
        {
            if (j * width <= height * (i + width / 2))
            {
                setMemoryMap(i, j);
            }
        }
    }
    for (int i = width / 2;i < width;i++)
    {
        for (int j = height / 2;j < height;j++)
        {
            if (j * width <= -height * i + 3 * width * height / 2)
            {
                setMemoryMap(i, j);
            }
        }
    }
}

ImageResource::ImageResource(QPixmap pix) :pix(pix)
{
    if (pix.isNull()) {
        // 图片未成功加载，执行错误处理操作
        qDebug() << "fault";
    }
}

ImageResource::ImageResource()
{

}

conditionDevelop::conditionDevelop() {}

conditionDevelop::conditionDevelop(int civilization, int sort_building, double needTimes, int need_Wood, int need_Food, int need_Stone, int need_Gold)
{
    this->civilization = civilization;
    this->sort_building = sort_building;
    this->need_Wood = need_Wood;
    this->need_Food = need_Food;
    this->need_Stone = need_Stone;
    this->need_Gold = need_Gold;
    this->times_second = needTimes;
}

void conditionDevelop::addPreCondition(conditionDevelop *con_need) { preCondition.push_back(con_need); }

void conditionDevelop::setCreatObjectAfterAction(int creatSort)
{
    isCreatObjectAction = true;
    creatObjectSort = creatSort;
}

void conditionDevelop::setCreatObjectAfterAction(int creatSort, int creatNum)
{
    setCreatObjectAfterAction(creatSort);
    creatObjectNum = creatNum;
}

bool conditionDevelop::executable(int wood, int food, int stone, int gold) { return wood >= need_Wood && food >= need_Food && stone >= need_Stone && gold >= need_Gold; }

void conditionDevelop::get_needResource(int &wood, int &food, int &stone, int &gold) { wood = need_Wood, food = need_Food, stone = need_Stone, gold = need_Gold; }

bool conditionDevelop::isShowable(int nowcivilization)
{
    if (civilization > nowcivilization) return false;

    for (list<conditionDevelop*>::iterator iter = preCondition.begin(); iter != preCondition.end(); iter++)
        if (!(*iter)->acttimes) return false;

    return true;
}

void conditionDevelop::finishAct() { acttimes++; }

bool conditionDevelop::isNeedCreatObject(int &creatSort, int &creatNum)
{
    creatSort = creatObjectSort;
    creatNum = creatObjectNum;
    return isCreatObjectAction;
}

bool conditionDevelop::isNeedCreatObject() { return isCreatObjectAction; }

int conditionDevelop::getActTimes() { return acttimes; }

st_upgradeLab::st_upgradeLab() {}

st_upgradeLab::~st_upgradeLab()
{
    while (headAct != endNode)
    {
        nowExecuteNode = headAct;
        headAct = headAct->nextDevAction;
        delete nowExecuteNode;
    }
    delete endNode;
}

void st_upgradeLab::setHead(conditionDevelop *head) { endNode = nowExecuteNode = headAct = head; }

void st_upgradeLab::push_back(conditionDevelop *node)
{
    endNode->nextDevAction = node;
    endNode = node;
}

void st_upgradeLab::endNodeAsOver() { endNode->nextDevAction = endNode; }

void st_upgradeLab::shift()
{
    if (nowExecuteNode != NULL)
    {
        overExecute();
        haveFinishedPhaseNum++;
        nowExecuteNode = nowExecuteNode->nextDevAction;
    }
}

bool st_upgradeLab::isShowAble(int nowcivilization)
{
    if (nowExecuteNode == NULL) return false;
    else return nowExecuteNode->isShowable(nowcivilization) && (!nowExecuting || nowExecuteNode == nowExecuteNode->nextDevAction);
}

bool st_upgradeLab::executable(int nowcivilization, int wood, int food, int stone, int gold) { return isShowAble(nowcivilization) && nowExecuteNode->executable(wood, food, stone, gold); }

void st_upgradeLab::beginExecute() { this->nowExecuting = true; }

void st_upgradeLab::overExecute() { this->nowExecuting = false; }

int st_upgradeLab::getPhaseTimes() { return this->haveFinishedPhaseNum; }

void st_upgradeLab::get_needResource(int &wood, int &food, int &stone, int &gold)
{
    if (nowExecuteNode != NULL)
        nowExecuteNode->get_needResource(wood, food, stone, gold);
    else wood = 0, food = 0, stone = 0, gold = 0;
}

bool st_upgradeLab::isNeedCreatObject() {
    if (nowExecuteNode != NULL) return nowExecuteNode->isNeedCreatObject();
    else return false;
}

st_buildAction::st_buildAction() {}

st_buildAction::~st_buildAction()
{
    if (buildCon != NULL)
    {
        delete buildCon;
        buildCon = NULL;
    }
}

void st_buildAction::finishBuild() { buildCon->finishAct(); }

void st_buildAction::finishAction(int actNum)
{
    actCon[actNum].nowExecuteNode->finishAct();
    actCon[actNum].shift();
}



Q_COREAPP_STARTUP_FUNCTION(ReadConfig)
void ReadConfig()
{
    // 1. 打开配置文件
    QFile file("config.json");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
       return;
    }
    // 2. 读取文件内容
    QByteArray jsonData = file.readAll();
    file.close();
    // 3. 解析JSON数据
    QJsonParseError parseError;
    QJsonDocument json_config = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        return;
    }
    // 4. 检查是否为JSON对象（而非数组）
    if (!json_config.isObject()) {
       return;
    }
    QJsonObject config=json_config.object();
    ///////////////////////////////////////////开始读取
    #define json(x) config[#x]
    #define jsonInt(x) x=json(x).toInt()
    #define jsonDouble(x) x=json(x).toDouble()
    #define jsonQString(x) x=json(x).toString()
    #define jsonString(x) x=json(x).toString().toStdString()
    #define jsonBool(x) x=json(x).toBool()
    ////////////////////////////////////////////
    jsonBool(IsExamining);
    jsonInt(TimePerFrame);
    jsonBool(EditorMode);
    jsonBool(GlobalVision);
    jsonBool(only_debug_Player0);
    jsonBool(filterRepetitionMessage);
    jsonInt(GAME_WIDTH);
    jsonInt(GAME_HEIGHT);
    jsonString(GAME_VERSION);
    jsonQString(GAME_TITLE);
    jsonString(MAPFILE_SUFFIX);
    jsonInt(GAME_LOSE_SEC);
    jsonInt(MAXPLAYER);
    jsonInt(NOWPLAYER);
    jsonInt(NOWPLAYERREPRESENT);
    jsonInt(INITIAL_FREQUENCY);
    jsonInt(MEMORYROW);
    jsonInt(MEMORYCOLUMN);
    jsonInt(GAMEWIDGET_WIDTH);
    jsonInt(GAMEWIDGET_HEIGHT);
    jsonDouble(BLOCKSIDELENGTH);
    jsonInt(UNLOAD_RADIAN);
    jsonInt(MAP_L);
    jsonInt(MAP_U);
    jsonDouble(HUMAN_SPEED);
    jsonDouble(WOOD_BOAT_SPEED);
    jsonDouble(ANIMAL_SPEED);
    jsonQString(RESPATH);
    jsonBool(OPTION_MUSIC);
    jsonBool(OPTION_SOUND);
    jsonBool(OPTION_SELECT);
    jsonBool(OPTION_LINE);
    jsonBool(OPTION_POS);
    jsonBool(OPTION_OVERLAP);
    jsonDouble(BUILDING_BLOOD_FIRE_SMALL);
    jsonDouble(BUILDING_BLOOD_FIRE_MIDDLE);
    jsonDouble(BUILDING_BLOOD_FIRE_BIG);
    jsonInt(HOUSE_HUMAN_NUM);
    jsonInt(VISION_GAZELLE);
    jsonInt(BLOOD_GAZELLE);
    jsonInt(CNT_GAZELLE);
    jsonInt(VISION_LION);
    jsonInt(BLOOD_LION);
    jsonInt(CNT_LION);
    jsonInt(ANIMAL_ATTACKRANGE_LION);
    jsonInt(VISION_ELEPHANT);
    jsonInt(BLOOD_ELEPHANT);
    jsonDouble(SPEED_ELEPHANT);
    jsonInt(CNT_ELEPHANT);
    jsonInt(ANIMAL_ATTACKRANGE_ELEPHANT);
    jsonInt(BLOOD_TREE);
    jsonInt(CNT_TREE);
    jsonInt(VISION_FARMER);
    jsonInt(BLOOD_FARMER);
    jsonInt(FARMER_CARRYLIMIT_WOOD);
    jsonInt(FARMER_CARRYLIMIT_FOOD);
    jsonInt(FARMER_CARRYLIMIT_GOLD);
    jsonInt(FARMER_CARRYLIMIT_STONE);
    jsonInt(FARMER_CARRYLIMIT_UPGRADEWOOD);
    jsonInt(FARMER_CARRYLIMIT_UPGRADEFOOD);
    jsonInt(FARMER_CARRYLIMIT_UPGRADEGOLD);
    jsonInt(FARMER_CARRYLIMIT_UPGRADESTONE);
    jsonDouble(FARMER_GATHERSPEED_WOOD);
    jsonDouble(FARMER_GATHERSPEED_FOOD);
    jsonDouble(FARMER_GATHERSPEED_GOLD);
    jsonDouble(FARMER_GATHERSPEED_STONE);
    jsonDouble(FARMER_CONSTRUCTSPEED);
    jsonInt(CNT_UPGRADEFARM);
    jsonInt(CNT_BUSH);
    jsonInt(CNT_STONE);
    jsonInt(CNT_GOLDORE);
    jsonInt(CNT_FISH);
    jsonBool(MAP_EXPLORE);
    jsonBool(MAP_VISIABLE);
    jsonInt(INITIAL_WOOD);
    jsonInt(INITIAL_MEAT);
    jsonInt(INITIAL_GOLD);
    jsonInt(INITIAL_STONE);
    jsonDouble(CRASHBOX_MICRO);
    jsonDouble(CRASHBOX_SINGLEBLOCK);
    jsonDouble(CRASHBOX_SMALLBLOCK);
    jsonDouble(CRASHBOX_SMALL);
    jsonDouble(CRASHBOX_MIDDLE);
    jsonDouble(CRASHBOX_BIG);
    jsonDouble(CRASHBOX_SINGLEOB);
    jsonDouble(CRASHBOX_SMALLOB);
    jsonDouble(CRASHBOX_BIGOB);
    jsonInt(BLOOD_BUILD_CENTER);
    jsonInt(VISION_CENTER);
    jsonInt(BUILD_CENTER_WOOD);
    jsonInt(TIME_BUILD_CENTER);
    jsonInt(BUILDING_CENTER_CREATEFARMER_FOOD);
    jsonInt(TIME_BUILDING_CENTER_CREATEFARMER);
    jsonInt(BUILDING_CENTER_UPGRADE_TOOLAGE_FOOD);
    jsonInt(TIME_BUILDING_CENTER_UPGRADE);
    jsonInt(BUILDING_CENTER_UPGRADE_BRONZEAGE_FOOD);
    jsonInt(BUILDING_CENTER_UPGRADE_BRONZEAGE_GOLD);
    jsonInt(BLOOD_BUILD_HOUSE);
    jsonInt(VISION_HOME);
    jsonInt(BUILD_HOUSE_WOOD);
    jsonInt(TIME_BUILD_HOME);
    jsonInt(BLOOD_BUILD_STOCK);
    jsonInt(VISION_STOCK);
    jsonInt(BUILD_STOCK_WOOD);
    jsonInt(TIME_BUILD_STOCK);
    jsonInt(BUILDING_STOCK_UPGRADE_CLOSER_ATTACK_FOOD);
    jsonInt(TIME_BUILDING_STOCK_UPGRADE_CLOSER_ATTACK);
    jsonInt(BUILDING_STOCK_UPGRADE_CLOSER_ATTACK_ADDITION_ATTACK);
    jsonInt(BUILDING_STOCK_UPGRADE_CLOSER_ATTACK_2_FOOD);
    jsonInt(BUILDING_STOCK_UPGRADE_CLOSER_ATTACK_2_GOLD);
    jsonInt(TIME_BUILDING_STOCK_UPGRADE_CLOSER_ATTACK_2);
    jsonInt(BUILDING_STOCK_UPGRADE_CLOSER_ATTACK_2_ADDITION_ATTACK);
    jsonInt(BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY_FOOD);
    jsonInt(TIME_BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY);
    jsonInt(BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY_ADDITION_DEFENSE_INFANTRY);
    jsonInt(BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY_2_FOOD);
    jsonInt(BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY_2_GOLD);
    jsonInt(TIME_BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY_2);
    jsonInt(BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY_2_ADDITION_DEFENSE_INFANTRY);
    jsonInt(BUILDING_STOCK_UPGRADE_MISSILE_DEFENSE_INFANTRY_FOOD);
    jsonInt(BUILDING_STOCK_UPGRADE_MISSILE_DEFENSE_INFANTRY_GOLD);
    jsonInt(TIME_BUILDING_STOCK_UPGRADE_MISSILE_DEFENSE_INFANTRY);
    jsonInt(BUILDING_STOCK_UPGRADE_MISSILE_DEFENSE_INFANTRY_ADDITION_DEFENSE_INFANTRY);
    jsonInt(BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER_FOOD);
    jsonInt(TIME_BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER);
    jsonInt(BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER_ADDITION_DEFENSE_ARCHER);
    jsonInt(BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER_2_FOOD);
    jsonInt(BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER_2_GOLD);
    jsonInt(TIME_BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER_2);
    jsonInt(BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER_2_ADDITION_DEFENSE_ARCHER);
    jsonInt(BUILDING_STOCK_UPGRADE_DEFENSE_RIDER_FOOD);
    jsonInt(TIME_BUILDING_STOCK_UPGRADE_DEFENSE_RIDER);
    jsonInt(BUILDING_STOCK_UPGRADE_DEFENSE_RIDER_ADDITION_DEFENSE_RIDER);
    jsonInt(BUILDING_STOCK_UPGRADE_DEFENSE_RIDER_2_FOOD);
    jsonInt(BUILDING_STOCK_UPGRADE_DEFENSE_RIDER_2_GOLD);
    jsonInt(TIME_BUILDING_STOCK_UPGRADE_DEFENSE_RIDER_2);
    jsonInt(BUILDING_STOCK_UPGRADE_DEFENSE_RIDER_2_ADDITION_DEFENSE_RIDER);
    jsonInt(BUILDING_GRANARY_UPGRADE_ARROWTOWER_FOOD);
    jsonInt(BUILDING_GRANARY_UPGRADE_ARROWTOWER_STONE);
    jsonInt(TIME_BUILDING_GRANARY_UPGRADE_ARROWTOWER);
        jsonInt(BUILDING_GRANARY_UPGRADE_ARROWTOWER_ADDITION_ATK);
        jsonInt(BUILDING_GRANARY_UPGRADE_ARROWTOWER_ADDITION_DIS);
        jsonInt(THROWMISSION_ARROWTOWER_UPGRADED);
    jsonInt(BLOOD_BUILD_GRANARY);
    jsonInt(VISION_GRANARY);
    jsonInt(BUILD_GRANARY_WOOD);
    jsonInt(TIME_BUILD_GRANARY);
    jsonInt(BUILDING_GRANARY_ARROWTOWER_FOOD);
    jsonInt(TIME_BUILDING_GRANARY_RESEARCH_ARROWTOWER);
    jsonInt(BUILDING_GRANARY_RESEARCH_WALL_FOOD);
    jsonInt(TIME_BUILDING_GRANARY_RESEARCH_WALL);
    jsonInt(BLOOD_BUILD_ARMYCAMP);
    jsonInt(VISION_ARMYCAMP);
    jsonInt(BUILD_ARMYCAMP_WOOD);
    jsonInt(TIME_BUILD_ARMYCAMP);
    jsonInt(BUILDING_ARMYCAMP_CREATE_CLUBMAN_FOOD);
    jsonInt(TIME_BUILDING_ARMYCAMP_CREATE_CLUBMAN);
    jsonInt(BUILDING_ARMYCAMP_UPGRADE_CLUBMAN_FOOD);
    jsonInt(TIME_BUILDING_ARMYCAMP_UPGRADE_CLUBMAN);
    jsonInt(BUILDING_ARMYCAMP_CREATE_SLINGER_FOOD);
    jsonInt(BUILDING_ARMYCAMP_CREATE_SLINGER_STONE);
    jsonInt(TIME_BUILDING_ARMYCAMP_CREATE_SLINGER);
    // 训练阔剑兵
    jsonInt(BUILDING_ARMYCAMP_CREATE_BROADSWORD_FOOD);
    jsonInt(BUILDING_ARMYCAMP_CREATE_BROADSWORD_GOLD);
    jsonInt(TIME_BUILDING_ARMYCAMP_CREATE_BROADSWORD);
    // 升级阔剑兵科技
    jsonInt(BUILDING_ARMYCAMP_UPGRADE_BROADSWORD_FOOD);
    jsonInt(BUILDING_ARMYCAMP_UPGRADE_BROADSWORD_GOLD);
    jsonInt(TIME_BUILDING_ARMYCAMP_UPGRADE_BROADSWORD);

    jsonInt(BLOOD_BUILD_RANGE);
    jsonInt(VISION_RANGE);
    jsonInt(BUILD_RANGE_WOOD);
    jsonInt(TIME_BUILD_RANGE);
    jsonInt(BUILDING_RANGE_CREATE_BOWMAN_FOOD);
    jsonInt(BUILDING_RANGE_CREATE_BOWMAN_WOOD);
    jsonInt(TIME_BUILDING_RANGE_CREATE_BOWMAN);
    // 训练复合弓兵
    jsonInt(BUILDING_RANGE_CREATE_COMPOSITE_BOWMAN_FOOD);
    jsonInt(BUILDING_RANGE_CREATE_COMPOSITE_BOWMAN_GOLD);
    jsonInt(TIME_BUILDING_RANGE_CREATE_COMPOSITE_BOWMAN);
    // 升级复合弓科技
    jsonInt(BUILDING_RANGE_UPGRADE_COMPOSITE_BOW_FOOD);
    jsonInt(BUILDING_RANGE_UPGRADE_COMPOSITE_BOW_WOOD);
    jsonInt(TIME_BUILDING_RANGE_UPGRADE_COMPOSITE_BOW);

    jsonInt(BLOOD_BUILD_STABLE);
    jsonInt(VISION_STABLE);
    jsonInt(BUILD_STABLE_WOOD);
    jsonInt(TIME_BUILD_STABLE);
    // 训练战车弓箭手
    jsonInt(BUILDING_RANGE_CREATE_CHARIOT_ARCHER_FOOD);
    jsonInt(BUILDING_RANGE_CREATE_CHARIOT_ARCHER_WOOD);
    jsonInt(TIME_BUILDING_RANGE_CREATE_CHARIOT_ARCHER);

    //训练侦察骑兵
    jsonInt(BUILDING_STABLE_CREATE_SCOUT_FOOD);
    jsonInt(TIME_BUILDING_STABLE_CREATE_SCOUT);
    //训练骑兵
    jsonInt(BUILDING_STABLE_CREATE_CAVALRY_FOOD);
    jsonInt(BUILDING_STABLE_CREATE_CAVALRY_GOLD);
    jsonInt(TIME_BUILDING_STABLE_CREATE_CAVALRY);

    // 训练战车
    jsonInt(BUILDING_STABLE_CREATE_CHARIOT_FOOD);
    jsonInt(BUILDING_STABLE_CREATE_CHARIOT_WOOD);
    jsonInt(TIME_BUILDING_STABLE_CREATE_CHARIOT);
    jsonInt(BLOOD_BUILD_MARKET);
    jsonInt(VISION_MARKET);
    jsonInt(BUILD_MARKET_WOOD);
    jsonInt(TIME_BUILD_MARKET);
    jsonInt(BLOOD_BUILD_DOCK);
    jsonInt(VISION_DOCK);
    jsonInt(BUILD_DOCK_WOOD);
    jsonInt(TIME_BUILD_DOCK);
    jsonInt(BUILDING_DOCK_CREATE_SHIP_WOOD);
    jsonInt(TIME_BUILDING_DOCK_CREATE_SHIP);
    jsonInt(BUILDING_DOCK_CREATE_SAILING_WOOD);
    jsonInt(TIME_BUILDING_DOCK_CREATE_SAILING);
    jsonInt(BUILDING_DOCK_CREATE_WOOD_BOAT_WOOD);
    jsonInt(TIME_BUILDING_DOCK_CREATE_WOOD_BOAT);
    jsonInt(BUILDING_MARKET_WOOD_UPGRADE_FOOD);
    jsonInt(BUILDING_MARKET_WOOD_UPGRADE_WOOD);
    jsonInt(TIME_BUILDING_MARKET_UPGRADE_CUTTING);
    jsonInt(BUILDING_MARKET_WOOD_UPGRADE_ADDITION_CARRY);
    jsonInt(BUILDING_MARKET_WOOD_UPGRADE_ADDITION_GATHERRATE);
    jsonInt(BUILDING_MARKET_WOOD_UPGRADE_ADDITION_DISSHOOT);
    jsonInt(BLOOD_BUILD_SIEGE);
    jsonInt(VISION_SIEGE);
    jsonInt(BUILD_SIEGE_WOOD);
    jsonInt(TIME_BUILD_SIEGE);
    jsonInt(BLOOD_BUILD_COLLAGE);
    jsonInt(VISION_COLLAGE);
    jsonInt(BUILD_COLLAGE_WOOD);
    jsonInt(TIME_BUILD_COLLAGE);
    jsonInt(BUILDING_COLLAGE_CREATE_HOPLITE_FOOD);
    jsonInt(BUILDING_COLLAGE_CREATE_HOPLITE_GOLD);
    jsonInt(TIME_BUILDING_COLLAGE_CREATE_HOPLITE);
    jsonInt(BUILDING_SIEGE_CREATE_STONE_THROWER_WOOD);
    jsonInt(BUILDING_SIEGE_CREATE_STONE_THROWER_GOLD);
    jsonInt(TIME_BUILDING_SIEGE_CREATE_STONE_THROWER);
    jsonInt(BUILDING_MARKET_STONE_UPGRADE_FOOD);
    jsonInt(BUILDING_MARKET_STONE_UPGRADE_STONE);
    jsonInt(TIME_BUILDING_MARKET_UPGRADE_DIGGINGSOTNE);
    jsonInt(BUILDING_MARKET_STONE_UPGRADE_ADDITION_CARRY);
    jsonInt(BUILDING_MARKET_STONE_UPGRADE_ADDITION_GATHERRATE);
    jsonInt(BUILDING_MARKET_STONE_UPGRADE_ADDITION_SILNGERATK);
    jsonInt(BUILDING_MARKET_STONE_UPGRADE_ADDITION_SILNGERDIS);
    jsonInt(BUILDING_MARKET_GOLD_UPGRADE_FOOD);
    jsonInt(BUILDING_MARKET_GOLD_UPGRADE_WOOD);
    jsonInt(TIME_BUILDING_MARKET_UPGRADE_GOLD);
    jsonInt(BUILDING_MARKET_GOLD_UPGRADE_ADDITION_CARRY);
    jsonInt(BUILDING_MARKET_GOLD_UPGRADE_ADDITION_GATHERRATE);
    jsonInt(BUILDING_MARKET_FARM_UPGRADE_FOOD);
    jsonInt(BUILDING_MARKET_FARM_UPGRADE_WOOD);
    jsonInt(TIME_BUILDING_MARKET_UPGRADE_FARM);
    jsonInt(BUILDING_MARKET_FARM_UPGRADE_ADDITION_FOOD);
    // 升级车轮
    jsonInt(BUILDING_MARKET_WHEEL_UPGRADE_FOOD);
    jsonInt(BUILDING_MARKET_WHEEL_UPGRADE_WOOD);
    jsonInt(TIME_BUILDING_MARKET_WHEEL_UPGRADE);
    // 研发工艺
    jsonInt(BUILDING_MARKET_CRAFT_UPGRADE_FOOD);
    jsonInt(BUILDING_MARKET_CRAFT_UPGRADE_WOOD);
    jsonInt(TIME_BUILDING_MARKET_CRAFT_UPGRADE);
    jsonInt(BUILDING_MARKET_CRAFT_UPGRADE_ADDITION_DISSHOOT);
    jsonInt(BUILDING_MARKET_CRAFT_UPGRADE_ADDITION_GATHERRATE);
    // 研发犁
    jsonInt(BUILDING_MARKET_PLOW_UPGRADE_FOOD);
    jsonInt(BUILDING_MARKET_PLOW_UPGRADE_WOOD);
    jsonInt(TIME_BUILDING_MARKET_PLOW_UPGRADE);
    jsonInt(BUILDING_MARKET_PLOW_UPGRADE_ADDITION_FOOD);
    jsonInt(BLOOD_BUILD_FARM);
    jsonInt(CNT_BUILD_FARM);
    jsonInt(VISION_FARM);
    jsonInt(BUILD_FARM_WOOD);
    jsonInt(TIME_BUILD_FARM);
    jsonInt(BLOOD_BUILD_ARROWTOWER);
    jsonDouble(VISION_ARROWTOWER);
    jsonInt(DIS_BUILD_ARROWTOWER);
    jsonInt(ATK_BUILD_ARROWTOWER);
    jsonInt(DEFSHOOT_BUILD_ARROWTOWER);
    jsonInt(BUILD_ARROWTOWER_STONE);
    jsonInt(TIME_BUILD_ARROWTOWER);
    jsonInt(BLOOD_BUILD_WALL);
    jsonInt(DEFSHOOT_BUILD_WALL);
    jsonInt(BUILD_WALL_STONE);
    jsonInt(TIME_BUILD_WALL);
    jsonDouble(DISTANCE_Manhattan_MoveEndNEAR);
    jsonDouble(DISTANCE_Manhattan_PathMove);
    jsonDouble(DISTANCE_Manhattan_Unload);
    jsonDouble(DISTANCE_Manhattan_Transport);
    jsonDouble(DISTANCE_ATTACK_CLOSE);
    jsonDouble(DISTANCE_HIT_TARGET);
    jsonDouble(DISTANCE_ELEPHANT_ATTACK);
    jsonDouble(SHIP_ACT_MAX_DISTANCE);
    jsonDouble(DIS_ARROWTOWER);
    jsonDouble(SPEED_CLUBMAN1);
    jsonInt(VISION_CLUBMAN1);
    jsonInt(DIS_CLUBMAN1);
    jsonDouble(INTERVAL_CLUBMAN1);
    jsonInt(ATK_CLUBMAN1);
    jsonInt(DEFCLOSE_CLUBMAN1);
    jsonInt(DEFSHOOT_CLUBMAN1);
    jsonInt(BLOOD_CLUBMAN1);
    jsonInt(BLOOD_CLUBMAN2);
    jsonDouble(SPEED_CLUBMAN2);
    jsonInt(VISION_CLUBMAN2);
    jsonInt(DIS_CLUBMAN2);
    jsonDouble(INTERVAL_CLUBMAN2);
    jsonInt(ATK_CLUBMAN2);
    jsonInt(DEFCLOSE_CLUBMAN2);
    jsonInt(DEFSHOOT_CLUBMAN2);
    jsonInt(BLOOD_SHORTSWORDSMAN1);
    jsonDouble(SPEED_SHORTSWORDSMAN1);
    jsonInt(VISION_SHORTSWORDSMAN1);
    jsonInt(DIS_SHORTSWORDSMAN1);
    jsonDouble(INTERVAL_SHORTSWORDSMAN1);
    jsonInt(ATK_SHORTSWORSMAN1);
    jsonInt(DEFCLOSE_SHORTSWORSMAN1);
    jsonInt(DEFSHOOT_SHORTSWORSMAN1);
    jsonInt(BLOOD_SHORTSWORDSMAN2);
    jsonDouble(SPEED_SHORTSWORDSMAN2);
    jsonInt(VISION_SHORTSWORDSMAN2);
    jsonInt(DIS_SHORTSWORDSMAN2);
    jsonDouble(INTERVAL_SHORTSWORDSMAN2);
    jsonInt(ATK_SHORTSWORSMAN2);
    jsonInt(DEFCLOSE_SHORTSWORSMAN2);
    jsonInt(DEFSHOOT_SHORTSWORSMAN2);
    jsonInt(BLOOD_SHORTSWORDSMAN3);
    jsonDouble(SPEED_SHORTSWORDSMAN3);
    jsonInt(VISION_SHORTSWORDSMAN3);
    jsonInt(DIS_SHORTSWORDSMAN3);
    jsonDouble(INTERVAL_SHORTSWORDSMAN3);
    jsonInt(ATK_SHORTSWORSMAN3);
    jsonInt(DEFCLOSE_SHORTSWORSMAN3);
    jsonInt(DEFSHOOT_SHORTSWORSMAN3);
    jsonInt(BLOOD_SHORTSWORDSMAN4);
    jsonDouble(SPEED_SHORTSWORDSMAN4);
    jsonInt(VISION_SHORTSWORDSMAN4);
    jsonInt(DIS_SHORTSWORDSMAN4);
    jsonDouble(INTERVAL_SHORTSWORDSMAN4);
    jsonInt(ATK_SHORTSWORSMAN4);
    jsonInt(DEFCLOSE_SHORTSWORSMAN4);
    jsonInt(DEFSHOOT_SHORTSWORSMAN4);
    // 阔剑兵属性
    jsonInt(BLOOD_BROADSWORDSMAN);
    jsonDouble(SPEED_BROADSWORDSMAN);
    jsonInt(VISION_BROADSWORDSMAN);
    jsonInt(ATK_BROADSWORDSMAN);
    jsonDouble(DIS_BROADSWORDSMAN);
    jsonDouble(INTERVAL_BROADSWORDSMAN);
    jsonInt(DEFCLOSE_BROADSWORDSMAN);
    jsonInt(DEFSHOOT_BROADSWORDSMAN);
    jsonInt(NOWRES_TIMER_BROADSWORDSMAN);

    jsonInt(BLOOD_SLINGER);
    jsonDouble(SPEED_SLINGER);
    jsonInt(VISION_SLINGER);
    jsonInt(DIS_SLINGER);
    jsonDouble(INTERVAL_SLINGER);
    jsonInt(ATK_SLINGER);
    jsonInt(DEFCLOSE_SLINGER);
    jsonInt(DEFSHOOT_SLINGER);
    jsonInt(BLOOD_BOWMAN);
    jsonDouble(SPEED_BOWMAN);
    jsonInt(VISION_BOWMAN);
    jsonInt(DIS_BOWMAN);
    jsonDouble(INTERVAL_BOWMAN);
    jsonInt(ATK_BOWMAN);
    jsonInt(DEFCLOSE_BOWMAN);
    jsonInt(DEFSHOOT_BOWMAN);
    // 复合弓兵属性
    jsonInt(BLOOD_COMPOSITE_BOWMAN);
    jsonDouble(SPEED_COMPOSITE_BOWMAN);
    jsonInt(VISION_COMPOSITE_BOWMAN);
    jsonInt(ATK_COMPOSITE_BOWMAN);
    jsonDouble(DIS_COMPOSITE_BOWMAN);
    jsonDouble(INTERVAL_COMPOSITE_BOWMAN);
    jsonInt(DEFCLOSE_COMPOSITE_BOWMAN);
    jsonInt(DEFSHOOT_COMPOSITE_BOWMAN);
    jsonInt(NOWRES_TIMER_COMPOSITE_BOWMAN);
    // 战车弓箭手属性
    jsonInt(BLOOD_CHARIOT_ARCHER);
    jsonDouble(SPEED_CHARIOT_ARCHER);
    jsonInt(VISION_CHARIOT_ARCHER);
    jsonInt(ATK_CHARIOT_ARCHER);
    jsonDouble(DIS_CHARIOT_ARCHER);
    jsonDouble(INTERVAL_CHARIOT_ARCHER);
    jsonInt(DEFCLOSE_CHARIOT_ARCHER);
    jsonInt(DEFSHOOT_CHARIOT_ARCHER);
    jsonInt(NOWRES_TIMER_CHARIOT_ARCHER);

    jsonInt(BLOOD_IMPROVEDBOWMAN1);
    jsonDouble(SPEED_IMPROVEDBOWMAN1);
    jsonInt(VISION_IMPROVEDBOWMAN1);
    jsonInt(DIS_IMPROVEDBOWMAN1);
    jsonDouble(INTERVAL_IMPROVEDBOWMAN1);
    jsonInt(ATK_IMPROVEDBOWMAN1);
    jsonInt(DEFCLOSE_IMPROVEDBOWMAN1);
    jsonInt(DEFSHOOT_IMPROVEDBOWMAN1);
    jsonInt(BLOOD_IMPROVEDBOWMAN2);
    jsonDouble(SPEED_IMPROVEDBOWMAN2);
    jsonInt(VISION_IMPROVEDBOWMAN2);
    jsonInt(DIS_IMPROVEDBOWMAN2);
    jsonDouble(INTERVAL_IMPROVEDBOWMAN2);
    jsonInt(ATK_IMPROVEDBOWMAN2);
    jsonInt(DEFCLOSE_IMPROVEDBOWMAN2);
    jsonInt(DEFSHOOT_IMPROVEDBOWMAN2);
    jsonInt(BLOOD_SCOUT);
    jsonDouble(SPEED_SCOUT);
    jsonInt(VISION_SCOUT);
    jsonInt(DIS_SCOUT);
    jsonDouble(INTERVAL_SCOUT);
    jsonInt(ATK_SCOUT);
    jsonInt(DEFCLOSE_SCOUT);
    jsonInt(DEFSHOOT_SCOUT);
    // 战车属性
    jsonInt(BLOOD_CHARIOT);
    jsonDouble(SPEED_CHARIOT);
    jsonInt(VISION_CHARIOT);
    jsonInt(ATK_CHARIOT);
    jsonDouble(DIS_CHARIOT);
    jsonDouble(INTERVAL_CHARIOT);
    jsonInt(DEFCLOSE_CHARIOT);
    jsonInt(DEFSHOOT_CHARIOT);
    jsonInt(NOWRES_TIMER_CHARIOT);
    jsonInt(BLOOD_CAVALRY);
    jsonDouble(SPEED_CAVALRY);
    jsonInt(VISION_CAVALRY);
    jsonInt(DIS_CAVALRY);
    jsonDouble(INTERVAL_CAVALRY);
    jsonInt(ATK_CAVALRY);
    jsonInt(DEFCLOSE_CAVALRY);
    jsonInt(DEFSHOOT_CAVALRY);
    jsonInt(BLOOD_SHIP);
    jsonDouble(SPEED_SHIP);
    jsonInt(VISION_SHIP);
    jsonInt(ATK_SHIP);
    jsonDouble(DIS_SHIP);
    jsonInt(INTERVAL_SHIP);
    jsonInt(DEFCLOSE_SHIP);
    jsonInt(DEFSHOOT_SHIP);
    jsonInt(BLOOD_STONE_THROWER);
    jsonDouble(SPEED_STONE_THROWER);
    jsonInt(VISION_STONE_THROWER);
    jsonInt(ATK_STONE_THROWER);
    jsonDouble(DIS_STONE_THROWER);
    jsonInt(INTERVAL_STONE_THROWER);
    jsonInt(DEFCLOSE_STONE_THROWER);
    jsonInt(DEFSHOOT_STONE_THROWER);
    jsonInt(BLOOD_PRIEST);
    jsonDouble(SPEED_PRIEST);
    jsonInt(VISION_PRIEST);
    jsonInt(ATK_PRIEST);
    jsonDouble(DIS_PRIEST);
    jsonInt(INTERVAL_PRIEST);
    jsonInt(DEFCLOSE_PRIEST);
    jsonInt(DEFSHOOT_PRIEST);
    jsonDouble(Missile_Speed_Spear);
    jsonDouble(Missile_Speed_Arrow);
    jsonDouble(Missile_Speed_Cobblestone);
    jsonDouble(Missile_Speed_Boulders);
    jsonInt(Missile_Boulders_Range);
    jsonInt(NOWRES_TIMER_FARMER);
    jsonInt(NOWRES_TIMER_CLUBMAN);
    jsonInt(NOWRES_TIMER_BOWMAN);
    jsonInt(NOWRES_TIMER_IMPROVEDBOWMAN1);
    jsonInt(NOWRES_TIMER_SCOUT);
    jsonInt(NOWRES_TIMER_CAVALRY);
    jsonInt(NOWRES_TIMER_SLINGER);
    jsonInt(NOWRES_TIMER_SWORSMAN);
    jsonInt(NOWRES_TIMER_LION);
    jsonInt(NOWRES_TIMER_ELEPHANT);
    jsonInt(NOWRES_TIMER_SHIP);
    jsonInt(NOWRES_TIMER_STONE_THROWER);
    jsonInt(NOWRES_TIMER_PRIEST);
    jsonInt(BLOOD_HOPLITE);
    jsonDouble(SPEED_HOPLITE);
    jsonInt(VISION_HOPLITE);
    jsonInt(ATK_HOPLITE);
    jsonDouble(DIS_HOPLITE);
    jsonDouble(INTERVAL_HOPLITE);
    jsonInt(DEFCLOSE_HOPLITE);
    jsonInt(DEFSHOOT_HOPLITE);
    jsonInt(NOWRES_TIMER_HOPLITE);
    jsonInt(Boulder_Trail_Effect_Duration);
    jsonInt(DefaultCivilization);
    jsonBool(OffScreen);
    jsonDouble(MUSIC_VOLUME);
    ////////////////////////////////////////////////

}
