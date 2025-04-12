#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <QDebug>
#include <QMouseEvent>
#include <unordered_set>

#include "MainWidget.h"
#include "Coordinate.h"
#include "Core.h"

struct GameState{
    short m_heightMap[GENERATE_L][GENERATE_U];
    Block cell[MAP_L][MAP_U];
    std::list<Building*>myBuilding;
    std::list<Human*>myHuman;
    std::list<Building*>enemyBuilding;
    std::list<Human*>enemyHuman;
    std::list<Animal*>animal;
    std::list<StaticRes*>resource;
};

namespace Ui {
class GameWidget;
}

class GameWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GameWidget(QWidget *parent = 0);
    ~GameWidget();

    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void SaveCurrentState(void*state);
    void *RollBackState();
    void ResumePreState();

    int tranX(int DR, int UR);
    int tranY(int DR,int UR);
    int tranDR(int X,int Y);
    int tranUR(int X, int Y);

    void insert(Coordinate *p,std::list<Coordinate*> *drawlist);
    void drawmemory(int X, int Y,  ImageResource&res, int globalNum);
    void emptymemorymap();

    bool judgeinWindow(double x, double y);

    int getBlockDR(){
        return BlockDR;
    }
    int getBlockUR(){
        return BlockUR;
    }
    Ui::GameWidget *ui;

    int BlockDR=13;//左上角对应地图的坐标
    int BlockUR=34;//在建筑绘制中需要参考这个变量

    //由于左上角有半个block且上边这个blockl就是那半个块所在位置
    //于是对应到相应的L,U中坐标应该为

    double DR=(BlockDR+0.5)*16*gen5;
    double UR=(BlockUR+0.5)*16*gen5;
    //边长为16倍根号5

    MainWidget *mainwidget;

    //框选用，起点和终点
    QPoint selectionStartPos;
    QPoint selectionEndPos;

    //是否展示地图的格子线
    bool showLine = false;
    int buildMode = -1;
//    bool pos = false;
    deque<void*>AllState;

private slots:
    void movemap();
    void UpdateData();
    void setBuildMode(int buildMode);
signals:
    void sendView(int BlockL, int BlockU, int num);

};

#endif // GAMEWIDGET_H
