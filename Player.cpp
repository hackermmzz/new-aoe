#include "Player.h"

Player::Player()
{

}

int Player::addBuilding(int Num, int BlockDR, int BlockUR)
{
    Building *newbuilding=new Building(Num,BlockDR,BlockUR,this->civilization);
    build.push_back(newbuilding);
    return 0;
}

int Player::addHuman(int Num, double DR, double UR)
{
    Human *newhuman=new Human(Num,DR,UR);
    human.push_back(newhuman);
    return 0;
}
