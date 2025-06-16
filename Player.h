#ifndef PLAYER_H
#define PLAYER_H

#include "Missile.h"
#include "Farmer.h"
#include "Army.h"
#include "Building.h"
#include "Building_Resource.h"

class Player
{
public:
    Player();
    Player(int);
    ~Player();


    //***************************************************************
    //建筑池子
    std::list<Building *> build;
    //人口池子
    std::list<Human *> human;
    //当前所属飞行物
    std::list<Missile*> missile;
    //***************************************************************
    //添加对象
    Building* addBuilding(int Num,int BlockDR,int BlockUR , double percent = 0);
    int addHuman(int Num,double DR,double UR);
    Army* addArmy(int Num , double DR , double UR);
    Army* addArmyAROUND(int Num,double DR,double UR,int status,int starttime,int finishtime,double dDR,double dUR);
    Army* addArmyDEFENSE(int Num,double DR,double UR,int status);
    Army* addArmyATTACK(int Num,double DR,double UR,int status,int starttime,int finishtime);
    int addFarmer(double DR,double UR);
    int addShip(int Num,double DR,double UR);
    Missile* addMissile( Coordinate* attacker , Coordinate* attackee  , int beginHeight);

    //删除实例对象
    list<Human*>::iterator deleteHuman( list<Human*>::iterator iterDele );
    list<Building*>::iterator deleteBuilding( list<Building*>::iterator iterDele );
    list<Missile*>::iterator deleteMissile( list<Missile*>::iterator iterDele );

    //删除missile的投掷者
    void deleteMissile_Attacker( Coordinate* attacker );


    //***************************************************************
    //player独有的游戏进度记录

    int getCiv(){ return playerScience->get_civilization(); }
    void setCiv(int civ){playerScience->set_civilization(civ);}

    //***控制资源****
    //获取资源持有数量
    int getWood(){return this->wood;}
    int getFood(){return this->food;}
    int getStone(){return this->stone;}
    int getGold(){return this->gold;}

    //设置初始资源
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
    //获取人口上限(可达到的)，最高50
    int getMaxHumanNum(){return playerScience->getHumanNumCanReach();}
    //获取当前人口
    int getHumanNum(){ return playerScience->get_humanNum(); }

   /*建筑行动相关********************************************/
    //判断是否可建筑
    //判断建筑是否可建造分两步，先判断建筑建造是否能显示，再判断资源是否足够
    bool get_isBuildingShowAble(int buildNum ){ return playerScience->get_isBuildingShowAble(buildNum , getCiv()); }
    bool get_isBuildingAble( int buildNum ){ return playerScience->get_isBuildingAble(buildNum,wood,food,stone,gold); }
    //判断建筑行动是否能进行的函数中，内含了判断行动是否能显示。
    bool get_isBuildActionAble( Building* actBuild,int actNum, int* oper = NULL ){ return playerScience->get_isBuildActionAble(actBuild->getNum(),actNum,getCiv(),wood,food,stone,gold, oper); }
    bool get_isBuildActionAble( int buildType, int actNum, int* oper = NULL){ return playerScience->get_isBuildActionAble(buildType , actNum , getCiv() , wood , food , stone , gold , oper); }
    bool get_isBuildActionShowAble( int buildNum , int actNum ){ return playerScience->get_isBuildActionShowAble(buildNum,actNum,getCiv());}

    bool get_buildActLevel( int buildNum , int actNum ){ return playerScience->getActLevel(buildNum , actNum); }

    //返回建筑行动预扣的资源
    void back_Resource_TS( Building* actBuild );
    //处理建筑行动结束带来的效果
    void finishBuild( Building* buildBuilding );

    //控制建筑行动
    void enforcementAction( Building* actBuild , vector<pair<Point,int>>Block_free );

    bool get_isBuildingHaveBuild( int buildNum ){ return playerScience->getBuildTimes(buildNum)>0; }

    //获取某时代建筑的当前数目（除了home、农田、市镇中心）
    int get_civiBuild_Times( int civilization ){ return playerScience->get_civiBuild_Times(civilization); }
   /*以上建筑行动相关********************************************/

    void set_AllTechnology(){ playerScience->all_technology_tree(); }

    int getScore()
    {
        return this->score;
    }
    void setScore(int score)
    {
        this->score = score;
    }

    void setStartScores(int type)
    {
        this->startScores[type] = true;
    }
    bool getStartScores(int type)
    {
        return this->startScores[type];
    }


    //增加人口，考虑未来可能有政府中心兵营兵种人口减半，故留函数接口以重写
    void humanNumIncrease(Human* newHuman){ playerScience->addHumanNum(); }
    void humanNumDecrease(Human* delHuman){ playerScience->subHumanNum(); }

    int get_centerNum(){ return playerScience->get_centerNum(); }

    void beginAttack();
private:
    int represent;  //player阵营

    //当前文明
//    int civilization=1;

    //人口容量
//    int maxHumanNum=0;  //人口上限

    int attackFrame = 0;

    //所拥有的四个资源
    int wood=INITIAL_WOOD;
    int food=INITIAL_MEAT;
    int stone=INITIAL_STONE;
    int gold=INITIAL_GOLD;
    int score=0;    //得分

    //研究技术进度与成果
    Development* playerScience;

    bool isArrowTowerUnlocked = false;
    bool marketResearch[3] = {false};
//    bool cheatMaxHumanNum=false;
    bool startScores[17] = {false};
};

#endif // PLAYER_H
