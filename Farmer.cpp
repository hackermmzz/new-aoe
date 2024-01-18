#include "Farmer.h"

std::list<ImageResource>* Farmer::Carry[5][8];
std::list<ImageResource>* Farmer::Walk[7][8];
std::list<ImageResource>* Farmer::Work[7][8];
std::list<ImageResource>* Farmer::Stand[7][8];
std::list<ImageResource>* Farmer::Attack[7][8];
std::list<ImageResource>* Farmer::Die[7][8];


std::string Farmer::FarmerName[7]={"Villager","Lumber","Gatherer","Miner","Hunter","Farmer","Worker"};
std::string Farmer::FarmerCarry[5]={"","CarryWood","CarryMeat","CarryStone","CarryGold"};

Farmer::Farmer()
{

}

void Farmer::nextframe()
{
    std::list<ImageResource> *nowlist=NULL;

    switch (this->nowstate) {
    case 0:
        nowlist=this->Stand[this->state][this->Angle];
        break;
    case 1:
        //            qDebug()<<"1";
        nowlist=this->Walk[this->state][this->Angle];
        break;
    case 2:
        nowlist=this->Attack[this->state][this->Angle];
        break;
    case 5:
        nowlist=this->Work[this->state][this->Angle];
        break;
        //    default:
        //        break;
    }


    nowres++;
    if(nowres==nowlist->end())
    {
        nowres=nowlist->begin();
        //读到最后回到最初
    }




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
                double ratio = HUMAN_SPEED / static_cast<double>(dis);
                VDR = round(dDR * ratio);
                VUR = round(dUR * ratio);
                if(countdistance(PreviousDR,PreviousUR,nextDR,nextUR)<HUMAN_SPEED)
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
                double ratio = HUMAN_SPEED / static_cast<double>(dis);
                VDR = round(dDR * ratio);
                VUR = round(dUR * ratio);
                if(countdistance(PreviousDR,PreviousUR,nextDR,nextUR)<HUMAN_SPEED)
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
                VDR=VariationDR[Angle]*HUMAN_SPEED;
                VUR=VariationUR[Angle]*HUMAN_SPEED;
                if(countdistance(PreviousDR,PreviousUR,nextDR,nextUR)<HUMAN_SPEED)
                {
                    PredictedDR=nextDR;
                    PredictedUR=nextUR;
                }
                else
                {
                    PredictedDR=PreviousDR+VDR;
                    PredictedUR=PreviousUR+VUR;
                }
                if(fabs(nextDR-DR)<0.00001&&fabs(nextUR-UR)<0.00001)
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
                double ratio = HUMAN_SPEED / static_cast<double>(dis);
                VDR = round(dDR * ratio);
                VUR = round(dUR * ratio);
                if(countdistance(DR,UR,nextDR,nextUR)<HUMAN_SPEED)
                {
                    PredictedDR=nextDR;
                    PredictedUR=nextUR;
                }
                else
                {
                    PredictedDR=PreviousDR+VDR;
                    PredictedUR=PreviousUR+VUR;
                }
                if(fabs(nextDR-DR)<0.00001&&fabs(nextUR-UR)<0.00001)
                {
                    pathI=0;
                    pathN=0;
                }
            }
        }

    }
    else
    {
        VDR=0;
        VUR=0;
    }
    this->BlockDR=DR/BLOCKSIDELENGTH;
    this->BlockUR=UR/BLOCKSIDELENGTH;
    //更新高度
    this->imageH=DR-UR;
}

void Farmer::setNowRes()
{

    switch (this->nowstate) {
    case MOVEOBJECT_STATE_STAND:
        this->nowres=this->Stand[this->state][this->Angle]->begin();
        break;
    case MOVEOBJECT_STATE_WALK:
        this->nowres=this->Walk[this->state][this->Angle]->begin();
        break;
    case MOVEOBJECT_STATE_ATTACK:
        this->nowres=this->Attack[this->state][this->Angle]->begin();
        break;
    case MOVEOBJECT_STATE_WORK:
        this->nowres=this->Work[this->state][this->Angle]->begin();
    default:
        break;
    }

}
