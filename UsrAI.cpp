#include "UsrAI.h"
tagGame tagUsrGame;
ins UsrIns;
/*##########请勿修改以上部分##########*/

#include <fstream>
double mid=36*BLOCKSIDELENGTH;  // 中间位置的坐标
bool once=true;
bool iscamp=false;  // 是否有军营
bool iscampfinish=false;  // 军营是否建造完成
int tmpframe=99999999;  // 临时帧计数

void UsrAI::processData(){
    int House=0;  // 统计房子的数量

    // 遍历所有建筑（此处用了C++的快速遍历方式）
    for(tagBuilding building:getInfo().buildings){
        if(building.Type==BUILDING_HOME&&building.Percent==100){
            // 如果建筑类型是房子且建造完成，增加房子数量计数
            House++;
        }else if(building.Type==BUILDING_ARMYCAMP){
            // 如果建筑类型是军营
            iscamp=true;  // 标记有军营
            if(getInfo().GameFrame>tmpframe+10){
                // 军营建好后过一会，执行创建棍棒兵的指令
                BuildingAction(building.SN,BUILDING_ARMYCAMP_CREATE_CLUBMAN);
            }
            if(building.Percent==100&&!iscampfinish){
                // 如果军营建造完成且尚未标记完成
                iscampfinish=true;  // 标记军营建造完成
                tmpframe=tagUsrGame.getInfo().GameFrame;  // 记录下当前帧数
            }
        }else if(building.Type==BUILDING_CENTER&&building.Project==ACT_NULL&&getInfo().farmers_n<8){
            // 如果建筑类型是城镇中心且没有进行任何项目且农民数量少于8，创建农民
            BuildingAction(building.SN,BUILDING_CENTER_CREATEFARMER);
        }
    }

    // 遍历所有农民（此处即传统遍历方式）
    for(int i=0;i<getInfo().farmers_n;i++){
        tagFarmer farmer=getInfo().farmers[i];
        if(i==0&&(farmer.NowState==HUMAN_STATE_IDLE||farmer.NowState==HUMAN_STATE_STOP)){
            // 如果是第一个农民且处于空闲或停止状态，建造房子
            static int x= 20;
            static int y= 20;
            if(x<75&&y<75&&House<2){
                HumanBuild(farmer.SN,BUILDING_HOME,x,y);
            }
            x+=10;
            y+=10;
        }
        else if(i==1&&(farmer.NowState==HUMAN_STATE_IDLE||farmer.NowState==HUMAN_STATE_STOP)){
            // 如果是第二个农民且处于空闲或停止状态，建造军营
            static int x= 20;
            static int y= 20;
            if(x<75&&y<75&&!iscamp){
                HumanBuild(farmer.SN,BUILDING_ARMYCAMP,75-x,y);
            }
            x+=10;
            y+=10;
        }
        else if(i<5&&farmer.NowState==HUMAN_STATE_IDLE){
            // 如果是前三个农民且处于空闲状态，采集浆果资源
            int sn=-1;
            double dis=99999;
            for(tagResource res:getInfo().resources){
                if(res.Type==RESOURCE_BUSH&&calDistance(mid,mid,res.DR,res.UR)<dis){
                    sn=res.SN;
                    dis=calDistance(mid,mid,res.DR,res.UR);
                }
            }
            HumanAction(farmer.SN,sn);
        }
        else if(i>=5&&farmer.NowState==HUMAN_STATE_IDLE){
            // 如果是第五个及以上的农民且处于空闲状态，采集树木资源
            int sn=-1;
            double dis=99999;
            for(tagResource res:getInfo().resources){
                if(res.Type==RESOURCE_TREE&&calDistance(mid,mid,res.DR,res.UR)<dis){
                    sn=res.SN;
                    dis=calDistance(mid,mid,res.DR,res.UR);
                }
            }
            HumanAction(farmer.SN,sn);
        }
    }
    // 遍历所有军队
    for(tagArmy army:getInfo().armies){
        if(calDistance(army.DR,army.UR,mid+100,mid-100)>100&&army.NowState==HUMAN_STATE_IDLE){
            // 如果军队距离中间位置超过200且处于空闲状态，移动到中间位置
            HumanMove(army.SN,mid+100,mid-100);
        }
    }

}
