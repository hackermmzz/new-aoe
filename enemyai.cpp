#include "EnemyAI.h"
tagGame tagEnemyGame;
ins EnemyIns;
/*##########DO NOT EDIT ABOVE##########*/
#define WAITING -1
#define CHASE 1
#define ATTACK 2
#define DESTORY 3
tagInfo enemyInfo;
static int mode=0;
static int num=0;
static double pos_L[50]={0};
static double pos_U[50]={0};
static int armystate[50]={0};
static int Blood[50]={0};
static double chasestart_L[50]={0};
static double chasestart_U[50]={0};

void EnemyAI::processData(){
/*##########YOUR CODE BEGINS HERE##########*/
//    qDebug()<<"#####EnemyStart#####";
    int nowState_Army;
    int buildtype;
    int targetBuilding;
    int find=0;
//    int target=0;
    enemyInfo=getInfo();
    if(g_frame==15){
        for(int i=0;i<enemyInfo.armies_n;i++){
            Blood[i]=enemyInfo.armies[i].Blood;
            armystate[i]=WAITING;
            qDebug()<<Blood[i];
            pos_L[i]=enemyInfo.armies[i].DR;
            pos_U[i]=enemyInfo.armies[i].UR;
        }
    }

//    if(g_frame>=15000) mode=3;
//    else if(g_frame>=7500) mode=2;
//    else if(g_frame>=2500) mode=1;
//    qDebug()<<mode;
    for(int i=0;i<enemyInfo.armies_n;i++){
        if(armystate[i]==CHASE){
            qDebug()<<"1";
            if(calDistance(enemyInfo.armies[i].DR,enemyInfo.armies[i].UR,chasestart_L[i],chasestart_U[i])>=100)
            {
                HumanMove(enemyInfo.armies[i].SN,chasestart_L[i],chasestart_U[i]);
                armystate[i]=WAITING;
            }}
    }
    if(mode==1){
        qDebug()<<num;
        if(g_frame>=5000){
            for(int i = 0 ;i<=1; i++)
                {
                    nowState_Army=enemyInfo.armies[i].NowState;
                    if(nowState_Army==MOVEOBJECT_STATE_STAND&&armystate[i]==WAITING)
                    {HumanMove(enemyInfo.armies[i].SN,pos_L[i],pos_U[i]);
                        armystate[i]==ATTACK;
//                        qDebug()<<"success";

        }}}
        else for(int i = 0 ;i<=1; i++)
            {
                nowState_Army=enemyInfo.armies[i].NowState;
                if(nowState_Army==MOVEOBJECT_STATE_STAND)
                {
                    HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_farmers[0].SN);
                num++;
//                qDebug()<<"success";


}
            }}
        else if(mode==2){
        if(enemyInfo.enemy_armies_n==0){
            for(int i = 0 ;i<=7; i++)
                {
                    nowState_Army=enemyInfo.armies[i].NowState;
                    if(nowState_Army==MOVEOBJECT_STATE_STAND )
                    {HumanMove(enemyInfo.armies[i].SN,pos_L[i],pos_U[i]);
                        qDebug()<<"success";
        }}}
        else for(int i=0;i<=7;i++){
                nowState_Army=enemyInfo.armies[i].NowState;
                if(nowState_Army==MOVEOBJECT_STATE_STAND )
                {HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[0].SN);
        /*    qDebug()<<"success";*/}}
        }
        else if(mode==3){
        for(int i=0;i<enemyInfo.enemy_buildings_n&&find!=1;i++){
            buildtype=enemyInfo.enemy_buildings[i].Type;
            if(buildtype==BUILDING_ARROWTOWER)
               { targetBuilding=i;
                 find=1;
                }
              for(int i=3;i<enemyInfo.armies_n;i++){
                nowState_Army=enemyInfo.armies[i].NowState;
                if(nowState_Army==MOVEOBJECT_STATE_STAND )
              {
                  HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_buildings[targetBuilding].SN);
//                  qDebug()<<"success";
                    }
                }}
        }
    for(int i=0;i<enemyInfo.armies_n;i++){
        if(armystate[i]==WAITING&&enemyInfo.armies[i].Blood!=Blood[i]){
//              qDebug()<<"1";
            for(int j=0;j<enemyInfo.enemy_armies_n;j++){

                if(enemyInfo.enemy_armies[j].WorkObjectSN==enemyInfo.armies[i].SN){
                    HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[j].SN);
                    qDebug()<<"success";
                    armystate[i]=CHASE;
                    Blood[i]=enemyInfo.armies[i].Blood;
                    chasestart_L[i]=enemyInfo.armies[i].DR;
                    chasestart_U[i]=enemyInfo.armies[i].UR;
                }
            }
            if(armystate[i]!=CHASE){
                for(int j=0;j<enemyInfo.enemy_farmers_n;j++){
//                    qDebug()<<"3";
                    if(enemyInfo.enemy_farmers[j].WorkObjectSN==enemyInfo.armies[i].SN){
                        HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_farmers[j].SN);
                        qDebug()<<"success";
                        armystate[i]=CHASE;
                        Blood[i]=enemyInfo.armies[i].Blood;
                        chasestart_L[i]=enemyInfo.armies[i].DR;
                        chasestart_U[i]=enemyInfo.armies[i].UR;
                    }
            }
        }
    }}
//    qDebug()<<"#####EnemyEnd#####";
    return;
/*###########YOUR CODE ENDS HERE###########*/
}

