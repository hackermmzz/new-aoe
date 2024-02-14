#ifndef MOVEOBJECT_H
#define MOVEOBJECT_H

#include <Coordinate.h>

class MoveObject:public Coordinate
{
protected:
    double Blood;
    int MaxBlood;

    double speed;

    int vision;

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
    std::list<ImageResource> *nowlist=NULL;

    //攻击相关
    Coordinate* attackObject = NULL;    //攻击目标
    Coordinate* avangeObject = NULL;    //受到攻击的来源
    int attackType = ATTACKTYPE_CANTATTACK;     //攻击类型
    int atk = 0;    //攻击力
    double dis_Attack = DISTANCE_ATTACK_CLOSE;  //攻击距离

    /** 攻击间隔尚未理解其意义,故没有其的获取函数*/
    double inter_Attack = 0; //攻击间隔
    int defence_close = 0;  //肉搏防御
    int defence_shoot = 0;  //投射防御
    
public:
    MoveObject();

    /***********虚函数************/
    virtual void setNowRes(){ }

    virtual double getSpeed(){ return speed; }
    virtual int getBlood(){ return (int)( Blood*MaxBlood ); }
    virtual int getVision(){ return vision; }

    virtual int getATK(){ return atk; }
    virtual int getDEF(int attackType_got){  if(attackType_got == ATTACKTYPE_CLOSE) return defence_close;
                                             else if(attackType_got == ATTACKTYPE_SHOOT) return defence_shoot;}
    virtual double getDis_attack(){ return dis_Attack; }
    virtual int get_AttackType(){ return attackType; }
    /*********以上虚函数************/

    bool isWalking(){return this->nowstate==1;}
    void updateMove();
    bool isDie(){return Blood<=0;}


    void calculateDiretionArray(stack<Point>& path);
    int calculateAngle(double L0,double U0);

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
    void setNowState(int PreState)
    {
        this->nowstate=PreState;
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
