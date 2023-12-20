#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QPainter>
#include <Map.h>
#include <qtimer.h>
#include <Player.h>
#include <QTextBrowser>
#include <GlobalVariate.h>
#include <config.h>
//#include <Animal.h>
#include <QElapsedTimer>
//#include <Missile.h>
#include <stack>
#include <queue>
#include <QButtonGroup>
#include <QMessageBox>
//#include "AI.h"
#include <windows.h>
#include <unordered_set>
#include <Core.h>
//#include <AboutDialog.h>
//#include <Option.h>

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(int MapJudge, QWidget *parent = 0);
    ~MainWidget();

    initmap();

    void initBlock();
    void initBuilding();
    void initAnimal();

    void deleteBlock();
    void deleteBuilding();
    void deleteAnimal();

    Core *core=new Core();
    Map *map;
    Player* player[MAXPLAYER];
    int **memorymap=new int*[MEMORYROW];

private slots:
    void FrameUpdate();



signals:
    void mapmove();
private:
    Ui::MainWidget *ui;
    QTimer *timer;
//    QTimer *showTimer;
    int gameframe;
//    tagGame *Game=new tagGame;
//    int Winnning=0;
//    int Lose = 0;
//    int flag=0;
//    QButtonGroup *pbuttonGroup;
//    double *ArrowTowerBlockL=new double[3];
//    double *ArrowTowerBlockU=new double[3];
//    int ArrowTowerBuilt[3]={0,0,0};
//    AI* worker;
//    QThread* AIthread;
//    time_t t,t0;
//    int LastFrame = 0;
//    int CollisionMap[72][72];
//    int findPathMap[72][72];
//    std::list<Coordinate *> CollisionObject;
//    std::list<Farmer *> *nowselectList=new std::list<Farmer *>;
//    bool music = false;
//    bool sound = false;
//    bool pause = false;
//    bool select = false;
//    bool line = false;
//    bool pos = false;
//    bool showOverlap = true;
};

#endif // MAINWIDGET_H
