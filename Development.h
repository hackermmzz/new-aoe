#ifndef DEVELOPMENT_H
#define DEVELOPMENT_H

#include <config.h>
#include <GlobalVariate.h>

class Development
{
public:
    Development();

    double get_rate_Move(int sort,int type);
    double get_rate_Blood(int sort , int type);
    int get_addition_Blood( int sort , int type );

    double get_rate_Attack( int sort , int type , int armyClass , int attackType );
    int get_addition_Attack( int sort , int type , int armyClass , int attackType );

    int get_addition_DisAttack( int sort, int type , int armyClass , int attackType );

    double get_rate_Defence( int sort , int type , int armyClass , int attackType_got );
    int get_addition_Defence( int sort , int type , int armyClass , int attackType_got );

    double get_rate_HitTarget(){ return 1+rate_hitTarget; }

    /**************资源相关**************/
    int get_addition_ResourceSort( int resourceSort );
    int get_addition_MaxCnt( int sort , int type );

    //获取建筑是否可以建造
    //对于建筑建造判断，先判断是否显示，再判断是否能执行
    bool get_isBuildingShowAble(int buildingNum , int civilization){ return developLab[buildingNum].buildCon->isShowable(civilization); }
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

    //获取升级次数/当前等级
    int getActLevel( int buildType , int actType ){ return developLab[buildType].actCon[actType].getPhaseTimes(); }
    int getBuildTimes( int buildType ){ return developLab[buildType].buildCon->getActTimes(); }

    void init_DevelopLab();

    void finishAction(int buildingType){ developLab[buildingType].finishBuild(); }
    void finishAction(int buildingType , int buildact){developLab[buildingType].finishAction(buildact);}

    bool isNeedCreatObjectAfterAction( int buildType , int actNum , int& creatObjectSort , int& creatObjectNum )
        { return developLab[buildType].actCon[actNum].nowExecuteNode->isNeedCreatObject(creatObjectSort,creatObjectNum); }
private:
    //研发工艺带来的数值加成
    double rate_FarmerMove = 0;
    double rate_FarmerBlood = 0;
    double rate_hitTarget = 0;

    map< int , st_buildAction > developLab;
};

#endif // DEVELOPMENT_H
