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
#define MODE3 22500
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
#define TAGDEFAULT 0
tagInfo enemyInfo;
//-----------新参数--------------//
static int around[100];
static bool ifAttack[50];
static map<long,long> timer;
static vector <int> Army;
static vector <int> Farmer;
static vector <int> Ship;
static vector <int> Boat;
static vector <int> Arrowtower;
static vector <int> Building;
static vector <int> Defend;
static map<long, bool> ifA;
static int sum;
static int mode = -3;
//-----------旧参数--------------//
//static double pos_L[50] = {0};
//static double pos_U[50] = {0};
//static int armystate[50] = {0};
//static int Blood[50] = {0};
//static int Lock[50];
//static int ChasingLock[50]={0};
//static int s;
//static int Pathn;
//static int Pathnset=1;
//static int t;
//static int a=0;
//static int b=0;
//static int Pathb;
//static int Pathbset=1;
//static int c=0;
//static int d=0;
//static int tar[50];
//static int target[150];
//static bool ifretreat[50];
//static long Chasingtar[50];
//static int Pathnum=0;
//static int Pathcar[20][2];
//static int Pathboat[20][2];
//static int point1[12][2]={{51,8},{50,18},{61,27},{60,41},{50,53},{44,65},{44,77},{36,88},{25,98},{21,112},{11,113},{2,113}};
//static int AroundLock[50];
//static int point2[12][2]={{58,8},{57,18},{68,27},{67,43},{58,56},{51,67},{51,77},{43,88},{32,98},{21,120},{11,120},{2,120}};
//static long visible[100];
bool isElementExists(const std::vector<int>& vec, int element,int sort) {
    switch(sort){
    case TAGDEFAULT:
        for (int i : vec) {
            if (i == element) {
                return true;
            }
        }
        return false;
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
static void seekenemy(){
    for(int i=0;i<enemyInfo.armies.size();i++){
        if(enemyInfo.armies[i].status!=2){
        for(int j=0;j<enemyInfo.enemy_farmers.size();j++){
          double temp=pow(pow(enemyInfo.armies[i].BlockDR-enemyInfo.enemy_farmers[j].BlockDR, 2) + pow(enemyInfo.armies[i].BlockUR-enemyInfo.enemy_farmers[j].BlockUR, 2), 0.5);
          if(temp<15){
              if (enemyInfo.enemy_farmers[j].FarmerSort==0&&!isElementExists(Farmer, enemyInfo.enemy_farmers[j].SN,TAGDEFAULT)){
              Farmer.push_back(enemyInfo.enemy_farmers[j].SN);
              }else if(enemyInfo.enemy_farmers[j].FarmerSort!=0&&!isElementExists(Boat,enemyInfo.enemy_farmers[j].SN,TAGDEFAULT))
             {Boat.push_back(enemyInfo.enemy_farmers[j].SN);
          }}
        }
        for(int j=0;j<enemyInfo.enemy_armies.size();j++){
          double temp=pow(pow(enemyInfo.armies[i].BlockDR-enemyInfo.enemy_armies[j].BlockDR, 2) + pow(enemyInfo.armies[i].BlockUR-enemyInfo.enemy_armies[j].BlockUR, 2), 0.5);
          if(temp<15){
              if (enemyInfo.enemy_armies[j].Sort!=7&&!isElementExists(Army,enemyInfo.enemy_armies[j].SN,TAGDEFAULT)){
              Army.push_back(enemyInfo.enemy_armies[j].SN);}
              else if(enemyInfo.enemy_armies[j].Sort==7&&!isElementExists(Ship,enemyInfo.enemy_armies[j].SN,TAGDEFAULT))
              {Ship.push_back(enemyInfo.enemy_armies[j].SN);
}}}
        for(int j=0;j<enemyInfo.enemy_buildings.size();j++){
          double temp=pow(pow(enemyInfo.armies[i].BlockDR-enemyInfo.enemy_buildings[j].BlockDR, 2) + pow(enemyInfo.armies[i].BlockUR-enemyInfo.enemy_buildings[j].BlockUR, 2), 0.5);
          if(temp<15&&enemyInfo.enemy_buildings[j].Type!=BUILDING_ARROWTOWER){

              if (!isElementExists(Building,enemyInfo.enemy_buildings[j].SN,TAGDEFAULT))
              Building.push_back(enemyInfo.enemy_buildings[j].SN);
          }
          else if(temp<15&&enemyInfo.enemy_buildings[j].Type==BUILDING_ARROWTOWER){
              if (!isElementExists(Arrowtower,enemyInfo.enemy_buildings[j].SN,TAGDEFAULT))
              Arrowtower.push_back(enemyInfo.enemy_buildings[j].SN);
          }
}
    }
        else if(enemyInfo.armies[i].status==2){
            for(int j=0;j<enemyInfo.enemy_farmers.size();j++){
              double temp=pow(pow(enemyInfo.armies[i].BlockDR-enemyInfo.enemy_farmers[0].BlockDR, 2) + pow(enemyInfo.armies[i].BlockUR-enemyInfo.enemy_farmers[0].BlockUR, 2), 0.5);
              if(temp<15){
                  if (!isElementExists(Defend,enemyInfo.enemy_farmers[j].SN,TAGDEFAULT))
                  Defend.push_back(enemyInfo.enemy_farmers[j].SN);

              }
            }
            for(int j=0;j<enemyInfo.enemy_armies.size();j++){
              double temp=pow(pow(enemyInfo.armies[i].BlockDR-enemyInfo.enemy_armies[j].BlockDR, 2) + pow(enemyInfo.armies[i].BlockUR-enemyInfo.enemy_armies[j].BlockUR, 2), 0.5);
              if(temp<15){

                   if (!isElementExists(Defend,enemyInfo.enemy_armies[j].SN,TAGDEFAULT))
                  Defend.push_back(enemyInfo.enemy_armies[j].SN);
              }
    }
            for(int j=0;j<enemyInfo.enemy_buildings.size();j++){
              double temp=pow(pow(enemyInfo.armies[i].BlockDR-enemyInfo.enemy_buildings[j].BlockDR, 2) + pow(enemyInfo.armies[i].BlockUR-enemyInfo.enemy_buildings[j].BlockUR, 2), 0.5);
              if(temp<15&&enemyInfo.enemy_buildings[j].Type!=BUILDING_ARROWTOWER){

                  if (!isElementExists(Defend,enemyInfo.enemy_buildings[j].SN,TAGDEFAULT))
                  Defend.push_back(enemyInfo.enemy_buildings[j].SN);
              }}
        }
    }
}
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
    if(Ship.size()!=0||Boat.size()!=0){
        for(int i=0;i<enemyInfo.armies.size();i++){
            if((enemyInfo.armies[i].status==1||enemyInfo.armies[i].status==3&&enemyInfo.armies[i].Sort==7)&& ifA[enemyInfo.armies[i].SN]==false){
                 ifA[enemyInfo.armies[i].SN]=true;
            }
        }
    }else if(Ship.size()+Boat.size()==0){
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

void EnemyAI::Around(){
    for(int i=0;i<enemyInfo.armies.size();i++){
    if(g_frame-timer[enemyInfo.armies[i].SN]>250&&enemyInfo.armies[i].status==AROUND&&ifA[enemyInfo.armies[i].SN]==false){
            if(around[i]==0){
            HumanMove(enemyInfo.armies[i].SN,enemyInfo.armies[i].destinaDR,enemyInfo.armies[i].destinaUR);
            timer[enemyInfo.armies[i].SN]=g_frame;
             around[i]=1-around[i];
            }else if(around[i]==1){
            HumanMove(enemyInfo.armies[i].SN,enemyInfo.armies[i].startpointDR,enemyInfo.armies[i].startpointUR);
            timer[enemyInfo.armies[i].SN]=g_frame;
             around[i]=1-around[i];
            }
}   else if(g_frame-timer[enemyInfo.armies[i].SN]>250&&enemyInfo.armies[i].status==ATTACK&&ifA[enemyInfo.armies[i].SN]==false){
        HumanMove(enemyInfo.armies[i].SN,enemyInfo.armies[i].startpointDR,enemyInfo.armies[i].startpointUR);
         timer[enemyInfo.armies[i].SN]=g_frame;
    }
}
    }

void EnemyAI::Attack(){
    for(int i=0;i<enemyInfo.armies.size();i++){
        if(g_frame-timer[enemyInfo.armies[i].SN]>250&&enemyInfo.armies[i].status==AROUND&& ifA[enemyInfo.armies[i].SN]==true){
            if(enemyInfo.armies[i].Sort!=7){
            if(Farmer.size()!=0){
                HumanAction(enemyInfo.armies[i].SN,Farmer.back());
                qDebug()<<"攻击更新"<< ifA[enemyInfo.armies[i].SN]<<g_frame;
            }
            else if(Army.size()!=0){
                 HumanAction(enemyInfo.armies[i].SN,Army.back());
            }
            else if(Building.size()!=0){
                 HumanAction(enemyInfo.armies[i].SN,Building.back());
            }}
            else if(enemyInfo.armies[i].Sort==7){
                if(Boat.size()!=0){
                    HumanAction(enemyInfo.armies[i].SN,Boat.back());
                }
                else if(Ship.size()!=0){
                    HumanAction(enemyInfo.armies[i].SN,Ship.back());
                }
            }
            timer[enemyInfo.armies[i].SN]=g_frame;
        }
        else if(g_frame-timer[enemyInfo.armies[i].SN]>250&&enemyInfo.armies[i].status==DEFENSE&& ifA[enemyInfo.armies[i].SN]==true){
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
//void check(){
//    for(int i=0;i<enemyInfo.armies.size();i++){
//        if(enemyInfo.armies[i].Blood==0){
//            timerv.erase(timerv.begin()+i);
//            ifAttackv.erase(ifAttackv.begin()+i);
//        }
//    }
//}
static void ifDead(vector <int> &x,int sort){
    if(x.size()>0)
    switch(sort){
    case VECTORFARMER:
        for(int i=0;i<x.size();i++){
                if(!isElementExists(Farmer,x[i],TAGFARMER))
                   {
                    x.erase(x.begin()+i);
                }
            if(i>x.size()) break;
        }
        break;
    case VECTORARMY:
        for(int i=0;i<x.size();i++){
                if(!isElementExists(Army,x[i],TAGARMY))
                   { x.erase(x.begin()+i);
            }
            if(i>x.size()) break;
        }
        break;
     case VECTORBOAT:
        qDebug()<<"运输船死亡"<<g_frame<<sort;
        for(int i=0;i<x.size();i++){
                if(!isElementExists(Boat,x[i],TAGFARMER))
                {
                    qDebug()<<"运输船删减成功";
                 x.erase(x.begin()+i);
}
            if(i>x.size()) break;
        }
        break;
     case VECTORSHIP:
        for(int i=0;i<x.size();i++){
                if(!isElementExists(Ship,x[i],TAGARMY))
                   { x.erase(x.begin()+i);
            }
            if(i>x.size()) break;
        }
        break;
     case VECTORARROWTOWER:
        for(int i=0;i<x.size();i++){
               if(!isElementExists(Arrowtower,x[i],TAGBUILDING))
                 {   x.erase(x.begin()+i);
            }
            if(i>x.size()) break;
        }
        break;
     case VECTORBUILDING:
        for(int i=0;i<x.size();i++){
                if(!isElementExists(Building,x[i],TAGBUILDING))
                 {   x.erase(x.begin()+i);
            }
            if(i>x.size()) break;
        }
        break;
     case VECTORDEFEND:
        for(int i=0;i<x.size();i++){
                if(!isElementExists(Defend,x[i],TAGARMY))
                  {  x.erase(x.begin()+i);
            }
            if(i>x.size()) break;
        }
         break;
}

}
void EnemyAI::processData() {
    enemyInfo=getInfo();
    int nowState_Army=0;
     //军队数据初始化
     if(g_frame==50){
         sum=enemyInfo.armies.size();
         for(int i=0;i<enemyInfo.armies.size();i++){
             ifA.insert(std::pair<double,bool>(enemyInfo.armies[i].SN,false));
             timer.insert(std::pair<double,long>(enemyInfo.armies[i].SN,false));
            }
     }
     qDebug()<<g_frame;
        if(g_frame>1000){
//            check();
            Around();
            seekenemy();
            ifATTACK();
            Attack();
              }
        if(g_frame>50&&g_frame%10==0){
            int s=Farmer.size();
            ifDead(Farmer,VECTORFARMER);
            if(s!=Farmer.size())
            ifDead(Army,VECTORARMY);
            ifDead(Boat,VECTORBOAT);
            ifDead(Ship,VECTORSHIP);
            ifDead(Defend,VECTORDEFEND);
            ifDead(Arrowtower,VECTORARROWTOWER);
            ifDead(Building,VECTORBUILDING);
            for(int i=0;i<enemyInfo.armies.size();i++){
                if(enemyInfo.armies[i].status==ATTACK)
                    qDebug()<<ifA[enemyInfo.armies[i].SN]<<g_frame;
            }
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
//    qDebug()<<"change";
}
