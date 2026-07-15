#ifndef BUILDING_H
#define BUILDING_H

#include "Coordinate.h"
#include "Development.h"
#include "Bloodhaver.h"

class Building:public Coordinate,public BloodHaver
{
public:
/********************静态资源**************************/
  static std::list<ImageResource> *build[4];//建设list
  static std::list<ImageResource> *built[4][2][BUILDING_TYPE_MAXNUM]; //建设完成的list [时代][敌我][建筑类型]
  // 箭塔强化后贴图：[时代][敌我]，资源键 ArrowTower2_Egypt / ArrowTower2_Daiwa（与 png 命名一致）
  static std::list<ImageResource> *builtArrowTowerUpgraded[4][2];
  // 敌我标识: 0=我方, 1=敌方
  static std::list<ImageResource> *buildFire[3];

  static array<std::string,4> Buildingname;
  static array<array<array<std::string,BUILDING_TYPE_MAXNUM>,2>,4> Builtname; // [时代][敌我][建筑类型]
  static array<std::string,BUILDING_TYPE_MAXNUM> BuildDisplayName;
  static array<std::string,3> BuildFireName;

  static array<array<int,ACT_WINDOW_NUM_FREE>,BUILDING_TYPE_MAXNUM> actNames;

  static array<string,BUILDING_TYPE_MAXNUM> sound_click;

  static array<int,BUILDING_TYPE_MAXNUM> BuildingMaxBlood;
  static array<int,BUILDING_TYPE_MAXNUM> BuildingFundation;
  static array<int,BUILDING_TYPE_MAXNUM> BuildingVision;
/********************静态资源**************************/
public:
    Building(){}
    Building(int Num, int BlockDR, int BlockUR, int civ = CIVILIZATION_STONEAGE, Development* playerScience = NULL, int playerRepresent = MAXPLAYER, int Percent=100);

  /**********************虚函数**************************/
    int getSort(){return SORT_BUILDING;}

    /***************状态与图像显示****************/
    void nextframe();
    void setPreAttack(){ defencing = true; missionThrowTimer = 0; }
    bool isAttacking(){ return defencing; }
    void setNowRes();


    /*******player相关*******/
    bool isPlayerControl(){ return true; }
    int getPlayerRepresent(){ return playerRepresent; }


    /*******状态与属性设置、获取*******/
    int getMaxBlood(){ return MaxBlood; }
    QString getChineseName(){ return QString::fromStdString(getDisplayName(Num)); }
    bool isMonitorObject(Coordinate* judOb);
    void init_Blood();
    int getVision();
    void setAttribute();


    int get_add_specialAttack() override;
    int showATK_Basic() override;
    int showATK_Addition() override;
    /** 箭塔射程（格）：基础 / 科技加成，供选中面板 "base+bonus" 显示 */
    int showArrowTowerRangeBaseBlocks() const;
    int showArrowTowerRangeBonusBlocks() const;
    /** 箭塔强化等级/射程科技加成：取当前主人科技实时值与转化冻结值中的较高者 */
    int getArrowTowerUpgradeLevel() const;
    int getArrowTowerRangeAddition() const;

    /*******战斗相关*******/
    double getDis_attack();
    bool is_missileThrow(){ return missionThrowTimer == missionThrowStep; }


    /*******行动相关*******/
    void setAction( int actNum );
    void initAction();
    void ActNumToActName();


    /*******音乐与音效*******/
    string getSound_Click(){return sound_click[Num];}

    /***************指针强制转化****************/
    //若要将Building类指针转化为父类指针,务必用以下函数!
    void printer_ToBloodHaver(void** ptr){ *ptr = dynamic_cast<BloodHaver*>(this); }    //传入ptr为BloodHaver类指针的地址
    void printer_ToBuilding(void** ptr){ *ptr = this; }
    /*************以上指针强制转化****************/
  /********************以上虚函数**************************/


  /********************静态函数**************************/
    static std::string getBuildingname(int index){return Buildingname[index];}
    static std::string getBuiltname(int age, int isEnemy, int buildType){return Builtname[age][isEnemy][buildType];}
    static std::string getDisplayName(int num){return BuildDisplayName[num];}
    static std::string getBuildingFireName(int index){ return BuildFireName[index]; }

