#ifndef PLAYER_H
#define PLAYER_H

#include <Farmer.h>
#include <Building.h>

class Player
{
public:
    Player();

    int addBuilding(int Num,int BlockDR,int BlockUR);

    int addHuman(int Num,double DR,double UR);

    int addFarmer(double DR,double UR);

    //建筑池子
    std::list<Building *> build;

    //人口池子
    std::list<Human *> human;

private:

    //当前文明
    int civilization=1;

    //人口容量
    int maxHumanNum=0;

    //所拥有的四个资源
    int wood=200;
    int food=200;
    int stone=150;
    int gold=0;
    int score=0;
};

#endif // PLAYER_H
