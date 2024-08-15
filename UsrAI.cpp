#include"UsrAI.h"
tagGame tagUsrGame;
ins UsrIns;
/*##########请勿修改以上部分##########*/
#include<fstream>
#include"Map.h"
#include<bits/stdc++.h>

tagInfo myInfo;
double mid=36*BLOCKSIDELENGTH;  //中间位置的坐标
bool once=true;
bool isCamp=false;  //是否有军营
bool isCampFinish=false;  //军营是否建造完成
bool isCreatArrowTower=false;  //是否研发箭塔
bool isArrowTower0 = false;   //箭塔0是否建造
bool isArrowTower1 = false;   //箭塔1是否建造
bool isArrowTower2 = false;   //箭塔2是否建造
bool isArrowTower3 = false;   //箭塔3是否建造
bool UpGrade_BattleAxe = false;  //是否已经升级棒棒兵为斧头兵
bool UpGrade_Defence_Infantry = false;  //是否已经升级步兵护甲
bool UpGrade_Usetool = false;  //是否已经升级近战攻击
bool Stone_Stock=false;   //是否在石矿附近建造仓库
bool UpGrade_Army_Max=false;  //是否需要更新各个兵种上限

int tmpFrame_Camp=(int)1e6;  // 用于军营的临时帧计数
int tmpFrame_Granary=(int)1e6;  //用于谷仓的临时帧计数
int tmpFrame_ArrowTower=(int)1e6;
int tmpFrame_Farmer=(int)1e6;
int tmpFrame_Army=(int)1e6;
int tmpFrame_Map=(int)1e6;
int house;  // 统计房子的数量
int camp;  // 统计军营的数量
int granary;  // 统计谷仓数量
int arrowtower;  //统计箭塔数量
int market;  //统计市场数量
int stock;  //统计仓库数量
int farm;  //统计农场数量
int clubman;  //统计棍棒兵/斧头兵数量
int bowman;   //统计弓箭手数量
int slinger;  //统计投石兵数量
int sum_myHuman; //统计总人口数量
int MaxHuman=12; //初始设定最大人口数为8
int MaxArmies=4;  //初始设定最大军队士兵数为4
int MaxFarmers=8;  //初始设定最大农民数为8，后续根据资源情况进行调整
int MaxHouse=2;  //初始设定最大房屋数量为2，后续根据资源情况进行调整
int MaxArrowTower=4;  //初始设定最大箭塔数量为4，后续根据资源情况进行调整
int MaxFarm =4;  //初始设定农田数量为4，后续根据游戏进程进行调整
int MaxClubman=2;  //初始设定棍棒兵/斧头兵数量为3,后续根据游戏进程进行调整
int MaxSlinger=2;  //初始设定投石兵数量为1，后续根据游戏进程调整
int MaxBowman=2;  //初始设定弓箭手数量为2，后续根据游戏进程调整

//命令数据初始化
int id_Wrong_lastFrame = -1;
int id_Build_ArrowTower0=-1;
int id_Build_ArrowTower1=-1;
int id_Build_ArrowTower2=-1;
int id_Build_ArrowTower3=-1;


//全局静态变量定义
//块坐标定义
static int x_center,y_center;
static int x_house,y_house;
static int x_granary,y_granary;
static int x_arrowtower,y_arrowtower;
static int x_market,y_market;
static int x_camp,y_camp;
static int x_farm,y_farm;
static int x_stone_stock,y_stone_stock;
static int Army_Sum;

//细节坐标定义
static int detail_x_center,detail_y_center;
static int detail_x_stock,detail_y_stock;
static int detail_x_granary,detail_y_granary;
static int detail_x_arrowtower,detail_y_arrowtower;

//创建地图资源
int MyMap[76][76]={0};

//创建农场数组
vector<tagBuilding>myFarm;

// 对于块坐标，DR为x轴，UR为y轴

