#include "EnemyAI.h"
tagGame tagEnemyGame;
ins EnemyIns;
/*##########DO NOT EDIT ABOVE##########*/
#define WAITING -1
#define CHASE 1
#define ATTACK 2
#define DESTROY 3
#define COUNTER 4
#define RETREAT 5
#define AROUND 6
#define MODE1 15
#define MODE2 15000
#define MODE3 22500
#define MODE4 30000
tagInfo enemyInfo;
static int mode = -3;
static double pos_L[50] = {0};
static double pos_U[50] = {0};
static int armystate[50] = {0};
static int Blood[50] = {0};
static int Lock[50];
static int timer[50] = {0};
static int ChasingLock[50]={0};
static int sum;
static int s;
static int Pathn;
static int Pathset=1;
static int t;
static int tar[50];
static bool ifretreat[50];
static long Chasingtar[50];
static int Pathnum=0;
static int Path[100][2];
static int point1[15][2]={{48,10},{56,11},{57,20},{55,30},{50,40},{44,50},{35,59},{27,69},{28,76},{32,82},{27,90},{22,100},{19,110},{15,118},{5,121}};
static int AroundLock[50];
static long visible[100];
static void seekenemy(){
    int n=0;
    for(int i=0;i<enemyInfo.armies.size();i++){
        for(int j=0;j<enemyInfo.enemy_farmers.size();j++){
          double temp=pow(pow(enemyInfo.armies[i].BlockDR-enemyInfo.enemy_farmers[0].BlockDR, 2) + pow(enemyInfo.armies[i].BlockUR-enemyInfo.enemy_farmers[0].BlockUR, 2), 0.5);
          if(temp<3.0){
              visible[n]=enemyInfo.enemy_farmers[i].SN;
              n++;
              continue;
          }
        }
    }
}
static void aroundseekenemy(int i){
    if(armystate[i]==AROUND&&enemyInfo.armies[i].Sort==7){
        for(int j=0;j<enemyInfo.enemy_farmers.size();j++){
          double temp=pow(pow(enemyInfo.armies[i].BlockDR-enemyInfo.enemy_farmers[0].BlockDR, 2) + pow(enemyInfo.armies[i].BlockUR-enemyInfo.enemy_farmers[0].BlockUR, 2), 0.5);
          if(temp<3.0){
           mode=5;
          }
        }
    }
    else if(armystate[i]==AROUND&&enemyInfo.armies[i].Sort!=7){
        for(int j=0;j<enemyInfo.enemy_farmers.size();j++){
          double temp=pow(pow(enemyInfo.armies[i].BlockDR-enemyInfo.enemy_farmers[0].BlockDR, 2) + pow(enemyInfo.armies[i].BlockUR-enemyInfo.enemy_farmers[0].BlockUR, 2), 0.5);
          if(temp<3.0){
           mode=6;
          }
}
}
}
static int seek(int Sort,int number){
    double temp=0;
    int SN=0;
    long min=1000000;
    int target=151;
    for(int i=0;i<enemyInfo.armies.size();i++){
        if(enemyInfo.armies[i].Sort==Sort)
            SN=i;
    }
    if(number==1){
        if(enemyInfo.enemy_farmers.size()!=0)
        for(int i=0;i<enemyInfo.enemy_farmers.size();i++){
            temp=pow(pow(enemyInfo.armies[SN].BlockDR-enemyInfo.enemy_farmers[i].BlockDR, 2) + pow(enemyInfo.armies[SN].BlockUR-enemyInfo.enemy_farmers[i].BlockUR, 2), 0.5);
            if (temp<min) {min=temp;
            target=i;
           }
        }return target;
   }
    else if(number==2){
        if(enemyInfo.enemy_armies.size()!=0){
                    for(int i=0;i<enemyInfo.enemy_armies.size();i++){
                        temp=pow(pow(enemyInfo.armies[SN].BlockDR-enemyInfo.enemy_armies[i].BlockDR, 2) + pow(enemyInfo.armies[SN].BlockUR-enemyInfo.enemy_armies[i].BlockUR, 2), 0.5);
                        if (temp<min) {min=temp;
                        target=i+50;
                }
    }
        }return target;
    }
    else if(number==3){
        if(enemyInfo.enemy_buildings.size()!=0){
        for(int i=0;i<enemyInfo.enemy_buildings.size();i++){
            if(enemyInfo.enemy_buildings[i].Type==BUILDING_ARROWTOWER){
            temp=pow(pow(enemyInfo.armies[SN].BlockDR-enemyInfo.enemy_buildings[i].BlockDR, 2) + pow(enemyInfo.armies[SN].BlockUR-enemyInfo.enemy_buildings[i].BlockUR, 2), 0.5);
            if(temp<min){
                min=temp;target=i+100;/*qDebug()<<target;*/
            }}
        }
        if(target==151){
            for(int i=0;i<enemyInfo.enemy_buildings.size();i++){
                    temp=pow(pow(enemyInfo.armies[SN].BlockDR-enemyInfo.enemy_buildings[i].BlockDR, 2) + pow(enemyInfo.armies[SN].BlockUR-enemyInfo.enemy_buildings[i].BlockUR, 2), 0.5);
                    if (temp<min) {min=temp;
                 target=i+100;
                    }}
            return target;
        }else return target;
    }}}
