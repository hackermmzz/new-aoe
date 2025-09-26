#include "Army.h"
#include "Map.h"
#include "Core.h"
#include "config.h"
#include "Bloodhaver.h"
#include "MainWidget.h"
#include <cmath>
#include <random>
#include <array>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using std::array;
using std::vector;
using std::random_device;
using std::mt19937;
using std::uniform_real_distribution;
using std::uniform_int_distribution;

//[playerrepresent][num][leve][angel]
std::list<ImageResource>* Army::Walk[2][20][2][8];
std::list<ImageResource>* Army::Disappear[2][20][2][8];
std::list<ImageResource>* Army::Stand[2][20][2][8];
std::list<ImageResource>* Army::Attack[2][20][2][8];
std::list<ImageResource>* Army::Die[2][20][2][8];

//[num][level]
std::string Army::ArmyName[20][2]={{"Clubman","Axeman"},
                                  {"Slinger","Slinger"},
                                  {"Archer","Archer"},
                                  {"Scout","Scout"},
                                  {"Sworder","Sworder"},
                                  {"ImprovedArcher","ImprovedArcher"},
                                  {"Cavalry","Cavalry"},
                                  {"Ship","Ship"}
                                 };

std::string Army::ArmyDisplayName[20][2]={{"棍棒兵","刀斧兵"},
                                         {"投石兵","投石兵"},
                                         {"弓箭手","弓箭手"},
                                         {"侦察骑兵","侦察骑兵"},
                                         {"Prof.Yan","Prof.Yan"},
                                         {"Prof.Lou","Prof.Lou"},
                                         {"Prof.Lu","Prof.Lu"},
                                         {"Prof.Wang","Prof.Wang"},
                                        };

string Army::click_sound = "Click_Army";

Army::Army()
{

}

Army::Army(double DR,double UR,int Num , Development* playerScience, int playerRepresent)
{
    //设置科技树和阵营
    this->playerScience = playerScience;
    this->playerRepresent = playerRepresent;

    this->Num = Num;

    setAttribute();

    setDRUR(DR, UR);
    updateBlockByDetail();

    setSideLenth();
    this->nextBlockDR=BlockDR;
    this->nextBlockUR=BlockUR;
    setPredictedDRUR(DR, UR);
    setPreviousDRUR(DR, UR);
    setDR0UR0(DR, UR);

    this->nowstate=MOVEOBJECT_STATE_STAND;
    this->status=ARMY_STATE_DEFAULT;
    isAttackable = true;
    this->status=0;
    this->ifAttack=false;
    this->timelock=15;

    // 初始化巡逻系统
    initPatrolSystem();

    setNowRes();
    updateImageXYByNowRes();
    this->imageH=DR-UR;

    //设置SN信息
    this->globalNum=10000*getSort()+g_globalNum;
    g_Object.insert({this->globalNum,this});
    g_globalNum++;
}

Army::Army(double DR,double UR,int Num ,int status, Development* playerScience, int playerRepresent,int starttime,int finishtime,double dDR,double dUR)
{
    //设置科技树和阵营
    this->playerScience = playerScience;
    this->playerRepresent = playerRepresent;

    this->Num = Num;

    setAttribute();

    setDRUR(DR, UR);
    updateBlockByDetail();

    setSideLenth();
    this->nextBlockDR=BlockDR;
    this->nextBlockUR=BlockUR;
    setPredictedDRUR(DR, UR);
    setPreviousDRUR(DR, UR);
    setDR0UR0(DR, UR);

    this->nowstate=MOVEOBJECT_STATE_STAND;
    this->status=status;
    this->starttime=starttime;
    this->finishtime=finishtime;
    this->destinaDR=dDR;
    this->destinaUR=dUR;
    this->startpointDR=DR;
    this->startpointUR=UR;
    this->ifAttack=false;
    this->timelock=15;
    isAttackable = true;

    // 初始化巡逻系统
    initPatrolSystem();

    setNowRes();
    updateImageXYByNowRes();
    this->imageH=DR-UR;

    //设置SN信息
    this->globalNum=10000*getSort()+g_globalNum;
    g_Object.insert({this->globalNum,this});
    g_globalNum++;
}

