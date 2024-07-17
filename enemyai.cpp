#include "EnemyAI.h"
tagGame tagEnemyGame;
ins EnemyIns;
/*##########DO NOT EDIT ABOVE##########*/
#define WAITING -1
#define CHASE 1
#define ATTACK 2
#define DESTORY 3
#define Cavalry 7
tagInfo enemyInfo;
static int mode=0;
static int num=0;
static double pos_L[50]={0};
static double pos_U[50]={0};
static int armystate[50]={0};
static int Blood[50]={0};
static double chasestart_L[50]={0};
static double chasestart_U[50]={0};
static int cheat=0;
static int FinalAtt=0;
void EnemyAI::processData(){
/*##########YOUR CODE BEGINS HERE##########*/
    int nowState_Army;
    int buildtype;
    int targetBuilding;
    int find=0;
    int target=0;
    enemyInfo=getInfo();
    if(g_frame==15){
        for(int i=0;i<enemyInfo.armies.size();i++){

            Blood[i]=enemyInfo.armies[i].Blood;
            armystate[i]=WAITING;
            qDebug()<<Blood[i];
            pos_L[i]=enemyInfo.armies[i].DR;
            pos_U[i]=enemyInfo.armies[i].UR;
        }
    }
    if(g_frame>=15){
    if(cheat==1||enemyInfo.armies[16].Blood!=Blood[16]||FinalAtt==1){
        FinalAtt=1;
        for(int i=0;i<enemyInfo.armies.size();i++){
            if(armystate[i]!=ATTACK)
            {HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_farmers[0].SN);
            armystate[i]=ATTACK;}
        }
    }}

    qDebug()<<FinalAtt;
    if(g_frame>=13500) mode=3;
    else if(g_frame>=9000) mode=2;
    else if(g_frame>=4500) mode=1;
    for(int i=0;i<enemyInfo.armies.size();i++){
        if(armystate[i]==CHASE){
            if(calDistance(enemyInfo.armies[i].DR,enemyInfo.armies[i].UR,chasestart_L[i],chasestart_U[i])>=200)
            {
                HumanMove(enemyInfo.armies[i].SN,chasestart_L[i],chasestart_U[i]);
                armystate[i]=WAITING;
            }}

    }
   // qDebug()<<mode;
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
                if(nowState_Army==HUMAN_STATE_IDLE || nowState_Army==HUMAN_STATE_STOP)
                {HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_farmers[0].SN);

}
            }}}
        else if(mode==2){
        if(enemyInfo.enemy_armies.size()==0){
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
                }}
        }
        else if(mode==3){
        for(int i=0;i<enemyInfo.enemy_buildings.size()&&find!=1;i++){
            buildtype=enemyInfo.enemy_buildings[i].Type;
            if(buildtype==BUILDING_ARROWTOWER)
               { targetBuilding=i;
                 find=1;
                }
              for(int i=3;i<enemyInfo.enemy_armies.size();i++){
                nowState_Army=enemyInfo.armies[i].NowState;
                if(nowState_Army==MOVEOBJECT_STATE_STAND )
              {
                  HumanAction(enemyInfo.armies[i].SN,enemyInfo.enemy_buildings[targetBuilding].SN);
                    }
                }}
        }
    for(int i=0;i<enemyInfo.armies.size();i++){
        if(armystate[i]==WAITING&&enemyInfo.armies[i].Blood!=Blood[i]){
            for(int j=0;j<enemyInfo.enemy_armies.size();j++){
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
                for(int j=0;j<enemyInfo.enemy_farmers.size();j++){
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
    return;
/*###########YOUR CODE ENDS HERE###########*/
}

void EnemyAI::onWaveAttack(int wave){
    //TODO: 发起第wave波进攻
    if(wave<1||wave>3){
        // std::string debugStr="非法波次："+std::to_string(wave);
        // call_debugText("black", " AI" + QString::number(0) + "打印：" + QString::fromStdString(debugStr), 0);
        return;
    }
    mode=wave;
}

