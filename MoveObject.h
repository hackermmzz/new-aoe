#ifndef MOVEOBJECT_H
#define MOVEOBJECT_H

#include <Coordinate.h>

class MoveObject:public Coordinate
{
protected:
    double speed;   //移动速度
    bool changeToRun = false;

    int Angle;
    //规定 从下顺时针分别为0 1 2 3 4 5 6 7

    double nextDR;
    double nextUR;
    //下一个格子中的具体位置

    int nextBlockDR;
    int nextBlockUR;
    //下一个格子的坐标位置

    int nextBlockDR0=0;
    int nextBlockUR0=0;
    //目的地的格子位置

    double VDR=0;
    double VUR=0;
    //ΔL ΔU

    double PreviousDR;
    double PreviousUR;
    //上一位置

    double PredictedDR;
    double PredictedUR;
    //预判位置

    double DR0;
    double UR0;//目的地

    stack<Point> path;
    //记录路径

    int *d=new int[6400];
    //计算好的路径方向

    int pathN=0;
    //路径长度

    int pathI=0;
    //当前进行到路径的哪一步

    // 单位向量
    double VariationDR[8] = {1 / sqrt(2), 0, -1 / sqrt(2), -1, -1 / sqrt(2), 0, 1 / sqrt(2), 1};
    double VariationUR[8] = {-1 / sqrt(2), -1, -1 / sqrt(2), 0, 1 / sqrt(2), 1, 1 / sqrt(2), 0};

    //stand walk attack die disappear work run
    int nowstate=0;//当前的状态
    int prestate=-1;//准备开始的状态 指示状态的切换
    
public:
    MoveObject();

  /**********************虚函数**************************/
    virtual int calculateAngle(double nextDR, double nextUR);
    virtual double getSpeed(){ return speed; }
    bool get_isActionEnd(){ return this->nowres == prev(nowlist->end()); }

    void resetCoreAttribute(){ changeToRun = false; }

    virtual void updateMove();
    /***************指针强制转化****************/
    void printer_ToMoveObject(void** ptr){ *ptr = this; }   //传入ptr为MoveObject类指针的地址,需要强制转换为（void**）
    /*************以上指针强制转化****************/
  /********************以上虚函数**************************/

    bool isWalking(){return this->nowstate==MOVEOBJECT_STATE_WALK;}
    bool isDying(){ return this->nowstate == MOVEOBJECT_STATE_DIE; }
    bool isWorking(){ return this->nowstate == MOVEOBJECT_STATE_WORK; }
    bool isStand(){return this->nowstate == MOVEOBJECT_STATE_STAND;}


    void beginRun(){ changeToRun = true; }

    void calculateDiretionArray(stack<Point>& path);

    void setPath(stack<Point> path)
    {
        if(!path.empty())
        {
            stack<Point> tempStack;
            Point bottomElement;
            while (!path.empty())
            {
                bottomElement = path.top();
                path.pop();
                tempStack.push(bottomElement);
            }
            // 在这里使用 bottomElement，即为堆栈的最底层元素
            while (!tempStack.empty())
            {
                path.push(tempStack.top());
                tempStack.pop();
            }
            this->nextBlockDR0=bottomElement.x;
            this->nextBlockUR0=bottomElement.y;
        }
        this->path=path;
        resetpathIN();
        nextBlockDR=BlockDR;
        nextBlockUR=BlockUR;
        calculateDiretionArray(path);
        if(!path.empty())
        {
            setNextBlock();
        }

    }
    stack<Point> getPath()
    {
        return this->path;
    }
    void setNextBlock()
    {
        if(pathI==0)
        {
            Point p=path.top();
            this->nextBlockDR=p.x;
            this->nextBlockUR=p.y;
            this->nextDR=(p.x+0.5)*BLOCKSIDELENGTH;
            this->nextUR=(p.y+0.5)*BLOCKSIDELENGTH;
            path.pop();
        }
        else
        {
            Point p=path.top();
            int lastBlockDR=nextBlockDR;
            int lastBlockUR=nextBlockUR;
            this->nextBlockDR=p.x;
            this->nextBlockUR=p.y;
            this->nextDR=DR+(nextBlockDR-lastBlockDR)*BLOCKSIDELENGTH;
            this->nextUR=UR+(nextBlockUR-lastBlockUR)*BLOCKSIDELENGTH;
            path.pop();
        }

    }
    void resetpathIN()
    {
        this->pathI=0;
        this->pathN=0;
    }
    void GoBackLU()
    {
        this->PredictedDR=PreviousDR;
        this->PredictedUR=PreviousUR;
    }
    void updateLU()
    {
        this->DR=PredictedDR;
        this->UR=PredictedUR;
    }

    bool needTranState()
    {
        return this->prestate!=-1;
    }

    void setdestination(double DR0,double UR0)
    {
        setDR0(DR0);
        setUR0(UR0);
        this->BlockDR=DR/BLOCKSIDELENGTH;
        this->BlockUR=UR/BLOCKSIDELENGTH;
        //更新高度
        this->imageH=DR-UR;
    }
    void setDR0(double DR0)
    {
        if(DR0<0)
        {
            this->DR0=0;
        }
        else if(DR0>MAP_L*BLOCKSIDELENGTH)
        {
            this->DR0=MAP_L*BLOCKSIDELENGTH-1;
        }
        else
        {
            this->DR0=DR0;
        }
    }
    void setUR0(double UR0)
    {
        if(UR0<0)
        {
            this->UR0=0;
        }
        else if(UR0>MAP_U*BLOCKSIDELENGTH)
        {
            this->UR0=MAP_U*BLOCKSIDELENGTH-1;
        }
        else
        {
            this->UR0=UR0;
        }
    }
    void setPreStand()
    {
        this->prestate=0;
    }
    void setPreWalk()
    {
        this->prestate=1;
    }
    void setPreDie(){ this->prestate = MOVEOBJECT_STATE_DIE; }
    void setPreWork(){ this->prestate = MOVEOBJECT_STATE_WORK; }
    void setNowState(int PreState)
    {
        this->nowstate=PreState;
        this->setNowRes();
    }
    void setPreStateIsIdle()
    {
        this->prestate=-1;
    }
    int getPreState()
    {
        return this->prestate;
    }
    double getDR0()
    {
        return this->DR0;
    }
    double getUR0()
    {
        return this->UR0;
    }

    //块、细节坐标转换
    double transDetail( int blockNum ){ return blockNum*BLOCKSIDELENGTH;  }
    int transBlock( double detailNum ){ return (int)detailNum/BLOCKSIDELENGTH; }
};

#endif // MOVEOBJECT_H
