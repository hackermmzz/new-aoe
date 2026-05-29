#include "enemyai.h"
#include "MainWidget.h"
#include "Human.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cfloat>
#include <unordered_map>
#include <map>
#include <utility>

using std::string;
using std::vector;
using std::unordered_map;

tagGame tagEnemyGame;
ins EnemyIns;
/*##########DO NOT EDIT ABOVE##########*/
#define WAITING -1
#define DEFENSE 2
#define ATTACK 3
#define DESTROY 3
#define COUNTER 4
#define RETREAT 5
#define AROUND 1
#define MODE1 15
#define MODE2 15000
#define MODE4 30000
#define VECTORARMY 1
#define VECTORFARMER 2
#define VECTORSHIP 3
#define VECTORBOAT 4
#define VECTORARROWTOWER 5
#define VECTORDEFEND 6
#define VECTORBUILDING 7
#define TAGFARMER 1
#define TAGARMY 2
#define TAGBUILDING 3
tagInfo enemyInfo;
//-----------新参数--------------//
#define FAT 6000     //第一波骚扰时间
#define SAT 12000    //第二波骚扰时间
#define TAT 19500    //第三波骚扰时间
#define radius_Outer 15
#define radius_Inner 10

static int vision[128][128];
static int around[100];
static bool ifAttack[50];
static map<int,int> timer;
static vector <int> Army;
static vector <int> Farmer;
static vector <int> Ship;
static vector <int> Boat;
static vector <int> Arrowtower;
static vector <int> Building;
static vector <int> Defend;
static map<int, bool> ifA;
static int sum;
static int mode = -3;

static pair<double,double>Enemy_Center;                    //enemy武器工程厂
static unordered_map<int,int> Defend_Center_Enemy;         //仅在内圈防御武器工程厂的人
// 第一波总目标（击杀 3 农民）是否已完成；完成后不再往 attackEnemy 里加人，避免反复加人/撤退
static bool wave1Completed = false;
static bool wave2Completed = false;
// 每单位上次分配攻击目标的帧号，避免每帧重复下令导致部分兵种抽搐
static map<int, int> lastAssignFrame;
// 每单位上次补发攻击指令的帧号（目标仍活着但引擎清空指令时），用较短间隔快速补发避免卡住
static map<int, int> lastReissueFrame;
// 每单位士兵当前的行动目标
static map<int, int> currentTarget;


static bool wave1Started = false;
static bool wave2Started = false;
static bool wave3Started = false;
static bool wave3Completed = false;

static vector<int> wave1Units;   // 第一波：2 个棍棒兵 + 2 个侦察兵
static vector<int> wave2Units;   // 第二波：指定铜器兵组合 + 第一波残兵
static vector<int> wave3Units;   // 第三波：20 个铜器陆军 + 前两波残兵

static vector<int> wave1TouchedFarmers;
static vector<int> wave2TouchedFarmers;
static vector<int> wave1KilledFarmers;
static vector<int> wave2KilledFarmers;

// 所有骚扰兵原始位置，用于撤退
static map<int, pair<double, double>> HarassHome;

// 武器攻城厂附近防守兵原始位置
static map<int, pair<double, double>> DefenseHome;

// 防止每帧重复下令导致单位抽搐
static map<int, int> waveLastOrderFrame;
static map<int, int> defenseLastOrderFrame;

static map<int, int> fieldSelfDefenseLastOrderFrame;

#define FIELD_SELF_DEFENSE_ORDER_INTERVAL 12
#define FIELD_ASSIST_RADIUS 8

#define TOWER_DANGER_RANGE (ATK_BUILD_ARROWTOWER + 1)
#define WAVE_ORDER_INTERVAL 50
#define DEFENSE_ORDER_INTERVAL 20
#define DEFENSE_CHASE_LIMIT (radius_Outer + 3)

//isElementExists函数，用于判断目标容器中的element值是否还存活，存在返回true，不存在返回false，sort为需要检查的类型
bool isElementExists( int element,int sort) {
    switch(sort){
    case TAGFARMER:
        for(int i=0;i<enemyInfo.enemy_farmers.size();i++){
            if(element==enemyInfo.enemy_farmers[i].SN){
                return true;
            }
        }
        return false;
    case TAGARMY:
        for(int i=0;i<enemyInfo.enemy_armies.size();i++){
            if(element==enemyInfo.enemy_armies[i].SN){
                return true;
            }
        }
        return false;
    case TAGBUILDING:
        for(int i=0;i<enemyInfo.enemy_buildings.size();i++){
            if(element==enemyInfo.enemy_buildings[i].SN){
                return true;
            }
        }
        return false;
    }

}

//isElement函数，用于判断当前容器中是否存在该element值
bool isElement(const std::vector<int>& vec,int element){
    auto it = std::find(vec.begin(), vec.end(), element);
    if (it != vec.end()) {
    return true;
    } else {
    return false;
    }
}

//visionChange函数，用于每隔一定帧数，刷新视野图，其中区分defend视野与非defend视野
static void visionChange(){
    for(int i=0;i<128;i++){
        for(int j=0;j<128;j++){
            vision[i][j]=-1;
        }
    }
    for(int i=0;i<enemyInfo.armies.size();i++){
        if(vision[enemyInfo.armies[i].BlockDR][enemyInfo.armies[i].BlockUR]<0&&enemyInfo.armies[i].status==ARMY_STATE_DEFENSE){
            for(int j=-3;j<4;j++){
                for(int n=-3;n<4;n++){
                    if(enemyInfo.armies[i].BlockDR+j>0&&enemyInfo.armies[i].BlockDR+j<128&&enemyInfo.armies[i].BlockUR+n>0&&enemyInfo.armies[i].BlockUR+n<128)
                    vision[enemyInfo.armies[i].BlockDR+j][enemyInfo.armies[i].BlockUR+n]=130;
                }
            }
       }else if(vision[enemyInfo.armies[i].BlockDR][enemyInfo.armies[i].BlockUR]<0&&enemyInfo.armies[i].status==ARMY_STATE_ATTACK){
            for(int j=-3;j<4;j++){
                for(int n=-3;n<4;n++){
                     if(enemyInfo.armies[i].BlockDR+j>0&&enemyInfo.armies[i].BlockDR+j<128&&enemyInfo.armies[i].BlockUR+n>0&&enemyInfo.armies[i].BlockUR+n<128)
                    vision[enemyInfo.armies[i].BlockDR+j][enemyInfo.armies[i].BlockUR+n]=255;
                }
            }
        }
        else if(vision[enemyInfo.armies[i].BlockDR][enemyInfo.armies[i].BlockUR]<0&&enemyInfo.armies[i].Sort==AT_SCOUT){
            for(int j=-5;j<6;j++){
                for(int n=-5;n<6;n++){
                     if(enemyInfo.armies[i].BlockDR+j>0&&enemyInfo.armies[i].BlockDR+j<128&&enemyInfo.armies[i].BlockUR+n>0&&enemyInfo.armies[i].BlockUR+n<128)
                    vision[enemyInfo.armies[i].BlockDR+j][enemyInfo.armies[i].BlockUR+n]=255;
                }
            }
        }
        else if(vision[enemyInfo.armies[i].BlockDR][enemyInfo.armies[i].BlockUR]<0&&enemyInfo.armies[i].Sort==AT_SHIP){
            for(int j=-9;j<10;j++){
                for(int n=-9;n<10;n++){
                     if(enemyInfo.armies[i].BlockDR+j>0&&enemyInfo.armies[i].BlockDR+j<128&&enemyInfo.armies[i].BlockUR+n>0&&enemyInfo.armies[i].BlockUR+n<128)
                    vision[enemyInfo.armies[i].BlockDR+j][enemyInfo.armies[i].BlockUR+n]=255;
                }
            }
        }
}
}

static void ifDestory(){
    auto it = ifA.begin();
    auto is = timer.begin();
    while (it != ifA.end()){
        if(!isElementExists(it->first,TAGARMY)){
            ifA.erase(it);
            timer.erase(is);
        }else if(!isElementExists(it->first,TAGBUILDING)) {
            ifA.erase(it);
            timer.erase(is);
        }
        else {it++;is++;}
    }
}

