#include "MoveObject.h"

//用于记录判断碰撞时需要检查的格子，[foundation][dblockDR][dblockUR];其中dblockDR和dblockUR均为实际值+1
//如左上[0][2]（实际为dblockDR = -1，dblockUR = 1）
vector<Point> MoveObject::jud_Block[2][3][3];
vector<Point> MoveObject::crashMove_Block[2][3][3];

//角度计算
int MoveObject::calculateAngle(double nextDR, double nextUR)
{
    int dDR=nextDR-DR;
    int dUR=nextUR-UR;
    int flag=Angle;
    if(abs(abs(dDR)-abs(dUR))>10)
    {
        if(abs(dDR)>abs(dUR))
        {
            if(dDR>0) flag=7;
            else flag=3;
        }
        else
        {
            if(dUR>0) flag=5;
            else flag=1;
        }
    }
    else
    {
        if(dDR>0&&dUR>0) flag=6;
        else if(dDR>0&&dUR<0) flag=0;
        else if(dDR<0&&dUR>0) flag=4;
        else flag=2;
    }
    //共8个方向
    return flag;
}

//更新状态
void MoveObject::setNowState(int PreState)
{
    this->nowstate=PreState;
    this->setNowRes();
}

void MoveObject::change_Angel(int Angel_new)
{
    if(Angel_new >=0 && Angel_new != Angle)
    {
        Angle=Angel_new;
        setNowRes();
    }
}

//*****************************路径相关*********************************
//路径设置
void MoveObject::setPath(stack<Point> path , double goalDR, double goalUR)
{
    setdestination(goalDR,goalUR);
    if(!path.empty())
    {
        nextBlockDR0 = transBlock(goalDR);
        nextBlockUR0 = transBlock(goalUR);
    }
    this->path=path;
    resetpathIN();  //重置path的步数

    update_path_useBlock();
}

void MoveObject::update_path_useBlock()
{
    PreviousBlockDR = transBlock(DR);
    PreviousBlockUR = transBlock(UR);

    if(!path.empty())
        setNextBlock();
    if(path.empty())
    {
        nextDR=DR0; nextUR=UR0;
        nextBlockDR = transBlock(nextDR);
        nextBlockUR = transBlock(nextUR);
    }
}

void MoveObject::resetpathIN()
{
    this->pathI=0;
//    this->pathN=0;
    pathInit = true;
}

void MoveObject::setdestination(double DR0,double UR0)
{
    setDR0(DR0);
    setUR0(UR0);
    BlockDR=transBlock(DR);
    BlockUR=transBlock(UR);
    imageH=DR-UR;//更新高度
}

void MoveObject::setDR0(double DR0)
{
    if(DR0<0)
        this->DR0=0;
    else if(DR0>MAP_L*BLOCKSIDELENGTH)
        this->DR0=MAP_L*BLOCKSIDELENGTH-1;
    else
        this->DR0=DR0;
}
void MoveObject::setUR0(double UR0)
{
    if(UR0<0)
        this->UR0=0;
    else if(UR0>MAP_U*BLOCKSIDELENGTH)
        this->UR0=MAP_U*BLOCKSIDELENGTH-1;
    else
        this->UR0=UR0;
}

void MoveObject::pathOptimize( Point addPoint )
{
    Point nextBlockPoint = Point(nextBlockDR,nextBlockUR);
    path.push(nextBlockPoint);
    if(!(addPoint == nextBlockPoint))
        path.push(addPoint);

    resetpathIN();
    update_path_useBlock();
}

//*************************移动处理
void MoveObject::setNextBlock()
{
    if(path.size())
    {
        Point p = path.top();
        if(pathInit)
        {
            nextBlockDR=p.x;
            nextBlockUR=p.y;
            nextDR=(p.x+0.5)*BLOCKSIDELENGTH;
            nextUR=(p.y+0.5)*BLOCKSIDELENGTH;
            pathInit = false;
        }
        else
        {
            nextDR = DR+(p.x - nextBlockDR)*BLOCKSIDELENGTH;
            nextUR = UR+(p.y - nextBlockUR)*BLOCKSIDELENGTH;
            nextBlockDR = p.x;
            nextBlockUR = p.y;
        }
        path.pop();
    }
}

