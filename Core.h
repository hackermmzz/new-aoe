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
    void infoShare();   ///将游戏信息同步给AIGame

    void getPlayerNowResource( int playerRepresent, int& wood, int& food, int& stone, int& gold );
    bool get_IsBuildAble(int playerRepresent,int buildNum){ return player[playerRepresent]->get_isBuildingAble(buildNum); }

    //获取指定Object是否空闲（不在交互表内）
    bool get_IsObjectFree(Coordinate* object){ return interactionList->isObject_Free(object); }
    //获取指定Object是否正在行动
    bool get_IsActionNow( Coordinate* object ){ return !interactionList->isObject_Free(object); }

    //如果指定object和全局nowobject指向同一地址，则设置nowobject为NULL
    void deleteOb_setNowobNULL(Coordinate* deOb){ if(deOb == nowobject) nowobject = NULL; }

    /************添加/删除表************/
    int addRelation( Coordinate* object1, Coordinate * object2, int eventType , bool respond = true){ return interactionList->addRelation(object1,object2,eventType,respond); }
    int addRelation( Coordinate* object1, double DR , double UR, int eventType , bool respond = true , int type = -1){ return interactionList->addRelation(object1,DR,UR,eventType,respond,type); }
    int addRelation( Coordinate* object1, int BlockDR , int BlockUR, int eventType , bool respond = true , int type = -1){ return interactionList->addRelation(object1,BlockDR,BlockUR,eventType,respond,type); }
    //建筑行动 actpercent
    int addRelation( Coordinate* object1, int evenType , int actNum){ return interactionList->addRelation(object1,evenType,actNum); }
    //指令手动停止
    void suspendRelation(Coordinate * object){ interactionList->suspendRelation(object); }
    /************添加/删除表************/
private:
    Player** player;    //player信息
    int** memorymap;    //记录出现在当前画面上的object,用于g_Object[]中访问
    MouseEvent *mouseEvent; //记录当前鼠标事件
    Core_List* interactionList;
    void updateByPlayer(int id,tagGame* newTagGames[]);  //更新tagGame

    /************管理添加表************/
    void manageMouseEvent();    //鼠标添加
    void manageOrder(int id);     //指令添加

};

#endif // CORE_H
