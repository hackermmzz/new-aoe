#ifndef CORE_CONDIFUNC_H
#define CORE_CONDIFUNC_H

#include "GlobalVariate.h"
#include "Map.h"
#include "Player.h"
#include "Logger.h"

//****************************************************************************************
//寻路相关
struct pathNode {
    static int cost_stra, cost_bias; //cost标准
    static std::map<Point, int> costLab;  //根据点之间位置，获得cost_total
    static Point goalPoint;     //记录终点
    Point position; //当前点
    int cost_predict = 0, cost_total = 0;

    int pathLength = 1;

    pathNode* preNode = NULL;   //记录前驱点

    //**************************
    pathNode() {}
    pathNode(Point newPoint);
    ~pathNode() {}

    //设置总目标点，每次寻路前需要调用
    static void setGoalPoint(int x, int y) { goalPoint.x = x; goalPoint.y = y; }

    //计算预期cost
    void calCost_predict() { cost_predict = calculateManhattanDistance(goalPoint.x, goalPoint.y, position.x, position.y) * 10; }
    //获取总cost
    int getCost() const { return cost_predict + cost_total; }

    //添加后续点，并返回后续点
    pathNode* pushNode(pathNode* nextNode);
    //获取输出
    Point toPoint() { return position; }

    bool operator <(const pathNode& __x) const { return getCost() < __x.getCost(); }
    bool operator >(const pathNode& __x) const { return getCost() > __x.getCost(); }
};

struct treeNode
{
    pathNode* value;
    treeNode* father = NULL, * lchild = NULL, * rchild = NULL;

    //**************************
    treeNode() {}
    treeNode(pathNode* newNode) { value = newNode; }
    ~treeNode() {}

    bool operator <(const treeNode& __x)const { return *value < *(__x.value); }
    bool operator >(const treeNode& __x)const { return *value > *(__x.value); }
};

struct lessHeap
{
    treeNode* head = NULL, * cacheNode = NULL;
    int nodeNum = 0;

    //**************************
    lessHeap() {}
    ~lessHeap();

    void clear();

    //返回堆顶最小元素
    treeNode* top() { return  head; }
    //删除头节点，并且调整小根堆
    void pop();
    //增加节点
    void addNode(pathNode* newNode);
    //小根堆上浮操作
    void floatUp(treeNode* obNode);
    //小根堆下沉操作
    void downChange();

private:
    void push_back();
    //节点交换，限一个父节点一个孩子节点
    void nodeChange(treeNode* __x, treeNode* __y);
};


//****************************************************************************************
//关系表结构体
struct relation_Object
{
    bool isExist, isUseAlterGoal = false;
    Coordinate* goalObject;     //目标对象
    Coordinate* alterOb;
    int sort;   //记录goalObject的Sort
    int resourceBuildingType;
    int relationAct;    //记录当前行动的种类
    int nowPhaseNum = 0;    //记录当前行动所属的detail阶段
    double DR_goal, UR_goal, DR_alter, UR_alter;   //移动等目标位置，alter为暂时更改的目标的位置
    double DR_Predicted, UR_Predicted;   //object1下一步的移动位置
    double crashLength_goal = 0, crashLength_alter = 0;
    double distance_AllowWork = 0, dis_AllowWork_alter = 0;  //若goalObject为可工作对象，human对其的可工作距离（游戏中具体距离）
    double distance_Record; //游戏中的距离的记录,为曼哈顿距离，用于更新relation
    double disAttack = 0;
    int height_Object = 0, height_GoalObject = 0;

    bool crash_DealPhase = false;
    Point crashMove_Point;
    stack<Point> crashPointLab;
    int crashRepresent = -1;

    bool nullPath;

    int times_Execution = 0;

    bool needResourceBuilding = false;
    bool respondConduct = false;

    //行动无用指标，当如移动长时间不实际进行时增加，当达到一定限度时被认为行动无用，强制结束行动
    int useless_norm = 0;

    int time_wait = 0;  //行动暂时停止时长，如为10，代表从当前帧暂停10帧

    //构造函数
    relation_Object() { isExist = false; goalObject = NULL; }
    relation_Object(int evenClass);
    relation_Object(Coordinate* goal, int eventClass);
    relation_Object(double DR_goal, double UR_goal, int eventClass);

    void set_goalPoint(double DR, double UR) { DR_goal = DR; UR_goal = UR; }
    void set_AlterPoint(double DR, double UR) { DR_alter = DR; UR_alter = UR; }

    void set_distance_AllowWork() {
        distance_AllowWork = goalObject->getSideLength() / 2.0 + 2 * CRASHBOX_SINGLEOB;
        //如果是建造船坞(首先得确定他是建筑)
        {
            void* obj = 0;goalObject->printer_ToBuilding(&obj);
            if (obj && goalObject->getNum() == BUILDING_DOCK)
            {
                distance_AllowWork = goalObject->getSideLength();
                return;
            }
        }
        //如果是捕鱼(首先得确定他是静态资源)
        {
            void* obj = 0;goalObject->printer_ToStaticRes(&obj);
            if (obj && goalObject->getNum() == NUM_STATICRES_Fish) {
                distance_AllowWork = goalObject->getSideLength() * 1.2;
                return;
            }
        }
    }

    void set_dis_AllowWork_alter() {
        dis_AllowWork_alter = alterOb->getSideLength() / 2.0 + 2 * CRASHBOX_SINGLEOB;
        {
            void* obj = 0;alterOb->printer_ToBuilding(&obj);
            if (obj && alterOb->getNum() == BUILDING_DOCK) {
                dis_AllowWork_alter = goalObject->getSideLength();
                return;
            }
        }
    }