//seek函数，每隔一定帧数，更新所有在视野图内的单位
static void seek(){
    for(int i=0;i<enemyInfo.enemy_armies.size();i++){
        if(vision[enemyInfo.enemy_armies[i].BlockDR][enemyInfo.enemy_armies[i].BlockUR]==255&&enemyInfo.enemy_armies[i].Sort!=7){
            if(!isElement(Army,enemyInfo.enemy_armies[i].SN))
            Army.push_back(enemyInfo.enemy_armies[i].SN);
        }
        else if(vision[enemyInfo.enemy_armies[i].BlockDR][enemyInfo.enemy_armies[i].BlockUR]==255&&enemyInfo.enemy_armies[i].Sort==7){
            if(!isElement(Ship,enemyInfo.enemy_armies[i].SN))
            Ship.push_back(enemyInfo.enemy_armies[i].SN);
        }
        else if(vision[enemyInfo.enemy_armies[i].BlockDR][enemyInfo.enemy_armies[i].BlockUR]==130){
            if(!isElement(Defend,enemyInfo.enemy_armies[i].SN))
            Defend.push_back(enemyInfo.enemy_armies[i].SN);
        }
    }
    for(int i=0;i<enemyInfo.enemy_farmers.size();i++){
        if(vision[enemyInfo.enemy_farmers[i].BlockDR][enemyInfo.enemy_farmers[i].BlockUR]==255&&enemyInfo.enemy_farmers[i].FarmerSort==0){
            if(!isElement(Farmer,enemyInfo.enemy_farmers[i].SN))
            Farmer.push_back(enemyInfo.enemy_farmers[i].SN);
        }
        else if(vision[enemyInfo.enemy_farmers[i].BlockDR][enemyInfo.enemy_farmers[i].BlockUR]==255&&enemyInfo.enemy_farmers[i].FarmerSort!=0){
            if(!isElement(Boat,enemyInfo.enemy_farmers[i].SN))
            Boat.push_back(enemyInfo.enemy_farmers[i].SN);
        }
//        else if(vision[enemyInfo.enemy_farmers[i].BlockDR][enemyInfo.enemy_farmers[i].BlockUR]==1){
//            Defend.push_back(enemyInfo.enemy_farmers[i].SN);
//        }
    }
    for(int i=0;i<enemyInfo.enemy_buildings.size();i++){
        if(vision[enemyInfo.enemy_buildings[i].BlockDR][enemyInfo.enemy_buildings[i].BlockUR]==255&&enemyInfo.enemy_buildings[i].Type!=BUILDING_ARROWTOWER){
            if(!isElement(Building,enemyInfo.enemy_buildings[i].SN))
            Building.push_back(enemyInfo.enemy_buildings[i].SN);
        }
        else if(vision[enemyInfo.enemy_buildings[i].BlockDR][enemyInfo.enemy_buildings[i].BlockUR]==255&&enemyInfo.enemy_buildings[i].Type==BUILDING_ARROWTOWER){
            if(!isElement(Arrowtower,enemyInfo.enemy_buildings[i].SN))
            Arrowtower.push_back(enemyInfo.enemy_buildings[i].SN);
        }
//        else if(vision[enemyInfo.enemy_buildings[i].BlockDR][enemyInfo.enemy_buildings[i].BlockUR]==1){
//            Defend.push_back(enemyInfo.enemy_buildings[i].SN);
//        }
    }
}

//ifVisible函数，每隔一定帧数，将所有离开视野的单位清除
static void ifVisible(){
    for(int i=0;i<enemyInfo.enemy_armies.size();i++){
        if(vision[enemyInfo.enemy_armies[i].BlockDR][enemyInfo.enemy_armies[i].BlockUR]<0&&enemyInfo.enemy_armies[i].Sort!=7&&isElement(Army,enemyInfo.enemy_armies[i].SN)){
            Army.erase(std::remove(Army.begin(),Army.end(),enemyInfo.enemy_armies[i].SN),Army.end());
        }
        else if(vision[enemyInfo.enemy_armies[i].BlockDR][enemyInfo.enemy_armies[i].BlockUR]<0&&enemyInfo.enemy_armies[i].Sort==7&&isElement(Ship,enemyInfo.enemy_armies[i].SN)){
            Ship.erase(std::remove(Ship.begin(),Ship.end(),enemyInfo.enemy_armies[i].SN),Ship.end());
        }
        else if(vision[enemyInfo.enemy_armies[i].BlockDR][enemyInfo.enemy_armies[i].BlockUR]<0&&isElement(Defend,enemyInfo.enemy_armies[i].SN)){
            Defend.erase(std::remove(Defend.begin(),Defend.end(),enemyInfo.enemy_armies[i].SN),Defend.end());
        }
}
    for(int i=0;i<enemyInfo.enemy_farmers.size();i++){
        if(vision[enemyInfo.enemy_farmers[i].BlockDR][enemyInfo.enemy_farmers[i].BlockUR]<0&&enemyInfo.enemy_farmers[i].FarmerSort==0&&isElement(Farmer,enemyInfo.enemy_farmers[i].SN)){
            Farmer.erase(std::remove(Farmer.begin(),Farmer.end(),enemyInfo.enemy_farmers[i].SN),Farmer.end());
        }
        else if(vision[enemyInfo.enemy_farmers[i].BlockDR][enemyInfo.enemy_farmers[i].BlockUR]<0&&enemyInfo.enemy_farmers[i].FarmerSort!=0&&isElement(Boat,enemyInfo.enemy_farmers[i].SN)){
            Boat.erase(std::remove(Boat.begin(),Boat.end(),enemyInfo.enemy_farmers[i].SN),Boat.end());
        }
//        else if(vision[enemyInfo.enemy_farmers[i].BlockDR][enemyInfo.enemy_farmers[i].BlockUR]<0&&isElement(Defend,enemyInfo.enemy_farmers[i].SN)){
//            Defend.erase(std::remove(Defend.begin(),Defend.end(),enemyInfo.enemy_farmers[i].SN),Defend.end());
//        }
    }
    for(int i=0;i<enemyInfo.enemy_buildings.size();i++){
        if(vision[enemyInfo.enemy_buildings[i].BlockDR][enemyInfo.enemy_buildings[i].BlockUR]<0&&enemyInfo.enemy_buildings[i].Type!=BUILDING_ARROWTOWER&&isElement(Building,enemyInfo.enemy_buildings[i].SN)){
            Building.erase(std::remove(Building.begin(),Building.end(),enemyInfo.enemy_buildings[i].SN),Building.end());
        }
        else if(vision[enemyInfo.enemy_buildings[i].BlockDR][enemyInfo.enemy_buildings[i].BlockUR]<0&&enemyInfo.enemy_buildings[i].Type==BUILDING_ARROWTOWER&&isElement(Arrowtower,enemyInfo.enemy_buildings[i].SN)){
            Arrowtower.erase(std::remove(Arrowtower.begin(),Arrowtower.end(),enemyInfo.enemy_buildings[i].SN),Arrowtower.end());
        }
//        else if(vision[enemyInfo.enemy_buildings[i].BlockDR][enemyInfo.enemy_buildings[i].BlockUR]<0&&isElement(Defend,enemyInfo.enemy_buildings[i].SN)){
//            Defend.erase(std::remove(Defend.begin(),Defend.end(),enemyInfo.enemy_buildings[i].SN),Defend.end());
//        }
    }
}

//ifATTACK函数，为不同类型的己方单位启动攻击，各类型单位的攻击仇恨权重不同
static void ifATTACK(){
    if(Army.size()!=0||Farmer.size()!=0||Building.size()!=0||Arrowtower.size()!=0){
        for(int i=0;i<enemyInfo.armies.size();i++){
            if((enemyInfo.armies[i].status==1||enemyInfo.armies[i].status==3&&enemyInfo.armies[i].Sort!=7)&&ifA[enemyInfo.armies[i].SN]==false){
                ifA[enemyInfo.armies[i].SN]=true;
            }
        }
    }else if(Army.size()+Farmer.size()+Building.size()+Arrowtower.size()==0){
        for(int i=0;i<enemyInfo.armies.size();i++)
            if((enemyInfo.armies[i].status==1||enemyInfo.armies[i].status==3&&enemyInfo.armies[i].Sort!=7)&& ifA[enemyInfo.armies[i].SN]==true){
                 ifA[enemyInfo.armies[i].SN]=false;
            }
    }
    if(Ship.size()!=0||Boat.size()!=0||Army.size()!=0||Farmer.size()!=0||Building.size()!=0||Arrowtower.size()!=0){
        for(int i=0;i<enemyInfo.armies.size();i++){
            if((enemyInfo.armies[i].status==1||enemyInfo.armies[i].status==3&&enemyInfo.armies[i].Sort==7)&& ifA[enemyInfo.armies[i].SN]==false){
                 ifA[enemyInfo.armies[i].SN]=true;
            }
        }
    }else if(Ship.size()+Boat.size()+Army.size()+Farmer.size()+Building.size()+Arrowtower.size()==0){
        for(int i=0;i<enemyInfo.armies.size();i++)
            if((enemyInfo.armies[i].status==1||enemyInfo.armies[i].status==3&&enemyInfo.armies[i].Sort==7)&& ifA[enemyInfo.armies[i].SN]==true){
                 ifA[enemyInfo.armies[i].SN]=false;
            }
    }
    if(Defend.size()!=0){
        for(int i=0;i<enemyInfo.armies.size();i++){
            if(enemyInfo.armies[i].status==2&& ifA[enemyInfo.armies[i].SN]==false){
                 ifA[enemyInfo.armies[i].SN]=true;
            }
        }
        for(int i=0;i<enemyInfo.buildings.size();i++){
            if(enemyInfo.buildings[i].Type==BUILDING_ARROWTOWER&&ifA[enemyInfo.buildings[i].SN]==false){
                ifA[enemyInfo.buildings[i].SN]=true;
            }
        }
    }
        else if(Defend.size()==0){
            for(int i=0;i<enemyInfo.armies.size();i++){
                if(enemyInfo.armies[i].status==2&& ifA[enemyInfo.armies[i].SN]==true){
                     ifA[enemyInfo.armies[i].SN]=false;
                }
            }
            for(int i=0;i<enemyInfo.buildings.size();i++){
                if(enemyInfo.buildings[i].Type==BUILDING_ARROWTOWER&&ifA[enemyInfo.buildings[i].SN]==true){
                    ifA[enemyInfo.buildings[i].SN]=false;
                }
            }
        }
    //追击检查
    for(int i=0;i<enemyInfo.armies.size();i++){
        if(ifA[enemyInfo.armies[i].SN]==true&&enemyInfo.armies[i].status==DEFENSE){
            if(countdistance(enemyInfo.armies[i].DR,enemyInfo.armies[i].UR,enemyInfo.armies[i].startpointDR,enemyInfo.armies[i].startpointUR)>600){
               ifA[enemyInfo.armies[i].SN]=false;
               timer[enemyInfo.armies[i].SN]=g_frame;
            }
        }
        else if(ifA[enemyInfo.armies[i].SN]==true&&enemyInfo.armies[i].status==AROUND){
            if(countdistance(enemyInfo.armies[i].DR,enemyInfo.armies[i].UR,enemyInfo.armies[i].startpointDR,enemyInfo.armies[i].startpointUR)>1500){
               ifA[enemyInfo.armies[i].SN]=false;
               timer[enemyInfo.armies[i].SN]=g_frame;
            }
        }
    }
    }