static int Pathfind(int a,int b){
    if(a==2&&b==50){
        for(int i=0;i<15;i++){
            Path[i][0]=point1[i][0];
            Path[i][1]=point1[i][1];
       }
    }
//    else if(){

//    }
     return 15;
    }

void EnemyAI::processData() {
     /*##########YOUR CODE BEGINS HERE##########*/
     enemyInfo=getInfo();
     s=0;
     t=0;
     if(g_frame==15){
         Pathnum=Pathfind(enemyInfo.buildings[0].BlockDR,enemyInfo.buildings[0].BlockUR);
     }


     //军队数据初始化
     if(g_frame==15){
         sum=enemyInfo.armies.size();
         for(int i=0;i<enemyInfo.armies.size();i++){
             Chasingtar[i]=0;
             Blood[i]=enemyInfo.armies[i].Blood;
             timer[i]=0;
             Lock[i]=0;
             ifretreat[i]=false;
             armystate[i]=WAITING;
 //            qDebug()<<Blood[i];
             pos_L[i]=enemyInfo.armies[i].DR;
             pos_U[i]=enemyInfo.armies[i].UR;
             ChasingLock[i]=0;
             tar[i]=0;
         }
         for(int i=0;i<enemyInfo.enemy_buildings.size();i++){
             if(enemyInfo.enemy_buildings[i].Type==BUILDING_CENTER)
               {  pos_L[49]=enemyInfo.enemy_buildings[i].BlockDR;
                  pos_U[49]=enemyInfo.enemy_buildings[i].BlockUR;
             }
         }
     }
     //更新波数
     if(g_frame==MODE4) mode=4;
     else if(g_frame==MODE3) mode=3;
     else if(g_frame==MODE2) mode=2;
     else if(g_frame==MODE1) {mode=1;
     }
     //巡逻进攻
     if(mode==5){
         for(int i=0;i<enemyInfo.enemy_armies.size();i++){
             if(enemyInfo.enemy_armies[i].Sort==7)
             armystate[i]==ATTACK;
         }
     }
     if(mode==6){
         for(int i=0;i<enemyInfo.enemy_armies.size();i++){
             if(enemyInfo.enemy_armies[i].Sort!=7)
             armystate[i]==ATTACK;
         }
     }

     if(g_frame%50==0){
         seekenemy();
     }
     double mindis=9999;
     //陆地巡逻
     if(g_frame==MODE1){
     for(int i=0;i<enemyInfo.armies.size();i++){
         if(enemyInfo.armies[i].Sort==3){
             armystate[i]=AROUND;
             }
     }
     for(int i=enemyInfo.armies.size();i>0;i--){
         if(enemyInfo.armies[i].Sort==7){
             armystate[i]=AROUND;
             break;
         }
     }
     }
     for(int i=0;i<enemyInfo.armies.size();i++){
         int a=0;
     if(mode>=1&&mode<4&&armystate[i]==AROUND&&AroundLock[i]==0&&enemyInfo.armies[i].Sort==3){
         timer[i]=g_frame;
         if(a%2==0)
         HumanMove(enemyInfo.armies[i].SN,Path[Pathn][0]*35,Path[Pathn][1]*35);
         else HumanMove(enemyInfo.armies[i].SN,Path[Pathn][0]*35-1,Path[Pathn][1]*35-1);
 //        else HumanMove(enemyInfo.armies[i].SN,Path[Pathn][0]*33,Path[Pathn][1]*33);
         AroundLock[i]=1;
         a++;
     }
     else if(mode>=1&&mode<4&&armystate[i]==AROUND&&AroundLock[i]==0&&enemyInfo.armies[i].Sort==7){
          timer[i]=g_frame;
          HumanMove(enemyInfo.armies[i].SN,(Path[Pathn][0]+4)*37,(Path[Pathn][1]+4)*37);
          AroundLock[i]=1;
     }
     else if(mode>=1&&mode<4&&armystate[i]==AROUND&&AroundLock[i]==1){
         if(g_frame-timer[i]>400){
             AroundLock[i]=0;
             Pathn+=Pathset;
             if(Pathn==15||Pathn==0){
             Pathset=-1*Pathset;
             }
         }
     }}
     for(int i=0;i<enemyInfo.armies.size();i++){
         if(armystate[i]==AROUND)aroundseekenemy(i);
     }










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
//     //强制总攻
// //    for(int i=0;i<enemyInfo.armies.size();i++)
// //    qDebug()<<calDistance(enemyInfo.armies[i].BlockDR,enemyInfo.armies[i].BlockUR,pos_L[49],pos_U[49])<<g_frame;
//     //撤退检查
// //    if(mode<0){
// //        for(int i=0;i<ATT2;i++){
// //            if(armystate[i]==RETREAT){
// //                if(enemyInfo.armies[i].NowState==MOVEOBJECT_STATE_STAND){
// //                    armystate[i]==WAITING;
// //                }
// //            }
// //        }
// //    }
// //    qDebug()<<armystate[0];
//     //根据波数启动军队,选定各兵种目标
// //    if(mode==1){
// //        for(int i=0;i<enemyInfo.armies.size();i++){
// //            if(armystate[i]==WAITING||armystate[i]==RETREAT&&enemyInfo.armies[i].Sort==3){
// //                armystate[i]==AROUND;
// //            }
// //        }
// //    }
// //    else if(mode==2){
// //        for(int i=0;i<ATT2;i++){
// //            if(armystate[i]==WAITING||armystate[i]==RETREAT){ armystate[i]=ATTACK;
// //                if(enemyInfo.armies[i].Sort==1){
// //                    target[1]=seek(1,3);
// //                    if(target[1]>150) target[1]=seek(1,2);
// //                    if(target[1]>150) target[1]=seek(1,1);
// //                }
// //                else if(enemyInfo.armies[i].Sort==2){
// //                    target[2]=seek(2,1);
// //                    if(target[2]>150) target[2]=seek(2,2);
// //                    if(target[2]>150) target[2]=seek(2,3);
// //                }
// //                else if(enemyInfo.armies[i].Sort==3){
// //                    target[3]=seek(3,1);
// //                    if(target[3]>150) target[3]=seek(3,2);
// //                    if(target[3]>150) target[3]=seek(3,3);
// //                }
// //                else if(enemyInfo.armies[i].Sort==0){
// //                    target[0]=seek(0,1);
// //                    if(target[3]>150) target[3]=seek(0,2);
// //                    if(target[3]>150) target[3]=seek(0,3);
// //                }
// //            }}
// //    }
// //    else if(mode==3){
// //        for(int i=0;i<enemyInfo.armies.size();i++){
// //            if(enemyInfo.armies[i].Sort==1){
// //                target[1]=seek(1,2);
// //                if(target[1]>150) target[1]=seek(1,3);
// //            }
// //            else if(enemyInfo.armies[i].Sort==2){
// //                target[2]=seek(2,2);
// //                if(target[2]>150) target[2]=seek(2,3);
// //            }
// //            else if(enemyInfo.armies[i].Sort==3){
// //                target[3]=seek(3,2);
// //                if(target[3]>150) target[3]=seek(3,3);
// //            }
// //            else if(enemyInfo.armies[i].Sort==0){
// //                target[0]=seek(0,2);
// //                if(target[0]>150) target[0]=seek(0,3);
// //            }
// //        }
// //    }
// ////    else if(mode==3){
// ////        for(int i=0;i<enemyInfo.armies.size();i++){
// ////            if(armystate[i]==CHASE||armystate[i]==WAITING) {armystate[i]=ATTACK;
// ////                target[4]=seek(enemyInfo.armies[Hero].Sort,3);
// ////                if(target[4]>150) target[2]=seek(enemyInfo.armies[Hero].Sort,2);
// ////                if(target[4]>150)target[2]=seek(enemyInfo.armies[Hero].Sort,1);
// ////            }
// ////        }

// ////    qDebug()<<armystate[0];
// //    // 自动反击
// ////    if(g_frame>15){
// ////    for(int i=0;i<enemyInfo.armies.size();i++){
// ////           if(enemyInfo.armies[i].Blood!=Blood[i]&&armystate[i]!=CHASE&&ChasingLock[i]==0){
// ////               for(int j=0;j<enemyInfo.enemy_armies.size();j++){
// ////                   if(enemyInfo.enemy_armies[j].WorkObjectSN==enemyInfo.armies[i].SN){
// ////                       HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[j].SN);
// ////                       armystate[i]=CHASE;
// ////                       Blood[i]=enemyInfo.armies[i].Blood;
// ////                       chasestart_L[i]=enemyInfo.armies[i].DR;
// ////                       chasestart_U[i]=enemyInfo.armies[i].UR;
// ////                       timer[i]=g_frame;
// ////                       ChasingLock[i]=1;
// ////                   }
// ////               }}
// ////               if(enemyInfo.armies[i].Blood!=Blood[i]&&armystate[i]!=CHASE&&ChasingLock[i]==0){
// ////                   for(int j=0;j<enemyInfo.enemy_farmers.size();j++){
// ////                       if(enemyInfo.enemy_farmers[j].WorkObjectSN==enemyInfo.armies[i].SN){
// ////                           HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_farmers[j].SN);
// ////                           armystate[i]=CHASE;
// ////                           Blood[i]=enemyInfo.armies[i].Blood;
// ////                           chasestart_L[i]=enemyInfo.armies[i].DR;
// ////                           chasestart_U[i]=enemyInfo.armies[i].UR;
// ////                           timer[i]=g_frame;
// ////                           ChasingLock[i]=1;
// ////                       }
// ////               }
// ////           }
// ////               if(enemyInfo.armies[i].Blood!=Blood[i]&&armystate[i]!=CHASE&&ChasingLock[i]==0){
// ////                  for(int j=0;j<enemyInfo.enemy_buildings.size();j++)
// ////                       if(enemyInfo.enemy_buildings[j].Type==BUILDING_ARROWTOWER){
// ////                           if(calDistance(enemyInfo.armies[Hero].BlockDR,enemyInfo.armies[Hero].BlockUR,enemyInfo.enemy_buildings[i].BlockDR,enemyInfo.enemy_buildings[i].BlockUR)<5*BLOCKSIDELENGTH)
// ////                           {
// ////                           HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_buildings[j].SN);
// ////                           armystate[i]=DESTROY;
// ////                           Blood[i]=enemyInfo.armies[i].Blood;
// ////                           chasestart_L[i]=enemyInfo.armies[i].DR;
// ////                           chasestart_U[i]=enemyInfo.armies[i].UR;
// ////                           timer[i]=g_frame;
// ////                           ChasingLock[i]=1;}
// ////                       }
// ////                       }

// ////               }
// ////           }
// //    if(mode<0){
// //        for(int i=0;i<enemyInfo.armies.size();i++){
// //            if(armystate[i]==WAITING||armystate[i]==RETREAT){
// //                for(int j=0;j<enemyInfo.armies.size();j++){
// //                    int temp=calDistance(enemyInfo.armies[i].DR,enemyInfo.armies[i].UR,enemyInfo.armies[j].DR,enemyInfo.armies[j].UR);
// //                    if(armystate[j]==CHASE&&temp<150){
// //                        armystate[i]=CHASE;
// //                        qDebug()<<enemyInfo.armies[j].WorkObjectSN;
// //                        HumanAction(enemyInfo.armies[i].SN,enemyInfo.armies[j].WorkObjectSN);
// //                        chasestart_L[i]=enemyInfo.armies[i].DR;
// //                        chasestart_U[i]=enemyInfo.armies[i].UR;
// //                        timer[i]=g_frame;
// //                        ChasingLock[i]=1;
// //                    }
// //                }
// //            }
// //        }
// //    }
// ////    qDebug()<<armystate[i]<<Lock[i]<<ChasingLock[i]<<enemyInfo.armies[i].NowState<<enemyInfo.armies[i].Sort<<g_frame;
// //        //反击检查
// ////    qDebug()<<armystate[0]<<ChasingLock[0]<<armystate[1]<<ChasingLock[1]<<g_frame;
// //    for(int i=0;i<enemyInfo.armies.size();i++){
// //           if(ChasingLock[i]!=0&&(armystate[i]!=CHASE&&armystate[i]!=DESTROY))ChasingLock[i]=0;
// //           if(armystate[i]==CHASE){

// //               if(calDistance(enemyInfo.armies[i].DR,enemyInfo.armies[i].UR,chasestart_L[i],chasestart_U[i])>=200)
// //               {
// //                   HumanMove(enemyInfo.armies[i].SN,chasestart_L[i],chasestart_U[i]);
// //                   timer[i] = 0;
// //                   ChasingLock[i]=0;
// //                   armystate[i]=WAITING;
// //               }
// //               if((g_frame - timer[i]) >= 30 && enemyInfo.armies[i].NowState !=8&&enemyInfo.armies[i].NowState!=10){
// //                   timer[i] = 0;
// //                   ChasingLock[i]=0;
// //                   armystate[i]=WAITING;
// //               }}
// //               if(armystate[i]==DESTROY){
// //                   if((g_frame - timer[i]) >= 15 && enemyInfo.armies[i].NowState == MOVEOBJECT_STATE_STAND){
// //                       timer[i] = 0;
// //                       ChasingLock[i]=0;
// //                       armystate[i]=WAITING;
// //                   }
// //               }
// //           }
// //      //攻击状态
// //      for(int i=0;i<enemyInfo.armies.size();i++)
// //      if(armystate[i]==ATTACK){
// //            if(mode==1){
// //            if(Lock[i]==0&&i<ATT1){
// //                     nowState_Army=enemyInfo.armies[i].NowState;
// //                     if(nowState_Army==MOVEOBJECT_STATE_STAND)
// //                     {

// //                         int tar=target[0];
// ////                         qDebug()<<tar<<"攻击对象";
// //                         if(tar==151)
// //                         armystate[i]=WAITING;
// //                         else if(enemyInfo.enemy_farmers.size()!=0) HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_farmers[tar].SN);
// //                         if(tar!=151){    Lock[i]=1;
// //                             timer[i]=g_frame;
// //                         }

// //           }
// //            }}
// //            else if(mode==2){
// //                nowState_Army=enemyInfo.armies[i].NowState;
// //                if(nowState_Army==MOVEOBJECT_STATE_STAND&&i<ATT2&&Lock[i]==0){
// //                if(enemyInfo.armies[i].Sort==2){
// //                    ChasingLock[i]=1;
// //                    int tar=target[2];
// //                    if(tar==151)
// //                    armystate[i]=WAITING;
// //                    else if(tar>=100&&enemyInfo.enemy_buildings.size()!=0)
// //                        HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_buildings[tar-100].SN);
// //                    else if(tar>=50&&enemyInfo.enemy_armies.size()!=0 ) HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[tar-50].SN);
// //                    else if(enemyInfo.enemy_farmers.size()!=0) HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_farmers[tar].SN);
// //                if(tar!=151){    Lock[i]=1;
// //                    timer[i]=g_frame;
// //                    }}
// //                else if(enemyInfo.armies[i].Sort==1)
// //                {
// //                    int tar=target[1];
// //                    if(tar==151)
// //                    armystate[i]=WAITING;
// //                    else if(tar>=100&&enemyInfo.enemy_buildings.size()!=0)
// //                        HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_buildings[tar-100].SN);
// //                    else  if(tar>=50&&enemyInfo.enemy_armies.size()!=0 ) HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[tar-50].SN);
// //                    else if(enemyInfo.enemy_farmers.size()!=0)HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_farmers[tar].SN);
// //                    if(tar!=151){    Lock[i]=1;
// //                        timer[i]=g_frame;
// //                }}
// //                else if(enemyInfo.armies[i].Sort==3)
// //                {
// //                    int tar=target[3];
// //                    if(tar==151)
// //                    armystate[i]=WAITING;
// //                    else if(tar>=100&&enemyInfo.enemy_buildings.size()!=0)
// //                    HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_buildings[tar-100].SN);
// //                    else  if(tar>=50&&enemyInfo.enemy_armies.size()!=0 ) HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[tar-50].SN);
// //                    else if(enemyInfo.enemy_farmers.size()!=0)HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_farmers[tar].SN);
// //                    if(tar!=151){    Lock[i]=1;
// //                        timer[i]=g_frame;
// //                }}
// //                else if(enemyInfo.armies[i].Sort==0){

// //                    int tar=target[0];
// //                    if(tar==151)
// //                    armystate[i]=WAITING;
// //                    else if(tar>=100&&enemyInfo.enemy_buildings.size()!=0)
// //                        HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_buildings[tar-100].SN);

// //                    else  if(tar>=50&&enemyInfo.enemy_armies.size()!=0 ) HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[tar-50].SN);
// //                    else if(enemyInfo.enemy_farmers.size()!=0) HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_farmers[tar].SN);
// //                    if(tar!=151){    Lock[i]=1;
// //                        timer[i]=g_frame;
// //                }
// //        }}}
// ////            else if(mode==3){
// ////                if(Lock[i]==0){
// ////                          nowState_Army=enemyInfo.armies[i].NowState;
// ////                          if(nowState_Army==MOVEOBJECT_STATE_STAND)
// ////                          {   int tar=target[4];
// ////                              if(tar==151)
// ////                              armystate[i]==WAITING;
// ////                              else if(tar>=100&&enemyInfo.enemy_buildings.size()!=0)
// ////                              HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_buildings[tar-100].SN);
// ////                              else if(tar>=50&&enemyInfo.enemy_armies.size()!=0 ) HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[tar-50].SN);
// ////                              else if(enemyInfo.enemy_farmers.size()!=0)HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_farmers[tar].SN);
// ////                              if(tar!=151){    Lock[i]=1;
// ////                                  timer[i]=g_frame;
// ////                            }
// ////            }
// ////        }
// ////               }

// //            if ((g_frame - timer[i]) >= 15 && enemyInfo.armies[i].NowState ==MOVEOBJECT_STATE_STAND) {
// //                Lock[i]=0;
// //                timer[i] = 0;
// //                armystate[i]=WAITING;
// //                            }
// //       }

// ////      if(mode==3){
// //////          qDebug()<<armystate[Hero]<<g_frame;
// //////          qDebug()<<armystate[Hero-1]<<g_frame;
// ////          for(int i=0;i<enemyInfo.enemy_armies.size();i++){
// ////              if(calDistance(enemyInfo.armies[Hero].BlockDR,enemyInfo.armies[Hero].BlockUR,enemyInfo.enemy_armies[i].BlockDR,enemyInfo.enemy_armies[i].BlockUR)<8*BLOCKSIDELENGTH){
// ////                  Atarget[s]=enemyInfo.enemy_armies[i].SN;s++;
// ////              }
// ////          }
// ////      }
// ////      if(mode==3){
// ////          int min=12000;
// ////          int temp=0;
// ////          for(int i=0;i<enemyInfo.enemy_buildings.size();i++){
// ////              /*if(enemyInfo.enemy_buildings[i].Type!=BUILDING_ARROWTOWER&&enemyInfo.enemy_buildings[i].Type!=BUILDING_CENTER)
// ////              if(calDistance(enemyInfo.armies[sum-1].BlockDR,enemyInfo.armies[sum-1].BlockUR,enemyInfo.enemy_buildings[i].BlockDR,enemyInfo.enemy_buildings[i].BlockUR)<5*BLOCKSIDELENGTH){
// ////                  Btarget[t]=enemyInfo.enemy_buildings[i].SN;t++;
// ////              }
// ////              else */
// ////              if(enemyInfo.enemy_buildings[i].Type==BUILDING_ARROWTOWER){
// ////                  temp=calDistance(enemyInfo.armies[Hero].BlockDR,enemyInfo.armies[Hero].BlockUR,enemyInfo.enemy_buildings[i].BlockDR,enemyInfo.enemy_buildings[i].BlockUR);
// ////                  if(temp<10*BLOCKSIDELENGTH){
// ////                      if(temp<min){
// ////                          min=temp;
// ////                          Btarget[0]=enemyInfo.enemy_buildings[i].SN;
// ////                          t++;
// ////                          /*qDebug()<<target;*/
// ////                      }
// ////                  }
// ////              }
// ////              else if(enemyInfo.enemy_buildings[i].Type==BUILDING_CENTER){
// ////                  if(calDistance(enemyInfo.armies[Hero].BlockDR,enemyInfo.armies[Hero].BlockUR,enemyInfo.enemy_buildings[i].BlockDR,enemyInfo.enemy_buildings[i].BlockUR)<8*BLOCKSIDELENGTH){
// ////                      Btarget[1]=enemyInfo.enemy_buildings[i].SN;t++;
// ////                  }
// ////              }
// ////          }
// ////      }
// ////      if(mode==3&&(s+t)!=0&&Hero!=-1&&armystate[Hero]!=CHASE){
// ////          armystate[Hero]=ATTACK;
// ////      }
// ////      if(mode==3&&armystate[Hero]==WAITING&&Hero!=-1){
// ////          for(int i=0;i<enemyInfo.armies.size();i++){
// ////              if(armystate[i]==WAITING){
// ////              HumanMove(enemyInfo.armies[i].SN,pos_L[49]*BLOCKSIDELENGTH,pos_U[49]*BLOCKSIDELENGTH);
// ////              armystate[i]=ATTACK;
// ////              }}

// ////      }
// ////      if(mode==3&&(armystate[Hero]==ATTACK||armystate[Hero]==CHASE)&&Hero!=-1){
// ////          for(int i=0;i<enemyInfo.armies.size();i++){
// ////              if(armystate[i]==WAITING){
// ////                 armystate[i]=ATTACK;
// ////              }}
// ////      }
// //      if(mode==3)
// //      for(int i=0;i<enemyInfo.armies.size();i++){
// //          if(armystate[i]==ATTACK&&Lock[i]==0&&s>0)
// //          if(enemyInfo.armies[i].Sort==0||enemyInfo.armies[i].Sort==2||enemyInfo.armies[i].Sort==3){
// //              ChasingLock[i]=1;
// //              HumanAction(enemyInfo.armies[i].SN,Atarget[0]);
// //              Lock[i]=1;
// //              timer[i]=g_frame;
// ////              qDebug()<<"sus1"<<g_frame;
// //          }
// //          if(armystate[i]==ATTACK&&Lock[i]==0&&s==0&&t>0)
// //             {     if(Btarget[0]!=0){
// //                   HumanAction(enemyInfo.armies[i].SN,Btarget[0]);
// //                   Lock[i]=1;
// //                   timer[i]=g_frame;
// //               Attacktimer=g_frame;}
// //              else if(Btarget[0]==0){HumanAction(enemyInfo.armies[i].SN,Btarget[1]);
// //              Lock[i]=1;
// //              timer[i]=g_frame;
// //              Attacktimer=g_frame;}
// //              }
// //          else if(armystate[i]==ATTACK&&Lock[i]==0&&t>0)
// //          if(enemyInfo.armies[i].Sort==1||enemyInfo.armies[i].Sort>=4){
// //              if(Btarget[0]!=0){
// //                                 HumanAction(enemyInfo.armies[i].SN,Btarget[0]);
// //                                 Lock[i]=1;
// //                                 timer[i]=g_frame;
// //               Attacktimer=g_frame;}
// //                            else if(Btarget[0]==0){HumanAction(enemyInfo.armies[i].SN,Btarget[1]);
// //                            Lock[i]=1;
// //                            timer[i]=g_frame;
// //               Attacktimer=g_frame;}
// //          }
// //      }
// //      if(mode==3)
// //          for(int i=0;i<enemyInfo.armies.size();i++){
// //              if(armystate[i]==ATTACK&&Lock[i]==1){
// //    //              qDebug()<<timer[i]<<g_frame<<enemyInfo.armies[i].NowState;
// //                  if ((g_frame - timer[i])>=75 && (enemyInfo.armies[i].NowState!=8&&enemyInfo.armies[i].NowState!=10/*&&enemyInfo.armies[i].NowState!=14*/)) {
// //                      Lock[i]=0;
// //                      timer[i]=0;
// //                      armystate[i]=WAITING;
// //                                  }
// //              }
// //          }
// //      if(mode==3)
// //      for(int i=0;i<enemyInfo.armies.size();i++)
// ////      qDebug()<<armystate[i]<<Lock[i]<<ChasingLock[i]<<enemyInfo.armies[i].NowState<<enemyInfo.armies[i].Sort<<g_frame;
// //      if(mode==3){
// //          for(int i=0;i<enemyInfo.armies.size();i++){
// //              if(armystate[i]!=ATTACK&&armystate[i]!=CHASE){
// //                  armystate[i]=ATTACK;
// //              }
// //          }
// //      }
// //      if(mode==3){
// //          for(int i=0;i<enemyInfo.armies.size();i++){
// //          if(Lock[i]==0){
// //          if(enemyInfo.armies[i].Sort==2){
// ////              ChasingLock[i]=1;
// //              int tar=target[2];
// //              if(tar==151)
// //              armystate[i]=WAITING;
// //              else if(tar>=100&&enemyInfo.enemy_buildings.size()!=0)
// //                  HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_buildings[tar-100].SN);
// //              else if(tar>=50&&enemyInfo.enemy_armies.size()!=0 ) HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[tar-50].SN);
// //          if(tar!=151){    Lock[i]=1;
// //              timer[i]=g_frame;
// //              }}
// //          else if(enemyInfo.armies[i].Sort==1)
// //          {
// //              int tar=target[1];
// //              if(tar==151)
// //              armystate[i]=WAITING;
// //              else if(tar>=100&&enemyInfo.enemy_buildings.size()!=0)
// //                  HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_buildings[tar-100].SN);
// //              else  if(tar>=50&&enemyInfo.enemy_armies.size()!=0 ) HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[tar-50].SN);
// //              if(tar!=151){    Lock[i]=1;
// //                  timer[i]=g_frame;
// //          }}
// //          else if(enemyInfo.armies[i].Sort==3)
// //          {
// //              int tar=target[3];
// //              if(tar==151)
// //              armystate[i]=WAITING;
// //              else if(tar>=100&&enemyInfo.enemy_buildings.size()!=0)
// //              HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_buildings[tar-100].SN);
// //              else  if(tar>=50&&enemyInfo.enemy_armies.size()!=0 ) HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[tar-50].SN);
// //              if(tar!=151){    Lock[i]=1;
// //                  timer[i]=g_frame;
// //          }}
// //          else if(enemyInfo.armies[i].Sort==0){

// //              int tar=target[0];
// //              if(tar==151)
// //              armystate[i]=WAITING;
// //              else if(tar>=100&&enemyInfo.enemy_buildings.size()!=0)
// //                  HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_buildings[tar-100].SN);

// //              else  if(tar>=50&&enemyInfo.enemy_armies.size()!=0 ) HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[tar-50].SN);
// //              if(tar!=151){    Lock[i]=1;
// //                  timer[i]=g_frame;
// //          }
// //  }}
// //          if ((g_frame - timer[i]) >= 15 && enemyInfo.armies[i].NowState!=8&&enemyInfo.armies[i].NowState!=10) {
// //              Lock[i]=0;
// //              timer[i] = 0;
// //              armystate[i]=WAITING;
// //                          }
// //      }}
// //    for(int i=0;i<5;i++) target[i]=151;
// //    Atarget[0]=0;
// ////    if(g_frame-Attacktimer>100){
// ////        if(Btarget[0]!=0&&tempBtarget[0]==Btarget[0])
// ////            for(int i=0;i<enemyInfo.armies.size();i++){
// ////                if(enemyInfo.armies[i].Sort==1||enemyInfo.armies[i]>=4){
// ////                    target[1]=seek(1,2);
// ////                    int tar=target[1];
// ////                    if(tar==151) break;
// ////                    else
// ////                }
// ////            }
// ////    }
// ////    tempBtarget[0]=Btarget[0];
// ////    tempBtarget[1]=Btarget[1];
// //    Btarget[0]=0;
// //    Btarget[1]=0;
// //    return;
//     /*###########YOUR CODE ENDS HERE###########*/
}
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