void Army::nextframe()
{
    if(isDie())
    {
        if( !isDying() )
        {
             setPreDie();
             requestSound_Die();
        }
        else if(!get_isActionEnd() && isNowresShift())
        {
            nowres++;
            if( !changeToDisappear && get_isActionEnd())
            {
                changeToDisappear = true;
                nowres_step = 1000;
                setNowRes();
            }
        }
    }
    else
    {
        if(isNowresShift())
        {
            if(nowres == nowlist->begin())
            {
                if(nowstate == MOVEOBJECT_STATE_ATTACK)
                    requestSound_Attack();
                else if(nowobject == this && nowstate == MOVEOBJECT_STATE_WALK && playerRepresent == NOWPLAYERREPRESENT)
                    requestSound_Walk();
            }

            nowres++;
            if(nowres==nowlist->end())
            {
                nowres=nowlist->begin();
                //读到最后回到最初
                initAttack_perCircle();
            }
        }

        updateMove();
        setNowRes();
    }

    if(playerRepresent != 0 && timer_Visible>0) time_BeVisible();

    updateImageXYByNowRes();
    
    // 更新巡逻系统（仅对敌方单位）
    if(playerRepresent == 1) {
        updatePatrol();
    }
}

void Army::setNowRes()
{
    std::list<ImageResource> *templist = NULL;
    if(Num==AT_SHIP){
        switch(this->nowstate){
        case MOVEOBJECT_STATE_STAND:case MOVEOBJECT_STATE_WALK:case MOVEOBJECT_STATE_ATTACK:
            templist =this->Stand[playerRepresent][Num][getLevel()][Angle];
            break;
        default:
                break;
        }
    }
    else{
        switch (this->nowstate) {
        case MOVEOBJECT_STATE_STAND:
            templist =this->Stand[playerRepresent][Num][getLevel()][Angle];
            break;
        case MOVEOBJECT_STATE_WALK:
            templist =this->Walk[playerRepresent][Num][getLevel()][Angle];
            break;
        case MOVEOBJECT_STATE_ATTACK:
            templist =this->Attack[playerRepresent][Num][getLevel()][Angle];
            break;
        case MOVEOBJECT_STATE_DIE:
            if(changeToDisappear) templist = this->Disappear[playerRepresent][Num][getLevel()][Angle];
            else templist =this->Die[playerRepresent][Num][getLevel()][Angle];
            break;
        default:
            break;
        }
    }
    if(templist!= nowlist && templist)
    {
        nowlist = templist;
        nowres = nowlist->begin();
        initAttack_perCircle();
        initNowresTimer();
    }
}


void Army::requestSound_Attack()
{
    if((Num == AT_IMPROVED|| Num == AT_BOWMAN) && isInWidget())
        soundQueue.push("Archer_Attack");
}

void Army::requestSound_Die()
{
    if(!isInWidget())
        return;

    if(Num == AT_SCOUT)
        soundQueue.push("Scout_Die");
    else
        soundQueue.push("Army_Die");
}

void Army::requestSound_Walk()
{
    if(Num == AT_SCOUT && isInWidget())
        soundQueue.push("Scout_Walk");
}
/***********************************************************/
//获取军队的各项数据
//移速
double Army::getSpeed()
{
    double moveSpeed;

    if( upgradable ) moveSpeed = speed_change[getLevel()];
    else moveSpeed = speed;

    return moveSpeed*playerScience->get_rate_Move(getSort(),Num);
}

//血量
int Army::getMaxBlood()
{
    int realmBlood;

    if(upgradable) realmBlood = MaxBlood_change[getLevel()];
    else realmBlood = MaxBlood;

    return  realmBlood*playerScience->get_rate_Blood(getSort(),Num)+playerScience->get_addition_Blood(getSort(),Num);
}

//视野
int Army::getVision()
{
    int realVision;
    if(upgradable) realVision = vision_change[getLevel()];
    else realVision = vision;

    return realVision + playerScience->get_addition_DisAttack(getSort(),Num,armyClass,get_AttackType());
}

//攻击力
int Army::getATK()
{
    int atkValue;//用于存储初始攻击力

    //赋值初始攻击力,依据兵种是否能升级,划分两类赋值方式
    if(upgradable) atkValue = atk_change[getLevel()];
    else atkValue = atk;

    //再atkValue基础上,计算player及科技带来的加成,并返回
//    return (int)( atkValue*playerScience->get_rate_Attack(getSort(),Num,armyClass,get_AttackType()), interactSort, interactNum) + \
//             get_add_specialAttack() + playerScience->get_addition_Attack(getSort(),Num,armyClass,get_AttackType());

    return (int)( atkValue*playerScience->get_rate_Attack(getSort(),Num,armyClass,get_AttackType(), interactSort, interactNum)) + \
            get_add_specialAttack() + playerScience->get_addition_Attack(getSort(),Num,armyClass,get_AttackType());
}

