#include "MoveObject.h"

//用于记录判断碰撞时需要检查的格子，[foundation][dblockDR][dblockUR];其中dblockDR和dblockUR均为实际值+1
//如左上[0][2]（实际为dblockDR = -1，dblockUR = 1）
vector<Point> MoveObject::jud_Block[2][3][3];

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
    nextBlockDR= transBlock(DR);
    nextBlockUR= transBlock(UR);
//    calculateDiretionArray(path);

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

//void MoveObject::calculateDiretionArray(stack<Point> &path)
//{
//    /**
//    *   dx  dy  值   意义     lk  rk
//    *   1   -1  0   下        2   0
//    *   0   -1  1   左下      1   0
//    *   -1  -1  2   左        0   0
//    *   -1  0   3   左上      0   1
//    *   -1  1   4   上        0   2
//    *   0   1   5   右上      1   2
//    *   1   1   6   右        2   2
//    *   1   0   7   右下      2   1
//    */
//    static int d_lab[3][3] = {\
//        {2 , 3 , 4},\
//        {1 , -1, 5},\
//        {0 , 7 , 6} \
//    };

//    stack<Point> pathCopy = path;  // 复制一份路径
//    this->pathN=0;
//    Point prevPoint(BlockDR,BlockUR);  // 第一个点
//    int dx,dy;
//    Point currentPoint;

//    while (!pathCopy.empty()) {
//        currentPoint = pathCopy.top();

//        dx = currentPoint.x - prevPoint.x;
//        dy = currentPoint.y - prevPoint.y;

//        // 根据dx和dy计算方向值，并将其添加到方向数组d中
//        d[pathN] = d_lab[dx+1][dy+1];

//        prevPoint = currentPoint;
//        pathCopy.pop();
//        pathN++;
//    }
//}

