#include "AI.h"
#include <random>
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
    qDebug()<<"#####begin#####";
    for(tagHuman human:AIGame.humans){
        if(human1==-1){
           human1=human.SN;
           HumanMove(human1,X[step1],Y[step1]);
           break;
        }
        if(human2==-1&&human.SN!=human1){
           human2=human.SN;
           break;
        }
    }

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
        }else if(human.SN==human2){
            qDebug()<<"human2:"<<human.NowState;
            if(human.NowState==HUMAN_STATE_IDLE){
                static int x= 20;
                static int y= 20;
                qDebug()<<"Build:"<<x<<" "<<y<<" "<<HumanBuild(human.SN,BUILDING_HOME,x,y);
                x+=10;
                y+=10;
            }
        }
    }

//    qDebug()<<AIGame.GameFrame;
//    qDebug()<<"#############";
    for(tagBuilding building:AIGame.buildings){
        if(building.Type==BUILDING_CENTER&&building.Project==ACT_NULL&&building.Percent==100){
            qDebug()<<"Build:"<<building.Percent<<":"<<BuildingAction(building.SN,BUILDING_CENTER_CREATEFARMER);
        }
    }
    sleep(5);
    qDebug()<<"#####end#####";
/*###########YOUR CODE ENDS HERE###########*/
    AIfinished=true;
}
AI::AI(){
    ;
}