//防御力,分为获取肉搏防御力和投射物防御力
int Army::getDEF(int attackType_got)
{
    int defValue = 0;//用于存储初始防御力

    //赋值defValue;根据attackType_got即收到的伤害类型,选择相应的防御类型:肉盾防御或投射防御.若为祭司转化或(投石车?等)无伤害减免
    if(attackType_got == ATTACKTYPE_CLOSE || attackType_got == ATTACKTYPE_ANIMAL)
    {
        if(upgradable) defValue = defence_close_change[getLevel()];
        else defValue = defence_close;
    }
    else if(attackType_got == ATTACKTYPE_SHOOT)
    {
        if(upgradable) defValue = defence_shoot_change[getLevel()];
        else defValue = defence_shoot;
    }

    //在defValue的基础上,计算player及科技带来的 加成,并返回
    return (int)( defValue*playerScience->get_rate_Defence(getSort(),Num,armyClass,attackType_got) ) + \
            playerScience->get_addition_Defence(getSort(),Num,armyClass,attackType_got);
}

int Army::showATK_Basic()
{
    int atkValue;//用于存储初始攻击力

    //赋值初始攻击力,依据兵种是否能升级,划分两类赋值方式
    if(upgradable) atkValue = atk_change[getLevel()];
    else atkValue = atk;

    return atkValue+get_add_specialAttack();
}

int Army::showDEF_Close()
{
    int defValue = 0;

    if(upgradable) defValue = defence_close_change[getLevel()];
    else defValue = defence_close;

    return defValue;
}

int Army::showDEF_Shoot()
{
    int defValue = 0;

    if(upgradable) defValue = defence_shoot_change[getLevel()];
    else defValue = defence_shoot;

    return defValue;
}

//攻击距离
double Army::getDis_attack()
{
    double dis;

    if(upgradable) dis = dis_Attack_change[getLevel()];
    else dis = dis_Attack;

    if(dis == 0) dis = DISTANCE_ATTACK_CLOSE + (attackObject->getSideLength())/2.0;
    else dis = ( dis + playerScience->get_addition_DisAttack(getSort(),Num,armyClass,get_AttackType() ) )*BLOCKSIDELENGTH;

    return dis;
}

int Army::get_add_specialAttack()
{
    int addition = 0;

    if(Num == AT_SLINGER)
    {
       /* if(interactSort == SORT_ARMY)
        {
            if(interactNum == AT_BOWMAN || interactNum == AT_IMPROVED) addition+=2;
        }
        else */if( interactSort == SORT_BUILDING)
        {
            if(interactNum == BUILDING_ARROWTOWER || interactNum == BUILDING_WALL)
                addition += DEFSHOOT_BUILD_ARROWTOWER;
        }
    }
//    else if(Num == AT_CAVALRY)
//    {
//        if(interactSort == SORT_ARMY)
//        {
//            if(interactNum == AT_CLUBMAN || interactNum == AT_SWORDSMAN)
//                addition+=5;
//        }

//    }

    return addition;
}
/*********************军队自动化参数*****************************/
int Army::getstatus(){
    return this->status;
}
int Army::getstarttime(){
    return this->starttime;
}
int Army::getfinishtime(){
    return this->finishtime;
}
double Army::getstartpointDR(){
    return this->startpointDR;
}
double Army::getstartpointUR(){
      return this->startpointUR;
}
double Army::getdestinaDR(){
    return this->destinaDR;
}
double Army::getdestinaUR(){
     return this->destinaUR;
}
bool Army::getifAttack(){
    return this->ifAttack;
}
int Army::gettimelock(){
    return this->timelock;
}


