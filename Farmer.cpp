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

Farmer::Farmer(double DR, double UR)
{
    this->Angle=rand()%8;
    this->MaxBlood=BLOOD_FARMER;
    this->Blood=MaxBlood;
    this->state=HUMAN_STATE_IDLE;
    this->resource=0;
    this->DR=DR;
    this->UR=UR;
    this->BlockDR=DR/BLOCKSIDELENGTH;
    this->BlockUR=UR/BLOCKSIDELENGTH;
    this->nextBlockDR=BlockDR;
    this->nextBlockUR=BlockUR;
    this->PredictedDR=DR;
    this->PredictedUR=UR;
    this->PreviousDR=DR;
    this->PreviousUR=UR;
    this->DR0=DR;
    this->UR0=UR;
    this->nowstate=MOVEOBJECT_STATE_STAND;
    this->Angle=0;
    setNowRes();
    this->imageX=this->nowres->pix.width()/2.0;
    this->imageY=this->nowres->pix.width()/4.0;
    this->imageH=DR-UR;
    this->globalNum=g_globalNum;
    g_Object.insert({this->globalNum,this});
    g_globalNum++;

    speed = HUMAN_SPEED;
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

    updateMove();
}

int Farmer::getSort()
{
    return SORT_FARMER;
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
