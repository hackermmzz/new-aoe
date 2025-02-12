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
#define MODE1 7500
#define MODE4 10500
#define MODE2 15000
#define MODE5 18000
#define MODE3 24000
tagInfo enemyInfo;
static int mode = -3;
static int Attacktimer=0;
static int target[5]={151,151,151,151,151};
static double pos_L[50] = {0};
static double pos_U[50] = {0};
static int armystate[50] = {0};
static int Blood[50] = {0};
static double chasestart_L[50] = {0};
static double chasestart_U[50] = {0};
static int Lock[50];
static int timer[50] = {0};
static int Hero=0;
static int Herotemp=0;
static int ATT1=2;
static int ATT2=6;
static int ChasingLock[50]={0};
static int sum;
static int AIfarmer;
static int AIFSN[50];
static int flag=0;
static int kill=0;
static int base=0;
static long Atarget[50];
static long Btarget[50];
static int s;
static int t;
static int tar[50];
static bool ifretreat[50];
static int heroexist=0;
static int tempBtarget[2]={0,0};
static long Chasingtar[50];
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
void EnemyAI::processData(){

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
