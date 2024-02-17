#ifndef CORE_H
#define CORE_H

#include <GlobalVariate.h>
#include <Map.h>
#include <Player.h>

class Core
{
public:
    Core();
    void gameUpdate(Map* map, Player* player[], int** memorymap,MouseEvent *mouseEvent);

    bool isValidPoint(const int (&map)[MAP_L][MAP_U], const Point& p);
    vector<Point> getAdjacentPoints(const int (&map)[MAP_L][MAP_U], const Point& p);
    stack<Point> findPath(const int (&findPathMap)[MAP_L][MAP_U],Map *map, const Point& start, const Point& destination);
    stack<Point> findPathAlternative(const int (&map)[MAP_L][MAP_U], const Point& start, const Point& destination);

    int tranBlockDR(double DR){return DR/BLOCKSIDELENGTH;}
    int tranBlockUR(double UR){return UR/BLOCKSIDELENGTH;}

    bool isObject_Free( Coordinate* object ){ return !relate_AllObject[object].isExist;}
    /** ******************************************
    *用于：判断对象是否处于空闲
    *传入：需要判断是否空闲的对象
    *传出:   true:当前对象已有关系，在关系表里
    *       false:当前对象不在表内
    */

private:
    Map* theMap;    //地图信息
    Player** player;    //player信息
    int** memorymap;    //记录出现在当前画面上的object,用于g_Object[]中访问
    MouseEvent *mouseEvent; //记录当前鼠标事件

    map<int , detail_EventPhase> relation_Event_static;     //静态表,描述行动的流程链的表
    map<Coordinate* , relation_Object> relate_AllObject;    //动态表,描述对象之间关系(行动)的表
    map<Coordinate* , relation_Object> relate_BuildingAndResource;

    /************管理添加表************/
    void manageMouseEvent();    //鼠标添加
    void manageOrder();     //指令添加

    /*********关系表相关维护***********/
    void manageRelationList();
    bool addRelation( Coordinate * object1, Coordinate * object2, int eventType);
    bool addRelation( Coordinate * object1, double DR , double UR, int eventType);
    void suspendRelation(Coordinate * object);  //这个删除，就很鸡肋，有智障的感觉  //但指令有手动取消的行动，故需保留

    /************控制行动************/
    void object_Move(Coordinate * object , double DR , double UR);  //控制移动
    void object_Attack(Coordinate* , Coordinate* ); //控制因object1影响object2血量
    void object_Gather(Coordinate* , Coordinate* ); //控制采集
    map<Coordinate* , relation_Object>::iterator object_FinishAction_Absolute(map<Coordinate* , relation_Object>::iterator);
    map<Coordinate* , relation_Object>::iterator object_FinishAction(map<Coordinate* , relation_Object>::iterator);

};

#endif // CORE_H
