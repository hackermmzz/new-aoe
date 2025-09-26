#include "Farmer.h"

std::list<ImageResource>* Farmer::Carry[7][8];
std::list<ImageResource>* Farmer::Walk[8][8];
std::list<ImageResource>* Farmer::Work[8][8];
std::list<ImageResource>* Farmer::Stand[8][8];
std::list<ImageResource>* Farmer::Attack[8][8];
std::list<ImageResource>* Farmer::Die[8][8];
std::list<ImageResource>* Farmer::Disappear[8][8];
std::list<ImageResource>* Farmer::ShipStand[10][8];
std::string Farmer::FarmerName[8]={"Villager","Lumber","Gatherer","Miner","Hunter","Farmer","Worker","Fisher"};
std::string Farmer::FarmerCarry[7]={"","CarryWood","CarryMeat","CarryStone","CarryGold","","CarryFish"};

string Farmer::sound_click = "Click_Villager";

std::string Farmer::sound_work[7] = {\
    "", "Cut", "Gather", "Mine", "Archer_Attack", "Plow", "Build"\
};

Farmer::Farmer(double DR, double UR , Development* playerScience, int playerRepresent,int farmerType_)
{
    FarmerType=farmerType_;
    this->playerScience = playerScience;
    this->playerRepresent = playerRepresent;

    this->Blood=1;
    this->MaxBlood=BLOOD_FARMER;
    setSpeed();
    this->atk = 3;
    attackType = ATTACKTYPE_CLOSE;
    phaseFromEnd_MissionAttack = THROWMISSION_FARMER;
    vision = VISION_FARMER;

    nowres_step = NOWRES_TIMER_FARMER;

    this->Angle=rand()%8;
    this->state=HUMAN_STATE_IDLE;
    this->resource=0;
    this->DR=DR;
    this->UR=UR;
    this->BlockDR=DR/BLOCKSIDELENGTH;
    this->BlockUR=UR/BLOCKSIDELENGTH;

    setSideLenth();
    crashLength = CRASHBOX_SINGLEOB;
    this->nextBlockDR=BlockDR;
    this->nextBlockUR=BlockUR;
    this->PredictedDR=DR;
    this->PredictedUR=UR;
    this->PreviousDR=DR;
    this->PreviousUR=UR;
    this->DR0=DR;
    this->UR0=UR;
    this->nowstate=MOVEOBJECT_STATE_STAND;

    isAttackable = true;
    type_Missile = Missile_Spear;
    setNowRes();
    this->imageX=this->nowres->pix.width()/2.0;
    this->imageY=this->nowres->pix.width()/4.0;
    this->imageH=DR-UR;
    this->globalNum=10000*getSort()+g_globalNum;
    g_Object.insert({this->globalNum,this});
    g_globalNum++;
}

void Farmer::nextframe()
{
    if(isDie())
    {
        if( !isDying() )
        {
            setPreDie();
            requestSound_Die();
        }
        else if(!get_isActionEnd() && isNowresShift())
        {
            if(nowres == nowlist->begin() && nowstate == MOVEOBJECT_STATE_ATTACK)
            {
                if(state == FARMER_HUNTER && isInWidget())
                     soundQueue.push(sound_work[state]);
            }

            nowres++;
            if( !changeToDisappear && get_isActionEnd())
            {
                changeToDisappear = true;
                nowres_step = 1000;
                setNowRes();
            }
        }

    }
    else
    {
        updateState();

        if(isNowresShift())
        {
            nowres++;
            if(nowres==nowlist->end())
            {
                requestSound_Work();
                nowres=nowlist->begin();
                initAttack_perCircle();
                //读到最后回到最初
            }            
        }

        updateMove();
        setNowRes();
    }

    updateImageXYByNowRes();
}


