#ifndef SELECTWIDGET_H
#define SELECTWIDGET_H

#include <QWidget>

#include "Coordinate.h"
#include "Core.h"


class MainWidget;
namespace Ui {
class SelectWidget;
}

class SelectWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SelectWidget(QWidget *parent = 0);
    ~SelectWidget();
    void paintEvent(QPaintEvent *event);
    void refreshActs();
    void updateActs();
    void drawActs();
    int doActs(int actName,Coordinate* nowobject=::nowobject);

    void setCore( Core* core ){ this->core = core; }

    int getSecend(){ return elapsedSec; }
    void resetSecond(){ elapsedSec = g_frame/25; elapsedFrame = (g_frame%25)*4; }
    //获取debug窗口显示时间
    QString getShowTime();
    int actions[ACT_WINDOW_NUM_FREE] = {0};
private:
    Ui::SelectWidget *ui;
    int actionStatus[ACT_WINDOW_NUM_FREE] = {0};
    MainWidget *mainPtr;

    //时间秒与帧 用于显示右下角计时器
    int elapsedSec = 0;
    int elapsedFrame = 0;

    Core* core = NULL;
    bool secondWidget_Build = false;

    void manageBuildBottom(int position, int actNum , int buildingNum );
    void showBuildActLab();
public slots:
    void widgetAct(int num);
    int  aiAct(int actName,Coordinate* self);
    void frameUpdate();
    void initActs();
    void getBuild(int BlockL, int BlockU, int num);
signals:
    void sendBuildMode(int buildMode);
};

#endif // SELECTWIDGET_H