void UsrAI::UpDateMap()
{
    //遍历所有建筑，更新地图数据
    for(tagBuilding building:myInfo.buildings)
    {
        //市镇中心 1
        if(building.Type==BUILDING_CENTER&&building.Percent>0)
        {
            int x=building.BlockDR;
            int y=building.BlockUR;
            for(int map_x=x;map_x<x+3;map_x++) for(int map_y=y;map_y<y+3;map_y++) MyMap[map_x][map_y]=1;
        }

        //房屋 2
        else if(building.Type==BUILDING_HOME&&building.Percent>0)
        {
            int x=building.BlockDR;
            int y=building.BlockUR;
            for(int map_x=x;map_x<x+2;map_x++) for(int map_y=y;map_y<y+2;map_y++) MyMap[map_x][map_y]=2;
        }

        //谷仓 3
        if(building.Type==BUILDING_GRANARY&&building.Percent>0)
        {
            int x=building.BlockDR;
            int y=building.BlockUR;
            for(int map_x=x;map_x<x+3;map_x++) for(int map_y=y;map_y<y+3;map_y++) MyMap[map_x][map_y]=3;
        }

        //兵营 4
        else if(building.Type==BUILDING_ARMYCAMP&&building.Percent>0)
        {
            int x=building.BlockDR;
            int y=building.BlockUR;
            for(int map_x=x;map_x<x+3;map_x++) for(int map_y=y;map_y<y+3;map_y++) MyMap[map_x][map_y]=4;
        }

        //箭塔 5
        else if(building.Type==BUILDING_ARROWTOWER&&building.Percent>0)
        {
            int x=building.BlockDR;
            int y=building.BlockUR;
            for(int map_x=x;map_x<x+2;map_x++) for(int map_y=y;map_y<y+2;map_y++) MyMap[map_x][map_y]=5;
        }

        //市场 6
        else if(building.Type==BUILDING_MARKET&&building.Percent>0)
        {
            int x=building.BlockDR;
            int y=building.BlockUR;
            for(int map_x=x;map_x<x+3;map_x++) for(int map_y=y;map_y<y+3;map_y++) MyMap[map_x][map_y]=6;
        }

        //农场 7
        if(building.Type==BUILDING_FARM&&building.Percent>0)
        {
            int x=building.BlockDR;
            int y=building.BlockUR;
            for(int map_x=x;map_x<x+3;map_x++) for(int map_y=y;map_y<y+3;map_y++) MyMap[map_x][map_y]=7;
        }

        //仓库 8
        else if(building.Type==BUILDING_STOCK)
        {
            int x=building.BlockDR;
            int y=building.BlockUR;
            for(int map_x=x;map_x<x+3;map_x++) for(int map_y=y;map_y<y+3;map_y++) MyMap[map_x][map_y]=8;
        }

        //靶场 9
        else if(building.Type==BUILDING_RANGE&&building.Percent>0)
        {
            int x=building.BlockDR;
            int y=building.BlockUR;
            for(int map_x=x;map_x<x+3;map_x++) for(int map_y=y;map_y<y+3;map_y++) MyMap[map_x][map_y]=9;
        }

        //马厩 10
        else if(building.Type==BUILDING_STABLE&&building.Percent>0)
        {
            int x=building.BlockDR;
            int y=building.BlockUR;
            for(int map_x=x;map_x<x+3;map_x++) for(int map_y=y;map_y<y+3;map_y++) MyMap[map_x][map_y]=10;
        }
    }

    //遍历所有资源，更新地图数据
    for(tagResource resource:myInfo.resources)
    {
        //浆果丛 11
        if(resource.Type==RESOURCE_BUSH)
        {
            int x=resource.BlockDR;
            int y=resource.BlockUR;
            MyMap[x][y]=11;
        }

        //树木 12
        else if(resource.Type==RESOURCE_TREE)
        {
            int x=resource.BlockDR;
            int y=resource.BlockUR;
            MyMap[x][y]=12;
        }

        //石堆 13
        if(resource.Type==RESOURCE_STONE)
        {
            int x=resource.BlockDR;
            int y=resource.BlockUR;
            MyMap[x][y]=13;
        }

        //瞪羚 14
        else if(resource.Type==RESOURCE_GAZELLE)
        {
            int x=resource.BlockDR;
            int y=resource.BlockUR;
            MyMap[x][y]=14;
        }

        //瞪羚尸体 15

        //大象 16
        else if(resource.Type=RESOURCE_ELEPHANT)
        {
            int x=resource.BlockDR;
            int y=resource.BlockUR;
            MyMap[x][y]=16;
        }

        //大象尸体 17

    }
        //村民 18
        //棍棒兵 19
        //战斧兵 20
        //弓箭手 21
        //投石兵 22
        //侦察骑兵 23
}

