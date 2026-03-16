#include "GlobalVariate.h"
#include "MainWidget.h"
#include <QApplication>
#include <QMap>
#include "Logger.h"
#include"EventFilter.h"

int main(int argc, char* argv[])
{
    //开启GPU加速
    QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    //
    QApplication app(argc, argv);
    Logger::init(Logger::LogLevel::Debug);
    //创建网络插件
    NetworkManager=new NetworkPlugin(&app);
    NetworkManager->start();
    //解析参数
    ParseArguments(app);
    //安装全局事件器
    eventFilter=new EventFilter();
    app.installEventFilter(eventFilter);
    // 添加排除文件，这些文件不会被Logger处理
    Logger::addExcludedFile("EnemyAI.cpp");
    Logger::addExcludedFile("UsrAI.cpp");

    QStringList args = app.arguments();
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
    return app.exec();
}

