#include "MainWidget.h"
#include <QApplication>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
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
    MainWidget w(1);
    w.show();
    return a.exec();
}
