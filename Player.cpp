#include "Player.h"

Player::Player()
{
    playerScience = new Development();
}

Player::~Player()
{
    delete playerScience;
}

Building* Player::addBuilding(int Num, int BlockDR, int BlockUR)
{
    Building *newbuilding=new Building(Num,BlockDR,BlockUR,this->civilization);
    newbuilding->setPlayerScience(playerScience);
    build.push_back(newbuilding);
    return newbuilding;
}

int Player::addHuman(int Num, double DR, double UR)
{
    Human *newhuman=new Human(Num,DR,UR);
    newhuman->setPlayerScience(playerScience);
    human.push_back(newhuman);
    return 0;
}

int Player::addFarmer(double DR, double UR)
{
    Farmer *newfarmer=new Farmer(DR,UR);
    newfarmer->setPlayerScience(playerScience);
    human.push_back(newfarmer);
    return 0;
}