/***********************************************************/
void Army::setAttribute()
{
    this->Blood=1;
    this->Angle=rand()%8;
    //设置军队属性
    switch (Num) {
    case AT_CLUBMAN:        //棍棒兵,可升级1次
        upgradable = true;
        dependBuildNum = BUILDING_ARMYCAMP;
        dependBuildAct = BUILDING_ARMYCAMP_UPGRADE_CLUBMAN;
        armyClass = ARMY_INFANTRY;
        attackType = ATTACKTYPE_CLOSE;

        MaxBlood_change = new int[2]{ BLOOD_CLUBMAN1,BLOOD_CLUBMAN2 };
        speed_change = new double[2]{ SPEED_CLUBMAN1,SPEED_CLUBMAN2 };
        vision_change = new int[2]{ VISION_CLUBMAN1,VISION_CLUBMAN2 };
        atk_change  = new int[2]{ATK_CLUBMAN1,ATK_CLUBMAN2};
        dis_Attack_change  = new double[2]{DIS_CLUBMAN1 , DIS_CLUBMAN2};
        inter_Attack_change = new double[2]{ INTERVAL_CLUBMAN1,INTERVAL_CLUBMAN2 };
        defence_close_change  = new int[2]{ DEFCLOSE_CLUBMAN1,DEFCLOSE_CLUBMAN2 };
        defence_shoot_change  = new int[2]{ DEFSHOOT_CLUBMAN1,DEFSHOOT_CLUBMAN2 };

        crashLength = CRASHBOX_SINGLEOB;
        nowres_step = NOWRES_TIMER_CLUBMAN;

        break;

//    case AT_SWORDSMAN:  //短剑兵,可升级3次
//        upgradable = true;
//        armyClass = ARMY_INFANTRY;
//        attackType = ATTACKTYPE_CLOSE;

//        MaxBlood_change = new int[4]{ BLOOD_SHORTSWORDSMAN1,BLOOD_SHORTSWORDSMAN2,BLOOD_SHORTSWORDSMAN3,BLOOD_SHORTSWORDSMAN4 };
//        speed_change = new double[4]{ SPEED_SHORTSWORDSMAN1,SPEED_SHORTSWORDSMAN2,SPEED_SHORTSWORDSMAN3,SPEED_SHORTSWORDSMAN4 };
//        vision_change = new int[4]{ VISION_SHORTSWORDSMAN1,VISION_SHORTSWORDSMAN2,VISION_SHORTSWORDSMAN3,VISION_SHORTSWORDSMAN4 };
//        atk_change  = new int[4]{ATK_SHORTSWORSMAN1,ATK_SHORTSWORSMAN2,ATK_SHORTSWORSMAN3,ATK_SHORTSWORSMAN4};
//        dis_Attack_change  = new double[4]{DIS_SHORTSWORDSMAN1 , DIS_SHORTSWORDSMAN2,DIS_SHORTSWORDSMAN3,DIS_SHORTSWORDSMAN4};
//        inter_Attack_change = new double[4]{ INTERVAL_SHORTSWORDSMAN1,INTERVAL_SHORTSWORDSMAN2,INTERVAL_SHORTSWORDSMAN3,INTERVAL_SHORTSWORDSMAN4 };
//        defence_close_change  = new int[4]{ DEFCLOSE_SHORTSWORSMAN1,DEFCLOSE_SHORTSWORSMAN2,DEFCLOSE_SHORTSWORSMAN3,DEFCLOSE_SHORTSWORSMAN4 };
//        defence_shoot_change  = new int[4]{ DEFSHOOT_SHORTSWORSMAN1,DEFSHOOT_SHORTSWORSMAN2,DEFSHOOT_SHORTSWORSMAN3,DEFSHOOT_SHORTSWORSMAN4 };
//        break;

    case AT_SWORDSMAN:    //投石者
        upgradable = false;
        dependBuildNum = BUILDING_ARMYCAMP;
        armyClass = ARMY_INFANTRY;
        attackType = ATTACKTYPE_CLOSE;

        MaxBlood = BLOOD_SHORTSWORDSMAN1;
        speed = SPEED_SHORTSWORDSMAN1;
        vision = VISION_SHORTSWORDSMAN1;
        atk = ATK_SHORTSWORSMAN1;
        dis_Attack = DIS_SHORTSWORDSMAN1;
        inter_Attack = INTERVAL_SHORTSWORDSMAN1;
        defence_close = DEFCLOSE_SHORTSWORSMAN1;
        defence_shoot = DEFSHOOT_SHORTSWORSMAN1;

        crashLength = CRASHBOX_SINGLEOB;
        nowres_step = NOWRES_TIMER_SWORSMAN;
        break;

    case AT_SLINGER:    //投石者
        upgradable = false;
        dependBuildNum = BUILDING_ARMYCAMP;
        armyClass = ARMY_INFANTRY;
        attackType = ATTACKTYPE_SHOOT;

        MaxBlood = BLOOD_SLINGER;
        speed = SPEED_SLINGER;
        vision = VISION_SLINGER;
        atk = ATK_SLINGER;
        dis_Attack = DIS_SLINGER;
        inter_Attack = INTERVAL_SLINGER;
        defence_close = DEFCLOSE_SLINGER;
        defence_shoot = DEFSHOOT_SLINGER;

        crashLength = CRASHBOX_SINGLEOB;

        type_Missile = Missile_Cobblestone;
        phaseFromEnd_MissionAttack = THROWMISSION_SLINGER;

        nowres_step = NOWRES_TIMER_SLINGER;
        break;

    case AT_BOWMAN:     //弓箭手
        upgradable = false;
        dependBuildNum = BUILDING_RANGE;
        armyClass = ARMY_ARCHER;
        attackType = ATTACKTYPE_SHOOT;

        MaxBlood = BLOOD_BOWMAN;
        speed = SPEED_BOWMAN;
        vision = VISION_BOWMAN;
        atk = ATK_BOWMAN;
        dis_Attack = DIS_BOWMAN;
        inter_Attack = INTERVAL_BOWMAN;
        defence_close = DEFCLOSE_BOWMAN;
        defence_shoot = DEFSHOOT_BOWMAN;

        crashLength = CRASHBOX_SINGLEOB;

        type_Missile = Missile_Arrow;
        phaseFromEnd_MissionAttack = THROWMISSION_ARCHER;

        nowres_step = NOWRES_TIMER_BOWMAN;
        break;

    case AT_IMPROVED:     //弓箭手
        upgradable = false;
        dependBuildNum = BUILDING_RANGE;
        armyClass = ARMY_ARCHER;
        attackType = ATTACKTYPE_SHOOT;

        MaxBlood = BLOOD_IMPROVEDBOWMAN1;
        speed = SPEED_IMPROVEDBOWMAN1;
        vision = VISION_IMPROVEDBOWMAN1;
        atk = ATK_IMPROVEDBOWMAN1;
        dis_Attack = DIS_IMPROVEDBOWMAN1;
        inter_Attack = INTERVAL_IMPROVEDBOWMAN1;
        defence_close = DEFCLOSE_IMPROVEDBOWMAN1;
        defence_shoot = DEFSHOOT_IMPROVEDBOWMAN1;

        crashLength = CRASHBOX_SINGLEOB;

        type_Missile = Missile_Arrow;
        phaseFromEnd_MissionAttack = THROWMISSION_IMPROVEDBOWMAN1;

        nowres_step = NOWRES_TIMER_IMPROVEDBOWMAN1;
        break;

    case AT_SCOUT:      //侦察骑兵
        upgradable = false;
        dependBuildNum = BUILDING_STABLE;
        armyClass = ARMY_RIDER;
        attackType = ATTACKTYPE_CLOSE;

        MaxBlood = BLOOD_SCOUT;
        speed = SPEED_SCOUT;
        vision = VISION_SCOUT;
        atk = ATK_SCOUT;
        dis_Attack = DIS_SCOUT;
        inter_Attack = INTERVAL_SCOUT;
        defence_close = DEFCLOSE_SCOUT;
        defence_shoot = DEFSHOOT_SCOUT;

        crashLength = CRASHBOX_SMALLOB;

        nowres_step = NOWRES_TIMER_SCOUT;
        break;

    case AT_CAVALRY:      //侦察骑兵
        upgradable = false;
        dependBuildNum = BUILDING_STABLE;
        armyClass = ARMY_RIDER;
        attackType = ATTACKTYPE_CLOSE;

        MaxBlood = BLOOD_CAVALRY;
        speed = SPEED_CAVALRY;
        vision = VISION_CAVALRY;
        atk = ATK_CAVALRY;
        dis_Attack = DIS_CAVALRY;
        inter_Attack = INTERVAL_CAVALRY;
        defence_close = DEFCLOSE_CAVALRY;
        defence_shoot = DEFSHOOT_CAVALRY;

        crashLength = CRASHBOX_SMALLOB;

        nowres_step = NOWRES_TIMER_CAVALRY;
        break;
    case AT_SHIP:           //战船
        upgradable = false;
        dependBuildNum = BUILDING_DOCK;
        armyClass = ARMY_ARCHER;
        attackType = ATTACKTYPE_SHOOT;

        MaxBlood = BLOOD_SHIP;
        speed = SPEED_SHIP;
        vision = VISION_SHIP;
        atk = ATK_SHIP;
        dis_Attack = DIS_SHIP;
        inter_Attack = INTERVAL_SHIP;
        defence_close = DEFCLOSE_SHIP;
        defence_shoot = DEFSHOOT_SHIP;

        crashLength = CRASHBOX_SINGLEOB;

        type_Missile = Missile_Arrow;
        phaseFromEnd_MissionAttack = THROWMISSION_ARCHER;

        nowres_step = NOWRES_TIMER_IMPROVEDBOWMAN1;
        break;
    default:
        incorrectNum = true;
        break;
    }

}


