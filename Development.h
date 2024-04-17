#ifndef DEVELOPMENT_H
#define DEVELOPMENT_H

#include <config.h>
#include <GlobalVariate.h>

class Development
{
public:
    Development();

    int get_level( int sort, int type );

    double get_rate_Move(int sort,int type);
    double get_rate_Blood(int sort , int type);
    int get_addition_Blood( int sort , int type );

    double get_rate_Attack( int sort , int type , int armyClass , int attackType );
    int get_addition_Attack( int sort , int type , int armyClass , int attackType );

    int get_addition_DisAttack( int sort, int type , int armyClass , int attackType );

    double get_rate_Defence( int sort , int type , int armyClass , int attackType_got );
    int get_addition_Defence( int sort , int type , int armyClass , int attackType_got );

    double get_rate_HitTarget(){ return 1+rate_hitTarget; }

    int get_addition_ResourceSort( int resourceSort );

    //获取建筑是否可以建造
    bool get_isBuildingAble( int buildingNum , int wood ,int food , int stone ,int gold )
        { return developLab[buildingNum].buildCon->executable(wood , food ,stone ,gold); }
    bool get_isBuildActionAble( int buildingNum, int actNum, int civilization ,int wood, int food , int stone, int gold )
        { return developLab[buildingNum].actCon[actNum].executable(civilization,wood,food,stone,gold); }

    //获取建造/行动的消耗（时间、资源等）
    void get_Resource_Consume( int buildNum ,int& wood,int& food,int& stone,int& gold ){ developLab[buildNum].buildCon->get_needResource(wood,food,stone,gold); }
    void get_Resource_Consume( int buildNum , int actNum ,int& wood,int& food,int& stone,int& gold  ){ developLab[buildNum].actCon[actNum].get_needResource(wood,food,stone,gold);}
    //获取时间
    double get_buildTime( int buildingNum ){ return developLab[buildingNum].buildCon->times_second; }
    double get_actTime( int buildingNum, int actNum ){ return developLab[buildingNum].actCon[actNum].nowExecuteNode->times_second;}


    void init_DevelopLab();

    void finishAction(int buildingType){ developLab[buildingType].finishBuild(); }
    void finishAction(int buildingType , int buildact){}

    bool isNeedCreatObjectAfterAction( int buildType , int actNum , int& creatObjectSort , int& creatObjectNum )
        { return developLab[buildType].actCon[actNum].nowExecuteNode->isNeedCreatObject(creatObjectSort,creatObjectNum); }
private:
    //研发工艺带来的数值加成
    double rate_FarmerMove = 0;
    double rate_FarmerBlood = 0;
    double rate_hitTarget = 0;
    int attack_close = 0;
    int defence_infantry = 0;
    int defence_archer = 0;
    int defence_rider = 0;
    int specialDefence_toShoot = 0;
    //采集加成
    int capacity_wood = 0;
    int capacity_stone = 0;
    int capacity_gold = 0;
    int capacity_farm = 0;
    int dis_shoot = 0;

    //研发工艺，解锁或升级兵种、建筑
    bool wheel = false;
    int wall = 0;
    int arrowTown = 0;
    int level_Clubman = 0;
    int level_ShortSwordsman = 0;

    int level_Wall = 0;
    int level_ArrowTower = 0;

    map< int , st_buildAction > developLab;
};

#endif // DEVELOPMENT_H
