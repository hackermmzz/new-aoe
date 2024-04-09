#include "AI.h"

bool ismove=false;

AI::AI()
{

}
void AI::processData(){
    if(!ismove){
        for(tagHuman human:AIGame.humans){
            qDebug()<<HumanMove(human.SN,1,1);
            qDebug()<<"move!";
            ismove=true;
            break;
        }
    }
}