int Army::getLevel()
{
    /**
    *   传出：士兵等级
    *   通过查询player科技树表，得到当前player管控的该种类士兵的等级
    *   如果该种类士兵无法升级，则默认为0级
    */
    if(upgradable) return playerScience->getActLevel(dependBuildNum , dependBuildAct);
    else return 0;
}

/*************************析构**********************************/
Army::~Army()
{
    if(MaxBlood_change!=NULL)
    {
        delete MaxBlood_change;
        MaxBlood_change = NULL;
    }

    if(speed_change!=NULL)
    {
        delete speed_change;
        speed_change = NULL;
    }

    if(vision_change!=NULL)
    {
        delete vision_change;
        vision_change = NULL;
    }

    if(atk_change!=NULL)
    {
        delete atk_change;
        atk_change = NULL;
    }

    if(dis_Attack_change!=NULL)
    {
        delete dis_Attack_change;
        dis_Attack_change = NULL;
    }

    if(inter_Attack_change!=NULL)
    {
        delete inter_Attack_change;
        inter_Attack_change = NULL;
    }

    if(defence_close_change!=NULL)
    {
        delete defence_close_change;
        defence_close_change = NULL;
    }

    if(defence_shoot_change!=NULL)
    {
        delete defence_shoot_change;
        defence_shoot_change = NULL;
    }

}

