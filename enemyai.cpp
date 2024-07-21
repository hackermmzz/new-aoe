#include "EnemyAI.h"
tagGame tagEnemyGame;
ins EnemyIns;
/*##########DO NOT EDIT ABOVE##########*/
#define WAITING -1
#define CHASE 1
#define ATTACK 2
#define DESTROY 3
#define COUNTER 4
#define ATT1 2
#define ATT2 6
#define ATT3 9
tagInfo enemyInfo;
static int mode = 0;
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
static int num=17;
static int seek(int SN){
    double temp=0;
    double min=10000;
    int target=101;
    if(SN<2){
        if(enemyInfo.enemy_farmers.size()!=0)
        for(int i=0;i<enemyInfo.enemy_farmers.size();i++){
            temp=pow(pow(enemyInfo.armies[SN].BlockDR-enemyInfo.enemy_farmers[i].BlockDR, 2) + pow(enemyInfo.armies[SN].BlockUR-enemyInfo.enemy_farmers[i].BlockUR, 2), 0.5);
            if (temp<min) {min=temp;
            target=i;
           }
        }
        else if(enemyInfo.enemy_armies.size()!=0){
            for(int i=0;i<enemyInfo.enemy_armies.size();i++){
                temp=pow(pow(enemyInfo.armies[SN].BlockDR-enemyInfo.enemy_armies[i].BlockDR, 2) + pow(enemyInfo.armies[SN].BlockUR-enemyInfo.enemy_armies[i].BlockUR, 2), 0.5);
                if (temp<min) {min=temp;
                target=i+50;
        }
        return target;
    }}}
    else if(SN>=2){
        if(enemyInfo.enemy_armies.size()!=0)
    for(int i=0;i<enemyInfo.enemy_armies.size();i++){
         temp=pow(pow(enemyInfo.armies[SN].BlockDR-enemyInfo.enemy_armies[i].BlockDR, 2) + pow(enemyInfo.armies[SN].BlockUR-enemyInfo.enemy_armies[i].BlockUR, 2), 0.5);
        if(temp<min){
            min=temp;
            target=i+50;
        }
    }
    else if(enemyInfo.enemy_farmers.size()!=0){
         for(int i=0;i<enemyInfo.enemy_farmers.size();i++){
            temp=pow(pow(enemyInfo.armies[SN].BlockDR-enemyInfo.enemy_farmers[i].BlockDR, 2) + pow(enemyInfo.armies[SN].BlockUR-enemyInfo.enemy_farmers[i].BlockUR, 2), 0.5);
            if(temp<min){
                min=temp;
                target=i;
    }
    }
            }
    return target;
    }
}
void EnemyAI::processData() {
    /*##########YOUR CODE BEGINS HERE##########*/
    int nowState_Army;
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
        }
    }

    //更新波数
    if(g_frame==22500) mode=3;
    else if(g_frame==15000) mode=2;
    else if(g_frame==7500) mode=1;
    //强制总攻
    if(g_frame>=15){
    if(enemyInfo.armies[Hero].Blood!=Blood[Hero]){
        mode=3;
    }}
    //根据波数启动军队
    if(mode==1){
         armystate[0]=ATTACK;armystate[1]=ATTACK;
    }
    else if(mode==2){

        for(int i=0;i<enemyInfo.armies.size();i++){
            if(armystate[i]==CHASE||armystate[i]==WAITING) armystate[i]=ATTACK;
        }
    }
    else if(mode==3){
        for(int i=0;i<enemyInfo.armies.size();i++){
            if(armystate[i]==CHASE||armystate[i]==WAITING) armystate[i]=ATTACK;
        }
    }
    // 自动反击
    for(int i=0;i<enemyInfo.armies.size();i++){
           if(armystate[i]==WAITING&&enemyInfo.armies[i].Blood!=Blood[i]){
               for(int j=0;j<enemyInfo.enemy_armies.size();j++){
                   if(enemyInfo.enemy_armies[j].WorkObjectSN==enemyInfo.armies[i].SN){
                       HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[j].SN);
                       armystate[i]=CHASE;
                       Blood[i]=enemyInfo.armies[i].Blood;
                       chasestart_L[i]=enemyInfo.armies[i].DR;
                       chasestart_U[i]=enemyInfo.armies[i].UR;
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
                       }
               }
           }}}
        //攻击状态
    for(int i=0;i<enemyInfo.armies.size();i++){
           if(armystate[i]==CHASE){
               if(calDistance(enemyInfo.armies[i].DR,enemyInfo.armies[i].UR,chasestart_L[i],chasestart_U[i])>=200)
               {
                   HumanMove(enemyInfo.armies[i].SN,chasestart_L[i],chasestart_U[i]);
                   armystate[i]=WAITING;
               }}
        else if(armystate[i]==ATTACK){
            if((enemyInfo.enemy_armies.size()+enemyInfo.enemy_farmers.size())!=0){
            if(mode==1){
            if(Lock[i]==0&&i<ATT1){
                     int tar=0;
                     nowState_Army=enemyInfo.armies[i].NowState;
                     if(nowState_Army==MOVEOBJECT_STATE_STAND)
                     {
                         tar=seek(i);
                         if(tar==101)
                         armystate[i]==WAITING;
                         else if(tar>=50)
                         HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[tar-50].SN);
                         else  HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_farmers[tar].SN);
                         Lock[i]=1;
                         timer[i]=g_frame;
           }
            if(num==16)armystate[1]=WAITING;
            else if(num==15)armystate[0]=WAITING;
            }}
            else if(mode==2){
                if(Lock[i]==0&&i<ATT1+ATT2){
                    int tar=0;
                          nowState_Army=enemyInfo.armies[i].NowState;
                          if(nowState_Army==MOVEOBJECT_STATE_STAND)
                          {
                              tar=seek(i);
                              qDebug()<<tar<<enemyInfo.enemy_farmers.size();
                              if(tar==101)
                              armystate[i]==WAITING;
                              else if(tar>=50)
                              HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[tar-50].SN);
                              else HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_farmers[tar].SN);
                              Lock[i]=1;
                              timer[i]=g_frame;
                }
            }
                if(num<17){
                    for(int i=17;i>num;i--){
                        armystate[i-10]=WAITING;
                    }
                }

        }
            else if(mode==3){
                if(Lock[i]==0){
                    int tar=0;
                          nowState_Army=enemyInfo.armies[i].NowState;
                          if(nowState_Army==MOVEOBJECT_STATE_STAND)
                          {
                              tar=seek(i);
                              if(tar==101)
                              armystate[i]==WAITING;
                              else if(tar>=50)
                              HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[tar-50].SN);
                              else HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_farmers[tar].SN);
                              Lock[i]=1;
                              timer[i]=g_frame;
            }
        }
               }
            if ((g_frame - timer[i]) >= 75 && enemyInfo.armies[i].NowState == MOVEOBJECT_STATE_STAND) {
                Lock[i]=0;
                timer[i] = 0;
            }
            int dif=num-enemyInfo.armies.size();
            if(dif!=0)
                for(int i=dif;i>0;i--){
                    armystate[i-1]==WAITING;
                }
        }}}

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
//    qDebug()<<"#####EnemyEnd#####";
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
}