    //如果goalObject是Resource的子类，则根据资源种类设置对应资源建筑的类型
    void set_ResourceBuildingType();

    void set_AlterOb(Coordinate* AlterObject, double dis_record);

    void reset_Object1Predicted(Coordinate* object1);

    void update_GoalPoint();

    void update_Attrib_alter();

    void init_AlterOb();

    void init_AttackAb(Coordinate* object1);

    void set_ExecutionTime(int times) { times_Execution = times; }
    void excutionOnce() { if (times_Execution > 0) times_Execution--; }
    bool is_ExecutionOver() { return times_Execution == 0; }

    void useless() { useless_norm++; }
    void useOnce()
    {
        if (useless_norm < 1)useless_norm = 0;
        else useless_norm--;
    }

    void wait(int time) { time_wait = time; }
    bool isWaiting() { return time_wait > 0; }
    void waiting() { if (time_wait > 0) time_wait--; }
};

struct conditionF
{
    int variableArgu;
    bool isNegation;
    bool (*condition)(Coordinate*, relation_Object&, int&, bool);

    //构造函数
    conditionF() {}
    conditionF(bool (*func)(Coordinate*, relation_Object&, int&, bool));
    conditionF(bool (*func)(Coordinate*, relation_Object&, int&, bool), int variableArgu);
    conditionF(bool (*func)(Coordinate*, relation_Object&, int&, bool), int variableArgu, bool isNegation);
};

struct detail_EventPhase
{
    /*关于条件
    * 对所有条件：true 代表执行切换阶段操作
    * 所有条件list中条件均为“或”连接
    */
    /*一定需要：phaseAmount<= CoreDetailLinkMaxNum-2*/

    int phaseAmount = 0;    //阶段总数
    int phaseInterrup = CoreDetailLinkMaxNum - 1;   //预设阶段 ， list中该位置存储强制中止行动的命令
    int phaseList[CoreDetailLinkMaxNum];      //core中，由此值指示进入函数
    conditionF chageCondition[CoreDetailLinkMaxNum - 2];   //切换phase的判断条件
    map<int, int> changeLinkedList;  //关系链，
    list<conditionF>forcedInterrupCondition;//细节控制的强制中止条件

    map< int, list<conditionF> > loopOverCondition;  //loop的中止条件
    /**
    * 第一个键值：    loopEndPhase 为loop中阶段编号最大的阶段，    如：loop:1->2->3->1，loopEndPhase为3
    * 第二个值：      loop的中止条件。在loopEndPhase进行判断，条件链表中有一个为真即中止
    *               loop中止后将进入loop后的一个阶段。   如：loop:1->2->3->1，  中止条件在3进行判断，判断中止loop后，进入阶段4
    */

    //构造函数
    detail_EventPhase();
    detail_EventPhase(int phaseAmount, int* theList, conditionF* conditionList, list< conditionF >forcedInterrupCondition);

    bool setLoop(int loopBeginPhase, int loopEndPhase, list<conditionF>overCondition);

    void setJump(int beginPhase, int endPhase) { changeLinkedList[beginPhase] = endPhase; }

    bool isLoop(int phase) { return phase<phaseAmount && phase>changeLinkedList[phase]; }
    //判断phase是否是loopEndPhase，是否需要loop中止判断
    //由于map对未在表内的键值自己设初值，可能会造成bug

    void setEnd_Absolute() { phaseList[phaseAmount] = CoreDetail_AbsoluteEnd; }
};

//****************************************************************************************
//关系函数定义
/** 参数表模板
 （ Coordinate* object1第一个对象，
    relation_Object& 对象的关系（内含目标对象） ，
    int& 操作控制符 ，
    bool 是否取反（true表取反，flase按原真值输出） ）

    条件函数返回true表示需要阶段切换/需要强制中止行动等
*/

//永真
bool condition_AllTrue(Coordinate*, relation_Object&, int&, bool);
//times次假
bool condition_TimesFalse(Coordinate*, relation_Object&, int&, bool);
//单一Object死亡
bool condition_UniObjectDie(Coordinate*, relation_Object&, int&, bool);
//单一object被攻击中
bool condition_UniObjectUnderAttack(Coordinate*, relation_Object&, int&, bool);
//单一object为NULL
bool condition_UniObjectNULL(Coordinate*, relation_Object&, int&, bool);
//单一object的行动完成度百分比
bool condition_UniObjectPercent(Coordinate*, relation_Object&, int&, bool);
//工作者背包空
bool condition_Object1_EmptyBackpack(Coordinate*, relation_Object&, int&, bool);
//工作者背包容量满
bool condition_Object1_FullBackpack(Coordinate*, relation_Object&, int&, bool);
//位置距离接近
bool condition_ObjectNearby(Coordinate*, relation_Object&, int&, bool);
//object目标能被采集
bool condition_Object2CanbeGather(Coordinate*, relation_Object&, int&, bool);
//object攻击进程
bool condition_Object1_AttackingEnd(Coordinate*, relation_Object&, int&, bool);
//取消判断无效的行动
bool condition_UselessAction(Coordinate*, relation_Object&, int&, bool);
//object目标已经被运输上船
bool condition_Object2_Transported(Coordinate* object1, relation_Object& relation, int& operate, bool isNegation);
//判断object是否可以卸货
bool condition_Object1_Unload(Coordinate* object1, relation_Object& relation, int& operate, bool isNegation);
//****************************************************************************************


#endif // CORE_CONDIFUNC_H