// 巡逻系统实现

void Army::initPatrolSystem()
{
    patrolState = PATROL_STATE_IDLE;
    patrolCenterDR = 0;
    patrolCenterUR = 0;
    currentPatrolTargetDR = 0;
    currentPatrolTargetUR = 0;
    chaseTarget = nullptr;
    patrolTimer = 0;
    patrolDirection = 1;  // 1顺时针, -1逆时针
    lastPatrolAngle = 0;
    patrolInitialized = false;
}

bool Army::hasPatrolArea()
{
    // 检查这个单位是否有巡逻区域 (通过globalNum查找enemyAreaLimit)
    extern Map* GlobalMap;
    if(!GlobalMap) return false;
    
    return GlobalMap->enemyAreaLimit.find(this->globalNum) != GlobalMap->enemyAreaLimit.end();
}

void Army::startPatrol()
{
    if(!hasPatrolArea()) return;
    
    extern Map* GlobalMap;
    auto& areaData = GlobalMap->enemyAreaLimit[this->globalNum];
    
    // 根据区域类型设置巡逻中心点
    if(areaData.first == "Circle") {
        auto* circleData = (array<double,3>*)areaData.second;
        patrolCenterDR = (*circleData)[0];
        patrolCenterUR = (*circleData)[1];
        // 半径信息在(*circleData)[2]中
    }
    else if(areaData.first == "Rect") {
        auto* rectData = (array<double,4>*)areaData.second;
        patrolCenterDR = (*rectData)[0];
        patrolCenterUR = (*rectData)[1];
    }
    else if(areaData.first == "Line") {
        auto* lineData = (vector<array<double,2>>*)areaData.second;
        if(!lineData->empty()) {
            // 使用第一个点作为中心点
            patrolCenterDR = (*lineData)[0][0];
            patrolCenterUR = (*lineData)[0][1];
        }
    }
    
    patrolState = PATROL_STATE_PATROLLING;
    patrolInitialized = true;
    generatePatrolPath();
}

