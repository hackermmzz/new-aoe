#include "GlobalVariate.h"
#include "MainWidget.h"
#include <QApplication>
#include <QMap>
#include "Logger.h"
#include"EventFilter.h"
int main(int argc, char* argv[])
{


    //
    QApplication app(argc, argv);
    Logger::init(Logger::LogLevel::Debug);
    //解析参数
    ParseArguments(app);
    //开启GPU加速
    QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    //创建网络插件
    NetworkManager=new NetworkPlugin(&app);
    NetworkManager->start();
    //安装全局事件器
    eventFilter=new EventFilter();
    app.installEventFilter(eventFilter);
    // 添加排除文件，这些文件不会被Logger处理
    Logger::addExcludedFile("EnemyAI.cpp");
    Logger::addExcludedFile("UsrAI.cpp");
    //运行窗口
    MainWidget w;
    w.show();
    return app.exec();
}

