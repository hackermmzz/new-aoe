#include "UsrAI.h"
tagGame tagUsrGame;
ins UsrIns;
/*##########请勿修改以上部分##########*/

#include <fstream>

tagInfo myInfo;

double mid=36*BLOCKSIDELENGTH;  // 中间位置的坐标
bool once=true;
bool isCamp=false;  // 是否有军营
bool isCampFinish=false;  // 军营是否建造完成
int tmpFrame=(int)1e6;  // 临时帧计数
int house;  // 统计房子的数量
int id_Wrong_lastFrame = -1;

void UsrAI::processData()
{

    int nowState_Farmer;
    int SN_res;
    double dis , temp_dis;
    int order_Record;
    static int timers = 1;

    house = 0;
    myInfo = getInfo(); //每帧获取新的tagInfo
    // cheatAction();
    // cheatRes();

    if(id_Wrong_lastFrame >= 0 && timers > 0) //记录了上一帧下达命令的编号
    {
        order_Record = myInfo.ins_ret[id_Wrong_lastFrame];  //获取上一帧下达命令的结果

        DebugText( ("命令" + QString::number(id_Wrong_lastFrame) + "的返回为" + QString::number(order_Record)).toStdString());
        id_Wrong_lastFrame = -1;

        timers --;  //只获取并打印一次
    }

    // 遍历所有建筑（此处用了C++的快速遍历方式）
    for(tagBuilding building : getInfo().buildings)
    {
        if(building.Type==BUILDING_HOME && building.Percent==100)//如果建筑类型是房子且建造完成，增加房子数量计数
        {
            house++;
        }
        else if( building.Type==BUILDING_ARMYCAMP )// 如果建筑类型是军营
        {
            isCamp=true;    //标记有军营

            if(getInfo().GameFrame>tmpFrame+10) // 军营建好后过一会，执行创建棍棒兵的指令
            {
                //下达命令并获取命令号
               id_Wrong_lastFrame = BuildingAction(building.SN,BUILDING_ARMYCAMP_CREATE_CLUBMAN);
            }

            if(building.Percent==100&&!isCampFinish)// 如果军营建造完成且尚未标记完成
            {
                isCampFinish=true;  // 标记军营建造完成

                tmpFrame=tagUsrGame.getInfo().GameFrame;  // 记录下当前帧数

                DebugText("我有兵营啦"); //打印信息到debugText窗口
            }
        }
        else if( building.Type==BUILDING_CENTER && building.Project==ACT_NULL && getInfo().farmers.size()<8 )
        {       // 如果建筑类型是城镇中心且没有进行任何项目且农民数量少于8，创建农民
            BuildingAction(building.SN,BUILDING_CENTER_CREATEFARMER);
        }
    }

    // 遍历所有农民（此处即传统遍历方式）
    for(int i = 0 ; i<myInfo.farmers.size() ; i++)
    {
        nowState_Farmer = myInfo.farmers[i].NowState;

        if( i == 0 && ( nowState_Farmer==HUMAN_STATE_IDLE || nowState_Farmer==HUMAN_STATE_STOP ) )
        {   // 如果是第一个农民且处于空闲或停止状态，建造房子
            static int x_home = 20;
            static int y_home = 20;

            if( x_home < 75 && y_home < 75 && house < 2 )
            {
                HumanBuild(myInfo.farmers[i].SN , BUILDING_HOME , x_home , y_home);
            }
            x_home += 10;
            y_home += 10;

        }
        else if( i == 1 && ( nowState_Farmer==HUMAN_STATE_IDLE || nowState_Farmer==HUMAN_STATE_STOP ) )
        {   // 如果是第二个农民且处于空闲或停止状态，建造军营
            static int x_camp = 20;
            static int y_camp = 20;

            if( x_camp < 75 && y_camp < 75 && !isCamp)
            {
                HumanBuild(myInfo.farmers[i].SN , BUILDING_ARMYCAMP , 75 - x_camp , y_camp);
            }
            x_camp += 10;
            y_camp += 10;

        }
        else if(i < 5 && nowState_Farmer == HUMAN_STATE_IDLE)
        {   // 如果是前三个农民且处于空闲状态，采集浆果资源
            SN_res = -1;
            dis = 1e6;

            for(int j = 0 ; j<myInfo.resources.size() ; j++)
            {
                temp_dis = calDistance(mid , mid , myInfo.resources[j].DR , myInfo.resources[j].UR);
                if(myInfo.resources[j].Type==RESOURCE_BUSH && temp_dis < dis )
                {
                    SN_res = myInfo.resources[j].SN;
                    dis = temp_dis;
                }
            }
            HumanAction(myInfo.farmers[i].SN , SN_res);
        }
        else if( i >= 5 && nowState_Farmer == HUMAN_STATE_IDLE)
        {   // 如果是第五个及以上的农民且处于空闲状态，采集树木资源
            SN_res = -1;
            dis = 1e6;

            for(int j = 0 ; j<myInfo.resources.size() ; j++)
            {
                temp_dis = calDistance(mid , mid , myInfo.resources[j].DR , myInfo.resources[j].UR);

                if(myInfo.resources[j].Type == RESOURCE_TREE && temp_dis < dis)
                {
                    SN_res = myInfo.resources[j].SN;
                    dis = temp_dis;
                }
            }
            HumanAction(myInfo.farmers[i].SN , SN_res);
        }
    }

    // 遍历所有军队
    for(int i = 0 ; i<myInfo.armies.size(); i++)
    {
        temp_dis = calDistance(myInfo.armies[i].DR , myInfo.armies[i].UR , mid+100 , mid-100);

        if(temp_dis > 100 && myInfo.armies[i].NowState==HUMAN_STATE_IDLE)
        {   // 如果军队距离中间位置超过200且处于空闲状态，移动到中间位置
            HumanMove(myInfo.armies[i].SN , mid+100 , mid-100);
        }
    }
}
