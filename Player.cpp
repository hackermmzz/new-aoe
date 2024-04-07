#include "Player.h"

Player::Player()
{
    playerScience = new Development();
}

Player::Player(int represent)
{
    this->represent = represent;
    playerScience = new Development();
}

Player::~Player()
{
    delete playerScience;
    //清空池子
    list<Human*>::iterator iter_deleHuman = human.begin();
    list<Building*>::iterator iter_deleBuild = build.begin();
    list<Missile*>::iterator iter_deleMissile = missile.begin();

    while(iter_deleHuman!=human.end())iter_deleHuman = deleteHuman(iter_deleHuman);
    while(iter_deleBuild!=build.end())iter_deleBuild = deleteBuilding(iter_deleBuild);
    while(iter_deleMissile!=missile.end())iter_deleMissile = deleteMissile(iter_deleMissile);
}

Building* Player::addBuilding(int Num, int BlockDR, int BlockUR)
{
    Building *newbuilding=new Building(Num,BlockDR,BlockUR,this->civilization);
    newbuilding->setPlayerScience(playerScience);
    newbuilding->setPlayerRepresent(represent);
    build.push_back(newbuilding);
    return newbuilding;
}

int Player::addHuman(int Num, double DR, double UR)
{
    Human *newhuman=new Human(Num,DR,UR);
    newhuman->setPlayerScience(playerScience);
    newhuman->setPlayerRepresent(represent);
    human.push_back(newhuman);
    return 0;
}

int Player::addFarmer(double DR, double UR)
{
    Farmer *newfarmer=new Farmer(DR,UR);
    newfarmer->setPlayerScience(playerScience);
    newfarmer->setPlayerRepresent(represent);
    human.push_back(newfarmer);
    return 0;
}

Missile* Player::addMissile( Coordinate* attacker , Coordinate* attackee )
{
    Missile* newMissile = NULL;
    BloodHaver* aterOb = NULL;

    attacker->printer_ToBloodHaver((void**)&aterOb);
    if(aterOb!=NULL)
    {
        newMissile = new Missile(aterOb->get_type_Missile(), attacker , attackee);
        newMissile->setPlayerScience(playerScience);
        newMissile->setPlayerRepresent(represent);
        missile.push_back(newMissile);
    }

    return newMissile;
}

void Player::changeResource( int resourceSort , int num )
{
    switch (resourceSort)
    {
        case HUMAN_GOLD:
            gold+=num;
            break;
        case HUMAN_WOOD:
            wood+=num;
            break;
        case HUMAN_STONE:
            stone+=num;
            break;
        case HUMAN_GRANARYFOOD:
        case HUMAN_STOCKFOOD:
            food+=num;
            break;
        default:
            break;
    }
}
