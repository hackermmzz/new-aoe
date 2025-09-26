#include "MainWidget.h"
#include <QApplication>
#include <QMap>
#include "Logger.h"
#include"EventFilter.h"
void ParseArguments(const QApplication&app);
int main(int argc, char* argv[])
{

    //
    QApplication app(argc, argv);
    QResource::registerResource("./res.rcc");
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
void ParseArguments(const QApplication&app){
    ////////////////////////////////解析参数
    QCommandLineParser parser;
    // 添加帮助选项（自动处理--help/-h）
    parser.addHelpOption();
    QCommandLineOption option0(
        QStringList()<<"exam",
         "开启考试模式",
                "true|false"
       );
    QCommandLineOption option1(
        QStringList()<<"indices",
         "本次在数据库得编号",
         "1"
       );
    QCommandLineOption option2(
        QStringList()<<"id",
         "学号",
         "923106840429"
       );
    QCommandLineOption option3(
        QStringList()<<"api",
         "请求的key",
         "api_dadawvavawfgaga"
       );
    QList<QCommandLineOption>options={option0,option1,option2,option3};
    parser.addOptions(options);
    parser.process(app);
    //
    if(parser.isSet("exam")){
        auto value=parser.value("exam");
        if(value=="true")IsExamining=true;
        else IsExamining=false;
    }
    //
    if(parser.isSet("indices")){
        auto value=parser.value("indices");
        Indices=value.toInt();
    }
    //
    if(parser.isSet("id")){
        auto value=parser.value("id");
        Id=value;
    }
    //
    if(parser.isSet("api")){
        auto value=parser.value("api");
        API_Value=value;
    }
}