void Farmer::setNowRes()
{
    std::list<ImageResource> *templist = NULL;
    if(FarmerType==FARMERTYPE_FARMER){
    switch (this->nowstate) {
        case MOVEOBJECT_STATE_STAND:
            templist=this->Stand[this->state][this->Angle];
            break;
        case MOVEOBJECT_STATE_WALK:
            if(get_MatchingOfResourceAndCarry() && resource > 0 && resourceSort!=HUMAN_GRANARYFOOD)
                templist = this->Carry[this->resourceSort][this->Angle];
            else
                templist=this->Walk[this->state][this->Angle];
            break;
        case MOVEOBJECT_STATE_ATTACK:
            templist=this->Attack[this->state][this->Angle];
            break;
        case MOVEOBJECT_STATE_WORK:
            templist=this->Work[this->state][this->Angle];
            break;
        case MOVEOBJECT_STATE_DIE:
            if(changeToDisappear) templist = this->Disappear[this->state][this->Angle];
            else templist = this->Die[this->state][this->Angle];
            break;
        default:
            break;
        }
    }else if(FarmerType==FARMERTYPE_SAILING||FarmerType==FARMERTYPE_WOOD_BOAT){
        switch (nowstate)
        {
            case MOVEOBJECT_STATE_STAND:case MOVEOBJECT_STATE_WALK:case MOVEOBJECT_STATE_ATTACK:case MOVEOBJECT_STATE_WORK:
            templist=this->ShipStand[FarmerType][this->Angle];
            break;
        }
    }
    if(templist!=nowlist && templist)
    {
        nowlist = templist;
        nowres = nowlist->begin();
        initNowresTimer();
        initAttack_perCircle();
    }
}

double Farmer::getDis_attack()
{
    double dis;

    if(get_AttackType() == ATTACKTYPE_SHOOT) dis = 3 ;
    else dis = 0;

    if(dis == 0)
    {
        dis = DISTANCE_ATTACK_CLOSE;

        if(attackObject != NULL)
            dis += (attackObject->getSideLength())/2.0 ;
    }
    else dis = ( dis + playerScience->get_addition_DisAttack(getSort(), Num , 0 ,get_AttackType() ) )*BLOCKSIDELENGTH;

    return dis;
}

int Farmer::get_AttackType()
{
    if(interactSort== SORT_ANIMAL)
    {
        if(interactNum == ANIMAL_TREE || interactNum == ANIMAL_FOREST) return ATTACKTYPE_CLOSE_TOTREE;
        else return ATTACKTYPE_SHOOT;
    }
    return ATTACKTYPE_CLOSE;
}

int Farmer::get_add_specialAttack()
{
    if(interactSort == SORT_ANIMAL) return 1;
    else return 0;
}

void Farmer::setSpeed()
{
    if(FarmerType!=FARMERTYPE_WOOD_BOAT)speed=HUMAN_SPEED;
    else speed=WOOD_BOAT_SPEED;
}

void Farmer::updateState()
{
    if(FarmerType==FARMERTYPE_FARMER){
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
            else if(interactNum==NUM_STATICRES_Fish)setState(FARMER_FISHER);
            else setState(3);
            break;
        case SORT_Building_Resource:
            if(interactBui_builtUp)
            {
                if(interactNum == BUILDING_FARM)
                    setState(5);
            }
            else setState(6);
            break;
        default:
            setState(0);
            break;
        }
    }
}

int Farmer::farmer_addition_ResourceHold()
{
    if(resourceSort==HUMAN_DOCKFOOD&&FarmerType==FARMERTYPE_SAILING)
        return 5;
    return 0;
}

vector<Human *>& Farmer::getHumanTransport()
{
    return HumanTransport;
}


void Farmer::requestSound_Work()
{
    if(!isInWidget())
        return;

    if( nowstate == MOVEOBJECT_STATE_WORK &&\
        ( state == FARMER_LUMBER || state == FARMER_MINER || state == FARMER_WORKER|| state == FARMER_FARMER || state == FARMER_GATHERER )\
        || nowstate == MOVEOBJECT_STATE_ATTACK && state == FARMER_LUMBER
    )
        soundQueue.push(sound_work[state]);

    return;
}
