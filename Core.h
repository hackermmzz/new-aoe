#ifndef CORE_H
#define CORE_H

#include <Core_List.h>

class SelectWidget;
class Core
{

public:
    Core(){}
    Core(Map* theMap, Player* player[], int** memorymap,MouseEvent *mouseEvent);
    void gameUpdate();
    void infoShare();   ///将游戏信息同步给AIGame
    SelectWidget *sel = nullptr;

private:
    Map* theMap;    //地图信息
    Player** player;    //player信息
    int** memorymap;    //记录出现在当前画面上的object,用于g_Object[]中访问
    MouseEvent *mouseEvent; //记录当前鼠标事件
    Core_List* interactionList;


    /************管理添加表************/
    void manageMouseEvent();    //鼠标添加
    void manageOrder();     //指令添加

//signals:
//    void clickOnObject();
    //点击对象时触发
};

#endif // CORE_H
