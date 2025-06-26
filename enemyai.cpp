#include "EnemyAI.h"
#include <iostream>
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
        }}
        else if(Defend.size()==0){
            for(int i=0;i<enemyInfo.armies.size();i++){
                if(enemyInfo.armies[i].status==2&& ifA[enemyInfo.armies[i].SN]==true){
                     ifA[enemyInfo.armies[i].SN]=false;
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

void EnemyAI::processData() {
    enemyInfo=getInfo();
     //军队数据初始化
     if(g_frame==50){
         sum=enemyInfo.armies.size();
         for(int i=0;i<enemyInfo.armies.size();i++){
             ifA.insert(std::pair<int,bool>(enemyInfo.armies[i].SN,false));
             timer.insert(std::pair<int,int>(enemyInfo.armies[i].SN,0));
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
        return;
    }
    mode = wave;
}
