#include "AI.h"

bool ismove=false;
int human1=-1;
int human2=-1;
double mid=36*BLOCKSIDELENGTH;
double X[4]={mid-100,mid+100,mid+100,mid-100};
double Y[4]={mid-100,mid-100,mid+100,mid+100};
int step1=0;
int step2=3;

void AI::processData(){
    AIfinished=false;
/*##########YOUR CODE BEGINS HERE##########*/
    for(tagHuman human:AIGame.humans){
        if(human1==-1){
           human1=human.SN;
           HumanMove(human1,X[step1],Y[step1]);
           break;
        }
        if(human2==-1&&human.SN!=human1){
           human2=human.SN;
           HumanMove(human2,X[step2],Y[step2]);
           break;
        }
    }
    sleep(5);
    for(tagHuman human:AIGame.humans){
        if(human.SN==human1&&human.NowState==HUMAN_STATE_IDLE){
            int sn=-1;
            double dis=99999;
            for(tagResource res:AIGame.resources){
                if(countdistance(mid,mid,res.L,res.U)<dis){
                    sn=res.SN;
                    dis=countdistance(mid,mid,res.L,res.U);
                }
            }
            HumanAction(human.SN,sn);
//            qDebug()<<"AI.cpp::"<<human.NowState;
//            qDebug()<<"AI::obSN:"<<human.WorkObjectSN;
//            qDebug()<<"AI::ResourceSORT:"<<human.ResourceSort;
//            qDebug()<<"AI::Resource:"<<human.Resource;
        }
    }


//    qDebug()<<AIGame.GameFrame;
//    qDebug()<<"#############";
/*###########YOUR CODE ENDS HERE###########*/
    AIfinished=true;
}
AI::AI(){
    ;
}
