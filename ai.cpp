#include "AI.h"
#include <random>
#include <fstream>
bool ismove=false;
int human1=-1;
int human2=-1;
double mid=36*BLOCKSIDELENGTH;
double X[4]={mid-100,mid+100,mid+100,mid-100};
double Y[4]={mid-100,mid-100,mid+100,mid+100};
int step1=0;
int step2=3;

void AI::processData(){
/*##########YOUR CODE BEGINS HERE##########*/
    qDebug()<<"#####begin#####";
    for(tagHuman human:AIGame.humans){
        if(human1==-1){
           human1=human.SN;
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
            qDebug()<<sn;
            qDebug()<<HumanAction(human.SN,sn);
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

    for(tagBuilding building:AIGame.buildings){
        if(building.Type==BUILDING_CENTER&&building.Project==ACT_NULL&&building.Percent==100){
            qDebug()<<"Build:"<<building.Percent<<":"<<BuildingAction(building.SN,BUILDING_CENTER_CREATEFARMER);
        }
    }

    ofstream fout; // 创建一个ofstream对象
    fout.open("map.txt"); // 打开或创建文件以写入数据

    for(int i=0;i<MAP_L;i++){
        for(int j=0;j<MAP_U;j++){
            fout<<AIGame.blocks[i][j].height<<" ";
        }
        fout<<endl;
    }
    fout.close();
    sleep(5);
    qDebug()<<"#####end#####";
/*###########YOUR CODE ENDS HERE###########*/
}
AI::AI(){
    ;
}
