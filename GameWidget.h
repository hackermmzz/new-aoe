#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <MainWidget.h>
#include <QDebug>
#include <Coordinate.h>
#include <QMouseEvent>
#include <unordered_set>

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
//    void keyPressEvent(QKeyEvent *event);

    int tranX(int L, int U);
    int tranY(int L,int U);
    int tranL(int X,int Y);
    int tranU(int X, int Y);

    void insert(Coordinate *p,std::list<Coordinate*> *drawlist);
    void drawmemory(int X, int Y, ImageResource res, int selectNum);
    void emptymemorymap();

private slots:
    void movemap();
    void UpdateData();

signals:

private:
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
    bool showLine = true;
//    bool pos = false;
};

#endif // GAMEWIDGET_H
