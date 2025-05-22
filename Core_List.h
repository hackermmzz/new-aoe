#ifndef CORE_LIST_H
#define CORE_LIST_H

#include "Core_CondiFunc.h"
#include<iostream>
#include <unordered_set>
#include<set>
#include<queue>
using namespace std;
extern Score usrScore;
extern Score enemyScore;

class Core_List
{
public:
    Core_List(){}
    Core_List(Map* theMap, Player* player[]);
    ~Core_List(){}


    void update();
    bool isObject_Free( Coordinate* object ){ return !relate_AllObject[object].isExist;}
    /** ******************************************
    *用于：判断对象是否处于空闲
    *传入：需要判断是否空闲的对象
    *传出:   true:当前对象已有关系，在关系表里
    *       false:当前对象不在表内
    */

    /*********关系表控制***********/
    int addRelation( Coordinate* object1, Coordinate * object2, int eventType , bool respond = true);
    int addRelation( Coordinate* object1, double DR , double UR, int eventType , bool respond = true);  //移动
    int addRelation( Coordinate* object1, int BlockDR , int BlockUR, int eventType , bool respond = true , int type = -1); //建造
    int addRelation( Coordinate* object1, int evenType , int actNum);  //建筑行动 actpercent
    void suspendRelation(Coordinate * object);  //指令手动停止
    void eraseRelation(Coordinate* object){ object->initAction(); relate_AllObject[object].isExist = false; } //指令因意外原因停止
    void eraseObject(Coordinate* eraseOb);
    void manageRelationList();


    /************关系表获取************/
    int getNowPhaseNum(Coordinate* object); //获取当前object的行动阶段，用于将信息传递给AIGame
    int getObjectSN(Coordinate* object);   //获取当前object的目标SN，用于将信息传递给AIGame


    /************管理诱发行动************/
    void conduct_Attacked(Coordinate*);  //受到攻击而诱发
    void manageMontorAct(); //添加监视的object的相应行动
    void resourceBuildHaveChange(){ this->resourceBuildingChange = true; }


private:
    Map* theMap;    //地图信息
    Player** player;    //player信息
    map<int , detail_EventPhase> relation_Event_static;     //静态表,描述行动的流程链的表
    map<Coordinate* , relation_Object> relate_AllObject;    //动态表,描述对象之间关系(行动)的表

    //寻路相关
    int map_HaveJud[MAP_L][MAP_U];
    int goalMap[MAP_L][MAP_U];

    bool resourceBuildingChange = false;
    bool needReset_resBuild = false;


    /*********初始化***********/
    void initDetailList();


    /*********关系表相关维护***********/
    void manageRelation_deleteGoalOb( Coordinate* goalObject );
    void manageRelation_updateMassage( Coordinate* );



    /*********辅助关系表维护***********/
    void findResourceBuiding( relation_Object& , list<Building*>&,Coordinate*obj1=0);
    bool canBuildDock(int BlockDR,int BlockUR,int w);
    int is_BuildingCanBuild(int buildtype , int BlockDR , int BlockUR ,int PlayerID, QString& chineseName);
    Missile* creatMissile(Coordinate* , Coordinate*);

    void deal_RangeAttack( Coordinate* attacker , Coordinate* attackee );

    void jud_resetResBuild(){ if(resourceBuildingChange){ resourceBuildingChange = false; needReset_resBuild = true; } }
    void init_resetResBuild(){ needReset_resBuild = false; }

    void requestSound_Action( Coordinate* object, int actionType, Coordinate* goalObject = NULL);


    /************控制行动************/
    void object_Move(Coordinate * object , double DR , double UR);  //控制移动
    void object_Attack(Coordinate* , Coordinate* ); //控制因object1影响object2血量
    void object_Gather(Coordinate* , Coordinate* ); //控制采集
    void object_Transport(Coordinate* , Coordinate* );//控制运输
     void object_Unload(Coordinate* , Coordinate* );//控制运输
    void object_ResourceChange( Coordinate* , relation_Object& );
    void object_RatioChange( Coordinate* , relation_Object& );  //控制完成度变化
    void object_FinishAction_Absolute(Coordinate*);
    void object_FinishAction(Coordinate*);


    /************寻路相关************/
    void initMap_HaveJud(){ memset(map_HaveJud , 0 ,sizeof(map_HaveJud)); }
    void haveJud_Map_Move( int blockDR , int blockUR ){ map_HaveJud[blockDR][blockUR] = true; }
    void haveJud_Map_Move(Point movePoi){ map_HaveJud[movePoi.x][movePoi.y] = true; }
    bool isHaveJud( int blockDR , int blockUR ){ return map_HaveJud[blockDR][blockUR]; }
    bool isHaveJud( Point judPoi){ return map_HaveJud[judPoi.x][judPoi.y]; }

    void setPath(MoveObject* moveOb, Coordinate* goalOb, double DR0, double UR0);
    void crashHandle(MoveObject* moveOb);
    void work_CrashPhase(MoveObject* moveOb);
    bool JudgeMoveObjIsShip(MoveObject*moveOb);
    pair<stack<Point>,array<double,2>> findPath(const int (&findPathMap)[MAP_L][MAP_U],Map *map, const Point& start, Point destination , Coordinate*object,Coordinate* goalOb);

    int tranBlockDR(double DR){return DR/BLOCKSIDELENGTH;}
    int tranBlockUR(double UR){return UR/BLOCKSIDELENGTH;}
    Point GetSameBlockInLine(const Point&point0,const Point&point1);
    Point GetSameBlockInLineNearest(const Point&point0,const Point&point1);
    bool checkIsCoast(int x,int y);
};

#endif // CORE_LIST_H
