#include "MoveObject.h"

MoveObject::MoveObject()
{

}

int MoveObject::calculateAngle(double L0, double U0)
{
    int dDR=DR0-DR;
    int dUR=UR0-UR;
    int flag=Angle;
    if(abs(abs(dDR)-abs(dUR))>10)
    {
        if(abs(dDR)>abs(dUR))
        {
            if(dDR>0)
            {
                flag=7;
            }
            else
            {
                flag=3;
            }
        }
        else
        {
            if(dUR>0)
            {
                flag=5;
            }
            else
            {
                flag=1;
            }
        }
    }
    else
    {
        if(dDR>0&&dUR>0)
        {
            flag=6;
        }
        else if(dDR>0&&dUR<0)
        {
            flag=0;
        }
        else if(dDR<0&&dUR>0)
        {
            flag=4;
        }
        else
        {
            flag=2;
        }
    }
    return flag;
}
