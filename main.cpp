#include "MainWidget.h"
#include <QApplication>
#include <QMap>
#include "Logger.h"
#include"EventFilter.h"
int main(int argc, char* argv[])
{
    //
    QApplication a(argc, argv);
    QResource::registerResource("./res.rcc");
    Logger::init(Logger::LogLevel::Debug);
    //安装全局事件器
    eventFilter=new EventFilter();
    a.installEventFilter(eventFilter);
    // 添加排除文件，这些文件不会被Logger处理
    Logger::addExcludedFile("EnemyAI.cpp");
    Logger::addExcludedFile("UsrAI.cpp");

    QStringList args = a.arguments();
    QMap<QString, int> mapOptions = {
        {"-l", 1},{"-last",1},    // 读取上一次的地图，"tmpMap.txt"
        {"-s", 2},{"-select",2}   // 读取指定地图，"gameMap.txt"
        // 添加更多启动参数...
    };
    int mapJudge = 0;  // 设置默认值

    //提取选项对应的参数
    for (const QString& arg : args) {
        if (mapOptions.contains(arg)) {
            mapJudge = mapOptions.value(arg);
        }
    }

    MainWidget w(mapJudge);
    w.show();
    return a.exec();
}
