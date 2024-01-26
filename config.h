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
#define ANIMAL_SPEED sqrt(7)
#define RESPATH ":/new/prefix1/res"
#define ForestMAX 2
#define TREEMAX 40
#define BLOCKPIXEL_X (65 - showLine)
#define BLOCKPIXEL_Y (33 - showLine)
#define GENERATELANDFORMS_NUM 4
#define KEY_UP 87
#define KEY_LEFT 65
#define KEY_DOWN 83
#define KEY_RIGHT 68
#define BLOCK_COUNT 29      // Block种类计数，包括所有种类和样式的地图块数量

/********** 地图块种类 **********/
#define MAPTYPE_FLAT 0      // 平地
#define MAPTYPE_L0_UPTOLU 1 // L0边（向左上方凸起）
#define MAPTYPE_L2_UPTOLU 2 // L2边（向左上方凸起，同L0）

/********** 地图块高度 **********/
#define MAPHEIGHT_EMPTY 0   // 未定义高度
#define MAPHEIGHT_FLAT 2    // 地形高度

/********** 地图块样式 **********/
#define MAPPATTERN_GRASS 0      // 草原
#define MAPPATTERN_DESERT 1     // 沙漠
#define MAPPATTERN_OCEAN 2      // 海洋/河流
#define MAPPATTERN_SHOAL 3      // 浅滩（河流中可行走部分）


/********** DebugText栏颜色 **********/
#define COLOR_RED(STRING) QString("<font color=red>%1</font><font color=black> </font>").arg(STRING)
#define COLOR_BLUE(STRING) QString("<font color=blue>%1</font><font color=black> </font>").arg(STRING)
#define COLOR_GREEN(STRING) QString("<font color=green>%1</font><font color=black> </font>").arg(STRING)

/********** 建筑种类 **********/
#define BUILDING_HOME 0
#define BUILDING_GRANARY 1
#define BUILDING_CENTER 2
#define BUILDING_STOCK 3
#define BUILDING_FARM 4
#define BUILDING_MARKET 5
#define BUILDING_ARROWTOWER 6
#define BUILDING_ARROWTOWERPOSITION -1

/********** 建筑状态 **********/
#define BUILDING_FREE 0 // 建筑状态为空闲（无工作）时用0表示

/********** 建筑动作 **********/
#define BUILDING_CENTER_CREATEFARMER 1
#define BUILDING_CENTER_UPGRADE 2
#define BUILDING_GRANARY_ARROWTOWER 3
#define BUILDING_MARKET_WOOD_UPGRADE 4
#define BUILDING_MARKET_STONE_UPGRADE 5
#define BUILDING_MARKET_FARM_UPGRADE 6
#define BUILDING_CANCEL 12

/********** 建筑动作命名 **********/
#define BUILDING_GRANARY_ARROWTOWER_NAME "研发:建造箭塔(花费:50食物)"
#define BUILDING_CENTER_CREATEFARMER_NAME "创造村民(花费:50食物)"
#define BUILDING_CENTER_UPGRADE_NAME "演进到工具时代(花费:500食物)"
#define BUILDING_MARKET_WOOD_UPGRADE_NAME "研发木材加工:远程攻击距离+1,伐木+2(花费:120食物,75木头)"
#define BUILDING_MARKET_STONE_UPGRADE_NAME "研发石矿开采:采石+3(花费:100食物,50石头)"
#define BUILDING_MARKET_FARM_UPGRADE_NAME "研发驯养动物:农场食物产量+75(花费:200食物,50木头)"
#define BUILDING_ACTION_CANCEL_NAME "中止"

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

/********** 人物手持资源种类 **********/
#define HUMAN_WOOD 1
#define HUMAN_GRANARYFOOD 2
#define HUMAN_STONE 3
#define HUMAN_GOLD 4
#define HUMAN_STOCKFOOD 5

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

/********** 行动消耗资源 **********/
#define BUILD_HOUSE_WOOD 30
#define BUILD_GRANARY_WOOD 120
#define BUILD_STOCK_WOOD 120
#define BUILD_FARM_WOOD 75
#define BUILD_MARKET_WOOD 150
#define BUILD_ARROWTOWER_STONE 150
#define BUILDING_CENTER_CREATEFARMER_FOOD 50
#define BUILDING_CENTER_UPGRADE_FOOD 500
#define BUILDING_GRANARY_ARROWTOWER_FOOD 50
#define BUILDING_MARKET_WOOD_UPGRADE_FOOD 120
#define BUILDING_MARKET_WOOD_UPGRADE_WOOD 75
#define BUILDING_MARKET_STONE_UPGRADE_FOOD 100
#define BUILDING_MARKET_STONE_UPGRADE_STONE 50
#define BUILDING_MARKET_FARM_UPGRADE_FOOD 200
#define BUILDING_MARKET_FARM_UPGRADE_WOOD 50

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

#define ACT_CREATEFARMER 1
#define ACT_UPGRADE_AGE 2
#define ACT_UPGRADE_TOWERBUILD 3
#define ACT_UPGRADE_WOOD 4
#define ACT_UPGRADE_STONE 5
#define ACT_UPGRADE_FARM 6
#define ACT_STOP 7
#define ACT_BUILD 8
#define ACT_BUILD_HOUSE 9
#define ACT_BUILD_GRANARY 10
#define ACT_BUILD_STOCK 11
#define ACT_BUILD_CANCEL 12
#define ACT_BUILD_FARM 13
#define ACT_BUILD_MARKET 14
#define ACT_BUILD_ARROWTOWER 15
#define ACT_NULL 0
#define ACT_WINDOW_NUM_FREE 8

/********** 对象视野 **********/
#define VISION_FARMER 4
#define VISION_GAZELLE 2
#define VISION_LION 3
#define VISION_ELEPHANT 4
#define VISION_CENTER 4
#define VISION_HOME 4
#define VISION_GRANARY 4
#define VISION_STOCK 4
#define VISION_MARKET 4
#define VISION_FARM 4
#define VISION_ARROWTOWER 10


#define ANIMAL_TREE 0
#define ANIMAL_GAZELLE 1
#define ANIMAL_ELEPHANT 2
#define ANIMAL_LION 3
#define ANIMAL_FOREST 4

#define COORES_BUSH 0
#define COORES_STONE 1
#define COORES_GOLDORE 2

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
#define SORT_FARM 9

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
#define CNT_FARM 250
#define CNT_UPGRADEFARM 325
#define CNT_BUSH 150
#define CNT_STONE 250
#define CNT_GOLDORE 200
#define CNT_FOREST 300

#define ANIMAL_ATTACKRANGE_LION 10
#define ANIMAL_ATTACKRANGE_ELEPHANT 10

#define FARMER_VILLAGER 0
#define FARMER_LUMBER 1
#define FARMER_GATHERER 2
#define FARMER_MINER 3
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



#endif // CONFIG_H
