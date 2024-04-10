#include "AI.h"

bool ismove=false;
int human1=-1;
int X[4]={1,2575,2575,1};
int Y[4]={1,1,2575,2575};
int step1=0;

void AI::processData(){
    AIlock=true;
/*##########YOUR CODE BEGINS HERE##########*/
    for(tagHuman human:AIGame.humans){
        if(human1==-1){
           human1=human.SN;
        }

    }
    for(tagHuman human:AIGame.humans){
        if(human.SN==human1&&human.NowState==0){
           qDebug()<<HumanMove(human1,X[step1],Y[step1]);
           qDebug()<<X[step1]<<Y[step1];
           step1=(step1+1)%3;
        }
    }



/*###########YOUR CODE ENDS HERE###########*/
    AIlock=false;
}
AI::AI(){
    ;
}
