#ifndef PLAYER_H
#define PLAYER_H

#include <Missile.h>
#include <Farmer.h>
#include <Army.h>
#include <Building.h>
#include <Building_Resource.h>

class Player
{
public:
    Player();
    Player(int);
    ~Player();

    //***************************************************************
    //添加对象
    Building* addBuilding(int Num,int BlockDR,int BlockUR , double percent = 0);
    int addHuman(int Num,double DR,double UR);
    Army* addArmy(int Num , double DR , double UR);
    int addFarmer(double DR,double UR);
    Missile* addMissile( Coordinate* attacker , Coordinate* attackee );

    //删除实例对象
    list<Human*>::iterator deleteHuman( list<Human*>::iterator iterDele );
    list<Building*>::iterator deleteBuilding( list<Building*>::iterator iterDele );
    list<Missile*>::iterator deleteMissile( list<Missile*>::iterator iterDele );

    //删除missile的投掷者
    void deleteMissile_Attacker( Coordinate* attacker );
    //***************************************************************
    //建筑池子
    std::list<Building *> build;
    //人口池子
    std::list<Human *> human;
    //当前所属飞行物
    std::list<Missile*> missile;

    //***************************************************************
    //player独有的游戏进度记录

    //***控制资源****
    //获取资源持有数量
    int getWood(){return this->wood;}
    int getFood(){return this->food;}
    int getStone(){return this->stone;}
    int getGold(){return this->gold;}

    void setWood(int wood){this->wood = wood;}
    void setFood(int food){this->food = food;}
    void setStone(int stone){this->stone = stone;}
    void setGold(int gold){this->gold = gold;}
    //更改资源数量
    void changeResource( int resourceSort , int num , bool negative = false);
    void changeResource(int wood,int food,int stone,int gold, bool negative = false);

    void changeResource_byBuild(int buildNum);
    void changeResource_byBuildAction(Building* actBuild , int buildact);

    //***人口记录****
    //获取人口上限
    int getMaxHumanNum(){return (homeNum+centerNum)*HOUSE_HUMAN_NUM;}
    //获取当前人口
    int getHumanNum(){ return this->humanNum; }

   /*建筑行动相关********************************************/
    //判断是否可建筑
    //判断建筑是否可建造分两步，先判断建筑建造是否能显示，再判断资源是否足够
    bool get_isBuildingShowAble(int buildNum ){ return playerScience->get_isBuildingShowAble(buildNum , civilization); }
    bool get_isBuildingAble( int buildNum ){ return playerScience->get_isBuildingAble(buildNum,wood,food,stone,gold); }
    //判断建筑行动是否能进行的函数中，内含了判断行动是否能显示。
    bool get_isBuildActionAble( Building* actBuild,int actNum ){ return playerScience->get_isBuildActionAble(actBuild->getNum(),actNum,civilization,wood,food,stone,gold); }
    bool get_isBuildActionAble(int buildType , int actNum){ return playerScience->get_isBuildActionAble(buildType , actNum , civilization , wood , food , stone , gold); }

    void back_Resource_TS( Building* actBuild );
    void finishBuild( Building* buildBuilding ){playerScience->finishAction(buildBuilding->getNum());}
    void enforcementAction( Building* actBuild );

    bool get_isBuildingHaveBuild( int buildNum ){ return playerScience->getBuildTimes(buildNum)>0; }
   /*以上建筑行动相关********************************************/




    //获取科技树
    bool *getmarketResearch()
    {
        return this->marketResearch;
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

private:
    int represent;  //player阵营

    //当前文明
    int civilization=1;

    //人口容量
//    int maxHumanNum=0;  //人口上限
    int humanNum = 0;   //当前人口数量

    //home数量，用于计算当前最大人口
    int homeNum = 0;
    int centerNum = 1;

    //所拥有的四个资源
    int wood=200;
    int food=200;
    int stone=150;
    int gold=0;
    int score=0;    //得分

    //研究技术进度与成果
    Development* playerScience;

    bool isArrowTowerUnlocked = false;
    bool marketResearch[3] = {false};
    bool cheatMaxHumanNum=false;
    bool startScores[17] = {false};


    //增加人口，考虑未来可能有政府中心兵营兵种人口减半，故留函数接口以重写
    void humanNumIncrease(Human* newHuman){ humanNum++; }
    void humanNumDecrease(Human* delHuman){ humanNum--; }
};

#endif // PLAYER_H
