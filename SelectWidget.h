#ifndef SELECTWIDGET_H
#define SELECTWIDGET_H

#include <QWidget>
#include <MainWidget.h>
#include <Core.h>
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
    void actionUpdate();

    void setCore( Core* core ){ this->core = core; }

    //获取debug窗口显示时间
    QString getShowTime();
    int actions[ACT_WINDOW_NUM_FREE] = {0};
private:
    Ui::SelectWidget *ui;
//    Building * objBuilding;
//    Coores * objCoores;
//    GameRes * objGameRes;
//    Human * objHuman;
    int actionStatus[ACT_WINDOW_NUM_FREE] = {0};
    MainWidget *mainPtr;

    //时间秒与帧
    int elapsedSec = 0;
    int elapsedFrame = 0;

    int human_num = 0;
    int build_hold_human_num = 0;
    bool isGranaryBuilt = false;
    bool isStockBuilt = false;
    bool isMarketBuilt = false;

    Core* core = NULL;

    void manageBuildBottom(int position, int actNum , int buildingNum );
    void setShowTimeFrame();
public slots:
    void widgetAct(int num);
    int  aiAct(int actName,Coordinate* self);
    void timeUpdate();
    void frameUpdate();
    void initActs();
    void getBuild(int BlockL, int BlockU, int num);
signals:
    void sendBuildMode(int buildMode);
};

#endif // SELECTWIDGET_H
