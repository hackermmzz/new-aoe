#ifndef PLAYER_H
#define PLAYER_H

#include <Farmer.h>
#include <Army.h>
#include <Building.h>

class Player
{
public:
    Player();
    Player(int);
    ~Player();

    int addBuilding(int Num,int BlockDR,int BlockUR);

    int addHuman(int Num,double DR,double UR);

    int addFarmer(double DR,double UR);

    list<Human*>::iterator deleteHuman( list<Human*>::iterator iterDele )
    {
        delete *iterDele;
        return human.erase(iterDele);
    }

    list<Building*>::iterator deleteBuilding( list<Building*>::iterator iterDele )
    {
        delete *iterDele;
        return build.erase(iterDele);
    }

    //建筑池子
    std::list<Building *> build;

    //人口池子
    std::list<Human *> human;

    int getWood()
    {
        return this->wood;
    }
    int getFood()
    {
        return this->food;
    }
    int getStone()
    {
        return this->stone;
    }
    int getGold()
    {
        return this->gold;
    }
    bool *getmarketResearch()
    {
        return this->marketResearch;
    }
    void setWood(int wood)
    {
        this->wood = wood;
    }
    void setFood(int food)
    {
        this->food = food;
    }
    void setStone(int stone)
    {
        this->stone = stone;
    }
    void setGold(int gold)
    {
        this->gold = gold;
    }
    void setCiv(int civ)
    {
        this->civilization = civ;
    }
    int getCiv()
    {
        return this->civilization;
    }
    bool getArrowTowerUnlocked()
    {
        return this->isArrowTowerUnlocked;
    }
    void setArrowTowerUnlocked(bool flag)
    {
        this->isArrowTowerUnlocked = flag;
    }
    bool getMarketResearch(int num)
    {
        return marketResearch[num];
    }
    void setMarketResearch(int num, bool flag)
    {
        this->marketResearch[num] = flag;
    }
    int getScore()
    {
        return this->score;
    }
    void setScore(int score)
    {
        this->score = score;
    }
    int getMaxHumanNum()
    {
        return this->maxHumanNum;
    }
    void setMaxHumanNum(int num)
    {
        this->maxHumanNum = num;
    }
    void setCheatMaxHumanNum(bool flag)
    {
        this->cheatMaxHumanNum = flag;
    }
    bool getCheatMaxHumanNum()
    {
        return this->cheatMaxHumanNum;
    }
    void setStartScores(int type)
    {
        this->startScores[type] = true;
    }
    bool getStartScores(int type)
    {
        return this->startScores[type];
    }

    void changeResource( int resourceSort , int num );

private:
    int represent;

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
    bool isArrowTowerUnlocked = false;
    bool marketResearch[3] = {false};
    bool cheatMaxHumanNum=false;
    bool startScores[17] = {false};

    //研究技术进度与成果
    Development* playerScience;
};

#endif // PLAYER_H
