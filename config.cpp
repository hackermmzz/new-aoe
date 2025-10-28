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
bool MAP_EXPLORE;
bool MAP_VISIABLE;
vector<string>sound_work;







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
    MAP_EXPLORE=root["MAP_EXPLORE"].toBool();
    MAP_VISIABLE=root["MAP_VISIABLE"].toBool();
    //
    for(auto ele:root["sound_work"].toArray()){
        sound_work.push_back(ele.toString().toStdString());
    }
}
