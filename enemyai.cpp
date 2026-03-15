#include "enemyai.h"
#include "MainWidget.h"
#include "Human.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cfloat>

using std::string;
using std::vector;

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
#define MODE3 21250
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

static vector<tagFarmer>deadFirst;
static vector<tagArmy>attackEnemy;
static vector<int> farmerSNs;
// 第一波总目标（击杀 3 农民）是否已完成；完成后不再往 attackEnemy 里加人，避免反复加人/撤退
static bool wave1Completed = false;
// 撤退目标为细节坐标 (DR, UR)，HumanMove 需要 double 而非区块坐标
static map<int, pair<double, double>> Army_location;
// 每单位上次分配攻击目标的帧号，避免每帧重复下令导致部分兵种抽搐
static map<int, int> lastAssignFrame;
// 每单位上次补发攻击指令的帧号（目标仍活着但引擎清空指令时），用较短间隔快速补发避免卡住
static map<int, int> lastReissueFrame;

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

//基于视野的目标分配系统
void EnemyAI::assignTargetsBasedOnVision(){
    // 第一阶段：收集所有陆地单位和战船发现的目标
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

void EnemyAI::processData() {

    enemyInfo=getInfo();

    for(tagBuilding&b:enemyInfo.buildings)
    {
        if(b.Type == BUILDING_ARROWTOWER&&b.Project==-1)
        {
            //找一个最近的
            int tar=-1;
            int dd=INT_MAX;
            for(tagHuman&obj:enemyInfo.enemy_armies){
                if(abs(obj.BlockDR-b.BlockDR)+abs(obj.BlockUR-b.BlockUR)<dd){
                    dd=abs(obj.BlockDR-b.BlockDR)+abs(obj.BlockUR-b.BlockUR);
                    tar=obj.SN;
                }
            }
            if(tar!=-1&&dd<=ATK_BUILD_ARROWTOWER){
                HumanAction(b.SN,tar);
            }
        }
    }
    onWaveAttack(1);
    return;
    // 新的基于视野的攻击系统
    assignTargetsBasedOnVision();
    return;
    //
    //军队数据初始化
    if(g_frame==50){
        for(int i=0;i<enemyInfo.armies.size();i++){
            ifA.insert(std::pair<int,bool>(enemyInfo.armies[i].SN,false));
            timer.insert(std::pair<int,int>(enemyInfo.armies[i].SN,0));
        }
        for(int i=0;i<enemyInfo.buildings.size();i++){
            if(enemyInfo.buildings[i].Type==BUILDING_ARROWTOWER){
                ifA.insert(std::pair<int,bool>(enemyInfo.buildings[i].SN,false));
                timer.insert(std::pair<int,int>(enemyInfo.buildings[i].SN,0));
            }
        }
    }
    if(g_frame>50&&g_frame % 8==0){
        visionChange();
        ifVisible();
    }
    if(g_frame>1000){
        Around();
        seek();
        ifATTACK();
        Attack();
    }
    if(g_frame>50&&g_frame % 10==0){
        int s=Farmer.size();
        int t=enemyInfo.buildings.size();
        ifDead(Farmer,VECTORFARMER);
        if(s!=Farmer.size())
            ifDead(Army,VECTORARMY);
        ifDead(Boat,VECTORBOAT);
        ifDead(Ship,VECTORSHIP);
        ifDead(Defend,VECTORDEFEND);
        ifDead(Arrowtower,VECTORARROWTOWER);
        ifDead(Building,VECTORBUILDING);
    }
}
     /*###########YOUR CODE ENDS HERE###########*/
void EnemyAI::onWaveAttack(int wave) {
    // TODO: 发起第wave波进攻
    if (wave < 1 || wave > 3) {
        // std::string debugStr = "非法波次：" + std::to_string(wave);
        // call_debugText("black", " AI" + QString::number(0) + "打印：" + QString::fromStdString(debugStr), 0);
       // return;
    }
    if(wave==1) FirstAttack();
    mode = wave;
}

void EnemyAI::FirstAttack()
{
    // 1. 初始化目标农民 (只在为空时执行一次)
        if(deadFirst.empty() && !enemyInfo.enemy_farmers.empty())
        {
            for(int i = 0; i < 3 && i < enemyInfo.enemy_farmers.size(); i++)
                deadFirst.push_back(enemyInfo.enemy_farmers[i]);
        }

        // 2. 初始化进攻部队 (只在为空且第一波未完成时执行一次；完成后不再补人)
        if(attackEnemy.empty() && !deadFirst.empty() && !wave1Completed)
        {
            double sumBlockdr = 0, sumBlockur = 0;
            for (tagFarmer& c : deadFirst) {
                sumBlockdr += c.BlockDR;
                sumBlockur += c.BlockUR;
            }
            int avgDr = sumBlockdr / deadFirst.size();
            int avgUr = sumBlockur / deadFirst.size();

            vector<pair<tagArmy, int>> cmp_Distance;
            for(tagArmy& army : enemyInfo.armies) {
                cmp_Distance.emplace_back(army, pow(army.BlockDR - avgDr, 2) + pow(army.BlockUR - avgUr, 2));
            }
            sort(cmp_Distance.begin(), cmp_Distance.end(), [](const pair<tagArmy, int>& a, const pair<tagArmy, int>& b){
                return a.second < b.second;
            });

            // 取前5个距离最近的士兵进入骚扰小组，并记录其初始位置用于撤退
            for(int i = 0; i < 5 && i < cmp_Distance.size(); i++) {
                const tagArmy& a = cmp_Distance[i].first;
                attackEnemy.push_back(a);
                Army_location[a.SN] = std::make_pair(a.DR, a.UR);
            }
        }

        // 3. 当前集火目标：按顺序取 deadFirst 中第一个仍存活的农民，全队集火同一目标；全部击杀后才集体撤退
        int currentTargetSN = -1;
        for (size_t i = 0; i < deadFirst.size(); i++) {
            bool alive = false;
            for (const auto& f : enemyInfo.enemy_farmers) {
                if (f.SN == deadFirst[i].SN && f.Blood > 0) { alive = true; break; }
            }
            if (alive) {
                currentTargetSN = deadFirst[i].SN;
                break;
            }
        }
        bool waveComplete = (currentTargetSN == -1);  // 3 个目标均已阵亡，集体撤退
        if (waveComplete) wave1Completed = true;      // 标记第一波已完成，之后不再往 attackEnemy 加人

        // 4. 任务分配与状态监控
        auto it = attackEnemy.begin();
        while (it != attackEnemy.end())
        {
            tagArmy& a_backup = *it;
            tagArmy* realArmy = nullptr;

            for (auto& real : enemyInfo.armies) {
                if (real.SN == a_backup.SN) {
                    realArmy = &real;
                    break;
                }
            }

            // A. 士兵已阵亡，从列表移除并清理状态
            if (!realArmy) {
                lastAssignFrame.erase(a_backup.SN);
                lastReissueFrame.erase(a_backup.SN);
                Army_location.erase(a_backup.SN);
                it = attackEnemy.erase(it);
                continue;
            }

            // B. 总目标已完成：集体撤退
            if (waveComplete) {
                auto posIt = Army_location.find(realArmy->SN);
                if (posIt != Army_location.end()) {
                    HumanMove(realArmy->SN, posIt->second.first, posIt->second.second);
                }
                lastAssignFrame.erase(realArmy->SN);
                lastReissueFrame.erase(realArmy->SN);
                Army_location.erase(realArmy->SN);
                it = attackEnemy.erase(it);
                continue;
            }

            // C. 未完成总目标：所有人集火 currentTargetSN；需要时补发指令（目标切换或引擎清空）
            int unitSN = realArmy->SN;
            if (realArmy->WorkObjectSN != currentTargetSN) {
                bool targetChanged = (a_backup.WorkObjectSN != currentTargetSN);
                int now = g_frame;
                bool throttleOk = (lastReissueFrame[unitSN] == 0 || now - lastReissueFrame[unitSN] >= 12);
                if (targetChanged || throttleOk) {
                    HumanAction(unitSN, currentTargetSN);
                    a_backup.WorkObjectSN = currentTargetSN;
                    lastReissueFrame[unitSN] = now;
                }
            }
            ++it;
        }
}
