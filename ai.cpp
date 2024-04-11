#include "AI.h"

bool ismove=false;
int human1=-1;
int human2=-1;
int mid=36*BLOCKSIDELENGTH;
int X[4]={mid-100,mid+100,mid+100,mid-100};
int Y[4]={mid-100,mid-100,mid+100,mid+100};
int step1=0;
int step2=3;

void AI::processData(){
    AIlock=true;
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
    for(tagHuman human:AIGame.humans){
        if(human.SN==human1&&abs(human.L-X[step1])<3&&abs(human.U-Y[step1])<3){
           step1=(step1+1)%4;
           HumanMove(human1,X[step1],Y[step1]);
        }
        if(human.SN==human2&&abs(human.L-X[step2])<3&&abs(human.U-Y[step2])<3){
           step2=(step2-1)>=0?step2-1:3;
           HumanMove(human2,X[step2],Y[step2]);
        }
        if(human.SN==human1){
//            qDebug()<<"AI.cpp::"<<human.NowState;
        }
    }




/*###########YOUR CODE ENDS HERE###########*/
    AIlock=false;
}
AI::AI(){
    ;
}