void Army::generatePatrolPath()
{
    if(!hasPatrolArea()) return;
    
    extern Map* GlobalMap;
    auto& areaData = GlobalMap->enemyAreaLimit[this->globalNum];
    
    static random_device rd;
    static mt19937 gen(rd());
    
    if(areaData.first == "Circle") {
        auto* circleData = (array<double,3>*)areaData.second;
        double centerDR = (*circleData)[0];
        double centerUR = (*circleData)[1];
        double radius = (*circleData)[2];
        
        // 在圆形区域内生成随机巡逻点
        uniform_real_distribution<> angleDist(0.0, 2 * M_PI);
        uniform_real_distribution<> radiusDist(0.0, radius * 0.8);  // 避免太靠近边缘
        
        double angle = angleDist(gen);
        double r = radiusDist(gen);
        
        currentPatrolTargetDR = centerDR + r * cos(angle);
        currentPatrolTargetUR = centerUR + r * sin(angle);
        lastPatrolAngle = angle;
    }
    else if(areaData.first == "Rect") {
        auto* rectData = (array<double,4>*)areaData.second;
        double centerDR = (*rectData)[0];
        double centerUR = (*rectData)[1];
        double width = (*rectData)[2];
        double height = (*rectData)[3];
        
        // 在矩形区域内生成随机巡逻点
        uniform_real_distribution<> drDist(centerDR - width/2 * 0.8, centerDR + width/2 * 0.8);
        uniform_real_distribution<> urDist(centerUR - height/2 * 0.8, centerUR + height/2 * 0.8);
        
        currentPatrolTargetDR = drDist(gen);
        currentPatrolTargetUR = urDist(gen);
    }
    else if(areaData.first == "Line") {
        auto* lineData = (vector<array<double,2>>*)areaData.second;
        if(!lineData->empty()) {
            // 在线性区域中随机选择一个点
            uniform_int_distribution<> pointDist(0, lineData->size() - 1);
            int pointIndex = pointDist(gen);
            
            currentPatrolTargetDR = (*lineData)[pointIndex][0];
            currentPatrolTargetUR = (*lineData)[pointIndex][1];
        }
    }
}

bool Army::isInPatrolArea()
{
    if(!hasPatrolArea()) return false;
    
    extern Map* GlobalMap;
    auto& areaData = GlobalMap->enemyAreaLimit[this->globalNum];
    
    double currentDR = getDR();
    double currentUR = getUR();
    
    if(areaData.first == "Circle") {
        auto* circleData = (array<double,3>*)areaData.second;
        double centerDR = (*circleData)[0];
        double centerUR = (*circleData)[1];
        double radius = (*circleData)[2];
        
        double dx = currentDR - centerDR;
        double dy = currentUR - centerUR;
        double distance = sqrt(dx*dx + dy*dy);
        
        return distance <= radius;
    }
    else if(areaData.first == "Rect") {
        auto* rectData = (array<double,4>*)areaData.second;
        double centerDR = (*rectData)[0];
        double centerUR = (*rectData)[1];
        double width = (*rectData)[2];
        double height = (*rectData)[3];
        
        double halfWidth = width / 2;
        double halfHeight = height / 2;
        
        return (currentDR >= centerDR - halfWidth && currentDR <= centerDR + halfWidth) &&
               (currentUR >= centerUR - halfHeight && currentUR <= centerUR + halfHeight);
    }
    else if(areaData.first == "Line") {
        // 对于线性区域，检查是否在任何线段附近
        auto* lineData = (vector<array<double,2>>*)areaData.second;
        const double tolerance = 50.0;  // 容忍距离
        
        for(size_t i = 0; i < lineData->size(); i++) {
            double dx = currentDR - (*lineData)[i][0];
            double dy = currentUR - (*lineData)[i][1];
            double distance = sqrt(dx*dx + dy*dy);
            
            if(distance <= tolerance) return true;
        }
        return false;
    }
    
    return false;
}

Coordinate* Army::detectEnemyInRange()
{
    // 检测视野范围内的敌方单位（玩家单位）
    double visionRange = getVision();
    double currentDR = getDR();
    double currentUR = getUR();
    
    extern Map* GlobalMap;
    if(!GlobalMap) return nullptr;
    
    // 检查玩家0的所有单位
    for(Human* human : GlobalMap->player[0]->human) {
        if(!human) continue;
        
        BloodHaver* bloodHaver = nullptr;
        human->printer_ToBloodHaver((void**)&bloodHaver);
        if(!bloodHaver || bloodHaver->isDie()) continue;
        
        double dx = human->getDR() - currentDR;
        double dy = human->getUR() - currentUR;
        double distance = sqrt(dx*dx + dy*dy);
        
        if(distance <= visionRange) {
            return human;  // 发现敌人
        }
    }
    
    return nullptr;
}

