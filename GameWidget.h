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
    vector<vector<short>> m_heightMap;
    vector<vector<Block>> cell;
    std::list<Building*>myBuilding;
    std::list<Human*>myHuman;
    std::list<Building*>enemyBuilding;
    std::list<Human*>enemyHuman;
    std::list<Animal*>animal;
    std::list<StaticRes*>resource;
    GameState(){
        m_heightMap=decltype(m_heightMap)(GENERATE_L,vector<short>(GENERATE_U));
        cell=decltype(cell)(MAP_L,vector<Block>(MAP_U));
    }
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
    void paintEdge(QPainter&painter);
    void paintEdge(QPainter&painter,Double dr,Double ur,Double w,Double h,QColor color=Qt::white);
    void paintLine(QPainter&painter);
    void paintEffect(QPainter&painter);
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void SaveCurrentState(void*state);
    void *RollBackState();
    void ResumePreState();
    Double TranGlobalPosToDR(int x,int y);
    Double TranGlobalPosToUR(int x,int y);
    int tranX(int DR, int UR);
    int tranY(int DR,int UR);
    int tranDR(int X,int Y);
    int tranUR(int X, int Y);
    void insert(Coordinate *p,std::vector<Coordinate*> *drawlist);
    void drawmemory(int X, int Y,  ImageResource&res, int globalNum);
    void emptymemorymap();
    void AddEdge(Double dr,Double ur,Double w,Double h,QColor color=Qt::white);
    void AddLine(Double dr0,Double ur0,Double dr1,Double ur1,QColor color=Qt::white);
    bool judgeinWindow(Double x, Double y);
    QImage GenBoulderTrailEffect();
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

    Double DR=(BlockDR+Double("0.5"))*16*gen5;
    Double UR=(BlockUR+Double("0.5"))*16*gen5;
    //边长为16倍根号5

    MainWidget *mainwidget;
    //绘制到buffer上
    QPixmap gameBuffer;
    //框选用，起点和终点
    QPoint selectionStartPos;
    QPoint selectionEndPos;

    //是否展示地图的格子线
    bool showLine = false;
    int buildMode = -1;
//    bool pos = false;
    deque<void*>AllState;
    //需要矩形画线框的队列
    queue<tuple<Double,Double,Double,Double,QColor>>EdgeQueue;
    //需要画单条直线的队列
    queue<tuple<Double,Double,Double,Double,QColor>>LineQueue;
private slots:
    void movemap();
    void UpdateData();
    void setBuildMode(int buildMode);
signals:
    void sendView(int BlockL, int BlockU, int num);

};

#endif // GAMEWIDGET_H
