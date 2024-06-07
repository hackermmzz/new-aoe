#include "UsrAI.h"
#include <random>
#include <fstream>

tagGame tagUsrGame;
QMutex tagUsrGameLock;
ins UsrIns;

bool ismove=false;
int human1=-1;
int human2=-1;
int human3=-1;
double mid=36*BLOCKSIDELENGTH;
double X[4]={mid-100,mid+100,mid+100,mid-100};
double Y[4]={mid-100,mid-100,mid+100,mid+100};
int step1=0;
int step2=3;
vector<int> ins_id;

void UsrAI::processData(){
/*##########YOUR CODE BEGINS HERE##########*/
    qDebug()<<"#####begin#####";
    sleep(2);
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
    sleep(2);
    for(tagFarmer human:getGameInfo().farmers){
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
    for(tagFarmer human:getGameInfo().farmers){
        if(human.SN==human1&&human.NowState==HUMAN_STATE_IDLE){
            int sn=-1;
            double dis=99999;
            for(tagResource res:getGameInfo().resources){
                if(countdistance(mid,mid,res.L,res.U)<dis){
                    sn=res.SN;
                    dis=countdistance(mid,mid,res.L,res.U);
                }
            }
            ins_id.push_back(HumanAction(human.SN,sn));
        }else if(human.SN==human2){
//            qDebug()<<"human2:"<<human.NowState;
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
            for(tagResource res:getGameInfo().resources){
                if(res.ProductSort==HUMAN_STOCKFOOD&&res.Type!=RESOURCE_BUSH&&countdistance(mid,mid,res.L,res.U)<dis){
                    sn=res.SN;
                    dis=countdistance(mid,mid,res.L,res.U);
                }
            }
            ins_id.push_back(HumanAction(human.SN,sn));
        }
    }

    for(tagBuilding building:getGameInfo().buildings){
        if(building.Type==BUILDING_CENTER&&building.Project==ACT_NULL&&building.Percent==100){
            ins_id.push_back(BuildingAction(building.SN,BUILDING_CENTER_CREATEFARMER));
        }
        else if(building.Type==BUILDING_ARMYCAMP&&building.Project==ACT_NULL&&building.Percent==100){
            ins_id.push_back(BuildingAction(building.SN,BUILDING_ARMYCAMP_CREATE_CLUBMAN));
        }
    }

    ///测试获取地形高度
    ofstream fout; // 创建一个ofstream对象
    fout.open("map.txt"); // 打开或创建文件以写入数据
    for(int i=0;i<MAP_L;i++){
        for(int j=0;j<MAP_U;j++){
            fout<<getGameInfo().blocks[i][j].height<<" ";
        }
        fout<<endl;
    }
    fout.close();

    qDebug()<<"#####end#####";
/*###########YOUR CODE ENDS HERE###########*/
}
UsrAI::UsrAI()
{
    this->id=0;
}
