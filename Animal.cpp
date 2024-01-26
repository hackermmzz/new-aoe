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
    this->visible = 0;
    
    //以下根据Num种类特判
    if( this->Num == ANIMAL_LION ) 
    {
        this->Friendly = 2;
        this->MaxCnt = CNT_LION;
        this->maxBlood = BLOOD_LION;
        speed = ANIMAL_SPEED;
    }
    else if( this->Num == ANIMAL_GAZELLE )
    {
        this->MaxCnt = CNT_GAZELLE;
        this->maxBlood = BLOOD_GAZELLE;
        speed = ANIMAL_SPEED;
    }
    else if( this->Num == ANIMAL_ELEPHANT )
    {
        this->MaxCnt = CNT_ELEPHANT;
        this->maxBlood = BLOOD_ELEPHANT;
        speed = ANIMAL_SPEED;
    }
    else if( this->Num == ANIMAL_TREE )
    {
        this->MaxCnt = CNT_TREE;
        this->maxBlood = BLOOD_TREE;
        speed = 0;
    }
    else if( this->Num == ANIMAL_FOREST )
    {
        this->MaxCnt = CNT_FOREST;
        this->maxBlood = BLOOD_FOREST;
        speed = 0;
    }
    
    //cnt与 maxcnt实际作用需区分
    this->Cnt =this->MaxCnt;
    this->blood = this->maxBlood;

    this->gatherable = true;

    this->state = ANIMAL_STATE_IDLE;

    this->Angle = 0;
    this->nowres = Stand[this->Num][this->Angle]->begin();

    this->imageX=this->nowres->pix.width()/2.0;
    this->imageY=this->nowres->pix.width()/4.0;

    this->globalNum=g_globalNum;
    g_Object.insert({this->globalNum,this});
    g_globalNum++;
    
    //
    this->Angle=2;
    this->nowres=Stand[this->Num][this->Angle]->begin();
    //
}

void Animal::nextframe()
{
    std::list<ImageResource> *nowlist=NULL;
    switch (this->nowstate) {
    case 0:
        nowlist=this->Stand[this->Num][this->Angle];
        break;
    case 1:
        nowlist=this->Walk[this->Num][this->Angle];
        break;
    case 2:
        nowlist=this->Attack[this->Num][this->Angle];
        break;
    case 3:
        nowlist=this->Die[this->Num][this->Angle];
        break;
    case 6:
        nowlist=this->Run[this->Num][this->Angle];
        break;
    }
    this->nowres++;
    if(this->nowres==nowlist->end())
    {
        nowres=nowlist->begin();
    }

    updateMove();
}

int Animal::getSort()
{
    return SORT_ANIMAL;
}
