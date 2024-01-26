#include "MainWidget.h"
#include <QApplication>
#include <QResource>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 注册二进制文件信息
    QResource::registerResource("./res.rcc");

    int MapJudge = 0;
    QStringList args = a.arguments();
    if (args.indexOf("-f") != -1)
    {
        MapJudge = 1;
    }
    if(args.indexOf("-r") != -1)
    {
        MapJudge = 2;
    }
    MainWidget w(MapJudge);
    w.show();

    return a.exec();
}
