#include "UsrAI.h"
tagGame tagUsrGame;
ins UsrIns;
/*##########DO NOT EDIT ABOVE##########*/


#include <random>
#include <fstream>
bool ismove=false;
int human1=-1;
int human2=-1;
int human3=-1;
double mid=36*BLOCKSIDELENGTH;
double X[4]={mid-100,mid+100,mid+100,mid-100};
double Y[4]={mid-100,mid-100,mid+100,mid+100};
int step1=0;
int step2=3;
vector<int> ins_id;//添加到监视的指令数组

void UsrAI::processData(){
/*##########YOUR CODE BEGINS HERE##########*/
    qDebug()<<"#####UsrStart#####";
//    qDebug()<<tagUsrGame.getGameFrame();
    sleep(2);
    //打印添加到监视的指令执行结果，并从监视中删除
    auto it = ins_id.begin();
    while (it != ins_id.end()) {
        instruction tmp=getInsRet(*it);
        if (tmp.isExist()) {
            printInsRet(*it);
            it = ins_id.erase(it);
        } else {
            ++it;
        }
    }

//    for(tagResource res:tagUsrGame.getResource()){
//        qDebug()<<res.Type<<" "<<res.BlockL<<" "<<res.BlockU;
//    }
    for(tagFarmer human:tagUsrGame.getFarmers()){
        if(human1==-1){
           human1=human.SN;
           break;
        }
        if(human2==-1&&human.SN!=human1){
           human2=human.SN;
           break;
        }
//        if(human3==-1&&human.Sort==SORT_ARMY){
//            human3=human.SN;
//            break;
//        }
    }
    for(tagFarmer human:tagUsrGame.getFarmers()){
        sleep(0.5);
        if(human.SN==human1&&human.NowState==HUMAN_STATE_IDLE){
            int sn=-1;
            double dis=99999;
            for(tagResource res:tagUsrGame.getResource()){
                if(countdistance(mid,mid,res.DR,res.UR)<dis){
                    sn=res.SN;
                    dis=countdistance(mid,mid,res.DR,res.UR);
                }
            }
            ins_id.push_back(HumanAction(human.SN,sn));
        }else if(human.SN==human2){
            qDebug()<<"human2:"<<human.NowState<<" "<<human.BlockDR;
            if(human.NowState==HUMAN_STATE_IDLE){
                static int x= 20;
                static int y= 20;
                if(x<75&&y<75){
                    ins_id.push_back(HumanBuild(human.SN,BUILDING_HOME,x,y));
                }
                x+=10;
                y+=10;
            }
        }else if(human.SN==human3&&human.NowState==HUMAN_STATE_IDLE){
            int sn=-1;
            double dis=99999;
            for(tagResource res:tagUsrGame.getResource()){
                if(res.ProductSort==HUMAN_STOCKFOOD&&res.Type!=RESOURCE_BUSH&&countdistance(mid,mid,res.DR,res.UR)<dis){
                    sn=res.SN;
                    dis=countdistance(mid,mid,res.DR,res.UR);
                }
            }
            ins_id.push_back(HumanAction(human.SN,sn));
        }
    }

    qDebug()<<"#####UsrEnd#####";

/*###########YOUR CODE ENDS HERE###########*/
}

