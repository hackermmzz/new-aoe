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
    int doActs(int actName);
    void actionUpdate();

    void setCore( Core* core ){ this->core = core; }
private:
    Ui::SelectWidget *ui;
//    Building * objBuilding;
//    Coores * objCoores;
//    GameRes * objGameRes;
//    Human * objHuman;
    int actions[ACT_WINDOW_NUM_FREE] = {0};
    int actionStatus[ACT_WINDOW_NUM_FREE] = {0};
    MainWidget *mainPtr;
    int elapsedSec = 0;
    int human_num = 0;
    int build_hold_human_num = 0;
    bool isGranaryBuilt = false;
    bool isStockBuilt = false;
    bool isMarketBuilt = false;

    Core* core = NULL;

public slots:
    void widgetAct(int num);
    void aiAct(int acter, int actName);
    void timeUpdate();
    void frameUpdate();
    void initActs();
    void getBuild(int BlockL, int BlockU, int num);
signals:
    void sendBuildMode(int buildMode);
};

#endif // SELECTWIDGET_H
