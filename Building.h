#ifndef BUILDING_H
#define BUILDING_H

#include "Coordinate.h"
#include "Development.h"
#include "Bloodhaver.h"

class Building:public Coordinate,public BloodHaver
{
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
    static std::string getBuiltname(int index1,int index2){return Builtname[index1][index2];}
    static std::string getDisplayName(int num){return BuildDisplayName[num];}
    static std::string getBuildingFireName(int index){ return BuildFireName[index]; }

    static void allocatebuild(int i){ build[i]=new std::list<ImageResource>;}
    static void allocatebuilt(int i,int j){built[i][j]=new std::list<ImageResource>;}
    static void allocatebuildFire( int type ){ buildFire[type] = new std::list<ImageResource>; }

    static std::list<ImageResource>* getBuild(int i) {return build[i];}
    static std::list<ImageResource>* getBuilt(int i,int j) { return built[i][j]; }
    static std::list<ImageResource>* getBuildFire(int type){ return buildFire[type]; }

    static void deallocatebuild(int i);
    static void deallocatebuilt(int i,int j);
    static void deallocatebuildFire(int type);

    static void setActNames(int buildNum , int num, int name){ actNames[buildNum][num] = name; }
  /********************静态函数**************************/


    /*****************act相关***************/
    int getActNames(int num){return actNames[this->Num][num];}

    int getActStatus(int num){return actStatus[num];}
    void setActStatus(int wood = 0 , int food = 0 , int stone = 0 , int gold = 0);
    void setActStatus(int num, int status){this->actStatus[num] = status;}

    double get_retio_Build();
    double get_retio_Action();

    bool is_ActionFinish(){ return actPercent>=100; }
    bool isActionNeedCreatObject(int &creatObjectSort, int& creatObjectNum){ return playerScience->isNeedCreatObjectAfterAction(getNum() , getActNum() , creatObjectSort , creatObjectNum);}
    bool isRepresentHumanHaveSpace(){ return playerScience->get_isHumanHaveSpace(); }

    void update_Action();
    void update_Build();

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
  /********************静态资源**************************/
    static std::list<ImageResource> *build[4];//建设list
    static std::list<ImageResource> *built[3][20]; //建设完成的list
    static std::list<ImageResource> *buildFire[3];

    static std::string Buildingname[4];
    static std::string Builtname[3][20];
    static std::string BuildDisplayName[20];
    static std::string BuildFireName[3];

    static int actNames[BUILDING_TYPE_MAXNUM][ACT_WINDOW_NUM_FREE];

    static string sound_click[20];

    static int BuildingMaxBlood[20];
    static int BuildingFundation[20];
    static int BuildingVision[20];
  /********************静态资源**************************/


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

    std::list<ImageResource>::iterator fireNowRes;
    std::list<ImageResource> *fireNowList = NULL;

    double fireImageX;
    double fireImageY;


    /***************状态与图像显示****************/
    void setFireNowRes();
};

#endif // BUILDING_H
