#include "Animal.h"

 //以下为图片资源
std::string Animal::Animalname[5]={"Tree","Gazelle","Elephant","Lion","Forest"};
std::string Animal::Animalcarcassname[5]={"Fallen_Tree","Gazelle","Elephant","Lion","Forest_Stool"};
std::string Animal::AnimalDisplayName[5]={"树","瞪羚","大象","狮子","树林"};
std::list<ImageResource>* Animal::Walk[5][8];
std::list<ImageResource>* Animal::Stand[5][8];
std::list<ImageResource>* Animal::Attack[5][8];
std::list<ImageResource>* Animal::Die[5][8];
std::list<ImageResource>* Animal::Run[5][8];
std::list<ImageResource>* Animal::Disappear[5][8];

std::string Animal::sound_click[5] = {\
    "", "", "Elephant_Stand", "Lion_Stand", ""\
};

//树， 瞪羚， 狮子， 大象， 森林
int Animal::AnimalAtk[5] = { 0, 0, 2, 10, 0 };
int Animal::AnimalFriendly[5] = { FRIENDLY_NULL, FRIENDLY_FRI, FRIENDLY_ENEMY, FRIENDLY_FENCY, FRIENDLY_NULL };
int Animal::AnimalCnt[5] = { CNT_TREE, CNT_GAZELLE, CNT_LION, CNT_ELEPHANT, CNT_FOREST };
int Animal::AnimalResouceSort[5] = { HUMAN_WOOD, HUMAN_STOCKFOOD, HUMAN_STOCKFOOD, HUMAN_STOCKFOOD, HUMAN_WOOD };
int Animal::AnimalVision[5] = { 0, VISION_GAZELLE, VISION_LION, VISION_ELEPHANT, 0};
int Animal::AnimalCrashLen[5] = { CRASHBOX_SINGLEBLOCK, CRASHBOX_SINGLEOB, CRASHBOX_SMALLOB, CRASHBOX_BIGOB, CRASHBOX_SMALLBLOCK };



Animal::Animal(int Num, double DR, double UR)
{
    this->Num=Num;

    setDRUR(DR, UR);
    setDR0UR0(DR, UR);
    setPredictedDRUR(DR, UR);
    setPreviousDRUR(DR, UR);

    updateBlockByDetail();

    this->visible = 0;

    this->Angle = rand()%8;
    
    //以下根据Num种类特判
    if( this->Num == ANIMAL_LION ) 
    {
        atk = 2;
        this->Friendly = FRIENDLY_ENEMY;
        this->MaxCnt = CNT_LION;
        resourceSort = HUMAN_STOCKFOOD;
        this->MaxBlood = BLOOD_LION;
        speed = ANIMAL_SPEED;
        attackType = ATTACKTYPE_ANIMAL;
        isAttackable = true;
        vision = VISION_LION;
        crashLength = CRASHBOX_SMALLOB;

        nowres_step = NOWRES_TIMER_LION;
    }
    else if( this->Num == ANIMAL_GAZELLE )
    {
        Friendly = FRIENDLY_FRI;
        this->MaxCnt = CNT_GAZELLE;
        resourceSort = HUMAN_STOCKFOOD;
        this->MaxBlood = BLOOD_GAZELLE;
        speed = ANIMAL_SPEED;
        crashLength = CRASHBOX_SINGLEOB;

        vision = VISION_GAZELLE;
    }
    else if( this->Num == ANIMAL_ELEPHANT )
    {
        atk = 10;
        Friendly = FRIENDLY_FENCY;
        this->MaxCnt = CNT_ELEPHANT;
        resourceSort = HUMAN_STOCKFOOD;
        this->MaxBlood = BLOOD_ELEPHANT;
        speed = SPEED_ELEPHANT;
        attackType = ATTACKTYPE_ANIMAL;
        isAttackable = true;
        vision = VISION_ELEPHANT;
        crashLength = CRASHBOX_BIGOB;
        dis_Attack = DISTANCE_ATTACK_CLOSE_BIGOB;

        nowres_step = NOWRES_TIMER_ELEPHANT;
        isRangeAttack = true;
    }
    else if( this->Num == ANIMAL_TREE )
    {
        this->Angle = 0;
        this->MaxCnt = CNT_TREE;
        resourceSort = HUMAN_WOOD;
        this->MaxBlood = BLOOD_TREE;
        speed = 0;
        moveAble = false;
        crashLength = CRASHBOX_SINGLEBLOCK;

        treeState = rand()%Stand[this->Num][this->Angle]->size();
    }
    else if( this->Num == ANIMAL_FOREST )
    {
        this->Angle = 0;
        BlockSizeLen = SIZELEN_SMALL;
        this->MaxCnt = CNT_FOREST;
        resourceSort = HUMAN_WOOD;
        this->MaxBlood = BLOOD_FOREST;
        speed = 0;
        moveAble = false;
        treeState = rand()%Stand[this->Num][this->Angle]->size();
        crashLength = CRASHBOX_SMALLBLOCK;
    }
    else incorrectNum = true;

    setSideLenth();
    this->Cnt = this->MaxCnt;
    this->gatherable = false;
    this->Blood = 1;

    this->state = ANIMAL_STATE_IDLE;
    setNowRes();
    updateImageXYByNowRes();

    this->globalNum=10000*getSort()+g_globalNum;
    g_Object.insert({this->globalNum,this});
    g_globalNum++;
}

