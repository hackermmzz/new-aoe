#ifndef MOVEOBJECT_H
#define MOVEOBJECT_H

#include "Coordinate.h"

class MoveObject:public Coordinate
{
protected:
    double speed;   //移动速度
    bool changeToRun = false;   //用于切换nowlist,

    int Angle;
    /** Angle规定
    *   对于Human、Animal类及派生类
    *       360度8等分， 从下顺时针分别为0 1 2 3 4 5 6 7
    *
    *   对于Missile类，角度划分视图像资源多少而定
    *       当前，spare和arrow 36等分
    *       投掷石头3等分
    */

    //下一个格子中的具体位置
    double nextDR,nextUR;
    //下一个格子的坐标位置
    int nextBlockDR,nextBlockUR;

    //目的地的格子位置
    int nextBlockDR0=0,nextBlockUR0=0;
    double DR0,UR0;//目的地

    int dMove_BDR = 0 , dMove_BUR = 0;
    //ΔDR ΔUR
    double VDR=0,VUR=0;
    // 单位向量
    double VariationDR[8] = {1 / sqrt(2), 0, -1 / sqrt(2), -1, -1 / sqrt(2), 0, 1 / sqrt(2), 1};
    double VariationUR[8] = {-1 / sqrt(2), -1, -1 / sqrt(2), 0, 1 / sqrt(2), 1, 1 / sqrt(2), 0};

    //上一位置
    double PreviousDR,PreviousUR;
    int PreviousBlockDR , PreviousBlockUR;
    //预判位置
    double PredictedDR,PredictedUR;

    stack<Point> path;    //记录路径
//    int pathN=0;    //路径长度
    int pathI=0;    //当前进行到路径的哪一步
    bool pathInit = false;

    int *d=new int[6400];   //计算好的路径方向

    Coordinate* crashOb = NULL;    //碰撞对象

    //stand walk attack die disappear work run
    int nowstate=0;//当前的状态
    int prestate=-1;//准备开始的状态 指示状态的切换
    bool changeToDisappear = false;

    /*********************静态数组***********************/
    //用于记录判断碰撞时需要检查的格子，[foundation][dblockDR][dblockUR];
    //其中dblockDR和dblockUR均为实际值+1
    //如左上[0][2]（实际为dblockDR = -1，dblockUR = 1）
    static vector<Point> jud_Block[2][3][3];
    static vector<Point> crashMove_Block[2][3][3];
    /*********************静态数组***********************/

    void update_path_useBlock();

    void resetpathIN(); //重置路径执行进度

    void setNextBlock();

    void change_Angel(int Angel_new);
    void update_PredictPoint();

    void jud_ArrivePhaseGoal(double dDR , double dUR , double distance);

    void update_moveDire( double dDR , double dUR );


    void setPredictedDRUR( double PredictedDR, double PredictedUR){ this->PredictedDR = PredictedDR; this->PredictedUR = PredictedUR; }

    void setPreviousDRUR( double PreviousDR, double PreviousUR ){ this->PreviousDR = PreviousDR; this->PreviousUR = PreviousUR; }
public:
    bool stateCrash=false;//用于传递状态给tagGame 判断碰撞

    MoveObject(){}

  /**********************虚函数**************************/
    virtual int calculateAngle(double nextDR, double nextUR);
    virtual double getSpeed(){ return speed; }
    bool get_isActionEnd(){ return this->nowres == prev(nowlist->end()); }

    void resetCoreAttribute(){ changeToRun = false; }
    void setDR0UR0( double DR0, double UR0 ){ this->DR0 = DR0; this->UR0 = UR0; }
    virtual void updateMove();
    /***************指针强制转化****************/
    void printer_ToMoveObject(void** ptr){ *ptr = this; }   //传入ptr为MoveObject类指针的地址,需要强制转换为（void**）
    /*************以上指针强制转化****************/
  /********************以上虚函数**************************/


  /*********************静态函数***********************/
    //设置需要判断碰撞的格子列表
    static void init_jud_Block( int foundation , int dblockDR, int dblockUR );
    static void init_crashMove_Block( int foundation, int dblockDR, int dblockUR );

  /*******************以上静态函数***********************/

    //设置状态（预设）
    void setPreStand(){this->prestate=MOVEOBJECT_STATE_STAND;}
    void setPreWalk(){this->prestate=MOVEOBJECT_STATE_WALK;}
    void setPreDie(){ this->prestate = MOVEOBJECT_STATE_DIE; }
    void setPreWork(){ this->prestate = MOVEOBJECT_STATE_WORK; }
    void setPreStateIsIdle(){this->prestate=-1;}
    //获取预设的状态
    int getPreState(){return this->prestate;}
    //实际设置状态
    void setNowState(int PreState);
    bool needTranState(){return this->prestate!=-1;}    //判断是否需要切换实际状态
    //判断moveObject对象当前状态
    bool isWalking(){return this->nowstate==MOVEOBJECT_STATE_WALK;}
    bool isDying(){ return this->nowstate == MOVEOBJECT_STATE_DIE; }
    bool isWorking(){ return this->nowstate == MOVEOBJECT_STATE_WORK; }
    bool isStand(){return this->nowstate == MOVEOBJECT_STATE_STAND;}

    //nowlist不使用walk，使用run
    void beginRun(){ changeToRun = true; }
    bool isDisappearing(){ return changeToDisappear; }

    //判断动作执行进程——通过nowres在nowlist中位置判断
    bool isAction_ResBegin(){ return nowres == nowlist->begin(); }

    void adjustAngle(double goalDR,double goalUR){ Angle = calculateAngle(goalDR,goalUR); }
    //*****路劲设置相关*****
    //设置寻路得到的路劲
    void setPath(const stack<Point>&path , double goalDR, double goalUR);
    //获取路径
    stack<Point> getPath(){return this->path;}
    int getPath_size(){ return path.size(); }
    //设置目标点
    void setdestination(double DR0,double UR0);
    void setDR0(double DR0);
    void setUR0(double UR0);
    //获取目标点
    double getDR0(){return this->DR0;}
    double getUR0(){return this->UR0;}

    void pathOptimize( Point addPoint );

    //*****处理移动*****
    void GoBackLU();
    void updateLU();
    double get_PredictedDR(){ return PredictedDR; }
    double get_PredictedUR(){ return PredictedUR; }
    Point get_PreviousBlock(){ return Point(PreviousBlockDR , PreviousBlockUR); }
    Point get_NextBlockPoint(){ return Point(nextBlockDR , nextBlockUR); }
    bool is_MoveFirstStep(){ return pathI == 0; }

    //******碰撞判断******
    //获取需要判断碰撞的格子
    vector<Point> get_JudCrush_Block();
    //判断是否碰撞
    bool isCrash(Coordinate* judOb);
    //初始化碰撞判断
    void initCrash(){ crashOb = NULL; }

    //获取碰撞对象
    Coordinate* getCrashOb(){ return crashOb;}
    Point getMoveDire(){ return Point(dMove_BDR,dMove_BUR); }
};

#endif // MOVEOBJECT_H