void UsrAI::processData()
{
    int nowState_Farmer;
    int SN_res;
    int order_Record;

    double dis,temp_dis,temp_enemydis;

    bool FindBush=false;

    //农场存在状态初始化
    bool farm0=false;
    bool farm1=false;
    bool farm2=false;
    bool farm3=false;

    //局部静态变量初始化
    static int timers = 1;
    static int Army_Quantity_timers=1;
    static int Once_Call_Farmer =1;
    static int Once_Army_Attack = 1;
    static int Once_UpdateMap =1;
    static int Once_XY_Granary=1;
    static int Once_XY_Camp = 1;
    static int Once_XY_Market = 1;
    static int Once_XY_House =1;
    static int Once_XY_ArrowTower0=1;
    static int Once_XY_ArrowTower1=1;
    static int Once_XY_ArrowTower2=1;
    static int Once_XY_ArrowTower3=1;
    static int Once_XY_Farm=1;
    static int Once_XY_Stone_Stock=1;


    //建筑数据初始化
    house = 0;
    camp=0;
    granary = 0;
    arrowtower = 0;
    market = 0;
    stock = 0;

    //兵种数量初始化
    clubman=0;
    bowman=0;
    slinger=0;

    //更新数据状态初始化
    UpGrade_Army_Max=false;

    //更新游戏内各项数据
    myInfo=getInfo(); //每帧获取新的tagInfo

    //更新地图，每50帧更新一次
    if(Once_UpdateMap||getInfo().GameFrame>tmpFrame_Map+50)
    {
        Once_UpdateMap=0;
        tmpFrame_Map=getInfo().GameFrame;
        UpDateMap();
    }

    //打印命令编号示例
    /*
    if(id_Wrong_lastFrame>=0&&timers>0) //记录了上一帧下达命令的编号
    {
       order_Record = myInfo.ins_ret[id_Wrong_lastFrame];  //获取上一帧下达命令的结果
       DebugText( ("命令"+ QString::number(id_Wrong_lastFrame) +"的返回为"+ QString::number(order_Record)).toStdString());
       id_Wrong_lastFrame = -1;
       timers--;  //只获取并打印一次
    }
    */

    //查看人口数是否达到上限，若是，则增加房屋建造数量提升人口上限
    sum_myHuman=myInfo.armies.size()+myInfo.farmers.size();

    //如果人口数达到上限，房子上限增加2且最高不超过11
    if(sum_myHuman==MaxHuman&&MaxHouse<9)
    {
        //增加的人口数一半给农民，一半给军队
        MaxHuman+=4;
        MaxFarmers+=2;
        MaxArmies+=2;
        MaxHouse+=1;
        UpGrade_Army_Max=true;
    }

    //根据军队人数上限更新每个兵种的上限
    if(UpGrade_Army_Max)
    {
        if(MaxArmies<=12)
        {
            MaxClubman=MaxArmies/2;
            MaxSlinger=MaxArmies/2;
        }
        else if(MaxArmies==16)
        {
            MaxBowman=4;
        }

        else if(MaxArmies>16)
        {
            MaxBowman+=MaxArmies-16;
        }
    }

    //遍历所有资源
    for(tagResource resource:myInfo.resources)
    {
        //如果浆果丛资源存在，设定存在为真
        if(resource.Type==RESOURCE_BUSH) FindBush=true;
    }

    // 遍历所有军队
    for(int i = 0 ; i<myInfo.armies.size(); i++)
    {
        //更新各个兵种的数量
        if(myInfo.armies[i].Sort==AT_CLUBMAN) clubman++;
        else if(myInfo.armies[i].Sort==AT_BOWMAN) bowman++;
        else if(myInfo.armies[i].Sort==AT_SLINGER) slinger++;

        //军队数量更新后，打印各个兵种的数目
        if(myInfo.armies.size()!=Army_Sum)
        {
            Army_Quantity_timers=1;
            Army_Sum=myInfo.armies.size();
        }

        // 如果军队距离箭塔位置超过200且处于空闲状态，移动到箭塔位置附近位置
        temp_dis = calDistance(myInfo.armies[i].DR,myInfo.armies[i].UR,detail_x_arrowtower+100,detail_y_arrowtower-100);
        if(temp_dis>100&&myInfo.armies[i].NowState==HUMAN_STATE_IDLE) HumanMove(myInfo.armies[i].SN,detail_x_arrowtower+100,detail_y_arrowtower-100);
    }

    //打印各军队兵种数量
    //每次军队士兵数量发生变化后打印一次
    if(Army_Quantity_timers>0)
    {
        Army_Quantity_timers=0;
        DebugText("棒棒兵/刀斧兵数量为"+QString::number(clubman).toStdString());
        DebugText("弓箭手的数量位"+QString::number(bowman).toStdString());
        DebugText("投石兵的数量为"+QString::number(slinger).toStdString());
    }

    // 遍历所有建筑（此处用了C++的快速遍历方式）
    for(tagBuilding building : getInfo().buildings)
    {
        //如果建筑类型是房子，增加房子数量计数
        if(building.Type==BUILDING_HOME) house++;

        //如果建筑类型是谷仓并且建造完成，增加谷仓计数
        else if(building.Type == BUILDING_GRANARY&&building.Percent == 100)
        {
            granary++;

            //定义细节坐标
            detail_x_granary = building.BlockDR*BLOCKSIDELENGTH;
            detail_y_granary = building.BlockUR*BLOCKSIDELENGTH;

            //如果没有研发箭塔，研发箭塔
            if(!isCreatArrowTower)
            {
                BuildingAction(building.SN,BUILDING_GRANARY_ARROWTOWER);
                tmpFrame_ArrowTower=tagUsrGame.getInfo().GameFrame;
                if(building.ProjectPercent==100) isCreatArrowTower = true;
            }
        }

        //如果建筑类型是农场且建造完成，增加计数
        else if(building.Type==BUILDING_FARM&&building.Percent==100)
        {
            farm++;
            myFarm.push_back(building);
        }

        //如果建筑类型是仓库
        else if(building.Type==BUILDING_STOCK)
        {
            stock++;

            //定义细节坐标
            detail_x_stock = building.BlockDR*BLOCKSIDELENGTH;
            detail_y_stock = building.BlockUR*BLOCKSIDELENGTH;

            //如果仓库已经有任务，跳过
            if(building.Project!=ACT_NULL) continue;

            //当农民达到上限且资源充足时，先升级军队近战防御
            if(!UpGrade_Defence_Infantry&&myInfo.Meat>=75&&myInfo.farmers.size()==MaxFarmers)
            {
                BuildingAction(building.SN,BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY);
                DebugText("仓库升级了军队近战防御");
                UpGrade_Defence_Infantry = true;
                continue;
            }

            //如果已经升级的近战防御，升级伐木速度/近战攻击力
            else if(UpGrade_Defence_Infantry&&!UpGrade_Usetool&&myInfo.Meat>=100)
            {
                BuildingAction(building.SN,BUILDING_STOCK_UPGRADE_USETOOL);
                DebugText("仓库升级了伐木速度/近战攻击力");
                UpGrade_Usetool=true;
                continue;
            }
        }

        // 如果建筑类型是军营
        else if( building.Type==BUILDING_ARMYCAMP&&building.Percent==100)
        {
            camp++;
            if(building.Project!=ACT_NULL) continue;

            //先生产两个近战兵和一个远程兵
            if(myInfo.armies.size()<MaxArmies)
            {
                //先造投石兵
                if(slinger<MaxSlinger&&myInfo.Meat>=BUILDING_ARMYCAMP_CREATE_SLINGER_FOOD&&myInfo.Stone>=BUILDING_ARMYCAMP_CREATE_SLINGER_STONE) id_Wrong_lastFrame = BuildingAction(building.SN,BUILDING_ARMYCAMP_CREATE_SLINGER);

                //再造棒棒兵/斧头兵
                else if(clubman<MaxClubman&&myInfo.Meat>=BUILDING_ARMYCAMP_CREATE_CLUBMAN_FOOD) id_Wrong_lastFrame = BuildingAction(building.SN,BUILDING_ARMYCAMP_CREATE_CLUBMAN);
            }

            //当军营没有进程时，一般来说军队人数已经达到上限攒资源升级斧头兵
            else if(building.Project=ACT_NULL)
            {
                //升级了伐木速度/近战攻击力后，升级斧头兵
                if(UpGrade_Usetool&&myInfo.Meat>=100&&myInfo.armies.size()==MaxArmies) BuildingAction(building.SN,BUILDING_ARMYCAMP_UPGRADE_CLUBMAN);
                if(UpGrade_Usetool&&building.ProjectPercent>0) UpGrade_BattleAxe=true;
            }
        }

        // 如果建筑类型是城镇中心且没有进行任何项目且农民数量少于MaxFarmers
        else if(building.Type==BUILDING_CENTER && building.Project==ACT_NULL && getInfo().farmers.size()<MaxFarmers)
        {
            //定义细节坐标
            detail_x_center = building.BlockDR*BLOCKSIDELENGTH;
            detail_y_center = building.BlockUR*BLOCKSIDELENGTH;

            BuildingAction(building.SN,BUILDING_CENTER_CREATEFARMER);
            x_center=building.BlockDR;
            y_center=building.BlockUR;
        }

        //如果建筑类型是箭塔且建造进度已满，增加箭塔计数
        else if(building.Type ==BUILDING_ARROWTOWER)
        {
            arrowtower++;
        }

        //如果建筑类型是市场
        else if(building.Type == BUILDING_MARKET)
        {
            market++;
        }
    }

    //根据农场数更新农场存在情况
    for(int i=0;i<myFarm.size();i++)
    {
        if(i==0) farm0=true;
        else if(i==1) farm1=true;
        else if(i==2) farm2=true;
        else if(i==3) farm3=true;
    }

    // 遍历所有农民（此处即传统遍历方式）
    if(Once_Call_Farmer||getInfo().GameFrame>tmpFrame_Farmer+5) //每5帧下达一次指令，防止指令下达过于频繁导致村民状态卡死
    {
        Once_Call_Farmer=0;
        for(int i = 0 ; i<myInfo.farmers.size() ; i++)
        {
            nowState_Farmer = myInfo.farmers[i].NowState;
            /*
            农民0：
            1.建造2个房屋
            2.伐木
            3.军营建好了后先采集浆果
            4.建造农田
            */
            if( i==0&& ( nowState_Farmer==HUMAN_STATE_IDLE || nowState_Farmer ==HUMAN_STATE_STOP ) )
            {
                //如果之前有未完成的建房工程，先将其完成
                for(tagBuilding building:myInfo.buildings) if(building.Type==BUILDING_HOME&&building.Percent<100) HumanBuild(myInfo.farmers[i].SN,BUILDING_HOME,building.BlockDR,building.BlockUR);

                // 如果是第零个农民且处于空闲或停止状态且资源足够，建造房子
                if(house<MaxHouse&&myInfo.Wood>30)
                {
                    if(Once_XY_House)
                    {
                        Once_XY_House=0;
                        x_house = x_center-5;
                        y_house = y_center-3;
                    }
                    //尝试在地图上的连续位置建造房子
                    DebugText("命令农民0建造房子");
                    if(x_house<75&&y_house<75) HumanBuild(myInfo.farmers[i].SN, BUILDING_HOME ,x_house,y_house);
                    y_house-=1;
                }

                /*
                // 如果已经研发了箭塔，建造箭塔
                else if(isCreatArrowTower&&arrowtower<MaxArrowTower&&myInfo.Stone>150)
                {
                    DebugText("命令农民0建造箭塔");
                    //在不同的建筑旁建造箭塔
                    if(arrowtower==0)
                    {
                        //第一个箭塔建造市镇中心附近
                        if(Once_XY_ArrowTower0)
                        {
                            Once_XY_ArrowTower0=0;
                            x_arrowtower=x_center+3;
                            y_arrowtower=y_center+3;
                        }
                    }
                    else if(arrowtower==1&&granary>0)
                    {
                        //第二个箭塔建在谷仓附近
                        if(Once_XY_ArrowTower1)
                        {
                            Once_XY_ArrowTower1=0;
                            x_arrowtower=x_granary+3;
                            y_arrowtower=y_granary+3;
                        }
                    }
                    else if(arrowtower==2&&camp>0)
                    {
                        //第三个箭塔建在兵营附近
                        if(Once_XY_ArrowTower2)
                        {
                            Once_XY_ArrowTower2=0;
                            x_arrowtower=x_camp+3;
                            y_arrowtower=y_camp+3;
                        }
                    }
                    else if(arrowtower==3&&house>0)
                    {
                        //第三个箭塔建在房屋附近
                        if(Once_XY_ArrowTower3)
                        {
                            Once_XY_ArrowTower3=0;
                            x_arrowtower=x_house+3;
                            y_arrowtower=y_house+3;
                        }
                    }
                    if(x_arrowtower<75&&y_arrowtower<75)
                    {
                        HumanBuild(myInfo.farmers[i].SN,BUILDING_ARROWTOWER,x_arrowtower,y_arrowtower);
                        x_arrowtower+=3;
                        y_arrowtower+=3;
                    }
                }
                */

                //如果谷仓建造完成且未建造市场且建造材料充足，建造市场
                else if(camp>0&&granary>0&&market<1&&myInfo.Wood>150)
                {
                    //如果之前有未完成的工程，先将其完成
                    for(tagBuilding building:myInfo.buildings)
                    {
                        if(building.Type==BUILDING_MARKET&&building.Percent<100) HumanBuild(myInfo.farmers[i].SN,BUILDING_MARKET,building.BlockDR,building.BlockUR);
                    }
                    if(Once_XY_Market)
                    {
                        Once_XY_Market=0;
                        x_market=20;
                        y_market=30;
                    }
                    DebugText("命令农民0建造市场");
                    if(x_market>0&&y_market>=0) HumanBuild(myInfo.farmers[i].SN,BUILDING_MARKET,x_market,y_market);
                    x_market+=1;
                    y_market+=1;
                }

                //如果农场0已经建造完成
                else if(farm0)
                {
                    //前往对应农场工作
                    HumanAction(myInfo.farmers[i].SN,myFarm[0].SN);
                }

                //市场创建完成后，检查木头数量是否足够建造农田，若足够且农场数量不足最大值，建造农场
                else if(market>0&&myInfo.Wood>75&&!farm0)
                {
                    //如果之前有未完成的工程，先将其完成
                    for(tagBuilding building:myInfo.buildings)
                    {
                        if(building.Type==BUILDING_FARM&&building.Percent<100) HumanBuild(myInfo.farmers[i].SN,BUILDING_FARM,building.BlockDR,building.BlockUR);
                    }
                    DebugText("命令农民0建造农场");
                    if(Once_XY_Farm)
                    {
                        Once_XY_Farm=0;
                        x_farm=x_center+3;
                        y_farm=y_center-3;
                    }
                    if(y_farm>=0)
                    {
                        HumanBuild(myInfo.farmers[i].SN,BUILDING_FARM,x_farm,y_farm);
                        y_farm-=3;
                    }
                    else
                    {
                        x_farm+=3;
                        y_farm=y_center;
                    }
                }

                //木材不足以建造农场或农场数量已经达到上限，伐木
                else
                {
                    DebugText("命令农民0伐木");
                    SN_res = -1;
                    dis = 1e6;

                    //优先寻找仓库附近的树木资源
                    for(int j = 0 ; j<myInfo.resources.size() ; j++)
                    {
                        temp_dis = calDistance(detail_x_stock,detail_y_stock,myInfo.resources[j].DR, myInfo.resources[j].UR);
                        if(myInfo.resources[j].Type==RESOURCE_TREE &&temp_dis<dis )
                        {
                            SN_res = myInfo.resources[j].SN;
                            dis = temp_dis;
                        }
                    }
                    HumanAction(myInfo.farmers[i].SN,SN_res);
                }
            }

            /*
            农民1：
            1.建造兵营
            2.伐木
            3.在谷仓旁建造箭塔
            */
            else if( i==1&& ( nowState_Farmer==HUMAN_STATE_IDLE || nowState_Farmer == HUMAN_STATE_STOP ) )
            {
                //如果是第一个农民且处于空闲或停止状态且资源充足，在谷仓建造完成后建造兵营
                if(granary>0&&camp<1&&myInfo.Wood>125)
                {

                    //如果之前有未完成的工程，先将其完成
                    for(tagBuilding building:myInfo.buildings) if(building.Type==BUILDING_ARMYCAMP&&building.Percent<100) HumanBuild(myInfo.farmers[i].SN,BUILDING_ARMYCAMP,building.BlockDR,building.BlockUR);
                    DebugText("命令农民1建造军营");
                    if(Once_XY_Camp)
                    {
                        //兵营需要尽量远离市镇中心，尽量不占据其它需要靠近市镇中心的建筑的位置
                        Once_XY_Camp=0;
                        x_camp = x_center+10;
                        y_camp = y_center;
                    }
                    if(x_camp<70)
                    {
                        HumanBuild(myInfo.farmers[i].SN, BUILDING_ARMYCAMP,x_camp, y_camp);
                        x_camp+=3;
                    }
                    if(x_camp>=70)
                    {
                        y_camp+=2;
                        x_camp=55;
                    }
                }

                //如果建造了至少一个农田，在农田旁建造第二个箭塔，便于保护村民
                else if(farm>0&&myInfo.Stone>150&&arrowtower<2&&!isArrowTower1&&isCreatArrowTower)
                {
                    if(Once_XY_ArrowTower1)
                    {
                        Once_XY_ArrowTower1=0;
                        x_arrowtower=x_farm+4;
                        y_arrowtower=y_farm;
                    }
                    if(x_arrowtower<75&&y_arrowtower<75)
                    {
                        DebugText("命令农民0建造箭塔");
                        id_Build_ArrowTower1 = HumanBuild(myInfo.farmers[i].SN,BUILDING_ARROWTOWER,x_arrowtower,y_arrowtower);

                        //更新箭塔的细节坐标
                        detail_x_arrowtower = x_arrowtower*BLOCKSIDELENGTH;
                        detail_y_arrowtower = y_arrowtower*BLOCKSIDELENGTH;

                        y_arrowtower-=1;
                        DebugText("命令的返回值为"+QString::number(id_Build_ArrowTower1).toStdString());
                    if(nowState_Farmer==HUMAN_STATE_BUILDING) isArrowTower1=true;
                    }
                }

                //否则伐木
                else
                {
                    SN_res = -1;
                    dis = 1e6;
                    for(int j = 0 ; j<myInfo.resources.size() ; j++)
                    {
                        temp_dis = calDistance(detail_x_stock,detail_y_stock,myInfo.resources[j].DR, myInfo.resources[j].UR);
                        if(myInfo.resources[j].Type==RESOURCE_TREE &&temp_dis<dis )
                        {
                            SN_res = myInfo.resources[j].SN;
                            dis = temp_dis;
                        }
                    }
                    DebugText("命令农民1伐木");
                    HumanAction(myInfo.farmers[i].SN,SN_res);
                }

            }

            /*
            农民2：
            1.采集浆果（特别的，农民2会先尝试在浆果丛附近建造谷仓）
            2.在谷仓附近建造第二个箭塔
            */
            else if(i==2&&(nowState_Farmer==HUMAN_STATE_IDLE||nowState_Farmer == HUMAN_STATE_STOP))
            {
                //如果是第二个农民,先搜索最近的浆果从，在其附近建造谷仓
                if(granary==0&&myInfo.Wood>120)
                {
                    //如果之前有未完成的工程，先将其完成
                    for(tagBuilding building:myInfo.buildings) if(building.Type==BUILDING_GRANARY&&building.Percent<100) HumanBuild(myInfo.farmers[i].SN,BUILDING_GRANARY,building.BlockDR,building.BlockUR);
                    dis = 1e6;
                    x_granary;
                    y_granary;
                    if(Once_XY_Granary)
                    {
                        Once_XY_Granary=0;
                        for(int j=0;j<myInfo.resources.size();j++)
                        {
                            temp_dis = calDistance(mid,mid,myInfo.resources[j].DR,myInfo.resources[j].UR);
                            if(myInfo.resources[j].Type == RESOURCE_BUSH && temp_dis<dis)
                            {
                                x_granary = myInfo.resources[j].BlockDR;
                                y_granary = myInfo.resources[j].BlockUR;
                            }
                        }
                    }
                    if(x_granary>0&&y_granary>0)
                    {
                        DebugText("命令农民2建造谷仓");
                        HumanBuild(myInfo.farmers[i].SN,BUILDING_GRANARY,x_granary,y_granary);
                    }
                    x_granary-=2;
                    y_granary-=2;
                }

                //箭塔研发完成后，如果资源足够，在谷仓附近建造一个箭塔
                else if(granary>0&&isCreatArrowTower&&!isArrowTower0&&myInfo.Stone>150&&arrowtower<1)
                {
                    //第一个箭塔建在谷仓附近
                    if(Once_XY_ArrowTower0)
                    {
                        Once_XY_ArrowTower0=0;
                        x_arrowtower=x_granary;
                        y_arrowtower=y_granary+2;
                    }
                    if(y_arrowtower>0)
                    {
                        DebugText("命令农民1在谷仓附近建造箭塔");
                        id_Build_ArrowTower0 = HumanBuild(myInfo.farmers[i].SN,BUILDING_ARROWTOWER,x_arrowtower,y_arrowtower);

                        //更新箭塔的细节坐标
                        detail_x_arrowtower = x_arrowtower*BLOCKSIDELENGTH;
                        detail_y_arrowtower = y_arrowtower*BLOCKSIDELENGTH;

                        y_arrowtower-=1;
                        DebugText("命令的返回值为"+QString::number(id_Build_ArrowTower0).toStdString());
                    }
                    if(nowState_Farmer==HUMAN_STATE_BUILDING) isArrowTower0=true;
                }

                //建造完谷仓后，采集浆果
                else if(granary>0&&FindBush)
                {
                    DebugText("命令农民"+QString::number(i).toStdString()+"采集浆果");
                    SN_res = -1;
                    dis = 1e6;
                    for(int j = 0 ; j<myInfo.resources.size() ; j++)
                    {
                        temp_dis = calDistance(mid,mid,myInfo.resources[j].DR, myInfo.resources[j].UR);
                        if(myInfo.resources[j].Type==RESOURCE_BUSH &&temp_dis<dis )
                        {
                            SN_res = myInfo.resources[j].SN;
                            dis = temp_dis;
                        }
                    }
                    HumanAction(myInfo.farmers[i].SN,SN_res);
                }
            }


            /*
            农民3,4,5,6：
            1.采集浆果
            2.否则伐木
            */
            else if(i<=6 &&(nowState_Farmer == HUMAN_STATE_IDLE||nowState_Farmer == HUMAN_STATE_STOP))
            {
                //如果地图上还存在浆果资源
                if(FindBush)
                {
                    // 如果是第三、四、五、六个农民且处于空闲状态，采集浆果资源
                    DebugText("命令农民"+QString::number(i).toStdString()+"采集浆果");
                    SN_res = -1;
                    dis = 1e6;
                    for(int j = 0 ; j<myInfo.resources.size() ; j++)
                    {
                        temp_dis = calDistance(mid,mid,myInfo.resources[j].DR, myInfo.resources[j].UR);
                        if(myInfo.resources[j].Type==RESOURCE_BUSH &&temp_dis<dis )
                        {
                            SN_res = myInfo.resources[j].SN;
                            dis = temp_dis;
                        }
                    }
                    HumanAction(myInfo.farmers[i].SN,SN_res);
                }

                //浆果采集完之后，伐木
                else
                {
                    DebugText("命令农民"+QString::number(i).toStdString()+"伐木");
                    SN_res = -1;
                    dis = 1e6;
                    for(int j = 0 ; j<myInfo.resources.size() ; j++)
                    {
                        temp_dis = calDistance(detail_x_stock, detail_y_stock, myInfo.resources[j].DR, myInfo.resources[j].UR);
                        if(myInfo.resources[j].Type== RESOURCE_TREE &&temp_dis<dis)
                        {
                            SN_res = myInfo.resources[j].SN;
                            dis = temp_dis;
                        }
                    }
                    HumanAction(myInfo.farmers[i].SN,SN_res);
                }
            }

            /*
            农民7：
            1.建好军营后优先采集浆果
            2.伐木
            */
            else if(i==7&&(nowState_Farmer == HUMAN_STATE_IDLE||nowState_Farmer == HUMAN_STATE_STOP))
            {
                //如果建造了军营，且均对人口还没满，先采集浆果
                if(camp>0&&myInfo.armies.size()<MaxArmies&&FindBush)
                {
                    DebugText("命令农民"+QString::number(i).toStdString()+"伐木");
                    SN_res = -1;
                    dis = 1e6;
                    for(int j = 0 ; j<myInfo.resources.size() ; j++)
                    {
                        temp_dis = calDistance(mid, mid, myInfo.resources[j].DR, myInfo.resources[j].UR);
                        if(myInfo.resources[j].Type== RESOURCE_BUSH &&temp_dis<dis)
                        {
                            SN_res = myInfo.resources[j].SN;
                            dis = temp_dis;
                        }
                    }
                    HumanAction(myInfo.farmers[i].SN,SN_res);
                }

                //否则砍树
                else
                {
                    DebugText("命令农民"+QString::number(i).toStdString()+"伐木");
                    SN_res = -1;
                    dis = 1e6;
                    for(int j = 0 ; j<myInfo.resources.size() ; j++)
                    {
                        temp_dis = calDistance(detail_x_stock, detail_y_stock, myInfo.resources[j].DR, myInfo.resources[j].UR);
                        if(myInfo.resources[j].Type== RESOURCE_TREE &&temp_dis<dis)
                        {
                            SN_res = myInfo.resources[j].SN;
                            dis = temp_dis;
                        }
                    }
                    HumanAction(myInfo.farmers[i].SN,SN_res);
                }
            }

            /*
            农民8：
            1.采集浆果
            2.建造农场
            */
            else if(i==8&&(nowState_Farmer == HUMAN_STATE_IDLE || nowState_Farmer == HUMAN_STATE_STOP))
            {
                //如果暂时还没有农场，就先采集浆果
                if(farm<1&&FindBush)
                {
                    DebugText("命令农民"+QString::number(i).toStdString()+"采集浆果");
                    SN_res = -1;
                    dis = 1e6;
                    for(int j = 0 ; j<myInfo.resources.size() ; j++)
                    {
                        temp_dis = calDistance(mid,mid,myInfo.resources[j].DR, myInfo.resources[j].UR);
                        if(myInfo.resources[j].Type==RESOURCE_BUSH &&temp_dis<dis )
                        {
                            SN_res = myInfo.resources[j].SN;
                            dis = temp_dis;
                        }
                    }
                    HumanAction(myInfo.farmers[i].SN,SN_res);
                }

                //否则建造农场1
                else if(market>0&&myInfo.Wood>75&&!farm1)
                {
                    //如果之前有未完成的工程，先将其完成
                    for(tagBuilding building:myInfo.buildings)
                    {
                        if(building.Type==BUILDING_FARM&&building.Percent<100) HumanBuild(myInfo.farmers[i].SN,BUILDING_FARM,building.BlockDR,building.BlockUR);
                    }
                    DebugText("命令农民0建造农场");
                    if(Once_XY_Farm)
                    {
                        Once_XY_Farm=0;
                        x_farm=x_center+3;
                        y_farm=y_center-3;
                    }
                    if(y_farm>=0)
                    {
                        HumanBuild(myInfo.farmers[i].SN,BUILDING_FARM,x_farm,y_farm);
                        y_farm-=3;
                    }
                    else
                    {
                        x_farm+=3;
                        y_farm=y_center;
                    }
                }

                //农场1建造完成后，前往工作
                else if(farm1)
                {
                    HumanAction(myInfo.farmers[i].SN,myFarm[1].SN);
                }
            }

            /*
            农民9：
            1.挖矿（特别的，农民11会尝试在矿物资源附近建造一个仓库）
            */

            else if(i==11&&(nowState_Farmer==HUMAN_STATE_IDLE||nowState_Farmer == HUMAN_STATE_STOP))
            {
                //如果木头资源足够，在石矿附近建造一个仓库
                if(!Stone_Stock&&myInfo.Wood>=150)
                {
                    //如果之前有未完成的工程，先将其完成
                    for(tagBuilding building:myInfo.buildings) if(building.Type==BUILDING_STOCK&&building.Percent<100) HumanBuild(myInfo.farmers[i].SN,BUILDING_GRANARY,building.BlockDR,building.BlockUR);
                    dis = 1e6;
                    x_stone_stock;
                    y_stone_stock;
                    if(Once_XY_Stone_Stock)
                    {
                        Once_XY_Stone_Stock=0;
                        for(int j=0;j<myInfo.resources.size();j++)
                        {
                            x_stone_stock = myInfo.resources[j].BlockDR;
                            y_stone_stock = myInfo.resources[j].BlockUR;
                        }
                    }
                    if(y_stone_stock>0)
                    {
                        DebugText("命令农民9建造仓库");
                        HumanBuild(myInfo.farmers[i].SN,BUILDING_STOCK,x_stone_stock,y_stone_stock);
                        x_stone_stock-=2;
                    }
                }

                //否则挖矿
                else
                {
                    DebugText("命令农民"+QString::number(i).toStdString()+"挖矿");
                    SN_res = -1;
                    dis = 1e6;
                    for(int j = 0 ; j<myInfo.resources.size() ; j++)
                    {
                        temp_dis = calDistance(mid,mid,myInfo.resources[j].DR, myInfo.resources[j].UR);
                        if(myInfo.resources[j].Type==RESOURCE_STONE &&temp_dis<dis )
                        {
                            SN_res = myInfo.resources[j].SN;
                            dis = temp_dis;
                        }
                    }
                    HumanAction(myInfo.farmers[i].SN,SN_res);
                }
            }


            /*
            农民10,11：
            1.采集浆果
            2.挖矿
            */
            else if(i<=10&&(nowState_Farmer == HUMAN_STATE_IDLE||nowState_Farmer == HUMAN_STATE_STOP))
            {
                //如果是第八、九、十个农民且处于空闲状态

                //还有浆果丛时，先采集浆果
                if(FindBush)
                {
                    DebugText("命令农民"+QString::number(i).toStdString()+"采集浆果");
                    SN_res = -1;
                    dis = 1e6;
                    for(int j = 0 ; j<myInfo.resources.size() ; j++)
                    {
                        temp_dis = calDistance(mid,mid,myInfo.resources[j].DR, myInfo.resources[j].UR);
                        if(myInfo.resources[j].Type==RESOURCE_BUSH &&temp_dis<dis )
                        {
                            SN_res = myInfo.resources[j].SN;
                            dis = temp_dis;
                        }
                    }
                    HumanAction(myInfo.farmers[i].SN,SN_res);
                }

                //否则挖矿
                else
                {
                    DebugText("命令农民"+QString::number(i).toStdString()+"挖矿");
                    SN_res = -1;
                    dis = 1e6;
                    for(int j = 0 ; j<myInfo.resources.size() ; j++)
                    {
                        temp_dis = calDistance(mid,mid,myInfo.resources[j].DR, myInfo.resources[j].UR);
                        if(myInfo.resources[j].Type==RESOURCE_STONE &&temp_dis<dis )
                        {
                            SN_res = myInfo.resources[j].SN;
                            dis = temp_dis;
                        }
                    }
                    HumanAction(myInfo.farmers[i].SN,SN_res);
                }
            }

            /*
            农民12,13：
            1.伐木
            2.挖矿

            */
            else if(i<=13&&(nowState_Farmer==HUMAN_STATE_IDLE||nowState_Farmer == HUMAN_STATE_STOP))
            {
                //如果是第十一、十二个农民且处于空闲状态,伐木
                if(myInfo.Wood<150)
                {
                    DebugText("命令农民"+QString::number(i).toStdString()+"伐木");
                    SN_res = -1;
                    dis = 1e6;
                    for(int j=0;j<myInfo.resources.size();j++)
                    {
                        temp_dis = calDistance(mid,mid,myInfo.resources[j].DR,myInfo.resources[j].UR);
                        if(myInfo.resources[j].Type == RESOURCE_TREE)
                        {
                            SN_res = myInfo.resources[j].SN;
                            dis =temp_dis;
                        }
                    }
                    HumanAction(myInfo.farmers[i].SN,SN_res);
                }

                //木材够的情况下，去挖矿
                else
                {
                    DebugText("命令农民"+QString::number(i).toStdString()+"挖矿");
                    SN_res = -1;
                    dis = 1e6;
                    for(int j=0;j<myInfo.resources.size();j++)
                    {
                        temp_dis = calDistance(mid,mid,myInfo.resources[j].DR,myInfo.resources[j].UR);
                        if(myInfo.resources[j].Type == RESOURCE_STONE)
                        {
                            SN_res = myInfo.resources[j].SN;
                            dis =temp_dis;
                        }
                    }
                    HumanAction(myInfo.farmers[i].SN,SN_res);
                }

            }

            /*
            农民14,15:
            1.打猎
            */
            else if(i<=15&&(nowState_Farmer==HUMAN_STATE_STOP||nowState_Farmer==HUMAN_STATE_IDLE))
            {
                DebugText("命令农民"+QString::number(i).toStdString()+"打猎");
                SN_res = -1;
                dis = 1e6;
                for(int j=0;j<myInfo.resources.size();j++)
                {
                    temp_dis = calDistance(mid,mid,myInfo.resources[j].DR,myInfo.resources[j].UR);
                    if(myInfo.resources[j].Type == RESOURCE_GAZELLE)
                    {
                        SN_res = myInfo.resources[j].SN;
                        dis =temp_dis;
                    }
                }
                HumanAction(myInfo.farmers[i].SN,SN_res);
            }
        }

        //更新游戏当前帧数
        tmpFrame_Farmer=getInfo().GameFrame;
    }

    //指挥攻击视野内的敌方士兵
    for(int j=0;j<myInfo.enemy_armies.size();j++)
    {
        if(Once_Army_Attack||getInfo().GameFrame>tmpFrame_Army+10) //和村民类似，每10帧下达一次指令
        {
            Once_Army_Attack=0;
            for(int i=0;i<myInfo.armies.size();i++)
            {
                //指挥军队攻击
                if(myInfo.armies[i].NowState==HUMAN_STATE_IDLE||myInfo.armies[i].NowState==HUMAN_STATE_STOP)
                {
                    temp_enemydis = calDistance(myInfo.armies[i].DR,myInfo.armies[i].UR,myInfo.enemy_armies[j].DR,myInfo.enemy_armies[j].UR);
                    if(temp_enemydis<1500) HumanAction(myInfo.armies[i].SN,myInfo.enemy_armies[j].SN);
                }

                //指挥箭塔攻击
                for(const auto building:myInfo.buildings)
                {
                    if(building.Project!=ACT_NULL) continue;
                    temp_enemydis = calDistance(building.BlockDR,building.BlockUR,myInfo.enemy_armies[j].DR,myInfo.enemy_armies[j].UR);

                    //如果距离足够近且箭塔暂无攻击目标，指挥该箭塔攻击
                    if(building.Type==BUILDING_ARROWTOWER&&building.Project==0&&temp_enemydis<2000) HumanAction(building.SN,myInfo.enemy_armies[j].SN);
                }
            }
            tmpFrame_Army = getInfo().GameFrame;
        }
    }

}