//巡逻
void EnemyAI::Around(){
    for(int i=0;i<enemyInfo.armies.size();i++){
    if(g_frame-timer[enemyInfo.armies[i].SN]>125&&enemyInfo.armies[i].status==AROUND&&ifA[enemyInfo.armies[i].SN]==false){
            if(around[i]==0){
            HumanMove(enemyInfo.armies[i].SN,enemyInfo.armies[i].destinaDR,enemyInfo.armies[i].destinaUR);
            timer[enemyInfo.armies[i].SN]=g_frame;
             around[i]=1-around[i];
            }else if(around[i]==1){
            HumanMove(enemyInfo.armies[i].SN,enemyInfo.armies[i].startpointDR,enemyInfo.armies[i].startpointUR);
            timer[enemyInfo.armies[i].SN]=g_frame;
             around[i]=1-around[i];
            }
}   else if(g_frame-timer[enemyInfo.armies[i].SN]>125&&enemyInfo.armies[i].status==ATTACK&&ifA[enemyInfo.armies[i].SN]==false){
        HumanMove(enemyInfo.armies[i].SN,enemyInfo.armies[i].startpointDR,enemyInfo.armies[i].startpointUR);
         timer[enemyInfo.armies[i].SN]=g_frame;
    }
}
    }

//进攻
void EnemyAI::Attack(){
    for(int i=0;i<enemyInfo.armies.size();i++){
        if(g_frame-timer[enemyInfo.armies[i].SN]>125&&enemyInfo.armies[i].status==AROUND&& ifA[enemyInfo.armies[i].SN]==true){
            if(enemyInfo.armies[i].Sort!=7){
            if(Farmer.size()!=0){
                HumanAction(enemyInfo.armies[i].SN,Farmer.back());
            }
            else if(Army.size()!=0){
                 HumanAction(enemyInfo.armies[i].SN,Army.back());
            }
            else if(Building.size()!=0){
                 HumanAction(enemyInfo.armies[i].SN,Building.back());
            }}
            else if(enemyInfo.armies[i].Sort==AT_SHIP){
                if(Boat.size()!=0){
                    HumanAction(enemyInfo.armies[i].SN,Boat.back());
                    qDebug()<<"攻击"<<g_frame;
                }
                else if(Ship.size()!=0){
                    HumanAction(enemyInfo.armies[i].SN,Ship.back());
                }
                else  if(Farmer.size()!=0){
                    HumanAction(enemyInfo.armies[i].SN,Farmer.back());
                }
                else if(Army.size()!=0){
                     HumanAction(enemyInfo.armies[i].SN,Army.back());
                }
                else if(Building.size()!=0){
                     HumanAction(enemyInfo.armies[i].SN,Building.back());
                }
            }
            timer[enemyInfo.armies[i].SN]=g_frame;
        }
        else if(g_frame-timer[enemyInfo.armies[i].SN]>125&&enemyInfo.armies[i].status==DEFENSE&& ifA[enemyInfo.armies[i].SN]==true){
            if(Defend.size()!=0){
                HumanAction(enemyInfo.armies[i].SN,Defend.back());
                timer[enemyInfo.armies[i].SN]=g_frame;
            }
        }
        else if(g_frame-timer[enemyInfo.armies[i].SN]>125&&enemyInfo.armies[i].status==ATTACK&& ifA[enemyInfo.armies[i].SN]==true){
            if(Building.size()!=0){
                             HumanAction(enemyInfo.armies[i].SN,Building.back());
                        }
            else if(Army.size()!=0){
                 HumanAction(enemyInfo.armies[i].SN,Army.back());
            }
            else if(Farmer.size()!=0){
                HumanAction(enemyInfo.armies[i].SN,Farmer.back());
            }
            timer[enemyInfo.armies[i].SN]=g_frame;
        }
    }
    for(int i=0;i<enemyInfo.buildings.size();i++){
        if(g_frame-timer[enemyInfo.buildings[i].SN]>125&&enemyInfo.buildings[i].Type==BUILDING_ARROWTOWER&&ifA[enemyInfo.buildings[i].SN]==true){
            if(Defend.size()!=0){
                HumanAction(enemyInfo.buildings[i].SN,Defend.back());
                timer[enemyInfo.buildings[i].SN]=g_frame;
            }
        }
    }

}

//ifDead函数，判断敌方单位是否死亡，把它从容器中删除
static void ifDead(vector <int> &x,int sort){
    if(x.size()>0)
    switch(sort){
    case VECTORFARMER:
        for(int i=0;i<x.size();i++){
                if(!isElementExists(x[i],TAGFARMER))
                   {
                    x.erase(x.begin()+i);
                }
            if(i>x.size()) break;
        }
        break;
    case VECTORARMY:
        for(int i=0;i<x.size();i++){
                if(!isElementExists(x[i],TAGARMY))
                   { x.erase(x.begin()+i);
            }
            if(i>x.size()) break;
        }
        break;
     case VECTORBOAT:
        for(int i=0;i<x.size();i++){
                if(!isElementExists(x[i],TAGFARMER))
                {
                 x.erase(x.begin()+i);
}
            if(i>x.size()) break;
        }
        break;
     case VECTORSHIP:
        for(int i=0;i<x.size();i++){
                if(!isElementExists(x[i],TAGARMY))
                   { x.erase(x.begin()+i);
            }
            if(i>x.size()) break;
        }
        break;
     case VECTORARROWTOWER:
        for(int i=0;i<x.size();i++){
               if(!isElementExists(x[i],TAGBUILDING))
                 {   x.erase(x.begin()+i);
            }
            if(i>x.size()) break;
        }
        break;
     case VECTORBUILDING:
        for(int i=0;i<x.size();i++){
                if(!isElementExists(x[i],TAGBUILDING))
                 {   x.erase(x.begin()+i);
            }
            if(i>x.size()) break;
        }
        break;
     case VECTORDEFEND:
        for(int i=0;i<x.size();i++){
                if(!isElementExists(x[i],TAGARMY))
                  {  x.erase(x.begin()+i);
            }
            if(i>x.size()) break;
        }
         break;
    }
}


static int BlockDis(int x1, int y1, int x2, int y2)
{
    return abs(x1 - x2) + abs(y1 - y2);
}

static int BlockDis2(int x1, int y1, int x2, int y2)
{
    int dx = x1 - x2;
    int dy = y1 - y2;
    return dx * dx + dy * dy;
}

static bool ContainsInt(const vector<int>& v, int x)
{
    return find(v.begin(), v.end(), x) != v.end();
}

static void AddUnique(vector<int>& v, int x)
{
    if (!ContainsInt(v, x)) v.push_back(x);
}

static tagArmy* FindMyArmyBySN(int sn)
{
    for (tagArmy& a : enemyInfo.armies) {
        if (a.SN == sn) return &a;
    }
    return nullptr;
}

static bool EnemyFarmerAlive(int sn)
{
    for (tagFarmer& f : enemyInfo.enemy_farmers) {
        if (f.SN == sn) return true;
    }
    return false;
}

static bool EnemyArmyAlive(int sn)
{
    for (tagArmy& a : enemyInfo.enemy_armies) {
        if (a.SN == sn) return true;
    }
    return false;
}

static bool EnemyBuildingAlive(int sn)
{
    for (tagBuilding& b : enemyInfo.enemy_buildings) {
        if (b.SN == sn) return true;
    }
    return false;
}

static bool EnemyArrowTowerAlive(int sn)
{
    for (tagBuilding& b : enemyInfo.enemy_buildings) {
        if (b.SN == sn && b.Type == BUILDING_ARROWTOWER) return true;
    }
    return false;
}

static void CleanDeadUnits(vector<int>& units)
{
    for (int i = 0; i < units.size(); ) {
        if (FindMyArmyBySN(units[i]) == nullptr) {
            units.erase(units.begin() + i);
        } else {
            i++;
        }
    }
}

