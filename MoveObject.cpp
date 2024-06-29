#include "MoveObject.h"

//用于记录判断碰撞时需要检查的格子，[foundation][dblockDR][dblockUR];其中dblockDR和dblockUR均为实际值+1
//如左上[0][2]（实际为dblockDR = -1，dblockUR = 1）
vector<Point> MoveObject::jud_Block[2][3][3];

MoveObject::MoveObject()
{

}

//*****************************路径相关*********************************
//路径设置
void MoveObject::setPath(stack<Point> path , double goalDR, double goalUR)
{
    setdestination(goalDR,goalUR);
    if(!path.empty())
    {
//        stack<Point> tempStack;
//        Point bottomElement;
//        while (!path.empty())
//        {
//            tempStack.push(path.top());
//            path.pop();
//        }
//        bottomElement = tempStack.top();
//        // 在这里使用 bottomElement，即为堆栈的最底层元素
//        while (!tempStack.empty())
//        {
//            path.push(tempStack.top());
//            tempStack.pop();
//        }

//        this->nextBlockDR0=bottomElement.x;
//        this->nextBlockUR0=bottomElement.y;
        nextBlockDR0 = transBlock(goalDR);
        nextBlockUR0 = transBlock(goalUR);
    }
    this->path=path;
    resetpathIN();  //重置path的步数
    nextBlockDR=BlockDR;
    nextBlockUR=BlockUR;
    calculateDiretionArray(path);

    if(!path.empty())
    {
        setNextBlock();
    }

}

void MoveObject::calculateDiretionArray(stack<Point> &path)
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

    stack<Point> pathCopy = path;  // 复制一份路径
    this->pathN=0;
    Point prevPoint(BlockDR,BlockUR);  // 第一个点
    int dx,dy;
    Point currentPoint;

    while (!pathCopy.empty()) {
        currentPoint = pathCopy.top();

        dx = currentPoint.x - prevPoint.x;
        dy = currentPoint.y - prevPoint.y;

        // 根据dx和dy计算方向值，并将其添加到方向数组d中
//        if (dx == 1 && dy == -1) {
//            d[pathN] = 0;  // 下
//        } else if (dx == 0 && dy == -1) {
//            d[pathN] = 1;  // 左下
//        } else if (dx == -1 && dy == -1) {
//            d[pathN] = 2;  // 左
//        } else if (dx == -1 && dy == 0) {
//            d[pathN] = 3;  // 左上
//        }else if (dx == -1 && dy == 1) {
//            d[pathN] = 4;  // 上
//        }else if (dx == 0 && dy == 1) {
//            d[pathN] = 5;  // 右上
//        }else if (dx == 1 && dy == 1) {
//            d[pathN] = 6;  // 右
//        }else if (dx == 1 && dy == 0) {
//            d[pathN] = 7;  // 右下
//        }
        d[pathN] = d_lab[dx+1][dy+1];

        prevPoint = currentPoint;
        pathCopy.pop();
        pathN++;
    }
}

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

void MoveObject::setNextBlock()
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

//void MoveObject::setNextBlock()
//{
//    if(path.size())
//    {
//        Point p = path.top();
//        if(pathInit)
//        {
//            this->nextBlockDR=p.x;
//            this->nextBlockUR=p.y;
//            this->nextDR=(p.x+0.5)*BLOCKSIDELENGTH;
//            this->nextUR=(p.y+0.5)*BLOCKSIDELENGTH;
//            pathInit = false;
//        }
//        else
//        {
//            nextDR = DR+(p.x - nextBlockDR)*BLOCKSIDELENGTH;
//            nextUR = UR+(p.y - nextBlockUR)*BLOCKSIDELENGTH;
//            this->nextBlockDR=p.x;
//            this->nextBlockUR=p.y;
//        }
//        path.pop();
//    }
//}

