#include "MoveObject.h"

MoveObject::MoveObject()
{

}

void MoveObject::calculateDiretionArray(stack<Point> &path)
{
    stack<Point> pathCopy = path;  // 复制一份路径
    this->pathN=0;
    Point prevPoint;  // 第一个点
    prevPoint.x=BlockDR;
    prevPoint.y=BlockUR;
//    qDebug()<<"prevPoint.x"<<prevPoint.x;
//    qDebug()<<"prevPoint.y"<<prevPoint.y;

    while (!pathCopy.empty()) {
        Point currentPoint = pathCopy.top();
//        qDebug()<<"currentPoint.x"<<currentPoint.x;
//        qDebug()<<"currentPoint.y"<<currentPoint.y;
        int dx = currentPoint.x - prevPoint.x;
        int dy = currentPoint.y - prevPoint.y;
        // 根据dx和dy计算方向值，并将其添加到方向数组d中
        if (dx == 1 && dy == -1) {
            d[pathN] = 0;  // 下
        } else if (dx == 0 && dy == -1) {
            d[pathN] = 1;  // 左下
        } else if (dx == -1 && dy == -1) {
            d[pathN] = 2;  // 左
        } else if (dx == -1 && dy == 0) {
            d[pathN] = 3;  // 左上
        }else if (dx == -1 && dy == 1) {
            d[pathN] = 4;  // 上
        }else if (dx == 0 && dy == 1) {
            d[pathN] = 5;  // 右上
        }else if (dx == 1 && dy == 1) {
            d[pathN] = 6;  // 右
        }else if (dx == 1 && dy == 0) {
            d[pathN] = 7;  // 右下
        }
//        qDebug()<<"pathN:"<<pathN;
//        qDebug()<<"dx:"<<dx;
//        qDebug()<<"dy:"<<dy;
//        qDebug()<<"d[pathN]:"<<d[pathN];
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
