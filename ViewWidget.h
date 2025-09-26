#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include "Building.h"
#include "Farmer.h"
#include "Animal.h"
#include "MainWidget.h"

#include <QWidget>
#include <QPainter>

class ViewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ViewWidget(QWidget *parent = nullptr);
    void setFriendlyBuildList(std::list<Building *> *list)
    {
        this->friendlyBuildList = list;
    }
    void setFriendlyFarmerList(std::list<Human *> *list)
    {
        this->friendlyFarmerList = list;
    }
    void setEnemyBuildList(std::list<Building *> *list)
    {
        this->enemyBuildList = list;
    }
    void setEnemyFarmerList(std::list<Human *> *list)
    {
        this->enemyFarmerList = list;
    }
    void setResList(std::list<StaticRes *> *list)
    {
        this->resList = list;
    }
    void setAnimalList(std::list<Animal *> *list)
    {
        this->animalList = list;
    }
    void paintEvent(QPaintEvent *);
    int screenL;
    int screenU;
signals:
private:
    std::list<Building *> *friendlyBuildList=NULL ,*enemyBuildList=NULL;
    std::list<Human *> *friendlyFarmerList=NULL, *enemyFarmerList=NULL;
    std::list<StaticRes *> *resList=NULL;
    std::list<Animal *> *animalList=NULL;
//    std::list<Ruin *> *ruinList=NULL;
    MainWidget *mainwidget;
public slots:
};

#endif // VIEWWIDGET_H
