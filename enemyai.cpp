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
static int timer[50] = {0};
static bool ifAttack[50];
static vector <int> timerv;
static vector <bool>ifAttackv;
static vector <int> Army;
static vector <int> Farmer;
static vector <int> Ship;
static vector <int> Boat;
static vector <int> Arrowtower;
static vector <int> Building;
static vector <int> Defend;
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
            if((enemyInfo.armies[i].status==1||enemyInfo.armies[i].status==3&&enemyInfo.armies[i].Sort!=7)&&ifAttack[i]==false){
                ifAttack[i]=true;
            }
        }
    }else if(Army.size()+Farmer.size()+Building.size()+Arrowtower.size()==0){
        for(int i=0;i<enemyInfo.armies.size();i++)
            if((enemyInfo.armies[i].status==1||enemyInfo.armies[i].status==3&&enemyInfo.armies[i].Sort!=7)&&ifAttack[i]==true){
                ifAttack[i]=false;
            }
    }
    if(Ship.size()!=0||Boat.size()!=0){
        for(int i=0;i<enemyInfo.armies.size();i++){
            if((enemyInfo.armies[i].status==1||enemyInfo.armies[i].status==3&&enemyInfo.armies[i].Sort==7)&&ifAttack[i]==false){
                ifAttack[i]=true;
            }
        }
    }else if(Ship.size()+Boat.size()==0){
        for(int i=0;i<enemyInfo.armies.size();i++)
            if((enemyInfo.armies[i].status==1||enemyInfo.armies[i].status==3&&enemyInfo.armies[i].Sort==7)&&ifAttack[i]==true){
                ifAttack[i]=false;
            }
    }
    if(Defend.size()!=0){
        for(int i=0;i<enemyInfo.armies.size();i++){
            if(enemyInfo.armies[i].status==2&&ifAttack[i]==false){
                ifAttack[i]=true;
            }
        }}
        else if(Defend.size()==0){
            for(int i=0;i<enemyInfo.armies.size();i++){
                if(enemyInfo.armies[i].status==2&&ifAttack[i]==true){
                    ifAttack[i]=false;
                }
            }
        }
    }

