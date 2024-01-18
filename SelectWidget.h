#ifndef SELECTWIDGET_H
#define SELECTWIDGET_H

#include <QWidget>
#include <MainWidget.h>

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
    void setObjType(int objType)
    {
        this->objType = objType;
    }
    int getObjType()
    {
        return this->objType;
    }
    void setObj(Coordinate *obj)
    {
        this->obj = obj;
    }
    Coordinate* getObj()
    {
        return this->obj;
    }
    void refreshActs();
    void updateActs();
    void initActs();
    void drawActs();
    void doActs();
private:
    Ui::SelectWidget *ui;
//    Building * objBuilding;
//    Coores * objCoores;
//    GameRes * objGameRes;
//    Human * objHuman;
    Coordinate *obj=NULL;
    int objType;//
    int actions[ACT_WINDOW_NUM_FREE] = {0};
    int actionStatus[ACT_WINDOW_NUM_FREE] = {0};
    MainWidget *mainPtr;
private slots:
    void widgetAct(int num);
    void aiAct(int acter, int actName);
};

#endif // SELECTWIDGET_H
