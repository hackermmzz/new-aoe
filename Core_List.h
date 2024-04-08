#ifndef CORE_LIST_H
#define CORE_LIST_H

#include <Core_CondiFunc.h>

class Core_List
{
public:
    Core_List(){ }
    Core_List(Map* theMap, Player* player[]);
    ~Core_List(){}

    bool isObject_Free( Coordinate* object ){ return !relate_AllObject[object].isExist;}
    /** ******************************************
    *用于：判断对象是否处于空闲
    *传入：需要判断是否空闲的对象
    *传出:   true:当前对象已有关系，在关系表里
    *       false:当前对象不在表内
    */

    /*********关系表控制***********/
    bool addRelation( Coordinate* object1, Coordinate * object2, int eventType , bool respond = true);
    bool addRelation( Coordinate* object1, double DR , double UR, int eventType , bool respond = true , int type = -1); //建造
    bool addRelation( Coordinate* object1, int evenType , int actNum);  //建筑行动 actpercent
    void suspendRelation(Coordinate * object);  //这个删除，就很鸡肋，有智障的感觉  //但指令有手动取消的行动，故需保留
    void eraseObject(Coordinate* eraseOb);
    void manageRelationList();

    /************管理诱发行动************/
    void conduct_Attacked(Coordinate*);

private:
    Map* theMap;    //地图信息
    Player** player;    //player信息
    map<int , detail_EventPhase> relation_Event_static;     //静态表,描述行动的流程链的表

    //需要优化，增加一个“NULL”表，goalObject为Null的行动移至该表，利于维护
    map<Coordinate* , relation_Object> relate_AllObject;    //动态表,描述对象之间关系(行动)的表

    void initDetailList();
    bool is_BuildingCanBuild(int buildtype , int BlockDR , int BlockUR){ return true;}
    Missile* creatMissile(Coordinate* , Coordinate*);

    /*********关系表相关维护***********/
    void manageRelation_deleteGoalOb( Coordinate* goalObject );
    void manageRelation_updateMassage( Coordinate* );
    void findResourceBuiding( relation_Object& , list<Building*>&);

    /************控制行动************/
    void object_Move(Coordinate * object , double DR , double UR);  //控制移动
    void object_Attack(Coordinate* , Coordinate* ); //控制因object1影响object2血量
    void object_Gather(Coordinate* , Coordinate* ); //控制采集
    void object_ResourceChange( Coordinate* , relation_Object& );
    void object_RatioChange( Coordinate* , relation_Object& );  //控制完成度变化
    void object_FinishAction_Absolute(Coordinate*);
    void object_FinishAction(Coordinate*);


    bool isValidPoint(const int (&map)[MAP_L][MAP_U], const Point& p);
    vector<Point> getAdjacentPoints(const int (&map)[MAP_L][MAP_U], const Point& p);
    stack<Point> findPath(const int (&findPathMap)[MAP_L][MAP_U],Map *map, const Point& start, const Point& destination);
    stack<Point> findPathAlternative(const int (&map)[MAP_L][MAP_U], const Point& start, const Point& destination);

    int tranBlockDR(double DR){return DR/BLOCKSIDELENGTH;}
    int tranBlockUR(double UR){return UR/BLOCKSIDELENGTH;}

};

#endif // CORE_LIST_H