    static void allocatebuild(int i){ build[i]=new std::list<ImageResource>;}
    static void allocatebuilt(int age, int isEnemy, int buildType){built[age][isEnemy][buildType]=new std::list<ImageResource>;}
    static void allocateBuiltArrowTowerUpgraded(int age, int isEnemy){ builtArrowTowerUpgraded[age][isEnemy] = new std::list<ImageResource>; }
    static void allocatebuildFire( int type ){ buildFire[type] = new std::list<ImageResource>; }

    static std::list<ImageResource>* getBuild(int i) {return build[i];}
    static std::list<ImageResource>* getBuilt(int age, int isEnemy, int buildType) { return built[age][isEnemy][buildType]; }
    static std::list<ImageResource>* getBuiltArrowTowerUpgraded(int age, int isEnemy) { return builtArrowTowerUpgraded[age][isEnemy]; }
    static std::string getArrowTowerUpgradedResourceName(int age, int isEnemy);
    static std::list<ImageResource>* getBuildFire(int type){ return buildFire[type]; }

    static void deallocatebuild(int i);
    static void deallocatebuilt(int age, int isEnemy, int buildType);
    static void deallocateBuiltArrowTowerUpgraded(int age, int isEnemy);
    static void deallocatebuildFire(int type);

    static void setActNames(int buildNum , int num, int name){ actNames[buildNum][num] = name; }
  /********************静态函数**************************/


