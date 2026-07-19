#include "Animal.h"

 //图片资源
array<std::string,5> Animal::Animalname;
array<std::string,5> Animal::Animalcarcassname;
array<std::string,5> Animal::AnimalDisplayName;
std::list<ImageResource>* Animal::Walk[5][8];
std::list<ImageResource>* Animal::Stand[5][8];
std::list<ImageResource>* Animal::Attack[5][8];
std::list<ImageResource>* Animal::Die[5][8];
std::list<ImageResource>* Animal::Run[5][8];
std::list<ImageResource>* Animal::Disappear[5][8];
//音效
array<std::string,5> Animal::sound_click;
//对象属性
//树， 瞪羚， 大象， 狮子， 森林
array<int,5> Animal::AnimalMaxBlood;
array<int,5>  Animal::AnimalResouceSort;
array<int,5> Animal::AnimalCnt;
array<int,5>  Animal::AnimalNowresStep;

array<int,5> Animal::AnimalVision;
array<Double,5> Animal::AnimalCrashLen;
array<Double,5> Animal::AnimalSpeed;
array<int,5>  Animal::AnimalFriendly;
array<bool,5> Animal::AnimalAttackable;
array<int,5> Animal::AnimalAtk;


/************构造与析构*************/
Animal::Animal(int Num, Double DR, Double UR)
{

    this->Num=Num;

    setDRUR(DR, UR);
    setDR0UR0(DR, UR);
    setPredictedDRUR(DR, UR);
    setPreviousDRUR(DR, UR);

    updateBlockByDetail();
    this->visible = 0;

    setAttribute();

    this->state = ANIMAL_STATE_IDLE;
    setNowRes();
    updateImageXYByNowRes();

    this->globalNum=10000*getSort()+g_globalNum;
    g_Object.insert({this->globalNum,this});
    g_globalNum++;
}


/************状态与图像显示*************/
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
        this->imageX=this->nowres->pix.width()/Double(2);
        this->imageY=this->nowres->pix.width()/Double(4);
    }
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


/*******状态与属性设置、获取*******/
void Animal::setAttribute()
{
    if(Num < 0 || Num >= 5)
    {
        incorrectNum = true;
        return;
    }
    int x=CNT_TREE;
    MaxBlood = AnimalMaxBlood[Num];
    resourceSort = AnimalResouceSort[Num];
    MaxCnt = AnimalCnt[Num];
    nowres_step = AnimalNowresStep[Num];

    vision = AnimalVision[Num];
    crashLength = AnimalCrashLen[Num];
    speed = AnimalSpeed[Num];
    Friendly = AnimalFriendly[Num];
    isAttackable = AnimalAttackable[Num];
    atk = AnimalAtk[Num];

    this->Cnt = this->MaxCnt;
    this->gatherable = false;
    this->Blood = 1;

    /***********特殊设定*************/
    if(Num == ANIMAL_FOREST)
        BlockSizeLen = SIZELEN_SMALL;

    if(Num == ANIMAL_FOREST || Num == ANIMAL_TREE)
    {
        this->Angle = 0;

        moveAble = false;
        treeState = rand()%Stand[this->Num][this->Angle]->size();
    }
    else
        this->Angle = rand()%8;

    if(Num == ANIMAL_ELEPHANT)
    {
        dis_Attack = DISTANCE_ELEPHANT_ATTACK;
        isRangeAttack = true;
    }

    if(Num == ANIMAL_ELEPHANT || Num == ANIMAL_LION)
        attackType = ATTACKTYPE_ANIMAL;

    setSideLenth();
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


/*******战斗相关*******/
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


/*******音乐与音效*******/
void Animal::requestSound_Die()
{
    if(!isInWidget())
        return;

    if(!isTree())
        soundQueue.push("Animal_Die");
}

void Animal::requestSound_Attack()
{
    if(!isInWidget())
        return;

    if(Num == ANIMAL_LION)
        soundQueue.push("Lion_Attack");
    else if(Num == ANIMAL_ELEPHANT)
        soundQueue.push("Elephant_Attack");
}


/*******静态函数*******/
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
