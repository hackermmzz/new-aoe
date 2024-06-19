#include "Animal.h"
#include <QDebug>

 //以下为图片资源
std::string Animal::Animalname[5]={"Tree","Gazelle","Elephant","Lion","Forest"};
std::string Animal::Animalcarcassname[5]={"Fallen_Tree","Gazelle","Elephant","Lion","Forest_Stool"};
std::string Animal::AnimalDisplayName[5]={"树","瞪羚","大象","狮子","树林"};
std::list<ImageResource>* Animal::Walk[5][8];
std::list<ImageResource>* Animal::Stand[5][8];
std::list<ImageResource>* Animal::Attack[5][8];
std::list<ImageResource>* Animal::Die[5][8];
std::list<ImageResource>* Animal::Run[5][8];


Animal::Animal()
{

}

Animal::Animal(int Num, double DR, double UR)
{
    this->Num=Num;
    this->DR=DR;
    this->UR=UR;
    this->BlockDR = transBlock(this->DR);
    this->BlockUR = transBlock(this->UR);
    this->DR0=DR;
    this->UR0=UR;
    this->PredictedDR=DR;
    this->PredictedUR=UR;
    this->PreviousDR=DR;
    this->PreviousUR=UR;
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
//        BlockSizeLen = SIZELEN_SMALL;
        Friendly = FRIENDLY_FENCY;
        this->MaxCnt = CNT_ELEPHANT;
        resourceSort = HUMAN_STOCKFOOD;
        this->MaxBlood = BLOOD_ELEPHANT;
        speed = ANIMAL_SPEED;
        attackType = ATTACKTYPE_ANIMAL;
        isAttackable = true;
        vision = VISION_ELEPHANT;
        crashLength = CRASHBOX_BIGOB;

        nowres_step = NOWRES_TIMER_ELEPHANT;
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
//    this->nowres = Stand[this->Num][this->Angle]->begin();
//    this->Angle=2;
    setNowRes();

    this->imageX=this->nowres->pix.width()/2.0;
    this->imageY=this->nowres->pix.width()/4.0;

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
        }
        else if(!get_isActionEnd() && isNowresShift()) nowres++;
    }
    else
    {
        if(Num!=ANIMAL_TREE && Num!=ANIMAL_FOREST)
        {

            if(isNowresShift())
            {
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

int Animal::getSort()
{
    return SORT_ANIMAL;
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
            if(changeToRun) templist=this->Run[this->Num][this->Angle];
            else  templist=this->Walk[this->Num][this->Angle];
            break;
        case MOVEOBJECT_STATE_ATTACK:
            templist=this->Attack[this->Num][this->Angle];
            break;
        case MOVEOBJECT_STATE_DIE:
            templist=this->Die[this->Num][this->Angle];
            break;
        default:
            break;
        }
//        this->nowres = nowlist->begin();

        if(templist!=nowlist && templist)
        {
            nowlist = templist;
            nowres = nowlist->begin();
            initNowresTimer();
            initAttack_perCircle();
        }
    }
}