    /*****************act相关***************/
    int getActNames(int num)
    {
        int actNum = actNames[this->Num][num];
        int civ = playerScience->get_civilization();
        // 检查当前节点的时代，如果是铜器时代且工具时代升级已完成，返回对应的铜器时代动作ID
        if (actNum == ACT_STOCK_UPGRADE_USETOOL)
        {
            // 检查当前节点是否可以显示（如果可以显示且是铜器时代，说明当前节点是铜器时代节点）
            if (civ >= CIVILIZATION_BRONZEAGE &&
                playerScience->get_isBuildActionShowAble(BUILDING_STOCK, BUILDING_STOCK_UPGRADE_USETOOL, civ))
            {
                // 检查当前节点是否是铜器时代节点（通过检查资源消耗是否包含黄金）
                int wood, food, stone, gold;
                playerScience->get_Resource_Consume(BUILDING_STOCK, BUILDING_STOCK_UPGRADE_USETOOL, wood, food, stone, gold);
                if (gold > 0)  // 如果资源消耗包含黄金，说明是铜器时代节点
                    return ACT_STOCK_UPGRADE_METALWORKING;
            }
        }
        else if (actNum == ACT_STOCK_UPGRADE_DEFENSE_INFANTRY)
        {
            if (civ >= CIVILIZATION_BRONZEAGE &&
                playerScience->get_isBuildActionShowAble(BUILDING_STOCK, BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY, civ))
            {
                int wood, food, stone, gold;
                playerScience->get_Resource_Consume(BUILDING_STOCK, BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY, wood, food, stone, gold);
                if (gold > 0)
                    return ACT_STOCK_UPGRADE_DEFENSE_INFANTRY_SCALE;
            }
        }
        else if (actNum == ACT_STOCK_UPGRADE_DEFENSE_ARCHER)
        {
            if (civ >= CIVILIZATION_BRONZEAGE &&
                playerScience->get_isBuildActionShowAble(BUILDING_STOCK, BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER, civ))
            {
                int wood, food, stone, gold;
                playerScience->get_Resource_Consume(BUILDING_STOCK, BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER, wood, food, stone, gold);
                if (gold > 0)
                    return ACT_STOCK_UPGRADE_DEFENSE_ARCHER_SCALE;
            }
        }
        else if (actNum == ACT_STOCK_UPGRADE_DEFENSE_RIDER)
        {
            if (civ >= CIVILIZATION_BRONZEAGE &&
                playerScience->get_isBuildActionShowAble(BUILDING_STOCK, BUILDING_STOCK_UPGRADE_DEFENSE_RIDER, civ))
            {
                int wood, food, stone, gold;
                playerScience->get_Resource_Consume(BUILDING_STOCK, BUILDING_STOCK_UPGRADE_DEFENSE_RIDER, wood, food, stone, gold);
                if (gold > 0)
                    return ACT_STOCK_UPGRADE_DEFENSE_RIDER_SCALE;
            }
        }
        // 对于市场建筑，根据当前节点的时代返回不同的动作ID
        else if (this->Num == BUILDING_MARKET && playerScience != NULL)
        {
            int actNum = actNames[this->Num][num];
            int civ = playerScience->get_civilization();
            // 检查当前节点的时代，如果是铜器时代且工具时代升级已完成，返回对应的铜器时代动作ID
            if (actNum == ACT_UPGRADE_WOOD)
            {
                // 检查当前节点是否可以显示（如果可以显示且是铜器时代，说明当前节点是铜器时代节点）
                if (civ >= CIVILIZATION_BRONZEAGE &&
                    playerScience->get_isBuildActionShowAble(BUILDING_MARKET, BUILDING_MARKET_WOOD_UPGRADE, civ))
                {
                    // 检查当前节点是否是铜器时代节点（通过检查资源消耗）
                    // 工艺需要150木材和170食物，而木材加工只需要75木材和120食物
                    int wood, food, stone, gold;
                    playerScience->get_Resource_Consume(BUILDING_MARKET, BUILDING_MARKET_WOOD_UPGRADE, wood, food, stone, gold);
                    if (wood >= 150)  // 如果资源消耗包含150木材，说明是铜器时代节点（工艺）
                        return ACT_UPGRADE_CRAFT;
                }
            }
            else if (actNum == ACT_UPGRADE_FARM)
            {
                if (civ >= CIVILIZATION_BRONZEAGE &&
                    playerScience->get_isBuildActionShowAble(BUILDING_MARKET, BUILDING_MARKET_FARM_UPGRADE, civ))
                {
                    int wood, food, stone, gold;
                    playerScience->get_Resource_Consume(BUILDING_MARKET, BUILDING_MARKET_FARM_UPGRADE, wood, food, stone, gold);
                    // 犁需要250食物和75木材，而驯养动物需要200食物和50木材
                    if (food >= 250)  // 如果资源消耗包含250食物，说明是铜器时代节点（犁）
                        return ACT_UPGRADE_PLOW;
                }
            }
        }
        // 谷仓：与市场「木材加工/工艺」同一槽位——static 仍为 ACT_UPGRADE_TOWERBUILD，显示在槽位 1 上在「研发箭塔」与「箭塔强化」间切换
        else if (this->Num == BUILDING_GRANARY && playerScience != NULL)
        {
            int actNumSlot = actNames[this->Num][num];
            if (actNumSlot == ACT_UPGRADE_TOWERBUILD)
            {
                if (playerScience->get_isBuildActionShowAble(BUILDING_GRANARY, BUILDING_GRANARY_ARROWTOWER, civ))
                    return ACT_UPGRADE_TOWERBUILD;
                if (playerScience->get_isBuildActionShowAble(BUILDING_GRANARY, BUILDING_GRANARY_ARROWTOWE_UPGRADE, civ))
                    return ACT_UPGRADE_ARROWTOWER;
            }
        }
        // 市镇中心：根据升时代科技链的已完成阶段切换“工具时代/铜器时代”按钮，避免资源配置变化影响 UI。
        else if (this->Num == BUILDING_CENTER && playerScience != NULL)
        {
            int action = actNames[this->Num][num];
            if (action == ACT_UPGRADE_AGE &&
                playerScience->getActLevel(BUILDING_CENTER, BUILDING_CENTER_UPGRADE) >= 1)
            {
                return ACT_UPGRADE_BRONZEAGE;
            }
        }
        return actNames[this->Num][num];
    }

    int getActStatus(int num){return actStatus[num];}
    void setActStatus(int wood = 0 , int food = 0 , int stone = 0 , int gold = 0);
    void setActStatus(int num, int status){this->actStatus[num] = status;}

    double get_retio_Build();
    double get_retio_Action();

    bool is_ActionFinish(){ return actPercent>=100; }
    bool isActionNeedCreatObject(int &creatObjectSort, int& creatObjectNum){ return playerScience->isNeedCreatObjectAfterAction(getNum() , getActNum() , creatObjectSort , creatObjectNum);}
    bool isRepresentHumanHaveSpace()
    {
        int requiredHalfSlots = playerScience->getActionPopulationHalfSlots(Num, actNum);
        return playerScience->get_isHumanHaveSpace(requiredHalfSlots);
    }
    void update_Action();
    void update_Build();
    /** 按修复血量比例扣修理费；资源不足返回 false。仅对已完工建筑收费。 */
    bool tryDeductRepairHpCost(double hpFractionRepaired, class Player* owner);

