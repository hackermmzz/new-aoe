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
tagInfo enemyInfo;
static int mode = 0;
static int target[5]={151,151,151,151,151};
static double pos_L[50] = {0};
static double pos_U[50] = {0};
static int armystate[50] = {0};
static int Blood[50] = {0};
static double chasestart_L[50] = {0};
static double chasestart_U[50] = {0};
static int Lock[50];
static int timer[50] = {0};
static int Find[50];
static int Hero=0;
static int Herotemp=0;
static int ATT1=2;
static int ATT2=8;
static int ChasingLock[17]={0};
static int sum;
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
                min=temp;target=i+100;qDebug()<<target;
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
void EnemyAI::processData() {
    /*##########YOUR CODE BEGINS HERE##########*/
    int nowState_Army=0;
    enemyInfo=getInfo();
    //军队数据初始化
    for(int i=0;i<enemyInfo.armies.size();i++){
        if(enemyInfo.armies[i].Sort>=4){
            Hero=i;
            if(Hero!=Herotemp){
                Blood[Hero]=Blood[Herotemp];
            }
        }
    }
    if(g_frame==15){
        sum=enemyInfo.armies.size();
        for(int i=0;i<enemyInfo.armies.size();i++){
            Blood[i]=enemyInfo.armies[i].Blood;
            timer[i]=0;
            Lock[i]=0;
            armystate[i]=WAITING;
//            qDebug()<<Blood[i];
            pos_L[i]=enemyInfo.armies[i].DR;
            pos_U[i]=enemyInfo.armies[i].UR;
            Find[i]=0;
            Hero=16;
            Herotemp=16;
            ChasingLock[i]=0;
        }
    }
    //更新波数
    if(g_frame==22500) mode=3;
    else if(g_frame==20000) mode=-2;
    else if(g_frame==15000) mode=2;
    else if(g_frame==12500)mode=-1;
    else if(g_frame==7500) mode=1;
    if((enemyInfo.enemy_armies.size()+enemyInfo.enemy_farmers.size())+enemyInfo.enemy_buildings.size()==0){
        mode=4;
    }
    //防止连续出兵
        if(g_frame>15&&enemyInfo.armies.size()!=sum){
            if(mode==1||mode==-1){
            int dif=(sum-enemyInfo.armies.size());
            sum=enemyInfo.armies.size();
            ATT1-=dif;
        }
            else if (mode==2||mode==-2){
                int dif=(sum-enemyInfo.armies.size());
                sum=enemyInfo.armies.size();
                ATT2-=dif;
            }
    }
    //撤退
    if(mode==-1)
        for(int i=0;i<ATT1;i++)
            if(armystate[i]!=RETREAT&&armystate[i]!=CHASE){
                armystate[i]=WAITING;
                if(calDistance(enemyInfo.armies[i].DR,enemyInfo.armies[i].UR,pos_L[i],pos_U[i])>=50){
                     HumanMove(enemyInfo.armies[i].SN,pos_L[i],pos_U[i]);
                     armystate[i]=RETREAT;
            }

            }
    if(mode==-2)
        for(int i=0;i<ATT2;i++)
        if(armystate[i]!=RETREAT&&armystate[i]!=CHASE){
            armystate[i]=WAITING;
            if(calDistance(enemyInfo.armies[i].DR,enemyInfo.armies[i].UR,pos_L[i],pos_U[i])>=50){
                 HumanMove(enemyInfo.armies[i].SN,pos_L[i],pos_U[i]);
                 armystate[i]=RETREAT;
        }

        }
    //强制总攻
    if(g_frame>=15){
    if(enemyInfo.armies[Hero].Blood!=Blood[Hero]){
        mode=3;
    }}
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
    //根据波数启动军队,选定各兵种目标
    if(mode==1){
         for(int i=0;i<ATT1;i++)
             if(armystate[i]==WAITING||armystate[i]==RETREAT){ armystate[i]=ATTACK;
                 target[0]=seek(0,1);
                 if(target[0]>150) target[0]=seek(0,2);
                 if(target[0]>150) target[0]=seek(0,3);
    }}
    else if(mode==2){
        for(int i=0;i<ATT2;i++){
            if(armystate[i]==WAITING||armystate[i]==RETREAT){ armystate[i]=ATTACK;
                if(enemyInfo.armies[i].Sort==1){
                    target[1]=seek(1,3);
                    if(target[1]>150) target[1]=seek(1,2);
                    if(target[1]>150) target[1]=seek(1,1);
                }
                else if(enemyInfo.armies[i].Sort==2){
                    target[2]=seek(2,1);
                    if(target[2]>150) target[2]=seek(2,2);
                    if(target[2]>150) target[2]=seek(2,3);
                }
                else if(enemyInfo.armies[i].Sort==3){
                    target[3]=seek(3,1);
                    if(target[3]>150) target[3]=seek(3,2);
                    if(target[3]>150) target[3]=seek(3,3);
                }
                else if(enemyInfo.armies[i].Sort==0){
                    target[0]=seek(0,1);
                    if(target[3]>150) target[3]=seek(0,2);
                    if(target[3]>150) target[3]=seek(0,3);
                }
            }}
    }
    else if(mode==3){
        for(int i=0;i<enemyInfo.armies.size();i++){
            if(armystate[i]==CHASE||armystate[i]==WAITING) {armystate[i]=ATTACK;
                target[4]=seek(enemyInfo.armies[Hero].Sort,3);
                if(target[4]>150) target[2]=seek(enemyInfo.armies[Hero].Sort,2);
                if(target[4]>150)target[2]=seek(enemyInfo.armies[Hero].Sort,1);
            }
        }
    }
//    qDebug()<<armystate[0];
    // 自动反击
    if(mode!=3&&g_frame>15){
    for(int i=0;i<enemyInfo.armies.size();i++){
           if(enemyInfo.armies[i].Blood!=Blood[i]&&armystate[i]!=CHASE&&ChasingLock[i]==0){
               for(int j=0;j<enemyInfo.enemy_armies.size();j++){
                   if(enemyInfo.enemy_armies[j].WorkObjectSN==enemyInfo.armies[i].SN){
                       HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[j].SN);
                       armystate[i]=CHASE;
                       Blood[i]=enemyInfo.armies[i].Blood;
                       chasestart_L[i]=enemyInfo.armies[i].DR;
                       chasestart_U[i]=enemyInfo.armies[i].UR;
                       timer[i]=g_frame;
                       ChasingLock[i]=1;
                   }
               }
               if(armystate[i]!=CHASE){
                   for(int j=0;j<enemyInfo.enemy_farmers.size();j++){
                       if(enemyInfo.enemy_farmers[j].WorkObjectSN==enemyInfo.armies[i].SN){
                           HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_farmers[j].SN);
                           armystate[i]=CHASE;
                           Blood[i]=enemyInfo.armies[i].Blood;
                           chasestart_L[i]=enemyInfo.armies[i].DR;
                           chasestart_U[i]=enemyInfo.armies[i].UR;
                           timer[i]=g_frame;
                           ChasingLock[i]=1;
                       }
               }
           }
               if(armystate[i]!=CHASE){
                   int targettemp=0;
                   targettemp=seek(enemyInfo.armies[i].Sort,3);
                           HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_buildings[targettemp-100].SN);
                           armystate[i]=CHASE;
                           Blood[i]=enemyInfo.armies[i].Blood;
                           chasestart_L[i]=enemyInfo.armies[i].DR;
                           chasestart_U[i]=enemyInfo.armies[i].UR;
                           timer[i]=g_frame;
                           ChasingLock[i]=1;
                       }
                   }
               }
           }
        //反击检查
    if(mode!=3)
    for(int i=0;i<enemyInfo.armies.size();i++){
           if(armystate[i]==CHASE){

               if(calDistance(enemyInfo.armies[i].DR,enemyInfo.armies[i].UR,chasestart_L[i],chasestart_U[i])>=200)
               {
                   HumanMove(enemyInfo.armies[i].SN,chasestart_L[i],chasestart_U[i]);
                   timer[i] = 0;
                   ChasingLock[i]=0;
                   armystate[i]=WAITING;
               }
               if((g_frame - timer[i]) >= 75 && enemyInfo.armies[i].NowState == MOVEOBJECT_STATE_STAND){
                   timer[i] = 0;
                   ChasingLock[i]=0;
                   armystate[i]=WAITING;
               }

           }}
      //攻击状态
      for(int i=0;i<enemyInfo.armies.size();i++)
      if(armystate[i]==ATTACK){
            if(mode==1){
            if(Lock[i]==0&&i<ATT1){

                     nowState_Army=enemyInfo.armies[i].NowState;
                     if(nowState_Army==MOVEOBJECT_STATE_STAND)
                     {

                         int tar=target[0];
                         if(tar==151)
                         armystate[i]==WAITING;
                         else if(tar>=100&&enemyInfo.enemy_buildings.size()!=0)
                             HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_buildings[tar-100].SN);
                         else  if(tar>=50&&enemyInfo.enemy_armies.size()!=0 ) HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[tar-50].SN);
                         else if(enemyInfo.enemy_farmers.size()!=0) HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_farmers[tar].SN);
                         if(tar!=151){    Lock[i]=1;
                             timer[i]=g_frame;
                         }

           }
            }}
            else if(mode==2){
                nowState_Army=enemyInfo.armies[i].NowState;
                if(nowState_Army==MOVEOBJECT_STATE_STAND&&i<ATT2&&Lock[i]==0){
                if(enemyInfo.armies[i].Sort==2){
                    ChasingLock[i]=1;
                    int tar=target[2];
                    if(tar==151)
                    armystate[i]==WAITING;
                    else if(tar>=100&&enemyInfo.enemy_buildings.size()!=0)
                        HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_buildings[tar-100].SN);
                    else if(tar>=50&&enemyInfo.enemy_armies.size()!=0 ) HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[tar-50].SN);
                    else if(enemyInfo.enemy_farmers.size()!=0) HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_farmers[tar].SN);
                if(tar!=151){    Lock[i]=1;
                    timer[i]=g_frame;
                    }}
                else if(enemyInfo.armies[i].Sort==1)
                {
                    int tar=target[1];
                    if(tar==151)
                    armystate[i]==WAITING;
                    else if(tar>=100&&enemyInfo.enemy_buildings.size()!=0)
                        HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_buildings[tar-100].SN);
                    else  if(tar>=50&&enemyInfo.enemy_armies.size()!=0 ) HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[tar-50].SN);
                    else if(enemyInfo.enemy_farmers.size()!=0)HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_farmers[tar].SN);
                    if(tar!=151){    Lock[i]=1;
                        timer[i]=g_frame;
                }}
                else if(enemyInfo.armies[i].Sort==3)
                {
                    int tar=target[3];
                    if(tar==151)
                    armystate[i]==WAITING;
                    else if(tar>=100&&enemyInfo.enemy_buildings.size()!=0)
                    HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_buildings[tar-100].SN);
                    else  if(tar>=50&&enemyInfo.enemy_armies.size()!=0 ) HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[tar-50].SN);
                    else if(enemyInfo.enemy_farmers.size()!=0)HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_farmers[tar].SN);
                    if(tar!=151){    Lock[i]=1;
                        timer[i]=g_frame;
                }}
                else if(enemyInfo.armies[i].Sort==0){

                    int tar=target[0];
                    if(tar==151)
                    armystate[i]==WAITING;
                    else if(tar>=100&&enemyInfo.enemy_buildings.size()!=0)
                        HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_buildings[tar-100].SN);

                    else  if(tar>=50&&enemyInfo.enemy_armies.size()!=0 ) HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[tar-50].SN);
                    else if(enemyInfo.enemy_farmers.size()!=0) HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_farmers[tar].SN);
                    if(tar!=151){    Lock[i]=1;
                        timer[i]=g_frame;
                }
        }}}
            else if(mode==3){
                if(Lock[i]==0){
                          nowState_Army=enemyInfo.armies[i].NowState;
                          if(nowState_Army==MOVEOBJECT_STATE_STAND)
                          {   int tar=target[4];
                              if(tar==151)
                              armystate[i]==WAITING;
                              else if(tar>=100&&enemyInfo.enemy_buildings.size()!=0)
                              HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_buildings[tar-100].SN);
                              else if(tar>=50&&enemyInfo.enemy_armies.size()!=0 ) HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[tar-50].SN);
                              else if(enemyInfo.enemy_farmers.size()!=0)HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_farmers[tar].SN);
                              if(tar!=151){    Lock[i]=1;
                                  timer[i]=g_frame;
                            }
            }
        }
               }

            if ((g_frame - timer[i]) >= 15 && enemyInfo.armies[i].NowState ==MOVEOBJECT_STATE_STAND) {
                Lock[i]=0;
                timer[i] = 0;
                armystate[i]=WAITING;
                            }
       }