void MoveObject::updateMove()
{
    if(isWalking())
    {
        if(DR!=DR0||UR!=UR0)
        {
            PreviousDR=DR;
            PreviousUR=UR;
            if(pathN==0)
            {
                nextDR=DR0;
                nextUR=UR0;
                double dDR=nextDR-DR;
                double dUR=nextUR-UR;
                double dis = round(sqrt(dDR*dDR + dUR*dUR));  // 计算与目标之间的距离
                // 根据速率计算每次的坐标变化量
                double ratio = getSpeed() / static_cast<double>(dis);
                VDR = round(dDR * ratio);
                VUR = round(dUR * ratio);
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
                //                    qDebug()<<"predictedDR:"<<PredictedDR;
                //                    qDebug()<<"predictedUR:"<<PredictedUR;
                int tempAngle=calculateAngle(nextDR,nextUR);
                if(tempAngle!=Angle)
                {
                    Angle=tempAngle;
                    setNowRes();
                }
            }
            else if(pathI==0&&pathI<pathN-1)
            {
                //                    qDebug()<<"pathI==0&&pathI<pathN-1";
                double dDR=nextDR-DR;
                double dUR=nextUR-UR;
                double dis = round(sqrt(dDR*dDR + dUR*dUR));  // 计算与目标之间的距离
                // 根据速率计算每次的坐标变化量
                double ratio = getSpeed() / static_cast<double>(dis);
                VDR = round(dDR * ratio);
                VUR = round(dUR * ratio);
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
                int tempAngle=d[pathI];
                if(tempAngle!=Angle)
                {
                    Angle=tempAngle;
                    setNowRes();
                }
                if(fabs(nextDR-DR)<0.01&&fabs(nextUR-UR)<0.01)
                {
                    pathI++;
                    setNextBlock();
                }

            }
            else if(pathI<pathN-1)
            {
                if(Angle!=d[pathI])
                {
                    Angle=d[pathI];
                    setNowRes();
                }
                VDR=VariationDR[Angle]*getSpeed();
                VUR=VariationUR[Angle]*getSpeed();
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
                if(fabs(nextDR-DR)<DISTANCE_Manhattan_MoveEndNEAR&&fabs(nextUR-UR)<DISTANCE_Manhattan_MoveEndNEAR)
                {
                    setNextBlock();
                    pathI++;
                }
            }
            else if(pathI==pathN-1)
            {
                //                    qDebug()<<"pathI==pathN-1";
                if(Angle!=d[pathI])
                {
                    Angle=d[pathI];
                    setNowRes();
                }
                nextDR=DR0;
                nextUR=UR0;
                double dDR=nextDR-DR;
                double dUR=nextUR-UR;
                double dis = round(sqrt(dDR*dDR + dUR*dUR));  // 计算与目标之间的距离
                // 根据速率计算每次的坐标变化量
                double ratio = getSpeed() / static_cast<double>(dis);
                VDR = round(dDR * ratio);
                VUR = round(dUR * ratio);
                if(countdistance(DR,UR,nextDR,nextUR)<getSpeed())
                {
                    PredictedDR=nextDR;
                    PredictedUR=nextUR;
                }
                else
                {
                    PredictedDR=PreviousDR+VDR;
                    PredictedUR=PreviousUR+VUR;
                }
                if(fabs(nextDR-DR)<DISTANCE_Manhattan_MoveEndNEAR&&fabs(nextUR-UR)<DISTANCE_Manhattan_MoveEndNEAR)
                {
                    pathI=0;
                    pathN=0;
                }
            }
        }
    }
    else
    {
//        if(getSort() == SORT_FARMER) qDebug()<<"farmermoveF"<<PredictedDR<<PredictedUR<<DR<<UR;
        VDR=0;
        VUR=0;
        PredictedDR = DR;
        PredictedUR = UR;
    }
    this->BlockDR=DR/BLOCKSIDELENGTH;
    this->BlockUR=UR/BLOCKSIDELENGTH;
    //更新高度
    this->imageH=DR-UR;
}

//void MoveObject::updateMove()
//{
//    double dDR,dUR,dis,ratio;
//    int dx,dy;
//    int tempAngle;

//    if(isWalking() && (DR!=DR0||UR!=UR0))
//    {
//        PreviousDR = DR;
//        PreviousUR = UR;

//        if(path.empty())
//        {
//            nextDR=DR0;
//            nextUR=UR0;
//            dDR=nextDR-DR;
//            dUR=nextUR-UR;
//            dis = round(sqrt(dDR*dDR + dUR*dUR));  // 计算与目标之间的距离
//            ratio = getSpeed() / static_cast<double>(dis);
//            VDR = round(dDR * ratio);
//            VUR = round(dUR * ratio);
//            if(countdistance(PreviousDR,PreviousUR,nextDR,nextUR)<getSpeed())
//            {
//                PredictedDR=nextDR;
//                PredictedUR=nextUR;
//            }
//            else
//            {
//                PredictedDR=PreviousDR+VDR;
//                PredictedUR=PreviousUR+VUR;
//            }

//            tempAngle = calculateAngle(nextDR,nextUR);
//            if(tempAngle != Angle)
//            {
//                Angle = tempAngle;
//                setNowRes();
//            }

//        }
//        else
//        {
//            dDR = nextDR - DR;
//            dUR = nextUR - UR;
//            if(dDR>0) dx = 1;
//            else if(dDR<0) dx = -1;
//            else dx = 0;

//            if(dUR > 0) dy = 1;
//            else if(dUR < 0) dy = -1;
//            else dy = 0;




//            if(Angle != d[pathI])
//            {
//                Angle=d[pathI];
//                setNowRes();
//            }

//            VDR=VariationDR[Angle]*getSpeed();
//            VUR=VariationUR[Angle]*getSpeed();

//            if(countdistance(PreviousDR,PreviousUR,nextDR,nextUR)<getSpeed())
//            {
//                PredictedDR=nextDR;
//                PredictedUR=nextUR;
//            }
//            else
//            {
//                PredictedDR=PreviousDR+VDR;
//                PredictedUR=PreviousUR+VUR;
//            }
//            if(fabs(nextDR-DR)<DISTANCE_Manhattan_MoveEndNEAR&&fabs(nextUR-UR)<DISTANCE_Manhattan_MoveEndNEAR)
//            {
//                setNextBlock();
//                pathI++;
//            }
//        }
//    }
//    else
//    {
//        VDR = 0;
//        VUR = 0;
//        PredictedDR = DR;
//        PredictedUR = UR;
//    }
//    imageH = DR-UR;
//}

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