void Animal::nextframe()
{
    if(isDie())
    {
        if( !isDying() )
        {
            setPreDie();
            changeToGatherAble();  //死亡后，设置资源为可采集
            requestSound_Die();
        }
        else if(!get_isActionEnd() && isNowresShift()) nowres++;
        else if(!changeToDisappear && !is_Surplus())
        {
            changeToDisappear = true;
            if(!isTree())
            {
                nowres_step = 1000;
                setNowRes();
            }
        }
    }
    else
    {
        if(Num!=ANIMAL_TREE && Num!=ANIMAL_FOREST)
        {

            if(isNowresShift())
            {
                if(nowres == nowlist->begin() && nowstate == MOVEOBJECT_STATE_ATTACK)
                    requestSound_Attack();

                this->nowres++;
                if(this->nowres==nowlist->end())
                {
                    nowres=nowlist->begin();
                    initAttack_perCircle();
                }
            }

            updateMove();
        }
        this->imageX=this->nowres->pix.width()/2.0;
        this->imageY=this->nowres->pix.width()/4.0;
    }
}

void Animal::requestSound_Die()
{
    if(!isTree())
        soundQueue.push("Animal_Die");
}

bool Animal::isMonitorObject(Coordinate* judOb)
{
    int judNum = judOb->getNum(), judSort = judOb->getSort();
    if(Num == ANIMAL_LION)
        return judSort == SORT_ARMY || judSort == SORT_FARMER || (judSort == SORT_ANIMAL && judNum == ANIMAL_GAZELLE);
    if(Num == ANIMAL_GAZELLE)
        return judSort == SORT_ARMY || judSort == SORT_FARMER || (judSort == SORT_ANIMAL && judNum == ANIMAL_LION);
    return false;
}

void Animal::requestSound_Attack()
{
    if(Num == ANIMAL_LION)
        soundQueue.push("Lion_Attack");
    else if(Num == ANIMAL_ELEPHANT)
        soundQueue.push("Elephant_Attack");
}

void Animal::setNowRes()
{
    std::list<ImageResource> *templist = NULL;

    if(isTree())
    {
        if(nowstate == MOVEOBJECT_STATE_DIE)
        {
            nowlist=this->Die[this->Num][this->Angle];
            this->nowres = nowlist->begin();
        }
        else
        {
            nowlist=this->Stand[this->Num][this->Angle];
            nowres = next(nowlist->begin(),treeState);
        }
    }else
    {
        switch (this->nowstate) {
        case 0:
            templist=this->Stand[this->Num][this->Angle];
            break;
        case MOVEOBJECT_STATE_WALK:
            if(changeToRun && Num != ANIMAL_ELEPHANT) templist=this->Run[this->Num][this->Angle];
            else  templist=this->Walk[this->Num][this->Angle];
            break;
        case MOVEOBJECT_STATE_ATTACK:
            templist=this->Attack[this->Num][this->Angle];
            break;
        case MOVEOBJECT_STATE_DIE:
            if(changeToDisappear)templist = this->Disappear[this->Num][this->Angle];
            else templist=this->Die[this->Num][this->Angle];
            break;
        default:
            break;
        }

        if(templist!=nowlist && templist)
        {
            nowlist = templist;
            nowres = nowlist->begin();
            initNowresTimer();
            initAttack_perCircle();
        }
    }
}

int Animal::get_add_specialAttack()
{
    int addition = 0;
    if(Num == ANIMAL_ELEPHANT)
    {
        if(interactSort == SORT_BUILDING )
        {
            if(interactNum == BUILDING_ARROWTOWER) addition +=40;
            else if(interactNum == BUILDING_WALL) addition +=80;
        }
    }

    return addition;
}

void Animal::deallocateWalk(int i, int j)
{
    delete Walk[i][j];
    Walk[i][j] = nullptr;
}

void Animal::deallocateStand(int i, int j)
{
    delete Stand[i][j];
    Stand[i][j] = nullptr;
}

void Animal::deallocateAttack(int i, int j)
{
    delete Attack[i][j];
    Attack[i][j] = nullptr;
}

void Animal::deallocateDie(int i, int j)
{
    delete Die[i][j];
    Die[i][j] = nullptr;
}

void Animal::deallocateRun(int i, int j)
{
    delete Run[i][j];
    Run[i][j] = nullptr;
}

void Animal::deallocateDisappear(int num, int angle)
{
    delete Disappear[num][angle];
    Disappear[num][angle] = nullptr;
}
