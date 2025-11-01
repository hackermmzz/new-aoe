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

}
