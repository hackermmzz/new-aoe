#include "MainWidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QResource::registerResource("./res.rcc");

    bool MapJudge = true;
    QStringList args = a.arguments();
    if (args.contains("-f"))
    {
        MapJudge = false;
    }
    MainWidget w(MapJudge);
    w.show();

    return a.exec();
}
