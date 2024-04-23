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
    this->Blood=1;
    this->MaxBlood=BLOOD_FARMER;
    speed = HUMAN_SPEED;
    this->atk = 3;
    attackType = ATTACKTYPE_CLOSE;

    this->Angle=rand()%8;
    this->state=HUMAN_STATE_IDLE;
    this->resource=0;
    this->DR=DR;
    this->UR=UR;
    this->BlockDR=DR/BLOCKSIDELENGTH;
    this->BlockUR=UR/BLOCKSIDELENGTH;

    setSideLenth();
    this->nextBlockDR=BlockDR;
    this->nextBlockUR=BlockUR;
    this->PredictedDR=DR;
    this->PredictedUR=UR;
    this->PreviousDR=DR;
    this->PreviousUR=UR;
    this->DR0=DR;
    this->UR0=UR;
    this->nowstate=MOVEOBJECT_STATE_STAND;

    type_Missile = Missile_Spear;
//    this->Angle=0;
    setNowRes();
    this->imageX=this->nowres->pix.width()/2.0;
    this->imageY=this->nowres->pix.width()/4.0;
    this->imageH=DR-UR;
    this->globalNum=g_globalNum;
    g_Object.insert({this->globalNum,this});
    g_globalNum++;
}

void Farmer::nextframe()
{
    if(isDie())
    {
        if( !isDying() ) setPreDie();
        else if(!get_isActionEnd() ) nowres++;
    }
    else
    {
        updateState();
        nowres++;
        if(nowres==nowlist->end())
        {
            nowres=nowlist->begin();
            //读到最后回到最初
        }

        updateMove();
    }

    this->imageX=this->nowres->pix.width()/2.0;
    this->imageY=this->nowres->pix.width()/4.0;
}

int Farmer::getSort()
{
    return SORT_FARMER;
}

void Farmer::setNowRes()
{
    switch (this->nowstate) {
    case MOVEOBJECT_STATE_STAND:
        nowlist=this->Stand[this->state][this->Angle];
        break;
    case MOVEOBJECT_STATE_WALK:
        if(get_MatchingOfResourceAndCarry() && resource != 0 && (resourceSort!=HUMAN_GRANARYFOOD||state == FARMER_FARMER ))
            nowlist = this->Carry[this->resourceSort][this->Angle];
        else
            nowlist=this->Walk[this->state][this->Angle];
        break;
    case MOVEOBJECT_STATE_ATTACK:
        nowlist=this->Attack[this->state][this->Angle];
        break;
    case MOVEOBJECT_STATE_WORK:
        nowlist=this->Work[this->state][this->Angle];
        break;
    case MOVEOBJECT_STATE_DIE:
        nowlist = this->Die[this->state][this->Angle];
        break;
    default:
        break;
    }

    nowres = nowlist->begin();
}

double Farmer::getDis_attack()
{
    double dis;

    if(get_AttackType() == ATTACKTYPE_SHOOT) dis = 3 ;
    else dis = 0;

    if(dis == 0) dis = DISTANCE_ATTACK_CLOSE;
    else dis = ( dis + playerScience->get_addition_DisAttack(getSort(),type , 0 ,get_AttackType() ) )*BLOCKSIDELENGTH;

    return dis;

}

int Farmer::get_AttackType()
{
    if(attackObject != NULL && attackObject->getSort() == SORT_ANIMAL)
    {
        if(attackObject->getNum() == ANIMAL_TREE || attackObject->getNum() == ANIMAL_FOREST) return ATTACKTYPE_CLOSE_TOTREE;
        else return ATTACKTYPE_SHOOT;
    }
    return ATTACKTYPE_CLOSE;
}

void Farmer::updateState()
{
    switch (interactSort) {
    case SORT_ANIMAL:
        if(interactNum ==ANIMAL_TREE || interactNum==ANIMAL_FOREST) setState(1);
        else setState(4);
        break;
    case SORT_BUILDING:
        setState(6);
        break;
    case SORT_STATICRES:
        if(interactNum == NUM_STATICRES_Bush) setState(2);
        else setState(3);
        break;
    case SORT_Building_Resource:
        if(interactNum == BUILDING_FARM) setState(5);
        break;
    default:
        setState(0);
        break;
    }


}
