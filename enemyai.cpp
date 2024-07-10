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

void EnemyAI::processData(){
/*##########YOUR CODE BEGINS HERE##########*/
    qDebug()<<"#####EnemyStart#####";
    int nowState_Army;
    int buildtype;
    int targetBuilding;
    int find=0;
//    int target=0;
    enemyInfo=getInfo();
    if(g_frame>=15000) mode=3;
    else if(g_frame>=7500) mode=2;
    else if(g_frame>=2500) mode=1;
    qDebug()<<mode;
    sleep(2);
    if(mode==1){

        for(int i = 0 ;i<=1; i++)
            {
                nowState_Army=enemyInfo.armies[i].NowState;
                if(nowState_Army==HUMAN_STATE_IDLE || nowState_Army==HUMAN_STATE_STOP)
                {HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_farmers[0].SN);
                qDebug()<<"success";

}
            }}
        else if(mode==2){
            for(int i=0;i<=3;i++){
                nowState_Army=enemyInfo.armies[i].NowState;
                if(nowState_Army==HUMAN_STATE_IDLE || nowState_Army==HUMAN_STATE_STOP)
                {HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_armies[0].SN);
            qDebug()<<"success";}}
        }
        else if(mode==3){
        for(int i=0;i<enemyInfo.enemy_buildings_n&&find!=1;i++){
            buildtype=enemyInfo.enemy_buildings[i].Type;
            if(buildtype==BUILDING_CENTER)
               { targetBuilding=i;
                 find=1;
                }
              for(int i=3;i<enemyInfo.armies_n;i++){
                nowState_Army=enemyInfo.armies[i].NowState;
                if(nowState_Army==HUMAN_STATE_IDLE || nowState_Army==HUMAN_STATE_STOP)
              {
                  HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_buildings[targetBuilding].SN);
                  qDebug()<<"success";
                    }
                }}
        }


    qDebug()<<"#####EnemyEnd#####";
    return;
/*###########YOUR CODE ENDS HERE###########*/
}