void MoveObject::resetpathIN()
{
    this->pathI=0;
    this->pathN=0;
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

//*************************移动处理
//void MoveObject::setNextBlock()
//{
//    if(pathI==0)
//    {
//        Point p=path.top();
//        this->nextBlockDR=p.x;
//        this->nextBlockUR=p.y;
//        this->nextDR=(p.x+0.5)*BLOCKSIDELENGTH;
//        this->nextUR=(p.y+0.5)*BLOCKSIDELENGTH;
//        path.pop();
//    }
//    else
//    {
//        Point p=path.top();
//        int lastBlockDR=nextBlockDR;
//        int lastBlockUR=nextBlockUR;
//        this->nextBlockDR=p.x;
//        this->nextBlockUR=p.y;
//        this->nextDR=DR+(nextBlockDR-lastBlockDR)*BLOCKSIDELENGTH;
//        this->nextUR=UR+(nextBlockUR-lastBlockUR)*BLOCKSIDELENGTH;
//        path.pop();
//    }
//}

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

//void MoveObject::updateMove()
//{
//    if(isWalking())
//    {
//        if(DR!=DR0||UR!=UR0)
//        {
//            PreviousDR=DR;
//            PreviousUR=UR;
//            if(pathN==0)
//            {
//                nextDR=DR0;
//                nextUR=UR0;
//                double dDR=nextDR-DR;
//                double dUR=nextUR-UR;
//                double dis = round(sqrt(dDR*dDR + dUR*dUR));  // 计算与目标之间的距离
//                // 根据速率计算每次的坐标变化量
//                double ratio = getSpeed() / static_cast<double>(dis);
//                VDR = round(dDR * ratio);
//                VUR = round(dUR * ratio);
//                if(countdistance(PreviousDR,PreviousUR,nextDR,nextUR)<getSpeed())
//                {
//                    PredictedDR=nextDR;
//                    PredictedUR=nextUR;
//                }
//                else
//                {
//                    PredictedDR=PreviousDR+VDR;
//                    PredictedUR=PreviousUR+VUR;
//                }
//                //                    qDebug()<<"predictedDR:"<<PredictedDR;
//                //                    qDebug()<<"predictedUR:"<<PredictedUR;
//                int tempAngle=calculateAngle(nextDR,nextUR);
//                if(tempAngle!=Angle)
//                {
//                    Angle=tempAngle;
//                    setNowRes();
//                }
//            }
//            else if(pathI==0&&pathI<pathN-1)
//            {
//                //                    qDebug()<<"pathI==0&&pathI<pathN-1";
//                double dDR=nextDR-DR;
//                double dUR=nextUR-UR;
//                double dis = round(sqrt(dDR*dDR + dUR*dUR));  // 计算与目标之间的距离
//                // 根据速率计算每次的坐标变化量
//                double ratio = getSpeed() / static_cast<double>(dis);
//                VDR = round(dDR * ratio);
//                VUR = round(dUR * ratio);
//                if(countdistance(PreviousDR,PreviousUR,nextDR,nextUR)<getSpeed())
//                {
//                    PredictedDR=nextDR;
//                    PredictedUR=nextUR;
//                }
//                else
//                {
//                    PredictedDR=PreviousDR+VDR;
//                    PredictedUR=PreviousUR+VUR;
//                }
//                int tempAngle=d[pathI];
//                if(tempAngle!=Angle)
//                {
//                    Angle=tempAngle;
//                    setNowRes();
//                }
//                if(fabs(nextDR-DR)<0.01&&fabs(nextUR-UR)<0.01)
//                {
//                    pathI++;
//                    setNextBlock();
//                }

//            }
//            else if(pathI<pathN-1)
//            {
//                if(Angle!=d[pathI])
//                {
//                    Angle=d[pathI];
//                    setNowRes();
//                }
//                VDR=VariationDR[Angle]*getSpeed();
//                VUR=VariationUR[Angle]*getSpeed();
//                if(countdistance(PreviousDR,PreviousUR,nextDR,nextUR)<getSpeed())
//                {
//                    PredictedDR=nextDR;
//                    PredictedUR=nextUR;
//                }
//                else
//                {
//                    PredictedDR=PreviousDR+VDR;
//                    PredictedUR=PreviousUR+VUR;
//                }
//                if(fabs(nextDR-DR)<DISTANCE_Manhattan_MoveEndNEAR&&fabs(nextUR-UR)<DISTANCE_Manhattan_MoveEndNEAR)
//                {
//                    setNextBlock();
//                    pathI++;
//                }
//            }
//            else if(pathI==pathN-1)
//            {
//                //                    qDebug()<<"pathI==pathN-1";
//                if(Angle!=d[pathI])
//                {
//                    Angle=d[pathI];
//                    setNowRes();
//                }
//                nextDR=DR0;
//                nextUR=UR0;
//                double dDR=nextDR-DR;
//                double dUR=nextUR-UR;
//                double dis = round(sqrt(dDR*dDR + dUR*dUR));  // 计算与目标之间的距离
//                // 根据速率计算每次的坐标变化量
//                double ratio = getSpeed() / static_cast<double>(dis);
//                VDR = round(dDR * ratio);
//                VUR = round(dUR * ratio);
//                if(countdistance(DR,UR,nextDR,nextUR)<getSpeed())
//                {
//                    PredictedDR=nextDR;
//                    PredictedUR=nextUR;
//                }
//                else
//                {
//                    PredictedDR=PreviousDR+VDR;
//                    PredictedUR=PreviousUR+VUR;
//                }
//                if(fabs(nextDR-DR)<DISTANCE_Manhattan_MoveEndNEAR&&fabs(nextUR-UR)<DISTANCE_Manhattan_MoveEndNEAR)
//                {
//                    pathI=0;
//                    pathN=0;
//                }
//            }
//        }
//    }
//    else
//    {
////        if(getSort() == SORT_FARMER) qDebug()<<"farmermoveF"<<PredictedDR<<PredictedUR<<DR<<UR;
//        VDR=0;
//        VUR=0;
//        PredictedDR = DR;
//        PredictedUR = UR;
//    }
//    this->BlockDR=DR/BLOCKSIDELENGTH;
//    this->BlockUR=UR/BLOCKSIDELENGTH;
//    //更新高度
//    this->imageH=DR-UR;
//}

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
    int dx,dy;

    if(isWalking() && (DR!=DR0||UR!=UR0))
    {
        PreviousDR = DR;
        PreviousUR = UR;

        dDR=nextDR-DR;  dUR=nextUR-UR;
        dx = nextBlockDR - PreviousBlockDR;
        dy = nextBlockUR - PreviousBlockUR;
        if(dx>0) dx = 1;
        else if(dx<0) dx = -1;
        if(dy>0) dy = 1;
        else if(dy<0) dy = -1;

        if( path.empty() )
        {
            dis = round(sqrt(dDR*dDR + dUR*dUR));  // 计算与目标之间的距离
            ratio = getSpeed() / static_cast<double>(dis);
            VDR = round(dDR * ratio);   VUR = round(dUR * ratio);

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

            change_Angel(d_lab[dx + 1][dy + 1]);
            update_PredictPoint();
            jud_ArrivePhaseGoal(dDR , dUR , DISTANCE_Manhattan_PathMove);
        }
        else
        {
            change_Angel(d_lab[dx + 1][dy + 1]);
            VDR=VariationDR[Angle]*getSpeed(); VUR=VariationUR[Angle]*getSpeed();

            update_PredictPoint();
            jud_ArrivePhaseGoal(dDR , dUR , DISTANCE_Manhattan_MoveEndNEAR);
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
    judOb->printer_ToMoveObject((void**)&moveOb);

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
                       " 与 " + judOb->getChineseName() + "(编号:" + QString::number(judOb->getglobalNum()) + " 发生碰撞");
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
