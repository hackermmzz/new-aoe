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
}

int Player::addBuilding(int Num, int BlockDR, int BlockUR)
{
    Building *newbuilding=new Building(Num,BlockDR,BlockUR,this->civilization);
    newbuilding->setPlayerScience(playerScience);
    newbuilding->setPlayerRepresent(represent);
    build.push_back(newbuilding);
    return 0;
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