static void MarkTouchedFarmer(vector<int>& touched, int targetSN)
{
    if (EnemyFarmerAlive(targetSN)) {
        AddUnique(touched, targetSN);
    }
}

static void UpdateKilledFarmers(vector<int>& touched, vector<int>& killed)
{
    for (int sn : touched) {
        if (!EnemyFarmerAlive(sn)) {
            AddUnique(killed, sn);
        }
    }
}

static bool IsDefenseArmySN(int sn)
{
    return Defend_Center_Enemy.find(sn) != Defend_Center_Enemy.end();
}

// 第一波兵种：棍棒兵、战斧、骑兵、弓箭手、侦察兵
static bool IsFirstWaveSort(int sort)
{
    return sort == AT_CLUBMAN
        || sort == AT_SWORDSMAN
        || sort == AT_CAVALRY
        || sort == AT_BOWMAN
        || sort == AT_SCOUT;
}

// 第二、三波铜器时代陆地兵种
static bool IsBronzeLandSort(int sort)
{
    return sort == AT_BROADSWORDSMAN
        || sort == AT_COMPOSITE_BOWMAN
        || sort == AT_CHARIOT_ARCHER
        || sort == AT_CHARIOT
        || sort == AT_CAVALRY
        || sort == AT_HOPLITE;
}

static pair<int, int> GetHarassCenterBlock()
{
    for (tagBuilding& b : enemyInfo.enemy_buildings) {
        if (b.Type == BUILDING_ARROWTOWER) {
            return make_pair(b.BlockDR, b.BlockUR);
        }
    }

    if (!enemyInfo.enemy_farmers.empty()) {
        return make_pair(enemyInfo.enemy_farmers[0].BlockDR,
                         enemyInfo.enemy_farmers[0].BlockUR);
    }

    if (!enemyInfo.enemy_buildings.empty()) {
        return make_pair(enemyInfo.enemy_buildings[0].BlockDR,
                         enemyInfo.enemy_buildings[0].BlockUR);
    }

    return make_pair(64, 64);
}

static bool IsInsideEnemyTowerRange(int blockDR, int blockUR)
{
    for (tagBuilding& b : enemyInfo.enemy_buildings) {
        if (b.Type != BUILDING_ARROWTOWER) continue;

        int d = BlockDis(blockDR, blockUR, b.BlockDR, b.BlockUR);
        if (d <= TOWER_DANGER_RANGE) return true;
    }

    return false;
}

static int FindNearestEnemyTower(int blockDR, int blockUR)
{
    int bestSN = -1;
    int bestDis = 1000000000;

    for (tagBuilding& b : enemyInfo.enemy_buildings) {
        if (b.Type != BUILDING_ARROWTOWER) continue;

        int d = BlockDis2(blockDR, blockUR, b.BlockDR, b.BlockUR);
        if (d < bestDis) {
            bestDis = d;
            bestSN = b.SN;
        }
    }

    return bestSN;
}

static int FindNearestFarmerByTowerState(int blockDR, int blockUR, bool wantInsideTower)
{
    int bestSN = -1;
    int bestDis = 1000000000;

    for (tagFarmer& f : enemyInfo.enemy_farmers) {
        bool inside = IsInsideEnemyTowerRange(f.BlockDR, f.BlockUR);
        if (inside != wantInsideTower) continue;

        int d = BlockDis2(blockDR, blockUR, f.BlockDR, f.BlockUR);
        if (d < bestDis) {
            bestDis = d;
            bestSN = f.SN;
        }
    }

    return bestSN;
}

static int FindThreatToArmy(const tagArmy& army)
{
    int bestSN = -1;
    int bestDis = 1000000000;

    for (tagArmy& enemyArmy : enemyInfo.enemy_armies) {
        if (enemyArmy.WorkObjectSN != army.SN) continue;

        int d = BlockDis2(army.BlockDR, army.BlockUR,
                          enemyArmy.BlockDR, enemyArmy.BlockUR);
        if (d < bestDis) {
            bestDis = d;
            bestSN = enemyArmy.SN;
        }
    }

    return bestSN;
}

static int FindWaveTarget(const tagArmy& army, int wave)
{
    if (EnemyArrowTowerAlive(army.WorkObjectSN)) {
        return army.WorkObjectSN;
    }

    // 第二波最高优先级：反击正在攻击自己的敌方士兵
    if (wave == 2) {
        int threat = FindThreatToArmy(army);
        if (threat != -1) return threat;
    }

    // 第一、二波：优先攻击箭塔范围外的农民
    int farmerOutside = FindNearestFarmerByTowerState(army.BlockDR, army.BlockUR, false);
    if (farmerOutside != -1) return farmerOutside;

    // 没有安全农民，就攻击箭塔
    int tower = FindNearestEnemyTower(army.BlockDR, army.BlockUR);
    if (tower != -1) return tower;

    // 第二波最后才攻击塔范围内农民
    if (wave == 2) {
        int farmerInside = FindNearestFarmerByTowerState(army.BlockDR, army.BlockUR, true);
        if (farmerInside != -1) return farmerInside;
    }

    return -1;
}

static int FindNearestEnemyArmy(int blockDR, int blockUR)
{
    int bestSN = -1;
    int bestDis = 1000000000;

    for (tagArmy& a : enemyInfo.enemy_armies) {
        if (a.Sort == AT_SHIP) continue;

        int d = BlockDis2(blockDR, blockUR, a.BlockDR, a.BlockUR);
        if (d < bestDis) {
            bestDis = d;
            bestSN = a.SN;
        }
    }

    return bestSN;
}

static int FindNearestEnemyFarmer(int blockDR, int blockUR)
{
    int bestSN = -1;
    int bestDis = 1000000000;

    for (tagFarmer& f : enemyInfo.enemy_farmers) {
        int d = BlockDis2(blockDR, blockUR, f.BlockDR, f.BlockUR);
        if (d < bestDis) {
            bestDis = d;
            bestSN = f.SN;
        }
    }

    return bestSN;
}

static int FindNearestEnemyBuilding(int blockDR, int blockUR)
{
    int bestSN = -1;
    int bestDis = 1000000000;

    for (tagBuilding& b : enemyInfo.enemy_buildings) {
        int d = BlockDis2(blockDR, blockUR, b.BlockDR, b.BlockUR);
        if (d < bestDis) {
            bestDis = d;
            bestSN = b.SN;
        }
    }

    return bestSN;
}

static bool NoPlayerFarmersLeft()
{
    for (tagFarmer& f : enemyInfo.enemy_farmers) {
        if (f.FarmerSort == 0) {
            return false;
        }
    }
    return true;
}

static int FindNearestArrowTowerForUnits(const vector<int>& units)
{
    int bestSN = -1;
    int bestDis = 1000000000;

    for (int sn : units) {
        tagArmy* army = FindMyArmyBySN(sn);
        if (!army) continue;

        for (tagBuilding& b : enemyInfo.enemy_buildings) {
            if (b.Type != BUILDING_ARROWTOWER) continue;

            int d = BlockDis2(army->BlockDR, army->BlockUR,
                              b.BlockDR, b.BlockUR);

            if (d < bestDis) {
                bestDis = d;
                bestSN = b.SN;
            }
        }
    }

    return bestSN;
}

static int FindPlayerBaseForUnits(const vector<int>& units)
{
    int bestSN = -1;
    int bestDis = 1000000000;

    // 优先找玩家基地 / 市镇中心
    for (int sn : units) {
        tagArmy* army = FindMyArmyBySN(sn);
        if (!army) continue;

        for (tagBuilding& b : enemyInfo.enemy_buildings) {
            if (b.Type != BUILDING_CENTER) continue;

            int d = BlockDis2(army->BlockDR, army->BlockUR,
                              b.BlockDR, b.BlockUR);

            if (d < bestDis) {
                bestDis = d;
                bestSN = b.SN;
            }
        }
    }

    if (bestSN != -1) return bestSN;

    // 兜底：如果 BUILDING_CENTER 已经没了，就拆最近的非箭塔建筑
    for (int sn : units) {
        tagArmy* army = FindMyArmyBySN(sn);
        if (!army) continue;

        for (tagBuilding& b : enemyInfo.enemy_buildings) {
            if (b.Type == BUILDING_ARROWTOWER) continue;

            int d = BlockDis2(army->BlockDR, army->BlockUR,
                              b.BlockDR, b.BlockUR);

            if (d < bestDis) {
                bestDis = d;
                bestSN = b.SN;
            }
        }
    }

    return bestSN;
}

static int FindTowerThenBaseTargetForUnits(const vector<int>& units)
{
    // 第一优先级：剩余箭塔
    int towerSN = FindNearestArrowTowerForUnits(units);
    if (towerSN != -1) return towerSN;

    // 第二优先级：玩家基地 / 市镇中心
    int baseSN = FindPlayerBaseForUnits(units);
    if (baseSN != -1) return baseSN;

    return -1;
}

