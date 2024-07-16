#include "Missile.h"

std::string Missile::missilename[NUMBER_MISSILE] = { "Spear" , "Arrow" , "Cobblestone"};
std::list<ImageResource>* Missile::missile[NUMBER_MISSILE];

Missile::Missile()
{

}

Missile::Missile(int type, Coordinate* attacker , Coordinate* attackee,int beginHeight , Development* playerScience, int playerRepresent)
{
    //划分玩家阵营
    this->playerScience = playerScience;
    this->playerRepresent = playerRepresent;
    recordAttacker(attacker);
    this->Num = type;
    this->DR = attacker->getDR();
    this->UR = attacker->getUR();
    this->DR0 = attackee->getDR();
    this->UR0 = attackee->getUR();
    BlockDR = transBlock(DR);
    BlockUR = transBlock(UR);
    Height_begin = beginHeight; //设置初始出发高度
    setAttribute();
    Angle = calculateAngle(this->DR0, this->UR0);
    setNowRes();
    globalNum = -1;
}

void Missile::setAttribute()
{
    switch (Num){
    case Missile_Spear:
        isAOE = false;
        isMandatoryArrive = false;
        speed = Missile_Speed_Spear;
        break;

    case Missile_Arrow:
        isAOE = false;
        isMandatoryArrive = false;
        speed = Missile_Speed_Arrow;
        break;

    case Missile_Cobblestone:
        isAOE = false;
        isMandatoryArrive = false;
        speed = Missile_Speed_Cobblestone;

    default:
        break;
    }
    calculateDMove();
}


void Missile::nextframe()
{
    updateMove();
    updateLU();
    this->imageX=this->nowres->pix.width()/2.0;
    this->imageY=this->nowres->pix.width()/8.0;
}

void Missile::setNowRes()
{
    nowlist = missile[Num];

    nowres = nowlist->begin();
    advance(nowres , Angle % (nowlist->size()));    //防止越界
}

void Missile::calculateDMove()
{
    double total;

    dDR = DR0 - DR;
    dUR = UR0 - UR;
    total = sqrt(dDR*dDR+dUR*dUR);

    if(total>0.1)
    {
        dDR = dDR*speed/total;
        dUR = dUR*speed/total;
    }
}

int Missile::calculateAngle(double nextDR, double nextUR)
{
    int tempAngle = 0 , partNum = 32;
    double dDR =nextDR - DR , dUR = nextUR - UR , sita = atan2(dUR,dDR) , gama , neta;
    const double pi = 3.1415926 ;
    double halfPi = pi/2, quarterPi = pi/4 , circle = 2*pi;

    if(sita>0)
    {
        gama = circle;

        neta = sita - halfPi;
        if(neta<0) gama -= sita*0.5;
        else gama -= quarterPi+ neta*1.5;
    }
    else
    {
        sita *=-1;
        neta = sita-halfPi;
        if(neta<0) gama = sita*1.5;
        else gama = halfPi + quarterPi + neta*0.5;
    }

    tempAngle = 3 + round( gama* partNum / circle );
    if(tempAngle>31) tempAngle-=32;

    return tempAngle;
}

void Missile::updateMove()
{
    if(!isWorking())setPreWalk();

    if(fabs(DR0-DR)<fabs(dDR) || fabs(UR0-UR)<fabs(dUR))
    {
        PredictedDR=DR0;
        PredictedUR=UR0;
    }
    else
    {
        PredictedDR=DR+dDR;
        PredictedUR=UR+dUR;
    }

    this->BlockDR=DR/BLOCKSIDELENGTH;
    this->BlockUR=UR/BLOCKSIDELENGTH;
    //更新高度
    this->imageH=DR-UR;
}


void Missile::recordAttacker(Coordinate* attacker)
{
    AttackSponsor = attacker;
    Building* record_build = NULL;
    Farmer* record_farmer = NULL;
    Army* record_army = NULL;

    this->Sort_attacker = attacker->getSort();
    //对象记录attacker信息，防止attacker死亡后无法访问
    switch (Sort_attacker) {
    case SORT_BUILDING:
        record_build = new Building();
        *record_build = *((Building*)attacker);
        AttackerRecord = record_build;
        break;
    case SORT_ARMY:
        record_army = new Army();
        *record_army = *((Army*)attacker);
        AttackerRecord = record_army;
        break;
    case SORT_FARMER:
        record_farmer = new Farmer();
        *record_farmer = *((Farmer*)attacker);
        AttackerRecord = record_farmer;
        break;
    default:
        break;
    }
}


BloodHaver* Missile::getAttackAponsor()
{
    BloodHaver* attacker = NULL;
    if(AttackSponsor!=NULL) AttackSponsor->printer_ToBloodHaver((void**)&attacker);
    else AttackerRecord->printer_ToBloodHaver((void**)&attacker);   //如果攻击发起者已经死亡，则使用记录的Attacker（进行攻击相关计算）

    return attacker;
}

void Missile::get_AttackSponsor_Position(double& DR , double& UR)
{
    if(isAttackerDie)
    {
        DR = AttackerRecord->getDR();
        UR = AttackerRecord->getUR();
    }
    else
    {
        DR = AttackSponsor->getDR();
        UR = AttackSponsor->getUR();
    }
}
