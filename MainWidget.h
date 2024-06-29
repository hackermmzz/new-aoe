#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QPainter>
#include <Map.h>
#include <qtimer.h>
//#include <Player.h>
#include <QTextBrowser>
#include <GlobalVariate.h>
#include <config.h>
#include <QElapsedTimer>
#include <stack>
#include <queue>
#include <QButtonGroup>
#include <QMessageBox>

#include <windows.h>
#include <unordered_set>
#include <SelectWidget.h>
//#include <AboutDialog.h>
//#include <Option.h>
#include <Core.h>
#include "UsrAI.h"
#include "EnemyAI.h"
#include <ActWidget.h>

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
    void paintEvent(QPaintEvent *);
    void initBlock();
    void initBuilding();
    void initAnimal();
    void initStaticResource();
    void initFarmer();
    void initArmy();
    void initMissile();

    void deleteBlock();
    void deleteBuilding();
    void deleteAnimal();
    void deleteStaticResource();
    void deleteFarmer();
    void deleteArmy();
    void deleteMissile();

    //判断胜利
//    void judgeVictory();


    //**********************************************************
    //输出框

    void respond_DebugMessage();
    void debugText(const QString& color,const QString& content);
    void clearDebugText();


    //**********************************************************

    ActWidget* getActs(int num)
    {
        return acts[num];
    }

    void statusUpdate();
    void showPlayerResource(int playerRepresent);

    SelectWidget *sel;
    Core *core;
    AI* UsrAi,*EnemyAi;
    bool eventFilter(QObject *watched, QEvent *event);
    Map *map;
    int **memorymap=new int*[MEMORYROW];//动态
    Player* player[MAXPLAYER];
    MouseEvent *mouseEvent=new MouseEvent();
    QLabel *tipLbl =NULL;
private slots:
    void FrameUpdate();



signals:
    void mapmove();
private:
    Ui::MainWidget *ui;
    QTimer *timer;
    QTimer *showTimer;
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

//    std::list<Coordinate *> CollisionObject;
//    std::list<Farmer *> *nowselectList=new std::list<Farmer *>;
//    bool music = false;
//    bool sound = false;
//    bool pause = false;
//    bool select = false;
//    bool line = false;
//    bool pos = false;
//    bool showOverlap = true;
    ActWidget *acts[ACT_WINDOW_NUM_FREE];
};

#endif // MAINWIDGET_H
