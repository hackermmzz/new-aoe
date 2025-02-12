#ifndef DEVELOPMENT_H
#define DEVELOPMENT_H

#include <config.h>
#include "GlobalVariate.h"

class Development
{
public:
    /************类的初始化***********/
    Development();
    Development(int represent);

    //初始化科技树
    void init_DevelopLab();
    /************类的初始化***********/


   /*****************加成信息*****************/
    double get_rate_Move(int sort,int type);
    double get_rate_Blood(int sort , int type);
    int get_addition_Blood( int sort , int type );

    double get_rate_Attack( int sort , int type , int armyClass , int attackType, int interSort = -1, int interNum = -1 );
    int get_addition_Attack( int sort , int type , int armyClass , int attackType );

    int get_addition_DisAttack( int sort, int type , int armyClass , int attackType );

    double get_rate_Defence( int sort , int type , int armyClass , int attackType_got );
    int get_addition_Defence( int sort , int type , int armyClass , int attackType_got );

    double get_rate_HitTarget(){ return 1+rate_hitTarget; }
    /**************资源相关**************/
    int get_addition_ResourceSort( int resourceSort );
    int get_addition_MaxCnt( int sort , int type );
    double get_rate_ResorceGather( int resourceSort );

   /******************加成信息*******************/



  /*****************游戏进程信息*******************/
    //获取某时代建筑建造总次数
    int get_civiBuild_Times( int civilization ){ return buildingNumber[civilization]; }

    void add_civiBuildNum( int buildNum ){ buildingNumber[ developLab[buildNum].buildCon->civilization]++; }
    void sub_civiBuildNum( int buildNum ){ buildingNumber[ developLab[buildNum].buildCon->civilization]--; }

    //获取时代信息
    int get_civilization(){ return civilization;}
    //用于设置起始时代
    void set_civilization( int civ );

    /*****************人口信息*******************/
    void addHumanNum(){ humanNum++; }
    void subHumanNum(){ humanNum--; }

    //当前人口数目
    int get_humanNum(){ return humanNum; }
    //获取人口上限
    int getMaxHumanNum(){return get_homeNum()*HOUSE_HUMAN_NUM;}
    //当前能达到的最大人口数目
    int getHumanNumCanReach(){ return getMaxHumanNum()<humanNum_Top? getMaxHumanNum(): humanNum_Top; }
    //是否仍有空间添加人口
    bool get_isHumanHaveSpace(){return get_humanNum()<getHumanNumCanReach(); }

    int get_centerNum(){ return centerNum; }
    /***************当前建筑信息*******************/
    //获取当前房屋数目
    int get_homeNum(){ return (int)(centerNum>0) + homeNum; }

    void addHome(){ homeNum++; }
    void subHome(){ homeNum--; }
    void subCenter(){ centerNum--; }
  /*****************以上游戏进程信息*******************/



  /***************科技树维护与查询**********************/
    //结束行动，维护
    void finishAction(int buildingType){ developLab[buildingType].finishBuild(); }
    void finishAction(int buildingType , int buildact);

    bool isNeedCreatObjectAfterAction( int buildType , int actNum , int& creatObjectSort , int& creatObjectNum )
        { return developLab[buildType].actCon[actNum].nowExecuteNode->isNeedCreatObject(creatObjectSort,creatObjectNum); }

    /*********获取建筑是否可以建造、行动*******/
    //对于建筑建造判断，先判断是否显示，再判断是否能执行
    bool get_isBuildingShowAble(int buildingNum , int civilization){ return developLab[buildingNum].buildCon->isShowable(civilization); }
    bool get_isBuildingAble( int buildingNum , int wood ,int food , int stone ,int gold )
        { return developLab[buildingNum].buildCon->executable(wood , food ,stone ,gold); }
    bool get_isBuildActionAble( int buildingNum, int actNum, int civilization ,int wood, int food , int stone, int gold ,  int* oper = NULL );

    bool get_isBuildActionShowAble( int buildingNum , int actNum , int civilization ){ return developLab[buildingNum].actCon[actNum].isShowAble(civilization); }

    /********获取建造/行动的消耗（时间、资源等）******/
    //获取消耗资源
    void get_Resource_Consume( int buildNum ,int& wood,int& food,int& stone,int& gold ){ developLab[buildNum].buildCon->get_needResource(wood,food,stone,gold); }
    void get_Resource_Consume( int buildNum , int actNum ,int& wood,int& food,int& stone,int& gold  ){ developLab[buildNum].actCon[actNum].get_needResource(wood,food,stone,gold);}
    //获取消耗时间
    double get_buildTime( int buildingNum ){ return developLab[buildingNum].buildCon->times_second; }
    double get_actTime( int buildingNum, int actNum ){ return developLab[buildingNum].actCon[actNum].nowExecuteNode->times_second;}

    void BuildingActionExecuting(int buildNum, int actNum){ developLab[buildNum].actCon[actNum].beginExecute(); }
    void BuildingActionOverExecuting(int buildNum, int actNum){ developLab[buildNum].actCon[actNum].overExecute(); }

    //获取升级次数/当前等级
    int getActLevel( int buildType , int actType ){ return developLab[buildType].actCon[actType].getPhaseTimes(); }
    int getBuildTimes( int buildType ){ return developLab[buildType].buildCon->getActTimes(); }
  /*************以上科技树维护与查询**********************/

    void all_technology_tree();


private:
    int civilization = CIVILIZATION_STONEAGE;
    int playerRepresent = 0;


    //home数量，用于计算当前最大人口
    int homeNum = 0;
    int centerNum = 1;
    int humanNum = 0;   //当前人口数量
    int humanNum_Top = 50;  //最大人口上限

    //研发工艺带来的数值加成
    double rate_FarmerMove = 0;
    double rate_FarmerBlood = 0;
    double rate_hitTarget = 0;

    int buildingNumber[6] = {0};

    map< int , st_buildAction > developLab;

    /*****************游戏进程信息*******************/
    //时代升级，进入下一时代
    void civiChange();
};

#endif // DEVELOPMENT_H
