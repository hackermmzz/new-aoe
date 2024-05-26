#ifndef CORE_H
#define CORE_H

#include <Core_List.h>

class SelectWidget;
class Core
{

public:
    SelectWidget *sel = nullptr;
    Map* theMap;    //地图信息

    Core(){}
    Core(Map* theMap, Player* player[], int** memorymap,MouseEvent *mouseEvent);
    void gameUpdate();
    void infoShare(int playerID);   ///将游戏信息同步给AIGame

    void getPlayerNowResource( int playerRepresent, int& wood, int& food, int& stone, int& gold );
    bool get_IsBuildAble(int playerRepresent,int buildNum){ return player[playerRepresent]->get_isBuildingAble(buildNum); }

    bool get_IsObjectFree(Coordinate* object){ return interactionList->isObject_Free(object); }
    bool get_IsActionNow( Coordinate* object ){ return !interactionList->isObject_Free(object); }

    void deleteOb_setNowobNULL(Coordinate* deOb){ if(deOb == nowobject) nowobject = NULL; }

    /************添加/删除表************/
    bool addRelation( Coordinate* object1, Coordinate * object2, int eventType , bool respond = true){ return interactionList->addRelation(object1,object2,eventType,respond); }
     //建造
    bool addRelation( Coordinate* object1, double DR , double UR, int eventType , bool respond = true , int type = -1){ return interactionList->addRelation(object1,DR,UR,eventType,respond,type); }
    bool addRelation( Coordinate* object1, int BlockDR , int BlockUR, int eventType , bool respond = true , int type = -1){ return interactionList->addRelation(object1,BlockDR,BlockUR,eventType,respond,type); }
    //建筑行动 actpercent
    bool addRelation( Coordinate* object1, int evenType , int actNum){ return interactionList->addRelation(object1,evenType,actNum); }
    //指令手动停止
    void suspendRelation(Coordinate * object){ interactionList->suspendRelation(object); }
    /************添加/删除表************/
private:
    Player** player;    //player信息
    int** memorymap;    //记录出现在当前画面上的object,用于g_Object[]中访问
    MouseEvent *mouseEvent; //记录当前鼠标事件
    Core_List* interactionList;


    /************管理添加表************/
    void manageMouseEvent();    //鼠标添加
    void manageOrder(int id);     //指令添加

//signals:
//    void clickOnObject();
    //点击对象时触发
};

#endif // CORE_H