void MoveObject::GoBackLU()
{
    this->PredictedDR=PreviousDR;
    this->PredictedUR=PreviousUR;
}

void MoveObject::updateLU()
{
    this->DR=PredictedDR;
    this->UR=PredictedUR;
    BlockDR = transBlock(DR);
    BlockUR = transBlock(UR);
}

void MoveObject::update_moveDire( double dDR , double dUR )
{
    //只要移动，一定会更新
    dMove_BDR = sgn(dDR);
    dMove_BUR = sgn(dUR);
}

void MoveObject::updateMove()
{
    /**
    *   dx  dy  值   意义     lk  rk
    *   1   -1  0   下        2   0
    *   0   -1  1   左下      1   0
    *   -1  -1  2   左        0   0
    *   -1  0   3   左上      0   1
    *   -1  1   4   上        0   2
    *   0   1   5   右上      1   2
    *   1   1   6   右        2   2
    *   1   0   7   右下      2   1
    */
    static int d_lab[3][3] = {\
        {2 , 3 , 4},\
        {1 , -1, 5},\
        {0 , 7 , 6} \
    };

    double dDR,dUR,dis,ratio;

    if(isWalking() && (DR!=DR0||UR!=UR0))
    {
        PreviousDR = DR;
        PreviousUR = UR;

        dDR=nextDR-DR;  dUR=nextUR-UR;
        if( path.empty() )
        {
            dis = round(sqrt(dDR*dDR + dUR*dUR));  // 计算与目标之间的距离
            ratio = getSpeed() / static_cast<double>(dis);
            VDR = round(dDR * ratio);   VUR = round(dUR * ratio);
            update_moveDire(dDR , dUR);
            //改变角度
            change_Angel( calculateAngle(nextDR,nextUR) );

            update_PredictPoint();
            jud_ArrivePhaseGoal(dDR , dUR , DISTANCE_Manhattan_MoveEndNEAR);
        }
        else if( pathI == 0 )
        {
            dis = round(sqrt(dDR*dDR + dUR*dUR));  // 计算与目标之间的距离
            ratio = getSpeed() / static_cast<double>(dis);
            VDR = round(dDR * ratio);   VUR = round(dUR * ratio);

            update_moveDire(nextBlockDR - PreviousBlockDR , nextBlockUR - PreviousBlockUR);
            change_Angel(d_lab[dMove_BDR + 1][dMove_BUR + 1]);
            update_PredictPoint();
            jud_ArrivePhaseGoal(dDR , dUR , DISTANCE_Manhattan_PathMove);
        }
        else
        {
            update_moveDire(nextBlockDR - PreviousBlockDR , nextBlockUR - PreviousBlockUR);
            change_Angel(d_lab[dMove_BDR + 1][dMove_BUR + 1]);
            VDR=VariationDR[Angle]*getSpeed(); VUR=VariationUR[Angle]*getSpeed();

            update_PredictPoint();
            jud_ArrivePhaseGoal(dDR , dUR , DISTANCE_Manhattan_PathMove);
        }
    }
    else
    {
        VDR = 0;    VUR = 0;
        PredictedDR = DR;   PredictedUR = UR;
    }
    BlockDR = transBlock(DR);   BlockUR = transBlock(UR);
    imageH = DR-UR;
}

void MoveObject::update_PredictPoint()
{
    if(countdistance(PreviousDR,PreviousUR,nextDR,nextUR)<getSpeed())
    {
        PredictedDR=nextDR;
        PredictedUR=nextUR;
    }
    else
    {
        PredictedDR=PreviousDR+VDR;
        PredictedUR=PreviousUR+VUR;
    }
}

void MoveObject::jud_ArrivePhaseGoal(double dDR , double dUR , double distance)
{
    if(fabs(dDR)<distance && fabs(dUR)<distance)
    {
        update_path_useBlock();
        pathI++;
    }
}