void Army::updatePatrol()
{
    // 只有敌方单位(player[1])才进行巡逻
    if(getPlayerRepresent() != 1) return;
    
    // 如果单位已死亡或正在死亡，停止巡逻
    if(isDie() || isDying()) {
        patrolState = PATROL_STATE_IDLE;
        return;
    }
    
    // 首次初始化巡逻
    if(!patrolInitialized && hasPatrolArea()) {
        startPatrol();
        return;
    }
    
    patrolTimer++;
    
    switch(patrolState) {
        case PATROL_STATE_IDLE:
            // 检查是否应该开始巡逻
            if(hasPatrolArea() && !isAttacking()) {
                startPatrol();
            }
            break;
            
        case PATROL_STATE_PATROLLING:
        {
            // 检测敌人
            Coordinate* enemy = detectEnemyInRange();
            if(enemy) {
                chaseTarget = enemy;
                patrolState = PATROL_STATE_CHASING;
                // 开始攻击敌人
                extern MainWidget* g_mainWidget;
                if(g_mainWidget && g_mainWidget->getCore() && g_mainWidget->getCore()->interactionList) {
                    g_mainWidget->getCore()->interactionList->addRelation(this, enemy, CoreEven_Attacking);
                }
                break;
            }
            
            // 检查是否到达巡逻目标点
            double dx = currentPatrolTargetDR - getDR();
            double dy = currentPatrolTargetUR - getUR();
            double distance = sqrt(dx*dx + dy*dy);
            
            if(distance < 30.0) {  // 到达目标点
                generatePatrolPath();  // 生成新的巡逻点
                patrolTimer = 0;
            } else if(!isWalking() && patrolTimer > 60) {  // 如果停止移动且超时
                // 移动到巡逻目标点
                extern MainWidget* g_mainWidget;
                if(g_mainWidget && g_mainWidget->getCore() && g_mainWidget->getCore()->interactionList) {
                    g_mainWidget->getCore()->interactionList->addRelation(this, currentPatrolTargetDR, currentPatrolTargetUR, CoreEven_JustMoveTo);
                }
                patrolTimer = 0;
            }
            break;
        }
        
        case PATROL_STATE_CHASING:
        {
            // 检查追击目标是否有效
            if(!chaseTarget) {
                patrolState = PATROL_STATE_RETURNING;
                break;
            }
            
            BloodHaver* targetBloodHaver = nullptr;
            chaseTarget->printer_ToBloodHaver((void**)&targetBloodHaver);
            if(!targetBloodHaver || targetBloodHaver->isDie()) {
                chaseTarget = nullptr;
                patrolState = PATROL_STATE_RETURNING;
                break;
            }
            
            // 检查是否失去目标（超出视野）
            double dx = chaseTarget->getDR() - getDR();
            double dy = chaseTarget->getUR() - getUR();
            double distance = sqrt(dx*dx + dy*dy);
            
            if(distance > getVision() * 1.5) {  // 失去目标
                chaseTarget = nullptr;
                patrolState = PATROL_STATE_RETURNING;
            }
            // 继续攻击逻辑由AI系统处理
            break;
        }
        
        case PATROL_STATE_RETURNING:
        {
            // 检查是否已回到巡逻区域
            if(isInPatrolArea()) {
                patrolState = PATROL_STATE_PATROLLING;
                generatePatrolPath();
                patrolTimer = 0;
            } else {
                // 移动回巡逻区域中心
                if(!isWalking() && patrolTimer > 60) {
                    extern MainWidget* g_mainWidget;
                    if(g_mainWidget && g_mainWidget->getCore() && g_mainWidget->getCore()->interactionList) {
                        g_mainWidget->getCore()->interactionList->addRelation(this, patrolCenterDR, patrolCenterUR, CoreEven_JustMoveTo);
                    }
                    patrolTimer = 0;
                }
            }
            break;
        }
    }
}

void Army::returnToPatrolArea()
{
    if(hasPatrolArea()) {
        patrolState = PATROL_STATE_RETURNING;
        chaseTarget = nullptr;
        patrolTimer = 0;
    }
}

void Army::stopPatrol()
{
    patrolState = PATROL_STATE_IDLE;
    chaseTarget = nullptr;
    patrolTimer = 0;
}