// 线性化进攻
//    if(mode==1){
//        qDebug()<<num;
//        if(g_frame>=5000){
//            for(int i = 0 ;i<=1; i++)
//                {
//                    nowState_Army=enemyInfo.armies[i].NowState;
//                    if(nowState_Army==MOVEOBJECT_STATE_STAND&&armystate[i]==WAITING)
//                    {HumanMove(enemyInfo.armies[i].SN,pos_L[i],pos_U[i]);
//                        armystate[i]==ATTACK;
////                        qDebug()<<"success";

//        }}}
//        else for(int i = 0 ;i<=1; i++)
//            {
//                nowState_Army=enemyInfo.armies[i].NowState;
//                if(nowState_Army==MOVEOBJECT_STATE_STAND)
//                {
//                    HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_farmers[0].SN);
//                num++;
////                qDebug()<<"success";
//                if(nowState_Army==HUMAN_STATE_IDLE || nowState_Army==HUMAN_STATE_STOP)
//                {HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_farmers[0].SN);

//}
//            }}}
//        else if(mode==2){
//        if(enemyInfo.enemy_armies.size()==0){
//            for(int i = 0 ;i<=7; i++)
//                {
//                    nowState_Army=enemyInfo.armies[i].NowState;
//                    if(nowState_Army==MOVEOBJECT_STATE_STAND )
//                    {HumanMove(enemyInfo.armies[i].SN,pos_L[i],pos_U[i]);
//                        qDebug()<<"success";
//        }}}
//        else for(int i=0;i<=7;i++){
//                nowState_Army=enemyInfo.armies[i].NowState;
//                if(nowState_Army==MOVEOBJECT_STATE_STAND )
//                {HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[0].SN);
//                }}
//        }
//        else if(mode==3){
//        for(int i=0;i<enemyInfo.enemy_buildings.size()&&find!=1;i++){
//            buildtype=enemyInfo.enemy_buildings[i].Type;
//            if(buildtype==BUILDING_ARROWTOWER)
//               { targetBuilding=i;
//                 find=1;
//                }
//              for(int i=3;i<enemyInfo.enemy_armies.size();i++){
//                nowState_Army=enemyInfo.armies[i].NowState;
//                if(nowState_Army==MOVEOBJECT_STATE_STAND )
//              {
//                  HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_buildings[targetBuilding].SN);
//                    }
//                }}
//        }
    //自动反击
    for(int i=0;i<5;i++) target[i]=151;
    return;
    /*###########YOUR CODE ENDS HERE###########*/
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