//*********************碰撞判断*********************
vector<Point> MoveObject::get_JudCrush_Block()
{
    int dblockDR,dblockUR;
    double dDR = PredictedDR - DR,dUR = PredictedUR - UR;
    vector<Point> blockLab;
    Point nowPosition(BlockDR,BlockUR);
    if(dDR == 0) dblockDR = 0;
    else dblockDR = dDR<0 ? -1 : 1;

    if(dUR == 0) dblockUR = 0;
    else dblockUR = dUR<0 ? -1 : 1;

    if(jud_Block[(int)BlockSizeLen][dblockDR+1][dblockUR+1].empty()) MoveObject::init_jud_Block((int)BlockSizeLen,dblockDR,dblockUR);

    blockLab = jud_Block[(int)BlockSizeLen][dblockDR+1][dblockUR+1];

    for(int i = 0 ; i<blockLab.size();i++) blockLab[i] = blockLab[i]+nowPosition;

    return blockLab;
}

bool MoveObject::isCrash(Coordinate* judOb)
{
    MoveObject* moveOb = NULL;
    double length = getCrashLength() + judOb->getCrashLength();
    double dx,dy;
    int blockDR_angle = sgn(DR - judOb->getDR()) , blockUR_angle = sgn(UR - judOb->getUR());
    judOb->printer_ToMoveObject((void**)&moveOb);

    if(blockDR_angle == dMove_BDR && blockUR_angle == dMove_BUR) return false;

    if(moveOb == NULL || !moveOb->isWalking())
    {
        dx = PredictedDR - judOb->getDR();
        dy = PredictedUR - judOb->getUR();
    }
    else
    {
        dx = PredictedDR - moveOb->get_PredictedDR();
        dy = PredictedUR - moveOb->get_PredictedUR();
    }

    if( fabs(dx)<length && fabs(dy)<length)
    {
        call_debugText("red", " 碰撞: " + getChineseName()+ "(编号:" + QString::number(getglobalNum()) +\
                       " 与 " + judOb->getChineseName() + "(编号:" + QString::number(judOb->getglobalNum()) + " 发生碰撞",getPlayerRepresent());
        crashOb = judOb;
        return true;
    }

    return false;
}

//静态函数，初始化判断碰撞的格子列表
void MoveObject::init_jud_Block( int foundation , int dblockDR, int dblockUR )
{
    Point temp;
    int slant_Jud = dblockDR & dblockUR;    //有一个为0则值为0
    int x = dblockDR,y = dblockUR , m = 1+foundation, outrow ,inrow;
    int lkey = dblockDR+1,rkey = dblockUR+1;

    //移动方向水平或竖直
    if(slant_Jud == 0)
    {
        if(dblockDR != 0)
        {
            outrow = x<0? -1 : foundation;
            inrow = outrow - x;
            for(int i = -1; i<m; i++)
            {
                temp.y = i;
                temp.x = outrow;
                jud_Block[foundation][lkey][rkey].push_back(temp);
                temp.x = inrow;
                jud_Block[foundation][lkey][rkey].push_back(temp);
            }

        }
        else if( dblockUR != 0)
        {
            outrow =  y < 0 ?  -1 : foundation;
            inrow = outrow -y;
            for(int i = -1; i<m;i++)
            {
                temp.x = i;
                temp.y = outrow;
                jud_Block[foundation][lkey][rkey].push_back(temp);
                temp.y = inrow;
                jud_Block[foundation][lkey][rkey].push_back(temp);
            }
        }
    }
    else    //否则为斜向
    {
        temp.x = x < 0 ? -1 : foundation;

        for(int i = -1; i < m; i++)
        {
            temp.y = i;
            jud_Block[foundation][lkey][rkey].push_back(temp);
        }

        if(x>0) m--;

        temp.y = y<0 ? -1 : foundation;
        for(int i = x<0 ? 0 : -1; i<m ; i++)
        {
            temp.x = i;
            jud_Block[foundation][lkey][rkey].push_back(temp);
        }

        temp.x = x<0 ? 0 : foundation -1 ;
        for(int i = 0 ; i < foundation ; i++)
        {
            temp.y = i;
            jud_Block[foundation][lkey][rkey].push_back(temp);
        }

        if(x>0) m = foundation -1;
        temp.y = y<0 ? 0 : foundation-1;
        for(int i = x<0 ? 1 : 0; i<m ; i++)
        {
            temp.x = i;
            jud_Block[foundation][lkey][rkey].push_back(temp);
        }
    }

    return;
}


void MoveObject::init_crashMove_Block( int foundation, int dblockDR, int dblockUR )
{
    //待编写以优化碰撞处理

}