void EnemyAI::OrderWaveUnitsToAttackTarget(vector<int>& units, int targetSN)
{
    if (targetSN == -1) return;

    for (int sn : units) {
        tagArmy* army = FindMyArmyBySN(sn);
        if (!army) continue;

        if (army->WorkObjectSN != targetSN ||
            g_frame - waveLastOrderFrame[sn] >= WAVE_ORDER_INTERVAL) {
            HumanAction(sn, targetSN);
            waveLastOrderFrame[sn] = g_frame;
        }
    }
}

static int FindFullAttackTarget(const tagArmy& army)
{
    // 第三波全面进攻优先级：
    // 敌方军队 > 敌方箭塔 > 敌方农民 > 敌方普通建筑
    int enemyArmy = FindNearestEnemyArmy(army.BlockDR, army.BlockUR);
    if (enemyArmy != -1) return enemyArmy;

    int tower = FindNearestEnemyTower(army.BlockDR, army.BlockUR);
    if (tower != -1) return tower;

    int farmer = FindNearestEnemyFarmer(army.BlockDR, army.BlockUR);
    if (farmer != -1) return farmer;

    int building = FindNearestEnemyBuilding(army.BlockDR, army.BlockUR);
    if (building != -1) return building;

    return -1;
}

static void SelectWaveUnits(vector<int>& dst,
                            int needCount,
                            bool (*sortChecker)(int),
                            const vector<int>& alreadyUsed)
{
    pair<int, int> center = GetHarassCenterBlock();

    vector<pair<int, int>> candidates;

    for (tagArmy& a : enemyInfo.armies) {
        if (IsDefenseArmySN(a.SN)) continue;
        if (ContainsInt(dst, a.SN)) continue;
        if (ContainsInt(alreadyUsed, a.SN)) continue;
        if (!sortChecker(a.Sort)) continue;

        int d = BlockDis2(a.BlockDR, a.BlockUR, center.first, center.second);
        candidates.push_back(make_pair(a.SN, d));
    }

    sort(candidates.begin(), candidates.end(),
         [](const pair<int, int>& x, const pair<int, int>& y) {
             return x.second < y.second;
         });

    for (int i = 0; i < candidates.size() && needCount > 0; i++) {
        int sn = candidates[i].first;
        tagArmy* a = FindMyArmyBySN(sn);
        if (!a) continue;

        dst.push_back(sn);
        HarassHome[sn] = make_pair(a->DR, a->UR);
        needCount--;
    }
}

static void SelectWaveUnitsBySort(vector<int>& dst,
                                  int unitSort,
                                  int needCount,
                                  const vector<int>& alreadyUsed)
{
    pair<int, int> center = GetHarassCenterBlock();
    vector<pair<int, int>> candidates;

    for (tagArmy& a : enemyInfo.armies) {
        if (IsDefenseArmySN(a.SN)) continue;
        if (ContainsInt(dst, a.SN)) continue;
        if (ContainsInt(alreadyUsed, a.SN)) continue;
        if (a.Sort != unitSort) continue;

        int d = BlockDis2(a.BlockDR, a.BlockUR, center.first, center.second);
        candidates.push_back(make_pair(a.SN, d));
    }

    sort(candidates.begin(), candidates.end(),
         [](const pair<int, int>& x, const pair<int, int>& y) {
             return x.second < y.second;
         });

    for (int i = 0; i < candidates.size() && needCount > 0; i++) {
        int sn = candidates[i].first;
        tagArmy* a = FindMyArmyBySN(sn);
        if (!a) continue;

        dst.push_back(sn);
        HarassHome[sn] = make_pair(a->DR, a->UR);
        needCount--;
    }
}

static bool IsCurrentActiveHarassUnit(int sn)
{
    // 第一波正在执行时，第一波兵不走普通自卫逻辑
    if (wave1Started && !wave1Completed && ContainsInt(wave1Units, sn)) {
        return true;
    }

    // 第二波正在执行时，第二波兵不走普通自卫逻辑
    // 因为 SecondAttack() 自己已经有反击逻辑
    if (wave2Started && !wave2Completed && ContainsInt(wave2Units, sn)) {
        return true;
    }

    // 第三波正在执行时，第三波兵不走普通自卫逻辑
    if (wave3Started && !wave3Completed && ContainsInt(wave3Units, sn)) {
        return true;
    }

    return false;
}

static int FindDirectThreatToArmySN(int myArmySN)
{
    tagArmy* myArmy = FindMyArmyBySN(myArmySN);
    if (!myArmy) return -1;

    int bestSN = -1;
    int bestDis = 1000000000;

    // 玩家士兵正在攻击我方这个 enemy 士兵
    for (tagArmy& enemyArmy : enemyInfo.enemy_armies) {
        if (enemyArmy.WorkObjectSN != myArmySN) continue;

        int d = BlockDis2(myArmy->BlockDR, myArmy->BlockUR,
                          enemyArmy.BlockDR, enemyArmy.BlockUR);

        if (d < bestDis) {
            bestDis = d;
            bestSN = enemyArmy.SN;
        }
    }

    // 玩家农民正在攻击我方这个 enemy 士兵
    for (tagFarmer& enemyFarmer : enemyInfo.enemy_farmers) {
        if (enemyFarmer.WorkObjectSN != myArmySN) continue;

        int d = BlockDis2(myArmy->BlockDR, myArmy->BlockUR,
                          enemyFarmer.BlockDR, enemyFarmer.BlockUR);

        if (d < bestDis) {
            bestDis = d;
            bestSN = enemyFarmer.SN;
        }
    }

    return bestSN;
}

static int FindAssistThreatNearArmy(const tagArmy& myArmy)
{
    int bestSN = -1;
    int bestDis = 1000000000;

    // 如果附近友军被玩家士兵攻击，则协助反击
    for (tagArmy& ally : enemyInfo.armies) {
        if (ally.SN == myArmy.SN) continue;
        if (IsDefenseArmySN(ally.SN)) continue;

        int allyDist = BlockDis(myArmy.BlockDR, myArmy.BlockUR,
                                ally.BlockDR, ally.BlockUR);

        if (allyDist > FIELD_ASSIST_RADIUS) continue;

        for (tagArmy& enemyArmy : enemyInfo.enemy_armies) {
            if (enemyArmy.WorkObjectSN != ally.SN) continue;

            int d = BlockDis2(myArmy.BlockDR, myArmy.BlockUR,
                              enemyArmy.BlockDR, enemyArmy.BlockUR);

            if (d < bestDis) {
                bestDis = d;
                bestSN = enemyArmy.SN;
            }
        }

        for (tagFarmer& enemyFarmer : enemyInfo.enemy_farmers) {
            if (enemyFarmer.WorkObjectSN != ally.SN) continue;

            int d = BlockDis2(myArmy.BlockDR, myArmy.BlockUR,
                              enemyFarmer.BlockDR, enemyFarmer.BlockUR);

            if (d < bestDis) {
                bestDis = d;
                bestSN = enemyFarmer.SN;
            }
        }
    }

    return bestSN;
}

void EnemyAI::AssignFieldSelfDefense()
{
    for (tagArmy& army : enemyInfo.armies) {
        // 厂区防守兵不归这里管
        if (IsDefenseArmySN(army.SN)) continue;

        // 正在执行波次骚扰/全面进攻的兵，不归这里管
        if (IsCurrentActiveHarassUnit(army.SN)) continue;

        int targetSN = -1;

        // 最高优先级：谁正在打我，我就反击谁
        targetSN = FindDirectThreatToArmySN(army.SN);

        // 如果我自己没被打，但附近友军被打，则过去帮忙
        if (targetSN == -1) {
            targetSN = FindAssistThreatNearArmy(army);
        }

        if (targetSN == -1) continue;

        if (army.WorkObjectSN != targetSN ||
            g_frame - fieldSelfDefenseLastOrderFrame[army.SN] >= FIELD_SELF_DEFENSE_ORDER_INTERVAL) {
            HumanAction(army.SN, targetSN);
            fieldSelfDefenseLastOrderFrame[army.SN] = g_frame;
        }
    }
}

