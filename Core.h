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



    int tranBlockDR(double DR)
    {
        return DR/BLOCKSIDELENGTH;
    }
    int tranBlockUR(double UR)
    {
        return UR/BLOCKSIDELENGTH;
    }

    bool isObject_Free( Coordinate* object ){ return !relate_AllObject[object].isExist;}
    /** ******************************************
    *用于：判断对象是否处于空闲
    *传入：需要判断是否空闲的对象
    *传出:   true:当前对象已有关系，在关系表里
    *       false:当前对象不在表内
    */

private:
    Map* theMap;
    Player** player;
    int** memorymap;
    MouseEvent *mouseEvent;

    map<int , detail_EventPhase> relation_Event_static;
    map<Coordinate* , relation_Object> relate_AllObject;
    map<Coordinate* , relation_Object> relate_BuildingAndResource;

    void manageMouseEvent();
    void manageOrder();
    //关系表相关维护
    void manageRelationList();
    bool addRelation( Coordinate * object1, Coordinate * object2, int eventType);
    bool addRelation( Coordinate * object1, double DR , double UR, int eventType);
    void suspendRelation(Coordinate * object);  //这个删除，就很鸡肋，有智障的感觉  //但指令有手动取消的行动，故需保留
    //控制行动
    void object_Move(Coordinate * object , double DR , double UR);
    map<Coordinate* , relation_Object>::iterator object_FinishAction_Absolute(map<Coordinate* , relation_Object>::iterator iter)
    {
        return relate_AllObject.erase(iter);
    }
    map<Coordinate* , relation_Object>::iterator object_FinishAction(map<Coordinate* , relation_Object>::iterator);

};

#endif // CORE_H
