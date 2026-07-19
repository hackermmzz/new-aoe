#ifndef DEVELOPMENT_H
#define DEVELOPMENT_H

#include "config.h"
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
    Double get_rate_Move(int sort,int type);
    Double get_rate_Blood(int sort , int type);
    int get_addition_Blood( int sort , int type );

    Double get_rate_Attack( int sort , int type , int armyClass , int attackType, int interSort = -1, int interNum = -1 );
    int get_addition_Attack( int sort , int type , int armyClass , int attackType );

    int get_addition_DisAttack( int sort, int type , int armyClass , int attackType );

    Double get_rate_Defence( int sort , int type , int armyClass , int attackType_got );
    int get_addition_Defence( int sort , int type , int armyClass , int attackType_got );

    Double get_rate_HitTarget(){ return 1+rate_hitTarget; }
    /**************资源相关**************/
    int get_addition_ResourceSort( int resourceSort );
    int get_addition_MaxCnt( int sort , int type );
    Double get_rate_ResorceGather( int resourceSort );

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
    // 使用“半人口槽”精确表示 0.5 人口：普通单位占 2 槽，后勤兵营单位占 1 槽。
    void addHumanPopulationHalfSlots(int halfSlots){ humanPopulationHalfSlots += halfSlots; }
    void subHumanPopulationHalfSlots(int halfSlots){ humanPopulationHalfSlots = std::max(0, humanPopulationHalfSlots - halfSlots); }
    void setHumanPopulationHalfSlots(int halfSlots){ humanPopulationHalfSlots = std::max(0, halfSlots); }
    int getHumanPopulationHalfSlots(){ return humanPopulationHalfSlots; }
    //当前人口数目
    Double get_humanNum(){ return humanPopulationHalfSlots / Double(2); }
    //获取人口上限
    int getMaxHumanNum(){return get_homeNum()*HOUSE_HUMAN_NUM;}
    //当前能达到的最大人口数目
    int getHumanNumCanReach(){ return getMaxHumanNum()<humanNum_Top? getMaxHumanNum(): humanNum_Top; }
    //是否仍有空间容纳指定数量的半人口槽
    bool get_isHumanHaveSpace(int requiredHalfSlots = 2)
        { return humanPopulationHalfSlots + requiredHalfSlots <= getHumanNumCanReach() * 2; }

    // 后勤科技及人口权重统一入口。
    bool isLogisticsResearched();
    int getPopulationHalfSlots(int sourceBuilding, int objectSort);
    int getActionPopulationHalfSlots(int buildingNum, int actNum);

    int get_centerNum(){ return centerNum; }
    /***************当前建筑信息*******************/
    //获取当前房屋数目
    int get_homeNum(){ return (int)(centerNum>0) + homeNum; }
    void addHome(){ homeNum++; }
    void subHome(){ homeNum--; }
    void addCenter(){ centerNum++; }
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
    Double get_buildTime( int buildingNum ){ return developLab[buildingNum].buildCon->times_second; }
    Double get_actTime( int buildingNum, int actNum ){ return developLab[buildingNum].actCon[actNum].nowExecuteNode->times_second;}

    void BuildingActionExecuting(int buildNum, int actNum){ developLab[buildNum].actCon[actNum].beginExecute(); }
    void BuildingActionOverExecuting(int buildNum, int actNum){ developLab[buildNum].actCon[actNum].overExecute(); }

    //获取升级次数/当前等级
    int getActLevel( int buildType , int actType ){ return developLab[buildType].actCon[actType].getPhaseTimes(); }
    int getBuildTimes( int buildType ){ return developLab[buildType].buildCon->getActTimes(); }
  /*************以上科技树维护与查询**********************/

    void all_technology_tree();
    /** 将敌方等作弊开局限定到某一时代：时代上限 max_civilization 与 civ 枚举一致（石器=1 …）；市镇中心升时代进度由 set_civilization 对齐，其余建筑的研发链仅快进「节点所需时代 ≤ max」的阶段，不会超过该时代对应的研发尽头。 */
    void technology_tree_up_to(int max_civilization);


private:
    int civilization = CIVILIZATION_STONEAGE;
    int playerRepresent = 0;


    //home数量，用于计算当前最大人口
    int homeNum = 0;
    int centerNum = 1;
    int humanPopulationHalfSlots = 0; //当前人口占用（单位：半人口槽）
    int humanNum_Top = 50;  //最大人口上限

    //研发工艺带来的数值加成
    Double rate_FarmerMove = 0;
    Double rate_FarmerBlood = 0;
    Double rate_hitTarget = 0;

    int buildingNumber[6] = {0};

    map< int , st_buildAction > developLab;

    /*****************游戏进程信息*******************/
    //时代升级，进入下一时代
    void civiChange();
};

#endif // DEVELOPMENT_H