//基于视野的目标分配系统
void EnemyAI::assignTargetsBasedOnVision(){
 /*   // 第一阶段：收集所有陆地单位和战船发现的目标
    vector<int> sharedLandTargets;  // 陆地单位共享的目标
    vector<int> sharedSeaTargets;   // 战船共享的目标
    
    // 遍历所有敌方单位，收集视野内的目标
    for(int i = 0; i < enemyInfo.armies.size(); i++){
        int unitSN = enemyInfo.armies[i].SN;
        int unitX = enemyInfo.armies[i].BlockDR;
        int unitY = enemyInfo.armies[i].BlockUR;
        int unitSort = enemyInfo.armies[i].Sort;
        int visionRange = getVisionRange(unitSort);
        
        // 只有攻击状态的单位或战船才贡献共享视野
        string status = getEnemyStatus(unitSN);
        bool contributeToSharedVision = (unitSort == AT_SHIP) || (isLandUnit(unitSort) && (status == "attack" || status.empty()));
        if(contributeToSharedVision) {
            // 寻找视野范围内的目标
            int foundTarget = findBestTargetInVision(unitX, unitY, visionRange, unitSort);
            
            if(foundTarget != -1) {
                if(unitSort == AT_SHIP) {
                    // 战船发现的目标加入海上共享目标
                    if(find(sharedSeaTargets.begin(), sharedSeaTargets.end(), foundTarget) == sharedSeaTargets.end()) {
                        sharedSeaTargets.push_back(foundTarget);
                    }
                } else {
                    // 陆地单位发现的目标加入陆地共享目标
                    if(find(sharedLandTargets.begin(), sharedLandTargets.end(), foundTarget) == sharedLandTargets.end()) {
                        sharedLandTargets.push_back(foundTarget);
                    }
                }
            }
        }
    }
    
    // 遍历所有敌方箭塔，收集其视野内的目标并加入共享视野
    for(int i = 0; i < enemyInfo.buildings.size(); i++){
        if(enemyInfo.buildings[i].Type != BUILDING_ARROWTOWER) continue;
        
        int buildingX = enemyInfo.buildings[i].BlockDR;
        int buildingY = enemyInfo.buildings[i].BlockUR;
        int visionRange = 7; // 箭塔视野范围
        
        // 箭塔发现的目标加入陆地共享目标（箭塔主要支援陆军）
        int foundTarget = findBestTargetInVision(buildingX, buildingY, visionRange, -1);
        
        if(foundTarget != -1) {
            if(find(sharedLandTargets.begin(), sharedLandTargets.end(), foundTarget) == sharedLandTargets.end()) {
                sharedLandTargets.push_back(foundTarget);
            }
        }
    }
    
    // 第二阶段：根据单位状态分配攻击目标
    for(int i = 0; i < enemyInfo.armies.size(); i++){
        // 如果单位已经有攻击目标，跳过
        if(enemyInfo.armies[i].WorkObjectSN != -1) continue;
        
        int unitSN = enemyInfo.armies[i].SN;
        int unitX = enemyInfo.armies[i].BlockDR;
        int unitY = enemyInfo.armies[i].BlockUR;
        int unitSort = enemyInfo.armies[i].Sort;
        int visionRange = getVisionRange(unitSort);
        
        string status = getEnemyStatus(unitSN);
        
        // 战船：默认攻击状态，参与协同攻击
        if(unitSort == AT_SHIP) {
            int bestTarget = -1;
            
            // 优先从海上共享目标中选择最近的目标
            if(!sharedSeaTargets.empty()) {
                bestTarget = findNearestTarget(unitX, unitY, sharedSeaTargets);
            }
            
            // 如果没有共享目标，寻找自己视野内的目标
            if(bestTarget == -1) {
                bestTarget = findBestTargetInVision(unitX, unitY, visionRange, unitSort);
            }
            
            if(bestTarget != -1) {
                HumanAction(unitSN, bestTarget);
            }
        }
        // 陆地单位：根据状态决定行为
        else if(isLandUnit(unitSort)) {
            if(status == "attack" || status.empty()) {  // 攻击状态（默认）
                int bestTarget = -1;
                
                // 优先从陆地共享目标中选择最近的目标
                if(!sharedLandTargets.empty()) {
                    bestTarget = findNearestTarget(unitX, unitY, sharedLandTargets);
                }
                
                // 如果没有共享目标，寻找自己视野内的目标
                if(bestTarget == -1) {
                    bestTarget = findBestTargetInVision(unitX, unitY, visionRange, unitSort);
                }
                
                if(bestTarget != -1) {
                    HumanAction(unitSN, bestTarget);
                }
            }
            else if(status == "defend") {  // 防守状态
                // 只攻击自己视野内的目标，不参与协同攻击
                int bestTarget = findBestTargetInVision(unitX, unitY, visionRange, unitSort);
                
                if(bestTarget != -1) {
                    HumanAction(unitSN, bestTarget);
                }
            }
        }
    }

    // 为箭塔分配目标（保持原有逻辑）
    for(int i = 0; i < enemyInfo.buildings.size(); i++){
        if(enemyInfo.buildings[i].Type != BUILDING_ARROWTOWER) continue;
        if(enemyInfo.buildings[i].Project != -1) continue;

        int buildingSN = enemyInfo.buildings[i].SN;
        int buildingX = enemyInfo.buildings[i].BlockDR;
        int buildingY = enemyInfo.buildings[i].BlockUR;
        int visionRange = 7; // 箭塔视野范围

        int bestTarget = findBestTargetInVision(buildingX, buildingY, visionRange, -1);

        if(bestTarget != -1){
            HumanAction(buildingSN, bestTarget);
        }
    }*/
    vector<int> sharedLandTargets;

        // 攻击状态陆军提供共享视野目标
        for (tagArmy& army : enemyInfo.armies) {
            if (army.Sort == AT_SHIP) continue;
            if (IsDefenseArmySN(army.SN)) continue;

            string status = getEnemyStatus(army.SN);
            if (!(status == "attack" || status.empty())) continue;

            int visionRange = getVisionRange(army.Sort);
            int foundTarget = findBestTargetInVision(army.BlockDR,
                                                     army.BlockUR,
                                                     visionRange,
                                                     army.Sort);

            if (foundTarget != -1) {
                AddUnique(sharedLandTargets, foundTarget);
            }
        }

        // 箭塔也可以提供共享目标
        for (tagBuilding& b : enemyInfo.buildings) {
            if (b.Type != BUILDING_ARROWTOWER) continue;

            int foundTarget = findBestTargetInVision(b.BlockDR, b.BlockUR, 7, -1);
            if (foundTarget != -1) {
                AddUnique(sharedLandTargets, foundTarget);
            }
        }

        // 给陆军分配目标，防守兵不参与普通协同
        for (tagArmy& army : enemyInfo.armies) {
            if (army.Sort == AT_SHIP) continue;
            if (IsDefenseArmySN(army.SN)) continue;
            if (army.WorkObjectSN != -1) continue;

            string status = getEnemyStatus(army.SN);
            if (!(status == "attack" || status.empty())) continue;

            int targetSN = -1;

            if (!sharedLandTargets.empty()) {
                targetSN = findNearestTarget(army.BlockDR, army.BlockUR, sharedLandTargets);
            }

            if (targetSN == -1) {
                int visionRange = getVisionRange(army.Sort);
                targetSN = findBestTargetInVision(army.BlockDR,
                                                  army.BlockUR,
                                                  visionRange,
                                                  army.Sort);
            }

            if (targetSN != -1) {
                HumanAction(army.SN, targetSN);
            }
        }

        // 箭塔自己攻击
        for (tagBuilding& b : enemyInfo.buildings) {
            if (b.Type != BUILDING_ARROWTOWER) continue;
            if (b.Project != -1) continue;

            int targetSN = findBestTargetInVision(b.BlockDR, b.BlockUR, 7, -1);
            if (targetSN != -1) {
                HumanAction(b.SN, targetSN);
            }
        }
}

//获取单位的视野范围
int EnemyAI::getVisionRange(int unitSort){
    if(unitSort == AT_SCOUT) return 5;      // 侦察兵：11x11 (-5到5)
    else if(unitSort == AT_SHIP) return 9;  // 船只：19x19 (-9到9)
    else return 3;                          // 普通单位：7x7 (-3到3)
}

//在指定视野范围内寻找最佳攻击目标
int EnemyAI::findBestTargetInVision(int centerX, int centerY, int range, int attackerSort){
    vector<int> farmersInVision;
    vector<int> armiesInVision;
    vector<int> buildingsInVision;

    // 收集视野范围内的敌方农民
    for(int i = 0; i < enemyInfo.enemy_farmers.size(); i++){
        int targetX = enemyInfo.enemy_farmers[i].BlockDR;
        int targetY = enemyInfo.enemy_farmers[i].BlockUR;

        if(abs(targetX - centerX) <= range && abs(targetY - centerY) <= range){
            // 按类型分类收集目标
            if(attackerSort == AT_SHIP && enemyInfo.enemy_farmers[i].FarmerSort != 0){
                farmersInVision.push_back(enemyInfo.enemy_farmers[i].SN); // 渔船
            } else if(attackerSort != AT_SHIP && enemyInfo.enemy_farmers[i].FarmerSort == 0){
                farmersInVision.push_back(enemyInfo.enemy_farmers[i].SN); // 农民
            } else {
                farmersInVision.push_back(enemyInfo.enemy_farmers[i].SN); // 其他农民类型
            }
        }
    }
    
    // 优先攻击最近的农民
    if(!farmersInVision.empty()){
        return findNearestTarget(centerX, centerY, farmersInVision);
    }

    // 收集视野范围内的敌方军队
    for(int i = 0; i < enemyInfo.enemy_armies.size(); i++){
        int targetX = enemyInfo.enemy_armies[i].BlockDR;
        int targetY = enemyInfo.enemy_armies[i].BlockUR;

        if(abs(targetX - centerX) <= range && abs(targetY - centerY) <= range){
            // 船只攻击船只，陆军攻击陆军
            if(attackerSort == AT_SHIP && enemyInfo.enemy_armies[i].Sort == 7){
                armiesInVision.push_back(enemyInfo.enemy_armies[i].SN);
            } else if(attackerSort != AT_SHIP && enemyInfo.enemy_armies[i].Sort != 7){
                armiesInVision.push_back(enemyInfo.enemy_armies[i].SN);
            } else {
                armiesInVision.push_back(enemyInfo.enemy_armies[i].SN);
            }
        }
    }
    
    // 如果有军队目标，攻击最近的
    if(!armiesInVision.empty()){
        return findNearestTarget(centerX, centerY, armiesInVision);
    }

    // 收集视野范围内的建筑物
    for(int i = 0; i < enemyInfo.enemy_buildings.size(); i++){
        int targetX = enemyInfo.enemy_buildings[i].BlockDR;
        int targetY = enemyInfo.enemy_buildings[i].BlockUR;

        if(abs(targetX - centerX) <= range && abs(targetY - centerY) <= range){
            buildingsInVision.push_back(enemyInfo.enemy_buildings[i].SN);
        }
    }
    
    // 最后攻击最近的建筑
    if(!buildingsInVision.empty()){
        return findNearestTarget(centerX, centerY, buildingsInVision);
    }

    return -1; // 没有找到目标
}