    void BuildingActionOver();


    /*************建筑行为对player资源的改变****************/
    //初始化暂存（建筑行动预计消耗的资源）资源
    void init_Resouce_TS(){ wood_TS = 0; food_TS = 0; stone_TS = 0; gold_TS = 0; }

    //设置当前建筑行动的暂存资源
    void set_Resource_TS( int wood, int food, int stone ,int gold ){ wood_TS = wood,food_TS = food, stone_TS = stone, gold_TS = gold; }

    //取消建筑行动，返还暂存资源
    void get_Resouce_TS( int& wood, int& food , int& stone , int& gold ){ wood = wood_TS , food = food_TS, stone = stone_TS, gold = gold_TS; }


    /*****************player相关***************/
    //以下两设置，用于转化时使用
    //设置科技，用于计算科技提升
    void setPlayerScience(Development* science){ this->playerScience = science; }
    //设置隶属player
    void setPlayerRepresent( int represent ){ playerRepresent = represent; }
    //转化等级冻结：把转化时刻(原主人科技下的)时代与箭塔科技等级快照到实例，保证转化后不降级
    void freezeLevel();
    //是否为祭司转化而来(转化时会冻结等级快照)，用于胜负判定等
    bool isConverted(){ return levelFrozen; }


    /*******状态与属性设置、获取*******/
    void setFundation();

    bool isFinish(){return this->Percent>=100;}
    double getPercent() {return this->Percent;}
    bool isConstructed(){ return constructed; } //判断已建造完成
    void recordConstruct(){ constructed = true; }

    int get_civilization();

    bool isMatchResourceType(int resourceType);


    /*******战斗相关*******/
    void init_BuildAttackAct(){ defencing = false; missionThrowTimer = 0; }

    bool isAttackBegin(){ return missionThrowTimer == 0;}


    /***************状态与图像显示****************/
    double getFireImageX(){ return fireImageX; }
    double getFireImageY(){ return fireImageY; }

    std::list<ImageResource>::iterator getFireNowRes(){return this->fireNowRes;}
    std::list<ImageResource>* getFireNowList(){ return fireNowList; }


protected:



    bool defencing = false;
    int missionThrowTimer = 0;
    int missionThrowStep = 0;

    //所属阵营
    int playerRepresent;

    //所属阵营的科技树
    Development* playerScience = NULL;

    int civ;
    //建筑所处时代 来确定不同时代建筑有何变化 ？时代要不要用player类下的
    bool constructed = false;   //是否已完成建造

    //==== 祭司转化等级快照（与原版一致：转化后建筑不降级；新主人科技更高时仍取较高者生效）====
    bool levelFrozen = false;               //是否已被转化冻结等级
    int frozenCiv = CIVILIZATION_STONEAGE;  //冻结时的时代（决定建筑贴图风格）
    int frozenArrowTowerLevel = 0;          //冻结时的箭塔强化等级
    int frozenAtkAddition = 0;              //冻结时的攻击科技加成（箭塔）
    int frozenRangeAddition = 0;            //冻结时的射程/视野科技加成（箭塔）

    int Foundation;
    //地基类型

    double Percent = 0;
    //完成百分比 100时表示建筑已经被建造完成 根据完成度有不同的贴图

    int Finish=0;//0为未完成 1为完成

    int actStatus[ACT_WINDOW_NUM_FREE];

    //存储建筑行动的预扣资源：
    int wood_TS = 0;
    int food_TS = 0;
    int stone_TS = 0;
    int gold_TS = 0;

    //修理资源小数累积（按帧扣费时取整）
    double repairResDebtWood = 0;
    double repairResDebtFood = 0;
    double repairResDebtStone = 0;
    double repairResDebtGold = 0;

    std::list<ImageResource>::iterator fireNowRes;
    std::list<ImageResource> *fireNowList = NULL;

    double fireImageX;
    double fireImageY;


    /***************状态与图像显示****************/
    void setFireNowRes();
};

#endif // BUILDING_H