void EnemyAI::Around(){
    for(int i=0;i<enemyInfo.armies.size();i++){
    if(g_frame-timer[i]>350&&enemyInfo.armies[i].status==AROUND&&ifAttack[i]==false){
            if(around[i]==0){
            HumanMove(enemyInfo.armies[i].SN,enemyInfo.armies[i].destinaDR,enemyInfo.armies[i].destinaUR);
            timer[i]=g_frame;
             around[i]=1-around[i];
            }else if(around[i]==1){
            HumanMove(enemyInfo.armies[i].SN,enemyInfo.armies[i].startpointDR,enemyInfo.armies[i].startpointUR);
            timer[i]=g_frame;
             around[i]=1-around[i];
            }
}
}
}
void EnemyAI::Attack(){
    for(int i=0;i<enemyInfo.armies.size();i++){
        if(g_frame-timer[i]>250&&enemyInfo.armies[i].status==AROUND&&ifAttack[i]==true){
            if(enemyInfo.armies[i].Sort!=7){
            if(Farmer.size()!=0){
                HumanAction(enemyInfo.armies[i].SN,Farmer.back());
                qDebug()<<"攻击更新"<<ifAttack[i]<<g_frame;
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
            timer[i]=g_frame;
        }
        else if(g_frame-timer[i]>250&&enemyInfo.armies[i].status==DEFENSE&&ifAttack[i]==true){
            if(Defend.size()!=0){
                HumanAction(enemyInfo.armies[i].SN,Defend.back());
                timer[i]=g_frame;
            }
        }
        else if(g_frame-timer[i]>125&&enemyInfo.armies[i].status==ATTACK&&ifAttack[i]==true){
            if(Building.size()!=0){
                             HumanAction(enemyInfo.armies[i].SN,Building.back());
                        }
            else if(Army.size()!=0){
                 HumanAction(enemyInfo.armies[i].SN,Army.back());
            }
            else if(Farmer.size()!=0){
                HumanAction(enemyInfo.armies[i].SN,Farmer.back());
            }
            timer[i]=g_frame;
        }
    }

}
void check(){
    for(int i=0;i<enemyInfo.armies.size();i++){
        if(enemyInfo.armies[i].Blood==0){
            timerv.erase(timerv.begin()+i);
            ifAttackv.erase(ifAttackv.begin()+i);
        }
    }
}
void ifDead(vector <int> x,int sort){
    if(x.size()>0)
    switch(sort){
    case VECTORFARMER:
        for(int i=0;i<x.size();i++){
             qDebug()<<"农民"<<g_frame<<sort;
            for(int j=0;j<enemyInfo.enemy_farmers.size();j++){{
                qDebug()<<!isElementExists(Farmer,x[i],TAGFARMER);
                if(!isElementExists(Farmer,x[i],TAGFARMER))
                   {
                    x.erase(x.begin()+i);
                    qDebug()<<"农民死亡"<<g_frame<<Farmer.size()<<sort;
                    break;
                }
            }}
            if(i<x.size()) break;
        }
        break;
    case VECTORARMY:
        for(int i=0;i<x.size();i++){
            for(int j=0;j<enemyInfo.enemy_armies.size();j++){
                if(!isElementExists(Army,x[i],TAGARMY))
                   { x.erase(x.begin()+i);
                 break;}
            }
            if(i<x.size()) break;
        }
        break;
     case VECTORBOAT:
        qDebug()<<"运输船死亡"<<g_frame<<sort;
        for(int i=0;i<x.size();i++){
            for(int j=0;j<enemyInfo.enemy_farmers.size();j++){
                if(!isElementExists(Boat,x[i],TAGFARMER))
                {
                 x.erase(x.begin()+i);
                 break;
            }}
            if(i<x.size()) break;
        }
        break;
     case VECTORSHIP:
        for(int i=0;i<x.size();i++){
            for(int j=0;j<enemyInfo.enemy_armies.size();j++){
                if(!isElementExists(Ship,x[i],TAGARMY))
                   { x.erase(x.begin()+i);
                 break;}
            }
            if(i<x.size()) break;
        }
        break;
     case VECTORARROWTOWER:
        for(int i=0;i<x.size();i++){
            for(int j=0;j<enemyInfo.enemy_buildings.size();j++){
               if(!isElementExists(Arrowtower,x[i],TAGBUILDING))
                 {   x.erase(x.begin()+i);
                 break;}
            }
            if(i<x.size()) break;
        }
        break;
     case VECTORBUILDING:
        for(int i=0;i<x.size();i++){
            for(int j=0;j<enemyInfo.enemy_buildings.size();j++){
                if(!isElementExists(Building,x[i],TAGBUILDING))
                 {   x.erase(x.begin()+i);
                 break;}
            }
            if(i<x.size()) break;
        }
        break;
     case VECTORDEFEND:
        for(int i=0;i<x.size();i++){
            for(int j=0;j<enemyInfo.enemy_armies.size();j++){
                if(!isElementExists(Defend,x[i],TAGARMY))
                  {  x.erase(x.begin()+i);
                 break;}
            }
            if(i<x.size()) break;
        }
         break;
}

}
void EnemyAI::processData() {
    //return;
    enemyInfo=getInfo();
    int nowState_Army=0;
     //军队数据初始化
     if(g_frame==50){
         sum=enemyInfo.armies.size();
         for(int i=0;i<enemyInfo.armies.size();i++){
             timerv.push_back(0);
             ifAttackv.push_back(false);
             ifAttack[i]=false;
            }
     }
        if(g_frame>1000){
            check();
            Around();
            seekenemy();
            ifATTACK();
            Attack();
            for(int i=0;i<enemyInfo.armies.size();i++)
                if(enemyInfo.armies[i].Sort!=7)
            qDebug()<<ifAttack[i]<<enemyInfo.armies[i].status<<g_frame;
              }
        if(g_frame>50&&g_frame%15==0){
            int s=Farmer.size();
            ifDead(Farmer,VECTORFARMER);
            if(s!=Farmer.size())
                qDebug()<<"删减成功"<<Farmer.size()<<g_frame;
            ifDead(Army,VECTORARMY);
            ifDead(Boat,VECTORBOAT);
            ifDead(Ship,VECTORSHIP);
            ifDead(Defend,VECTORDEFEND);
            ifDead(Arrowtower,VECTORARROWTOWER);
            ifDead(Building,VECTORBUILDING);
        }
}

//    if(mode==5){
//            for(int i=0;i<enemyInfo.enemy_armies.size();i++){
//                if(enemyInfo.enemy_armies[i].Sort==7)
//                armystate[i]==ATTACK;
//            }
//        }
//        if(mode==6){
//            for(int i=0;i<enemyInfo.enemy_armies.size();i++){
//                if(enemyInfo.enemy_armies[i].Sort!=7)
//                armystate[i]==ATTACK;
//            }
//        }
//        if(g_frame%50==0){
//            seekenemy();
//        }
//        //陆地巡逻
//        if(g_frame==MODE1){
//        for(int i=0;i<enemyInfo.armies.size();i++){
//            if(enemyInfo.armies[i].Sort==3){
//                armystate[i]=AROUND;
//                }
//        }
//        for(int i=0;i<enemyInfo.armies.size();i++){
//            if(enemyInfo.armies[i].Sort==7){
//                armystate[i]=AROUND;
//                break;
//            }
//        }
//        }
//        for(int i=0;i<enemyInfo.armies.size();i++){
//        if(mode>=1&&mode<4&&armystate[i]==AROUND&&AroundLock[i]==0&&enemyInfo.armies[i].Sort==3){
//            timer[i]=g_frame;
//            if(a%2==0)
//            HumanMove(enemyInfo.armies[i].SN,Pathcar[Pathn][0]*35,Pathcar[Pathn][1]*35);
//            else HumanMove(enemyInfo.armies[i].SN,(Pathcar[Pathn][0]*35)-1,(Pathcar[Pathn][1]*35)-1);
//    //        else HumanMove(enemyInfo.armies[i].SN,Path[Pathn][0]*33,Path[Pathn][1]*33);
//            AroundLock[i]=1;
//            a++;
//        }
//        else if(mode>=1&&mode<4&&armystate[i]==AROUND&&AroundLock[i]==0&&enemyInfo.armies[i].Sort==7){
//             timer[i]=g_frame;
//             HumanMove(enemyInfo.armies[i].SN,(Pathboat[Pathb][0])*35,(Pathboat[Pathb][1])*35);
//             AroundLock[i]=1;
//             c++;
//        }
//        else if(mode>=1&&mode<4&&armystate[i]==AROUND&&AroundLock[i]==1){
//            if(g_frame-timer[i]>400){
//                AroundLock[i]=0;
//            }
//        }
//        if(a-b==2){
//            b=a;
//            Pathn+=Pathnset;
//            if(Pathn==12||Pathn==0){
//            Pathnset=-1*Pathnset;
//            Pathn+=Pathnset;
//            }
//        }
//        if(c-d==1){
//            d=c;
//            Pathb+=Pathbset;
//            if(Pathb==12||Pathb==0){
//            Pathbset=-1*Pathbset;
//            Pathb+=Pathbset;
//            }

//        }

//        }
//        for(int i=0;i<enemyInfo.armies.size();i++){
//            if(armystate[i]==AROUND){aroundseekenemy(i);
//            qDebug()<<"success"<<g_frame;}
//        }


     //更新波数
//     if(g_frame==MODE4) mode=4;
//     else if(g_frame==MODE3) mode=3;
//     else if(g_frame==MODE2) mode=2;
//     else if(g_frame==MODE1) {mode=1;

//     }
//     qDebug()<<mode<<g_frame;
//     //巡逻进攻
//     if(mode==5){
//         for(int i=0;i<enemyInfo.enemy_armies.size();i++){
//             if(enemyInfo.enemy_armies[i].Sort==7)
//             armystate[i]==ATTACK;
//         }
//     }
//     if(mode==6){
//         for(int i=0;i<enemyInfo.enemy_armies.size();i++){
//             if(enemyInfo.enemy_armies[i].Sort!=7)
//             armystate[i]==ATTACK;
//         }
//     }
//     if(g_frame%50==0){
//         seekenemy();
//     }
//     //陆地巡逻
//     if(g_frame==MODE1){
//     for(int i=0;i<enemyInfo.armies.size();i++){
//         if(enemyInfo.armies[i].Sort==3){
//             armystate[i]=AROUND;
//             }
//     }
//     for(int i=0;i<enemyInfo.armies.size();i++){
//         if(enemyInfo.armies[i].Sort==7){
//             armystate[i]=AROUND;
//             break;
//         }
//     }
//     }
//     for(int i=0;i<enemyInfo.armies.size();i++){
//     if(mode>=1&&mode<4&&armystate[i]==AROUND&&AroundLock[i]==0&&enemyInfo.armies[i].Sort==3){
//         timer[i]=g_frame;
//         if(a%2==0)
//         HumanMove(enemyInfo.armies[i].SN,Pathcar[Pathn][0]*35,Pathcar[Pathn][1]*35);
//         else HumanMove(enemyInfo.armies[i].SN,(Pathcar[Pathn][0]*35)-1,(Pathcar[Pathn][1]*35)-1);
// //        else HumanMove(enemyInfo.armies[i].SN,Path[Pathn][0]*33,Path[Pathn][1]*33);
//         AroundLock[i]=1;
//         a++;
//     }
//     else if(mode>=1&&mode<4&&armystate[i]==AROUND&&AroundLock[i]==0&&enemyInfo.armies[i].Sort==7){
//          timer[i]=g_frame;
//          HumanMove(enemyInfo.armies[i].SN,(Pathboat[Pathb][0])*35,(Pathboat[Pathb][1])*35);
//          AroundLock[i]=1;
//          c++;
//     }
//     else if(mode>=1&&mode<4&&armystate[i]==AROUND&&AroundLock[i]==1){
//         if(g_frame-timer[i]>400){
//             AroundLock[i]=0;
//         }
//     }
//     if(a-b==2){
//         b=a;
//         Pathn+=Pathnset;
//         if(Pathn==12||Pathn==0){
//         Pathnset=-1*Pathnset;
//         Pathn+=Pathnset;
//         }
//     }
//     if(c-d==1){
//         d=c;
//         Pathb+=Pathbset;
//         if(Pathb==12||Pathb==0){
//         Pathbset=-1*Pathbset;
//         Pathb+=Pathbset;
//         }

//     }

//     }
//     for(int i=0;i<enemyInfo.armies.size();i++){
//         if(armystate[i]==AROUND){aroundseekenemy(i);
//         qDebug()<<"success"<<g_frame;}
//     }



//     if(enemyInfo.armies.size()!=sum){
// //        sum=enemyInfo.armies.size();
//         for(int i=0;i<enemyInfo.armies.size();i++){
//              Blood[i]=enemyInfo.armies[i].Blood;
//              if(mode==1||mode==2||mode==-1||mode==-2)
//             { armystate[i]=WAITING;
//              ChasingLock[i]=0;}
// //             Lock[i]=0;
//         }
//     }
// //    qDebug()<<mode<<g_frame;
//     //撤退
//     if(mode==-1)
//         for(int i=0;i<ATT1;i++)
//             if(armystate[i]!=RETREAT&&armystate[i]!=CHASE&&ifretreat[i]==false){
//                 armystate[i]=WAITING;
//                 if(calDistance(enemyInfo.armies[i].DR,enemyInfo.armies[i].UR,pos_L[i],pos_U[i])>=50){
//                      HumanMove(enemyInfo.armies[i].SN,pos_L[i],pos_U[i]);
//                      armystate[i]=RETREAT;
//                      ifretreat[i]=true;
//             }

//             }
//     if(mode==-2)
//         for(int i=0;i<ATT2;i++)
//         if(armystate[i]!=RETREAT&&armystate[i]!=CHASE&&ifretreat[i]==false){
//             armystate[i]=WAITING;
//             if(calDistance(enemyInfo.armies[i].DR,enemyInfo.armies[i].UR,pos_L[i],pos_U[i])>=50){
//                  HumanMove(enemyInfo.armies[i].SN,pos_L[i],pos_U[i]);
//                  armystate[i]=RETREAT;
//                  ifretreat[i]=true;
//         }
//         }
//     if(mode<0)
//         for(int i=0;i<enemyInfo.armies.size();i++){
//             if(ifretreat[i]==true)
//                if(calDistance(enemyInfo.armies[i].BlockDR,enemyInfo.armies[i].BlockUR,pos_L[49],pos_U[49])<30)
//                     ifretreat[i]=false;
//     }
     //强制总攻
 //    for(int i=0;i<enemyInfo.armies.size();i++)
 //    qDebug()<<calDistance(enemyInfo.armies[i].BlockDR,enemyInfo.armies[i].BlockUR,pos_L[49],pos_U[49])<<g_frame;
     //撤退检查
 //    if(mode<0){
 //        for(int i=0;i<ATT2;i++){
 //            if(armystate[i]==RETREAT){
 //                if(enemyInfo.armies[i].NowState==MOVEOBJECT_STATE_STAND){
 //                    armystate[i]==WAITING;
 //                }
 //            }
 //        }
 //    }
 //    qDebug()<<armystate[0];
    // 根据波数启动军队,选定各兵种目标
//     if(mode==1){
//         for(int i=0;i<enemyInfo.armies.size();i++){
//             if(armystate[i]==WAITING||armystate[i]==RETREAT&&enemyInfo.armies[i].Sort==3){
//                 armystate[i]==AROUND;
//             }
//         }
//     }
//     else if(mode==2){
//         for(int i=0;i<7;i++){
//             if(armystate[i]==WAITING||armystate[i]==RETREAT){ armystate[i]=ATTACK;
//                 if(enemyInfo.armies[i].Sort==1){
//                     target[1]=seek(1,3);
//                     if(target[1]>150) target[1]=seek(1,2);
//                     if(target[1]>150) target[1]=seek(1,1);
//                 }
//                 else if(enemyInfo.armies[i].Sort==2){
//                     target[2]=seek(2,1);
//                     if(target[2]>150) target[2]=seek(2,2);
//                     if(target[2]>150) target[2]=seek(2,3);
//                 }
//                 else if(enemyInfo.armies[i].Sort==3){
//                     target[3]=seek(3,1);
//                     if(target[3]>150) target[3]=seek(3,2);
//                     if(target[3]>150) target[3]=seek(3,3);
//                 }
//                 else if(enemyInfo.armies[i].Sort==0){
//                     target[0]=seek(0,1);
//                     if(target[3]>150) target[3]=seek(0,2);
//                     if(target[3]>150) target[3]=seek(0,3);
//                 }
//             }}
//     }
//     else if(mode==3){
//         for(int i=0;i<enemyInfo.armies.size();i++){
//             if(enemyInfo.armies[i].Sort==1){
//                 target[1]=seek(1,2);
//                 if(target[1]>150) target[1]=seek(1,3);
//             }
//             else if(enemyInfo.armies[i].Sort==2){
//                 target[2]=seek(2,2);
//                 if(target[2]>150) target[2]=seek(2,3);
//             }
//             else if(enemyInfo.armies[i].Sort==3){
//                 target[3]=seek(3,2);
//                 if(target[3]>150) target[3]=seek(3,3);
//             }
//             else if(enemyInfo.armies[i].Sort==0){
//                 target[0]=seek(0,2);
//                 if(target[0]>150) target[0]=seek(0,3);
//             }
//         }
//     }
 //    else if(mode==3){
 //        for(int i=0;i<enemyInfo.armies.size();i++){
 //            if(armystate[i]==CHASE||armystate[i]==WAITING) {armystate[i]=ATTACK;
 //                target[4]=seek(enemyInfo.armies[Hero].Sort,3);
 //                if(target[4]>150) target[2]=seek(enemyInfo.armies[Hero].Sort,2);
 //                if(target[4]>150)target[2]=seek(enemyInfo.armies[Hero].Sort,1);
 //            }
 //        }

 //    qDebug()<<armystate[0];
     // 自动反击
 //    if(g_frame>15){
 //    for(int i=0;i<enemyInfo.armies.size();i++){
 //           if(enemyInfo.armies[i].Blood!=Blood[i]&&armystate[i]!=CHASE&&ChasingLock[i]==0){
 //               for(int j=0;j<enemyInfo.enemy_armies.size();j++){
 //                   if(enemyInfo.enemy_armies[j].WorkObjectSN==enemyInfo.armies[i].SN){
 //                       HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[j].SN);
 //                       armystate[i]=CHASE;
 //                       Blood[i]=enemyInfo.armies[i].Blood;
 //                       chasestart_L[i]=enemyInfo.armies[i].DR;
 //                       chasestart_U[i]=enemyInfo.armies[i].UR;
 //                       timer[i]=g_frame;
 //                       ChasingLock[i]=1;
 //                   }
 //               }}
 //               if(enemyInfo.armies[i].Blood!=Blood[i]&&armystate[i]!=CHASE&&ChasingLock[i]==0){
 //                   for(int j=0;j<enemyInfo.enemy_farmers.size();j++){
 //                       if(enemyInfo.enemy_farmers[j].WorkObjectSN==enemyInfo.armies[i].SN){
 //                           HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_farmers[j].SN);
 //                           armystate[i]=CHASE;
 //                           Blood[i]=enemyInfo.armies[i].Blood;
 //                           chasestart_L[i]=enemyInfo.armies[i].DR;
 //                           chasestart_U[i]=enemyInfo.armies[i].UR;
 //                           timer[i]=g_frame;
 //                           ChasingLock[i]=1;
 //                       }
 //               }
 //           }
 //               if(enemyInfo.armies[i].Blood!=Blood[i]&&armystate[i]!=CHASE&&ChasingLock[i]==0){
 //                  for(int j=0;j<enemyInfo.enemy_buildings.size();j++)
 //                       if(enemyInfo.enemy_buildings[j].Type==BUILDING_ARROWTOWER){
 //                           if(calDistance(enemyInfo.armies[Hero].BlockDR,enemyInfo.armies[Hero].BlockUR,enemyInfo.enemy_buildings[i].BlockDR,enemyInfo.enemy_buildings[i].BlockUR)<5*BLOCKSIDELENGTH)
 //                           {
 //                           HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_buildings[j].SN);
 //                           armystate[i]=DESTROY;
 //                           Blood[i]=enemyInfo.armies[i].Blood;
 //                           chasestart_L[i]=enemyInfo.armies[i].DR;
 //                           chasestart_U[i]=enemyInfo.armies[i].UR;
 //                           timer[i]=g_frame;
 //                           ChasingLock[i]=1;}
 //                       }
 //                       }

 //               }
 //           }
 //    if(mode<0){
 //        for(int i=0;i<enemyInfo.armies.size();i++){
 //            if(armystate[i]==WAITING||armystate[i]==RETREAT){
 //                for(int j=0;j<enemyInfo.armies.size();j++){
 //                    int temp=calDistance(enemyInfo.armies[i].DR,enemyInfo.armies[i].UR,enemyInfo.armies[j].DR,enemyInfo.armies[j].UR);
 //                    if(armystate[j]==CHASE&&temp<150){
 //                        armystate[i]=CHASE;
 //                        qDebug()<<enemyInfo.armies[j].WorkObjectSN;
 //                        HumanAction(enemyInfo.armies[i].SN,enemyInfo.armies[j].WorkObjectSN);
 //                        chasestart_L[i]=enemyInfo.armies[i].DR;
 //                        chasestart_U[i]=enemyInfo.armies[i].UR;
 //                        timer[i]=g_frame;
 //                        ChasingLock[i]=1;
 //                    }
 //                }
 //            }
 //        }
 //    }
 ////    qDebug()<<armystate[i]<<Lock[i]<<ChasingLock[i]<<enemyInfo.armies[i].NowState<<enemyInfo.armies[i].Sort<<g_frame;
 //        //反击检查
 ////    qDebug()<<armystate[0]<<ChasingLock[0]<<armystate[1]<<ChasingLock[1]<<g_frame;
 //    for(int i=0;i<enemyInfo.armies.size();i++){
 //           if(ChasingLock[i]!=0&&(armystate[i]!=CHASE&&armystate[i]!=DESTROY))ChasingLock[i]=0;
 //           if(armystate[i]==CHASE){

 //               if(calDistance(enemyInfo.armies[i].DR,enemyInfo.armies[i].UR,chasestart_L[i],chasestart_U[i])>=200)
 //               {
 //                   HumanMove(enemyInfo.armies[i].SN,chasestart_L[i],chasestart_U[i]);
 //                   timer[i] = 0;
 //                   ChasingLock[i]=0;
 //                   armystate[i]=WAITING;
 //               }
 //               if((g_frame - timer[i]) >= 30 && enemyInfo.armies[i].NowState !=8&&enemyInfo.armies[i].NowState!=10){
 //                   timer[i] = 0;
 //                   ChasingLock[i]=0;
 //                   armystate[i]=WAITING;
 //               }}
 //               if(armystate[i]==DESTROY){
 //                   if((g_frame - timer[i]) >= 15 && enemyInfo.armies[i].NowState == MOVEOBJECT_STATE_STAND){
 //                       timer[i] = 0;
 //                       ChasingLock[i]=0;
 //                       armystate[i]=WAITING;
 //                   }
 //               }
 //           }
       //攻击状态
//       for(int i=0;i<enemyInfo.armies.size();i++)
//       if(armystate[i]==ATTACK){
//             if(mode==1){
//             if(Lock[i]==0){
//                      nowState_Army=enemyInfo.armies[i].NowState;
//                      if(nowState_Army==MOVEOBJECT_STATE_STAND)
//                      {

//                          int tar=target[0];
// //                         qDebug()<<tar<<"攻击对象";
//                          if(tar==151)
//                          armystate[i]=WAITING;
//                          else if(enemyInfo.enemy_farmers.size()!=0) HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_farmers[tar].SN);
//                          if(tar!=151){    Lock[i]=1;
//                              timer[i]=g_frame;
//                          }

//            }
//             }}
//             else if(mode==2){
//                 nowState_Army=enemyInfo.armies[i].NowState;
//                 if(nowState_Army==MOVEOBJECT_STATE_STAND&&Lock[i]==0){
//                 if(enemyInfo.armies[i].Sort==2){
//                    int tar=target[1];
//                     ChasingLock[i]=1;
//                     if(tar==151)
//                     armystate[i]=WAITING;
//                     else if(tar>=100&&enemyInfo.enemy_buildings.size()!=0)
//                         HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_buildings[tar-100].SN);
//                     else if(tar>=50&&enemyInfo.enemy_armies.size()!=0 ) HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[tar-50].SN);
//                     else if(enemyInfo.enemy_farmers.size()!=0) HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_farmers[tar].SN);
//                 if(tar!=151){    Lock[i]=1;
//                     timer[i]=g_frame;
//                     }}
//                 else if(enemyInfo.armies[i].Sort==1)
//                 {
//                     int tar=target[1];
//                     if(tar==151)
//                     armystate[i]=WAITING;
//                     else if(tar>=100&&enemyInfo.enemy_buildings.size()!=0)
//                         HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_buildings[tar-100].SN);
//                     else  if(tar>=50&&enemyInfo.enemy_armies.size()!=0 ) HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[tar-50].SN);
//                     else if(enemyInfo.enemy_farmers.size()!=0)HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_farmers[tar].SN);
//                     if(tar!=151){    Lock[i]=1;
//                         timer[i]=g_frame;
//                 }}
//                 else if(enemyInfo.armies[i].Sort==3)
//                 {
//                     int tar=target[3];
//                     if(tar==151)
//                     armystate[i]=WAITING;
//                     else if(tar>=100&&enemyInfo.enemy_buildings.size()!=0)
//                     HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_buildings[tar-100].SN);
//                     else  if(tar>=50&&enemyInfo.enemy_armies.size()!=0 ) HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[tar-50].SN);
//                     else if(enemyInfo.enemy_farmers.size()!=0)HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_farmers[tar].SN);
//                     if(tar!=151){    Lock[i]=1;
//                         timer[i]=g_frame;
//                 }}
//                 else if(enemyInfo.armies[i].Sort==0){

//                     int tar=target[0];
//                     if(tar==151)
//                     armystate[i]=WAITING;
//                     else if(tar>=100&&enemyInfo.enemy_buildings.size()!=0)
//                         HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_buildings[tar-100].SN);

//                     else  if(tar>=50&&enemyInfo.enemy_armies.size()!=0 ) HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[tar-50].SN);
//                     else if(enemyInfo.enemy_farmers.size()!=0) HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_farmers[tar].SN);
//                     if(tar!=151){    Lock[i]=1;
//                         timer[i]=g_frame;
//                 }
//         }}}
//             else if(mode==3){
//                 if(Lock[i]==0){
//                           nowState_Army=enemyInfo.armies[i].NowState;
//                           if(nowState_Army==MOVEOBJECT_STATE_STAND)
//                           {   int tar=target[4];
//                               if(tar==151)
//                               armystate[i]==WAITING;
//                               else if(tar>=100&&enemyInfo.enemy_buildings.size()!=0)
//                               HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_buildings[tar-100].SN);
//                               else if(tar>=50&&enemyInfo.enemy_armies.size()!=0 ) HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[tar-50].SN);
//                               else if(enemyInfo.enemy_farmers.size()!=0)HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_farmers[tar].SN);
//                               if(tar!=151){    Lock[i]=1;
//                                   timer[i]=g_frame;
//                             }
//             }
//         }
//              }}


//        }

 //      if(mode==3){
 ////          qDebug()<<armystate[Hero]<<g_frame;
 ////          qDebug()<<armystate[Hero-1]<<g_frame;
 //          for(int i=0;i<enemyInfo.enemy_armies.size();i++){
 //              if(calDistance(enemyInfo.armies[Hero].BlockDR,enemyInfo.armies[Hero].BlockUR,enemyInfo.enemy_armies[i].BlockDR,enemyInfo.enemy_armies[i].BlockUR)<8*BLOCKSIDELENGTH){
 //                  Atarget[s]=enemyInfo.enemy_armies[i].SN;s++;
 //              }
 //          }
 //      }
 //      if(mode==3){
 //          int min=12000;
 //          int temp=0;
 //          for(int i=0;i<enemyInfo.enemy_buildings.size();i++){
 //              /*if(enemyInfo.enemy_buildings[i].Type!=BUILDING_ARROWTOWER&&enemyInfo.enemy_buildings[i].Type!=BUILDING_CENTER)
 //              if(calDistance(enemyInfo.armies[sum-1].BlockDR,enemyInfo.armies[sum-1].BlockUR,enemyInfo.enemy_buildings[i].BlockDR,enemyInfo.enemy_buildings[i].BlockUR)<5*BLOCKSIDELENGTH){
 //                  Btarget[t]=enemyInfo.enemy_buildings[i].SN;t++;
 //              }
 //              else */
 //              if(enemyInfo.enemy_buildings[i].Type==BUILDING_ARROWTOWER){
 //                  temp=calDistance(enemyInfo.armies[Hero].BlockDR,enemyInfo.armies[Hero].BlockUR,enemyInfo.enemy_buildings[i].BlockDR,enemyInfo.enemy_buildings[i].BlockUR);
 //                  if(temp<10*BLOCKSIDELENGTH){
 //                      if(temp<min){
 //                          min=temp;
 //                          Btarget[0]=enemyInfo.enemy_buildings[i].SN;
 //                          t++;
 //                          /*qDebug()<<target;*/
 //                      }
 //                  }
 //              }
 //              else if(enemyInfo.enemy_buildings[i].Type==BUILDING_CENTER){
 //                  if(calDistance(enemyInfo.armies[Hero].BlockDR,enemyInfo.armies[Hero].BlockUR,enemyInfo.enemy_buildings[i].BlockDR,enemyInfo.enemy_buildings[i].BlockUR)<8*BLOCKSIDELENGTH){
 //                      Btarget[1]=enemyInfo.enemy_buildings[i].SN;t++;
 //                  }
 //              }
 //          }
 //      }
 //      if(mode==3&&(s+t)!=0&&Hero!=-1&&armystate[Hero]!=CHASE){
 //          armystate[Hero]=ATTACK;
 //      }
 //      if(mode==3&&armystate[Hero]==WAITING&&Hero!=-1){
 //          for(int i=0;i<enemyInfo.armies.size();i++){
 //              if(armystate[i]==WAITING){
 //              HumanMove(enemyInfo.armies[i].SN,pos_L[49]*BLOCKSIDELENGTH,pos_U[49]*BLOCKSIDELENGTH);
 //              armystate[i]=ATTACK;
 //              }}

 //      }
 //      if(mode==3&&(armystate[Hero]==ATTACK||armystate[Hero]==CHASE)&&Hero!=-1){
 //          for(int i=0;i<enemyInfo.armies.size();i++){
 //              if(armystate[i]==WAITING){
 //                 armystate[i]=ATTACK;
 //              }}
 //      }
 //      if(mode==3)
 //      for(int i=0;i<enemyInfo.armies.size();i++){
 //          if(armystate[i]==ATTACK&&Lock[i]==0&&s>0)
 //          if(enemyInfo.armies[i].Sort==0||enemyInfo.armies[i].Sort==2||enemyInfo.armies[i].Sort==3){
 //              ChasingLock[i]=1;
 //              HumanAction(enemyInfo.armies[i].SN,Atarget[0]);
 //              Lock[i]=1;
 //              timer[i]=g_frame;
 ////              qDebug()<<"sus1"<<g_frame;
 //          }
 //          if(armystate[i]==ATTACK&&Lock[i]==0&&s==0&&t>0)
 //             {     if(Btarget[0]!=0){
 //                   HumanAction(enemyInfo.armies[i].SN,Btarget[0]);
 //                   Lock[i]=1;
 //                   timer[i]=g_frame;
 //               Attacktimer=g_frame;}
 //              else if(Btarget[0]==0){HumanAction(enemyInfo.armies[i].SN,Btarget[1]);
 //              Lock[i]=1;
 //              timer[i]=g_frame;
 //              Attacktimer=g_frame;}
 //              }
 //          else if(armystate[i]==ATTACK&&Lock[i]==0&&t>0)
 //          if(enemyInfo.armies[i].Sort==1||enemyInfo.armies[i].Sort>=4){
 //              if(Btarget[0]!=0){
 //                                 HumanAction(enemyInfo.armies[i].SN,Btarget[0]);
 //                                 Lock[i]=1;
 //                                 timer[i]=g_frame;
 //               Attacktimer=g_frame;}
 //                            else if(Btarget[0]==0){HumanAction(enemyInfo.armies[i].SN,Btarget[1]);
 //                            Lock[i]=1;
 //                            timer[i]=g_frame;
 //               Attacktimer=g_frame;}
 //          }
 //      }
 //      if(mode==3)
 //          for(int i=0;i<enemyInfo.armies.size();i++){
 //              if(armystate[i]==ATTACK&&Lock[i]==1){
 //    //              qDebug()<<timer[i]<<g_frame<<enemyInfo.armies[i].NowState;
 //                  if ((g_frame - timer[i])>=75 && (enemyInfo.armies[i].NowState!=8&&enemyInfo.armies[i].NowState!=10/*&&enemyInfo.armies[i].NowState!=14*/)) {
 //                      Lock[i]=0;
 //                      timer[i]=0;
 //                      armystate[i]=WAITING;
 //                                  }
 //              }
 //          }
 //      if(mode==3)
 //      for(int i=0;i<enemyInfo.armies.size();i++)
 ////      qDebug()<<armystate[i]<<Lock[i]<<ChasingLock[i]<<enemyInfo.armies[i].NowState<<enemyInfo.armies[i].Sort<<g_frame;
 //      if(mode==3){
 //          for(int i=0;i<enemyInfo.armies.size();i++){
 //              if(armystate[i]!=ATTACK&&armystate[i]!=CHASE){
 //                  armystate[i]=ATTACK;
 //              }
 //          }
 //      }
 //      if(mode==3){
 //          for(int i=0;i<enemyInfo.armies.size();i++){
 //          if(Lock[i]==0){
 //          if(enemyInfo.armies[i].Sort==2){
 ////              ChasingLock[i]=1;
 //              int tar=target[2];
 //              if(tar==151)
 //              armystate[i]=WAITING;
 //              else if(tar>=100&&enemyInfo.enemy_buildings.size()!=0)
 //                  HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_buildings[tar-100].SN);
 //              else if(tar>=50&&enemyInfo.enemy_armies.size()!=0 ) HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[tar-50].SN);
 //          if(tar!=151){    Lock[i]=1;
 //              timer[i]=g_frame;
 //              }}
 //          else if(enemyInfo.armies[i].Sort==1)
 //          {
 //              int tar=target[1];
 //              if(tar==151)
 //              armystate[i]=WAITING;
 //              else if(tar>=100&&enemyInfo.enemy_buildings.size()!=0)
 //                  HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_buildings[tar-100].SN);
 //              else  if(tar>=50&&enemyInfo.enemy_armies.size()!=0 ) HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[tar-50].SN);
 //              if(tar!=151){    Lock[i]=1;
 //                  timer[i]=g_frame;
 //          }}
 //          else if(enemyInfo.armies[i].Sort==3)
 //          {
 //              int tar=target[3];
 //              if(tar==151)
 //              armystate[i]=WAITING;
 //              else if(tar>=100&&enemyInfo.enemy_buildings.size()!=0)
 //              HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_buildings[tar-100].SN);
 //              else  if(tar>=50&&enemyInfo.enemy_armies.size()!=0 ) HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[tar-50].SN);
 //              if(tar!=151){    Lock[i]=1;
 //                  timer[i]=g_frame;
 //          }}
 //          else if(enemyInfo.armies[i].Sort==0){

 //              int tar=target[0];
 //              if(tar==151)
 //              armystate[i]=WAITING;
 //              else if(tar>=100&&enemyInfo.enemy_buildings.size()!=0)
 //                  HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_buildings[tar-100].SN);

 //              else  if(tar>=50&&enemyInfo.enemy_armies.size()!=0 ) HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[tar-50].SN);
 //              if(tar!=151){    Lock[i]=1;
 //                  timer[i]=g_frame;
 //          }
 //  }}
 //          if ((g_frame - timer[i]) >= 15 && enemyInfo.armies[i].NowState!=8&&enemyInfo.armies[i].NowState!=10) {
 //              Lock[i]=0;
 //              timer[i] = 0;
 //              armystate[i]=WAITING;
 //                          }
 //      }}
 //    for(int i=0;i<5;i++) target[i]=151;
 //    Atarget[0]=0;
 ////    if(g_frame-Attacktimer>100){
 ////        if(Btarget[0]!=0&&tempBtarget[0]==Btarget[0])
 ////            for(int i=0;i<enemyInfo.armies.size();i++){
 ////                if(enemyInfo.armies[i].Sort==1||enemyInfo.armies[i]>=4){
 ////                    target[1]=seek(1,2);
 ////                    int tar=target[1];
 ////                    if(tar==151) break;
 ////                    else
 ////                }
 ////            }
 ////    }
 ////    tempBtarget[0]=Btarget[0];
 ////    tempBtarget[1]=Btarget[1];
 //    Btarget[0]=0;
 //    Btarget[1]=0;
 //    return;
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