// 获取敌人单位的状态
string EnemyAI::getEnemyStatus(int unitSN) {
    extern MainWidget* g_mainWidget;
    if (!g_mainWidget) return "";
    
    // 通过SN找到对应的Coordinate对象
    // SN在这里应该对应globalNum，使用getglobalNum()进行匹配
    for (Human* human : g_mainWidget->player[1]->human) {
        if (human->getglobalNum() == unitSN) {
            return g_mainWidget->getEnemyStatus(human);
        }
    }
    return "";
}

// 判断是否为陆地单位
bool EnemyAI::isLandUnit(int unitSort) {
    return unitSort != AT_SHIP;  // 除了战船外都是陆地单位
}

// 判断单位是否应该参与协同攻击
bool EnemyAI::shouldCooperateAttack(int unitSN) {
    for(int i = 0; i < enemyInfo.armies.size(); i++) {
        if(enemyInfo.armies[i].SN == unitSN) {
            int unitSort = enemyInfo.armies[i].Sort;
            
            // 战船总是协同攻击
            if (unitSort == AT_SHIP) {
                return true;
            }
            
            // 陆地单位根据状态决定
            if (isLandUnit(unitSort)) {
                string status = getEnemyStatus(unitSN);
                // 攻击状态的单位参与协同攻击，防守状态的不参与
                return status == "attack" || status.empty();  // 没有状态默认为攻击
            }
            break;
        }
    }
    return false;
}

// 计算两点之间的距离
double EnemyAI::calculateDistance(int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}

// 从目标列表中找到最近的目标
int EnemyAI::findNearestTarget(int attackerX, int attackerY, const vector<int>& targets) {
    if (targets.empty()) return -1;
    
    int nearestTarget = -1;
    double minDistance = DBL_MAX;
    
    for (int targetSN : targets) {
        // 找到目标的坐标
        bool found = false;
        int targetX = -1, targetY = -1;
        
        // 在农民中查找
        for (int i = 0; i < enemyInfo.enemy_farmers.size(); i++) {
            if (enemyInfo.enemy_farmers[i].SN == targetSN) {
                targetX = enemyInfo.enemy_farmers[i].BlockDR;
                targetY = enemyInfo.enemy_farmers[i].BlockUR;
                found = true;
                break;
            }
        }
        
        // 在军队中查找
        if (!found) {
            for (int i = 0; i < enemyInfo.enemy_armies.size(); i++) {
                if (enemyInfo.enemy_armies[i].SN == targetSN) {
                    targetX = enemyInfo.enemy_armies[i].BlockDR;
                    targetY = enemyInfo.enemy_armies[i].BlockUR;
                    found = true;
                    break;
                }
            }
        }
        
        // 在建筑中查找
        if (!found) {
            for (int i = 0; i < enemyInfo.enemy_buildings.size(); i++) {
                if (enemyInfo.enemy_buildings[i].SN == targetSN) {
                    targetX = enemyInfo.enemy_buildings[i].BlockDR;
                    targetY = enemyInfo.enemy_buildings[i].BlockUR;
                    found = true;
                    break;
                }
            }
        }
        
        if (found) {
            double distance = calculateDistance(attackerX, attackerY, targetX, targetY);
            if (distance < minDistance) {
                minDistance = distance;
                nearestTarget = targetSN;
            }
        }
    }
    
    return nearestTarget;
}

tagArmy EnemyAI::Threated(tagArmy *army)
{
    tagArmy nearestTarget;
    nearestTarget.SN = -1; // 标记为无效
    double minDistance = 10;
    
    for(tagArmy& enemyarmy:enemyInfo.enemy_armies)
    {
        double Distance=pow(enemyarmy.BlockDR-army->BlockDR,2)+pow(enemyarmy.BlockUR-army->BlockUR,2);
        if(enemyarmy.WorkObjectSN==army->SN&&Distance<=minDistance)
        {
            nearestTarget = enemyarmy;
            minDistance = Distance;
        }
    }
    return nearestTarget;
}

void EnemyAI::processData() {
        enemyInfo = getInfo();

        Initialize_Enemycenter();
        Initialize_Enemymap();

        // 武器攻城厂周围防守兵逻辑，永远优先执行
        AssignDefense();
        AssignFieldSelfDefense();
        // 自家箭塔自动攻击靠近的玩家单位
        for (tagBuilding& b : enemyInfo.buildings) {
            if (b.Type != BUILDING_ARROWTOWER) continue;
            if (b.Project != -1) continue;

            int targetSN = -1;
            int bestDis = 1000000000;

            for (tagArmy& obj : enemyInfo.enemy_armies) {
                int d = BlockDis(b.BlockDR, b.BlockUR, obj.BlockDR, obj.BlockUR);
                if (d < bestDis) {
                    bestDis = d;
                    targetSN = obj.SN;
                }
            }

            for (tagFarmer& obj : enemyInfo.enemy_farmers) {
                int d = BlockDis(b.BlockDR, b.BlockUR, obj.BlockDR, obj.BlockUR);
                if (d < bestDis) {
                    bestDis = d;
                    targetSN = obj.SN;
                }
            }

            if (targetSN != -1 && bestDis <= ATK_BUILD_ARROWTOWER) {
                if (g_frame - timer[b.SN] >= DEFENSE_ORDER_INTERVAL) {
                    HumanAction(b.SN, targetSN);
                    timer[b.SN] = g_frame;
                }
            }
        }
        // 到点强制触发：倒序判断，避免第二波未完成时挡住第三波。
        if (g_frame >= TAT && !wave3Completed) {
            onWaveAttack(3);
            return;
        }

        if (g_frame >= SAT && !wave2Completed) {
            onWaveAttack(2);
            return;
        }

        if (g_frame >= FAT && !wave1Completed) {
            onWaveAttack(1);
            return;
        }
    }
     /*###########YOUR CODE ENDS HERE###########*/
void EnemyAI::Initialize_Enemycenter()
{
    if(Enemy_Center.first) return;
    for(tagBuilding&building:enemyInfo.buildings)
    {
        if(building.Type==BUILDING_SIEGE)
        {
            Enemy_Center=make_pair(building.BlockDR,building.BlockUR);
            break;
        }
    }
    return;
}
void EnemyAI::Initialize_Enemymap()
{
    if (Enemy_Center.first == 0 && Enemy_Center.second == 0) return;

    for (tagArmy& army : enemyInfo.armies) {
        int d2 = BlockDis2(army.BlockDR, army.BlockUR,
                           Enemy_Center.first, Enemy_Center.second);

        if (d2 < radius_Inner * radius_Inner) {
            Defend_Center_Enemy[army.SN] = 1;

            if (DefenseHome.find(army.SN) == DefenseHome.end()) {
                DefenseHome[army.SN] = make_pair(army.DR, army.UR);
            }
        }
    }
}
void EnemyAI::AssignDefense()
{
    if (Enemy_Center.first == 0 && Enemy_Center.second == 0) return;

    for (auto it = Defend_Center_Enemy.begin(); it != Defend_Center_Enemy.end(); ) {
        int sn = it->first;
        tagArmy* army = FindMyArmyBySN(sn);

        if (!army) {
            DefenseHome.erase(sn);
            defenseLastOrderFrame.erase(sn);
            it = Defend_Center_Enemy.erase(it);
            continue;
        }

        int distToCenter = BlockDis(army->BlockDR, army->BlockUR,
                                    Enemy_Center.first, Enemy_Center.second);

        // 如果防守兵追太远，立刻回防
        if (distToCenter > DEFENSE_CHASE_LIMIT) {
            if (g_frame - defenseLastOrderFrame[sn] >= DEFENSE_ORDER_INTERVAL) {
                auto home = DefenseHome.find(sn);
                if (home != DefenseHome.end()) {
                    HumanMove(sn, home->second.first, home->second.second);
                } else {
                    HumanMove(sn, Enemy_Center.first, Enemy_Center.second);
                }
                defenseLastOrderFrame[sn] = g_frame;
            }

            ++it;
            continue;
        }

        // 只攻击靠近武器攻城厂的玩家单位
        int targetSN = -1;
        int bestDis = 1000000000;

        for (tagArmy& enemyArmy : enemyInfo.enemy_armies) {
            int dToCenter = BlockDis(enemyArmy.BlockDR, enemyArmy.BlockUR,
                                     Enemy_Center.first, Enemy_Center.second);
            if (dToCenter > radius_Outer) continue;

            int d = BlockDis2(army->BlockDR, army->BlockUR,
                              enemyArmy.BlockDR, enemyArmy.BlockUR);
            if (d < bestDis) {
                bestDis = d;
                targetSN = enemyArmy.SN;
            }
        }

        for (tagFarmer& enemyFarmer : enemyInfo.enemy_farmers) {
            int dToCenter = BlockDis(enemyFarmer.BlockDR, enemyFarmer.BlockUR,
                                     Enemy_Center.first, Enemy_Center.second);
            if (dToCenter > radius_Outer) continue;

            int d = BlockDis2(army->BlockDR, army->BlockUR,
                              enemyFarmer.BlockDR, enemyFarmer.BlockUR);
            if (d < bestDis) {
                bestDis = d;
                targetSN = enemyFarmer.SN;
            }
        }

        if (targetSN != -1) {
            if (army->WorkObjectSN != targetSN ||
                g_frame - defenseLastOrderFrame[sn] >= DEFENSE_ORDER_INTERVAL) {
                HumanAction(sn, targetSN);
                defenseLastOrderFrame[sn] = g_frame;
            }
        } else {
            // 没敌人靠近，防守兵回原位
            if (distToCenter > radius_Inner &&
                g_frame - defenseLastOrderFrame[sn] >= DEFENSE_ORDER_INTERVAL) {
                auto home = DefenseHome.find(sn);
                if (home != DefenseHome.end()) {
                    HumanMove(sn, home->second.first, home->second.second);
                    defenseLastOrderFrame[sn] = g_frame;
                }
            }
        }

        ++it;
    }
}
void EnemyAI::onWaveAttack(int wave)
{
    if (wave == 1) {
        FirstAttack();
    } else if (wave == 2) {
        SecondAttack();
    } else if (wave == 3) {
        ThirdAttack();
    }
}

void EnemyAI::FirstAttack()
{
    if (wave1Completed) return;

    // 第一次进入第一波时，选 2 个棍棒兵和 2 个侦察兵
    if (!wave1Started) {
        vector<int> emptyUsed;
        SelectWaveUnitsBySort(wave1Units, AT_CLUBMAN, 2, emptyUsed);
        SelectWaveUnitsBySort(wave1Units, AT_SCOUT, 2, emptyUsed);
        wave1Started = true;
    }

    CleanDeadUnits(wave1Units);
    UpdateKilledFarmers(wave1TouchedFarmers, wave1KilledFarmers);

    // 第一波派出去的兵全死，第一波结束
    if (wave1Units.empty()) {
        wave1Completed = true;
        mode = 2;
        return;
    }

    // 杀够 3 个农民，有兵存活，撤退回原位置
    if (wave1KilledFarmers.size() >= 3) {
        for (int sn : wave1Units) {
            tagArmy* army = FindMyArmyBySN(sn);
            if (!army) continue;

            auto home = HarassHome.find(sn);
            if (home != HarassHome.end()) {
                HumanMove(sn, home->second.first, home->second.second);
            }
        }

        wave1Completed = true;
        mode = 2;
        return;
    }

    // 没杀够 3 个农民，但玩家农民已经死完：
    // 本波不撤退，先拆剩余箭塔，再拆玩家基地
    if (NoPlayerFarmersLeft() && wave1KilledFarmers.size() < 3) {
        int targetSN = FindTowerThenBaseTargetForUnits(wave1Units);

        if (targetSN != -1) {
            OrderWaveUnitsToAttackTarget(wave1Units, targetSN);
        } else {
            // 已经没有箭塔、没有基地/建筑可拆了，本波可以结束
            wave1Completed = true;
            mode = 2;
        }

        return;
    }
    // 未杀够：动态寻找 塔外农民 > 箭塔
    for (int sn : wave1Units) {
        tagArmy* army = FindMyArmyBySN(sn);
        if (!army) continue;

        int targetSN = FindWaveTarget(*army, 1);
        if (targetSN == -1) continue;

        MarkTouchedFarmer(wave1TouchedFarmers, targetSN);

        if (army->WorkObjectSN != targetSN ||
            g_frame - waveLastOrderFrame[sn] >= WAVE_ORDER_INTERVAL) {
            HumanAction(sn, targetSN);
            waveLastOrderFrame[sn] = g_frame;
        }
    }
}

void EnemyAI::SecondAttack()
{
    if (wave2Completed) return;

    // 第一次进入第二波：第一波残兵 + 指定铜器兵组合
    if (!wave2Started) {
        CleanDeadUnits(wave1Units);
        wave1Completed = true;

        // 加入第一波残兵
        for (int sn : wave1Units) {
            tagArmy* army = FindMyArmyBySN(sn);
            if (!army) continue;

            AddUnique(wave2Units, sn);

            if (HarassHome.find(sn) == HarassHome.end()) {
                HarassHome[sn] = make_pair(army->DR, army->UR);
            }
        }

        SelectWaveUnitsBySort(wave2Units, AT_HOPLITE, 3, wave1Units);
        SelectWaveUnitsBySort(wave2Units, AT_BROADSWORDSMAN, 2, wave1Units);
        SelectWaveUnitsBySort(wave2Units, AT_COMPOSITE_BOWMAN, 3, wave1Units);
        SelectWaveUnitsBySort(wave2Units, AT_CHARIOT_ARCHER, 1, wave1Units);

        wave2Started = true;
    }

    CleanDeadUnits(wave2Units);
    UpdateKilledFarmers(wave2TouchedFarmers, wave2KilledFarmers);

    // 第二波派出去的兵全死，第二波结束
    if (wave2Units.empty()) {
        wave2Completed = true;
        return;
    }

    // 杀够 8 个农民，有兵存活，撤退
    if (wave2KilledFarmers.size() >= 8) {
        for (int sn : wave2Units) {
            tagArmy* army = FindMyArmyBySN(sn);
            if (!army) continue;

            auto home = HarassHome.find(sn);
            if (home != HarassHome.end()) {
                HumanMove(sn, home->second.first, home->second.second);
            }
        }

        wave2Completed = true;
        return;
    }

    // 没杀够 8 个农民，但玩家农民已经死完：
    // 本波不撤退，先拆剩余箭塔，再拆玩家基地
    if (NoPlayerFarmersLeft() && wave2KilledFarmers.size() < 8) {
        int targetSN = FindTowerThenBaseTargetForUnits(wave2Units);

        if (targetSN != -1) {
            OrderWaveUnitsToAttackTarget(wave2Units, targetSN);
        } else {
            // 已经没有箭塔、没有基地/建筑可拆了，本波可以结束
            wave2Completed = true;
        }

        return;
    }

    // 第二波目标优先级：
    // 攻击我方士兵的敌方士兵 > 塔外农民 > 箭塔 > 塔内农民
    for (int sn : wave2Units) {
        tagArmy* army = FindMyArmyBySN(sn);
        if (!army) continue;

        int targetSN = FindWaveTarget(*army, 2);
        if (targetSN == -1) continue;

        MarkTouchedFarmer(wave2TouchedFarmers, targetSN);

        if (army->WorkObjectSN != targetSN ||
            g_frame - waveLastOrderFrame[sn] >= WAVE_ORDER_INTERVAL) {
            HumanAction(sn, targetSN);
            waveLastOrderFrame[sn] = g_frame;
        }
    }
}

void EnemyAI::ThirdAttack()
{
    if (wave3Completed) return;

    // 第一次进入第三波：前两波残兵 + 20 个新铜器兵
    if (!wave3Started) {
        CleanDeadUnits(wave1Units);
        CleanDeadUnits(wave2Units);
        wave1Completed = true;
        wave2Completed = true;

        for (int sn : wave1Units) {
            tagArmy* army = FindMyArmyBySN(sn);
            if (!army) continue;
            AddUnique(wave3Units, sn);
        }

        for (int sn : wave2Units) {
            tagArmy* army = FindMyArmyBySN(sn);
            if (!army) continue;
            AddUnique(wave3Units, sn);
        }

        vector<int> alreadyUsed = wave3Units;
        SelectWaveUnits(wave3Units, 20, IsBronzeLandSort, alreadyUsed);

        wave3Started = true;
    }

    CleanDeadUnits(wave3Units);

    if (wave3Units.empty()) {
        wave3Completed = true;
        return;
    }

    // 第三波全面进攻：
    // 敌方军队 > 敌方箭塔 > 敌方农民 > 敌方普通建筑
    bool hasTarget = false;

    for (int sn : wave3Units) {
        tagArmy* army = FindMyArmyBySN(sn);
        if (!army) continue;

        int targetSN = FindFullAttackTarget(*army);
        if (targetSN == -1) continue;

        hasTarget = true;

        if (army->WorkObjectSN != targetSN ||
            g_frame - waveLastOrderFrame[sn] >= WAVE_ORDER_INTERVAL) {
            HumanAction(sn, targetSN);
            waveLastOrderFrame[sn] = g_frame;
        }
    }

    if (!hasTarget) {
        wave3Completed = true;
    }
}