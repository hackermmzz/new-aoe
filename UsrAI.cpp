#include "UsrAI.h"
tagGame tagUsrGame;
ins UsrIns;
/*##########请勿修改以上部分##########*/
#include<QDebug>
#include<cmath>
tagInfo myInfo;
const int MAP_SIZE=72;
const int MAX_BUSHES = 6;                                   //有六个浆果
const int MAX_GAZELLE = 6;                                  //有6只瞪羚
const int MAX_TOWER = 4 ;                                   //箭塔数量

static bool isGranary=false;                                //是否有谷仓
static bool ishouse3=false;                                 //是否建完3个房子
static bool isStock_Stone=false;                            //是否建完石头旁的仓库
static bool isResearchArchertower=false;                    //是否研发箭塔
static bool isArmycamp=false;                               //是否建完兵营
static bool isMarket=false;                                 //是否建完市场
int houseProgress[5]={0};
int currentFarmers = myInfo.farmers.size();                 // 当前农民的数量

int Center_x;
int Center_y;                                               //获取市镇中心坐标
int Stock_x;
int Stock_y;                                                //获取仓库坐标
int start_x;
int start_y;
int startcamp_x;
int startcamp_y;                                            //兵营开始坐标
int startmarket_x;
int startmarket_y;                                          //市场开始坐标
int bestX_granary=-1;
int bestY_granary=-1;
int bestX_stock=-1;
int bestY_stock=-1;
int bushCount = 0;                                          //浆果丛数量
int towerCount =0;                                          //箭塔数量
int stoneCount;
int gazelleCount=0;
int CenterStone_x1;
int CenterStone_y1;
int CenterStone_x2;
int CenterStone_y2;                                         //两堆石头的中心位置

int gamemap[MAP_SIZE][MAP_SIZE];
int id_Wrong_lastFrame=-1;                                  //上一帧下达的命令编号
Position BushPosition[MAX_BUSHES];                          //储存浆果位置
Position StonePosition[23];
//Position Stone2[12];
void UsrAI::processData()
{
    static int house = 0;                                   //统计房子数量
    static int armycamp=0;                                  //统计兵营数量
    static int market=0;                                    //统计市场数量

    static int granary = 0;
    bool isBushFinished;                                    //浆果是否被采集
    int order_Record;                                       //用于存储命令结果的变量
    int nowState_Farmer;                                    //储存当时农民状态
    int SN_res;                                             //暂时序列号
    int dis,temp_dis;
    int StoneDis1;
    int StoneDid2;
    static int timers =1;                                   //用于计时的静态变量
    myInfo=getInfo();                                       //获取当前游戏状态信息


    if(id_Wrong_lastFrame>=0&&timers>0)
    {
        order_Record=myInfo.ins_ret[id_Wrong_lastFrame];    //记录上一帧下达命令的结果
        DebugText(("命令 " + QString::number(id_Wrong_lastFrame) + " 的返回为 " + QString::number(order_Record)).toStdString());
        id_Wrong_lastFrame=-1;
        timers--;                                           //只打印一次
    }


    InitializeMap();                                        //初始化地图数组函数
    DrawMap();                                              //遍历地图获取最新状态
    writeMapToFile();
    start_x=Center_x;
    start_y=Center_y;
    startcamp_x=Center_x+6;
    startcamp_y=Center_y/2;
    startmarket_x=Center_x+6;
    startmarket_y=Center_y/2;
    bool houseCompleted[5] = {false,false,false,false,false};  //跟踪房子是否完成
    for(tagBuilding building:getInfo().buildings)
    {
        //持续造农民
        if(building.Type==BUILDING_CENTER && building.Project==ACT_NULL && getInfo().farmers.size()<11 &&myInfo.Meat >=80)
        {
            BuildingAction(building.SN,BUILDING_CENTER_CREATEFARMER);
            currentFarmers++;
        }
        //兵营
        if(building.Type==BUILDING_ARMYCAMP && building.Percent==100)
        {
            isArmycamp=true;
            //升级棍棒兵为斧头兵
            if(myInfo.Meat>=BUILDING_ARMYCAMP_CREATE_CLUBMAN_FOOD)
            {
                BuildingAction(building.SN,BUILDING_ARMYCAMP_UPGRADE_CLUBMAN);
            }
        }
        //更新盖房子进度
        if(building.Type==BUILDING_HOME)
        {
            for(int i=0;i<5;i++)
            {
                if (!houseCompleted[i] && building.Percent == 100)
                {
                    houseProgress[i] = 100;
                    houseCompleted[i] = true; // 标记该房子已经完成
                    break;
                }
            }
        }

        if(building.Type==BUILDING_HOME && houseCompleted[2]==true && !ishouse3)
        {
            ishouse3=true;
            qDebug()<<ishouse3;
        }
        if(building.Type==BUILDING_MARKET && building.Percent == 100)
        {
            isMarket=true;
        }
    }
    //遍历农民
    for(int i=0;i<myInfo.farmers.size();i++)
    {
        nowState_Farmer = myInfo.farmers[i].NowState;

        //令第0号农民先盖2个房子，再盖一个兵营，再盖一房子
        if(i==0 && house<5 &&(nowState_Farmer==HUMAN_STATE_IDLE||nowState_Farmer==HUMAN_STATE_STOP) && myInfo.Wood>=BUILD_HOUSE_WOOD)
        {
            int homeLength = 2;
            int homeWidth = 2;
            int numhome = 5;
            int totaLength = homeLength * numhome;

            if(isEmptyToBuildHome(start_x,start_y,homeWidth,totaLength))
            {
                static int x_home=start_x;
                static int y_home=start_y;
                //qDebug() << "(" << x_home<< ", " << y_home << ")";
                //先建2个房子
                if(house<2 || (isMarket&&house<5))
                {
                    qDebug() << "Building house, current house count: " << house;
                    HumanBuild(myInfo.farmers[i].SN,BUILDING_HOME, x_home, y_home);
                    y_home+=2;
                    house++;
                    qDebug() << "House built, updated house count: " << house;
                }
                //1.材料不够则砍树,砍完一棵才能建兵营
                //2.材料够则建兵营
                else if(house == 2 && !isArmycamp )
                {
                    if(myInfo.Wood < BUILD_ARMYCAMP_WOOD)
                    {
                        if(myInfo.farmers[i].WorkObjectSN != -1)
                        {
                            SN_res=myInfo.farmers[i].WorkObjectSN;
                        }
                        else
                        {
                            SN_res=-1;
                            dis=1e6;
                            for(int j=0;j<myInfo.resources.size();j++)
                            {
                                temp_dis=Distance(myInfo.farmers[i].BlockDR,myInfo.farmers[i].BlockUR,myInfo.resources[j].BlockDR,myInfo.resources[j].BlockUR);
                                if(myInfo.resources[j].Type ==RESOURCE_TREE && temp_dis<dis)
                                {
                                    dis=temp_dis;
                                    SN_res=myInfo.resources[j].SN;
                                }

                            }
                            myInfo.farmers[i].WorkObjectSN=SN_res;
                        }
                        if(SN_res != -1)
                        {
                            HumanAction(myInfo.farmers[i].SN,SN_res);
                        }
                    }
                    else
                    {
                        int armycampLength=3;
                        int armycampWidth=3;
                        if(isEmptyToBuildArmycamp(startcamp_x,startcamp_y,armycampLength,armycampWidth))
                        {
                            if(armycamp<1)
                            {
                                HumanBuild(myInfo.farmers[i].SN,BUILDING_ARMYCAMP,startcamp_x,startcamp_y);
                                armycamp++;
                            }
                        }
                    }

                }
                //建第3个房子
                else if(house==2&&isArmycamp)
                {
                    HumanBuild(myInfo.farmers[i].SN, BUILDING_HOME, x_home, y_home);
                    y_home+=2;
                    house++;
                }
            }
        }
        //第0号农民盖完3个房子后
        //1.材料不够则砍树,砍完一棵才能建市场
        //2.材料够则建市场
        if (i == 0 && ishouse3 && (nowState_Farmer == HUMAN_STATE_IDLE || nowState_Farmer == HUMAN_STATE_STOP))
        {
            if(myInfo.Wood<BUILD_MARKET_WOOD && !isMarket)
            {
                if (myInfo.farmers[i].WorkObjectSN != -1)
                {
                    SN_res = myInfo.farmers[i].WorkObjectSN;
                }
                else
                {
                    SN_res = -1;
                    dis = 1e6;
                    for (int j = 0; j < myInfo.resources.size(); j++)
                    {
                        temp_dis = Distance(myInfo.farmers[i].BlockDR, myInfo.farmers[i].BlockUR, myInfo.resources[j].BlockDR, myInfo.resources[j].BlockUR);
                        if(myInfo.resources[j].Type == RESOURCE_TREE && temp_dis<dis)
                        {
                            dis = temp_dis;
                            SN_res = myInfo.resources[j].SN;
                        }
                    }
                    myInfo.farmers[i].WorkObjectSN = SN_res;
                }
                if (SN_res != -1)
                {
                    HumanAction(myInfo.farmers[i].SN, SN_res);
                }
            }
            else if(house==3 && !isMarket)
            {
                int marketLength=3;
                int marketWidth=3;
                if(isEmptyToBuildMarket(startmarket_x,startmarket_y,marketLength,marketWidth))
                {
                    if(market<1)
                    {
                        HumanBuild(myInfo.farmers[i].SN,BUILDING_MARKET,startmarket_x,startmarket_y);
                        market++;
                    }
                }
            }

        }
        //0号农民盖完房子继续盖仓库
        /*if(i==0 && ishouse3==true && isStock_Stone==false && myInfo.Wood>=BUILD_STOCK_WOOD && (nowState_Farmer==HUMAN_STATE_IDLE||nowState_Farmer==HUMAN_STATE_STOP))
           {
               StoneDis1=Distance(myInfo.farmers[i].BlockDR,myInfo.farmers[i].BlockUR,CenterStone_x1,CenterStone_y2);
               StoneDid2=Distance(myInfo.farmers[i].BlockDR,myInfo.farmers[i].BlockUR,CenterStone_x2,CenterStone_y2);
               if(StoneDis1<=StoneDid2)
               {
                         if(isFindBestStockLocation1(bestX_stock,bestY_stock))
                         {
                             HumanBuild(myInfo.farmers[i].SN,BUILDING_STOCK,bestX_stock,bestY_stock);
                             isStock_Stone=true;
                         }

               }
               else
               {
                           if(isFindBestStockLocation2(bestX_stock,bestY_stock))
                           {
                             HumanBuild(myInfo.farmers[i].SN,BUILDING_STOCK,bestX_stock,bestY_stock);
                             isStock_Stone=true;
                           }
               }
            }
            */

        //令第1号农民盖谷仓并研发箭塔
        if(i==1  && (nowState_Farmer==HUMAN_STATE_IDLE||nowState_Farmer==HUMAN_STATE_STOP)&& !isGranary)
        {
            if(isFindBestGranaryLocation(bestX_granary,bestY_granary))
            {
                HumanBuild(myInfo.farmers[i].SN,BUILDING_GRANARY,bestX_granary, bestY_granary);
                granary++;
            }
            if(granary==1)
            {
                isGranary=true;
            }

        }
        //if(i==1 && (nowState_Farmer==HUMAN_STATE_IDLE||nowState_Farmer==HUMAN_STATE_STOP) && isGranary )
        //研发箭塔
        for(tagBuilding building1:getInfo().buildings)
        {
            if(building1.Type==BUILDING_GRANARY && isGranary==true &&myInfo.Meat>=BUILDING_GRANARY_ARROWTOWER_FOOD)
            {

                BuildingAction(building1.SN,BUILDING_GRANARY_ARROWTOWER);
                //qDebug()<<building1.ProjectPercent;
                //qDebug()<<building1.Percent;
                if(building1.Percent==100)
                {
                    isResearchArchertower=true;
                    //qDebug()<<"箭塔完成";
                }
            }
        }

        //序号2,3,4采浆果之后采石头,8-10采浆果
        if((i>=2&&i<=4||i<=10&&i>=8) &&nowState_Farmer==HUMAN_STATE_IDLE)
        {
            isBushFinished=false;
            //先采浆果
            if(!unassignedBushes.empty())                     //若容器不为空
            {
                int bushSN = unassignedBushes.back();         //获取最后未分配浆果序号

                unassignedBushes.pop_back();                  //移除改序号

                HumanAction(myInfo.farmers[i].SN,bushSN);
                for(int j=0;j<myInfo.resources.size();j++)
                {
                    if(bushSN==myInfo.resources[j].SN)
                    {
                        if(myInfo.resources[j].Cnt==0)
                        {
                            isBushFinished=true;
                        }
                    }
                }
            }
            //第2,3,4号村民采完浆果后打瞪羚
            else if(!isBushFinished && i>=2 && i<=4)
            {
                SN_res=-1;
                dis=1e6;
                for(int j=0;j<myInfo.resources.size();j++)
                {
                    temp_dis=Distance(myInfo.farmers[i].BlockDR,myInfo.farmers[i].BlockUR,myInfo.resources[j].BlockDR,myInfo.resources[j].BlockUR);
                    if(myInfo.resources[j].Type==RESOURCE_GAZELLE && temp_dis<dis)
                    {
                        if(myInfo.resources[j].Blood==0)
                        {
                            continue;
                        }
                        dis=temp_dis;
                        SN_res=myInfo.resources[j].SN;
                    }

                }
                if(myInfo.farmers[2].NowState==myInfo.farmers[3].NowState && (myInfo.farmers[3].NowState==HUMAN_STATE_IDLE||myInfo.farmers[3].NowState==HUMAN_STATE_GOTO_OBJECT))
                {
                    HumanAction(myInfo.farmers[2].SN,SN_res);
                    HumanAction(myInfo.farmers[3].SN,SN_res);
                }
            }

            //第8,9,10号村民采完浆果后采石头
            else if(!isBushFinished && i<=10 &&i>=8)
            {
                SN_res=-1;
                dis=1e6;
                for(int j=0;j<myInfo.resources.size();j++)
                {
                    temp_dis=Distance(myInfo.farmers[i].BlockDR,myInfo.farmers[i].BlockUR,myInfo.resources[j].BlockDR,myInfo.resources[j].BlockUR);
                    if(myInfo.resources[j].Type==RESOURCE_STONE && temp_dis<dis)
                    {
                        dis=temp_dis;
                        SN_res=myInfo.resources[j].SN;
                    }
                }
                HumanAction(myInfo.farmers[i].SN,SN_res);
            }

        }
        if(i>=5 && i<=6 && nowState_Farmer==HUMAN_STATE_IDLE)
        {
            if(myInfo.farmers[i].WorkObjectSN != -1)
            {
                SN_res=myInfo.farmers[i].WorkObjectSN;
            }
            else
            {
                SN_res=-1;
                dis=1e6;
                for(int j=0;j<myInfo.resources.size();j++)
                {
                    temp_dis=Distance(myInfo.farmers[i].BlockDR,myInfo.farmers[i].BlockUR,myInfo.resources[j].BlockDR,myInfo.resources[j].BlockUR);
                    if(myInfo.resources[j].Type ==RESOURCE_TREE && temp_dis<dis)
                    {
                        dis=temp_dis;
                        SN_res=myInfo.resources[j].SN;
                    }
                }
                myInfo.farmers[i].WorkObjectSN=SN_res;
            }
            if(SN_res != -1)
            {
                HumanAction(myInfo.farmers[i].SN,SN_res);
            }
        }

        if((i>6 && i<=7||i>10) && nowState_Farmer==HUMAN_STATE_IDLE)
        {
            if(myInfo.farmers[i].WorkObjectSN != -1)
            {
                SN_res=myInfo.farmers[i].WorkObjectSN;
            }
            else
            {
                SN_res=-1;
                dis=1e6;
                for(int j=0;j<myInfo.resources.size();j++)
                {
                    temp_dis=Distance(Stock_x,Stock_y,myInfo.resources[j].BlockDR,myInfo.resources[j].BlockUR);
                    if(myInfo.resources[j].Type ==RESOURCE_TREE && temp_dis<dis)
                    {
                        dis=temp_dis;
                        SN_res=myInfo.resources[j].SN;
                    }
                }
                myInfo.farmers[i].WorkObjectSN=SN_res;
            }
            if(SN_res != -1)
            {
                HumanAction(myInfo.farmers[i].SN,SN_res);
            }
        }


        if(i==1 &&(nowState_Farmer==HUMAN_STATE_IDLE||nowState_Farmer==HUMAN_STATE_STOP) && isResearchArchertower==true && myInfo.Stone>=BUILD_ARROWTOWER_STONE)
        {

            RangeToBuildArrowtower(Center_x,Center_y,15,20,21);
            for (const auto& pos : towerPositions) {
                qDebug()<< "Position: (" << pos.first << ", " << pos.second << ")";
            }
            if(!towerPositions.empty())
            {
                auto pos = towerPositions.front();                                   // 获取第一个位置
                int tower_x=pos.first;
                int tower_y=pos.second;
                HumanBuild(myInfo.farmers[i].SN,BUILDING_ARROWTOWER,tower_x,tower_y);
                //qDebug()<<"ok";
                towerPositions.erase(towerPositions.begin());
            }
        }
    }
}
//初始化地图数组
void UsrAI::InitializeMap()
{
    for(int i=0;i< MAP_SIZE;i++)
    {
        for(int j=0;j< MAP_SIZE;j++)
        {
            gamemap[i][j]=myInfo.theMap[i][j]+'0';
        }
    }
}

//遍历地图函数
void UsrAI::DrawMap()
{
    //遍历资源
    stoneCount = 0;
    CenterStone_x1=0;
    CenterStone_y1=0;
    CenterStone_x2=0;
    CenterStone_y2=0;
    int stoneNumber=0;                                                  //石头总数
    for(tagResource resource:getInfo().resources)
    {
        if(resource.Type==RESOURCE_STONE)
        {
            stoneNumber++;                                              //获取石头总数
        }
    }
    //qDebug()<<stoneNumber;
    for(tagResource resource:getInfo().resources)
    {
        //浆果部分
        if(resource.Type==RESOURCE_BUSH)
        {
            if(bushCount<MAX_BUSHES)
            {
                BushPosition[bushCount].x=resource.BlockDR;
                BushPosition[bushCount].y=resource.BlockUR;
                bushCount++;
                unassignedBushes.push_back(resource.SN);            //储存未分配浆果序号
            }
        }
        //石头部分
        if(resource.Type==RESOURCE_STONE)
        {
            StonePosition[stoneCount].x=resource.BlockDR;
            StonePosition[stoneCount].y=resource.BlockUR;
            if(stoneCount<11)
            {
                CenterStone_x1+=resource.BlockDR;
                CenterStone_y1+=resource.BlockUR;
            }
            if(stoneCount>=11&&stoneCount<stoneNumber)
            {
                CenterStone_x2+=resource.BlockDR;
                CenterStone_y2+=resource.BlockUR;
            }
            //qDebug()<<"石头坐标；("<<StonePosition[stoneCount].x<<","<<StonePosition[stoneCount].y<<")";
            stoneCount++;

        }
        //瞪羚部分
        if(resource.Type==RESOURCE_GAZELLE)
        {
            if(gazelleCount<MAX_GAZELLE)
            {
                aliveGazelles.push_back(resource.SN);                        //储存存活羊序列号
                gazelleCount++;
            }
            aliveGazelles_Blood.push_back(resource.Blood);
        }
        gamemap[resource.BlockDR][resource.BlockUR]='A'+resource.Type;
    }
    CenterStone_x1/=11;
    CenterStone_y1/=11;
    CenterStone_x2/=(stoneNumber-11);
    CenterStone_y2/=(stoneNumber-11);
    //      qDebug() << "CenterStone_x1: " << CenterStone_x1;
    //      qDebug() << "CenterStone_y1: " << CenterStone_y1;
    //      qDebug() << "CenterStone_x2: " << CenterStone_x2;
    //      qDebug() << "CenterStone_y2: " << CenterStone_y2;
    //遍历建筑
    for(tagBuilding building:getInfo().buildings)
    {
        int buildingWidth = 3;
        int buildingLength= 3;
        int campWidth = 2;
        int campLength= 2;
        //获取市镇中心坐标
        if(building.Type== BUILDING_CENTER)
        {
            Center_x=building.BlockDR+1;
            Center_y=building.BlockUR+1;
        }
        //获取仓库坐标
        if(building.Type == BUILDING_STOCK)
        {
            Stock_x=building.BlockDR+1;
            Stock_y=building.BlockUR+1;
        }
        // 标记整个建筑区域
        if(building.Type== BUILDING_HOME||building.Type==BUILDING_ARROWTOWER)                       //房屋占2*2
        {
            for (int i = 0; i < campWidth; i++)
            {
                for (int j = 0; j < campLength; j++)
                {
                    gamemap[building.BlockDR+i][building.BlockUR+j]='a'+building.Type;
                }
            }
        }
        else                                                    //其他建筑占3*3
        {
            for (int i = 0; i < buildingWidth; i++)
            {
                for (int j = 0; j < buildingLength; j++)
                {
                    gamemap[building.BlockDR+i][building.BlockUR+j]='a'+building.Type;
                }
            }
        }
    }

    //遍历单位（我方农民和士兵）
    for(tagFarmer farmer:getInfo().farmers)
    {
        gamemap[farmer.BlockDR][farmer.BlockUR]='*';
    }
    for(tagArmy army:getInfo().armies)
    {
        gamemap[army.BlockDR][army.BlockUR]='*';
    }
}

//将最新地图导入文件方便查看
void UsrAI::writeMapToFile()
{
    std::ofstream outFile("PrintMap.txt");
    if (outFile.is_open())
    {
        for (int i = 0; i < MAP_SIZE; i++)
        {
            for (int j = 0; j < MAP_SIZE; j++)
            {
                outFile << static_cast<char>(gamemap[i][j]);         //整型转为字符，gamemap存的是ASCII码值（整型）
            }
            outFile << std::endl;
        }
        outFile.close();
        //qDebug() << "地图已成功写入文件。";
    }
    else
    {
        qDebug() << "无法打开文件。";
    }
}

//判断是否有足够空地建房子,直到找到合适位置
bool UsrAI::isEmptyToBuildHome(int x,int y,int width,int length)
{
    for (int i =x; i >=0;i--)
    {
        for (int j = y; j<MAP_SIZE-length;j++)
        {
            char firstChar1 = static_cast<char>(gamemap[i][j]);                    //获取第一个地图字符，int转char

            if(firstChar1 !='0'&&firstChar1 !='1'&&firstChar1 !='2'&&firstChar1 !='3')
            {
                // qDebug()<<"不符合";
                break;
            }
            bool isEmpty = true;
            for(int k=0 ; k<width ; k++)
            {
                for(int l=0 ; l<length ; l++)
                {
                    //qDebug() << "Checking sub-position: (" << i + k << ", " << j + l << ")";
                    if(static_cast<char>(gamemap[i+k][j+l]) != firstChar1)
                    {
                        isEmpty = false;
                        break;
                    }
                }
                if (!isEmpty) { break; }
            }
            if (isEmpty)
            {
                start_x=i;
                start_y=j;
                //qDebug() << "建房位置: (" << i << ", " << j << ")";
                return true;
            }
        }
    }
    return false;
}

//寻找谷仓的最佳位置
bool UsrAI::isFindBestGranaryLocation(int bestX,int bestY)
{
    int minDistance=12*MAP_SIZE;
    for (int i=0;i<=MAP_SIZE - 3;i++)
    {
        for (int j=0;j<=MAP_SIZE - 3;j++)
        {
            char firstChar2 = static_cast<char>(gamemap[i][j]);
            if(firstChar2 >= '0' && firstChar2 <= '3')
            {
                bool isValid = true;
                for (int x=i;x<i+3;x++)
                {
                    for (int y=j;y<j+3;y++)
                    {
                        if(static_cast<char>(gamemap[x][y]) != firstChar2)
                        {
                            isValid = false;
                            break;
                        }
                    }
                    if (!isValid)
                    {
                        break;
                    }
                }
                if(isValid)
                {
                    int totalDistance = 0;
                    for(int k=0;k<MAX_BUSHES;k++)
                    {
                        int distance=std::abs(i + 1 - BushPosition[k].x) + std::abs(j + 1 - BushPosition[k].y);
                        totalDistance += distance;
                    }
                    if(totalDistance<=minDistance)
                    {
                        minDistance=totalDistance;
                        bestX=i;
                        bestY=j;
                    }
                }
            }
        }
    }
    if (bestX != -1 && bestY != -1)
    {
        //qDebug()<<"最佳谷仓位置: (" << bestX << ", " << bestY << ")" ;
        bestX_granary=bestX;
        bestY_granary=bestY;
        return true;
    }

    return false;
}

//曼哈顿函数算距离
int UsrAI::Distance(int x1,int y1,int x2,int y2)
{
    return std::abs(x1 -x2 ) + std::abs(y1- y2);
}

//寻找石头旁最佳位置建仓库
bool UsrAI::isFindBestStockLocation1(int x,int y)
{
    int minDistance1=12*MAP_SIZE;
    for (int i=0;i<MAP_SIZE - 3;i++)
    {
        for (int j=0;j<MAP_SIZE - 3;j++)
        {
            char firstChar3 = static_cast<char>(gamemap[i][j]);
            if(firstChar3 >= '0' && firstChar3 <= '3')
            {
                bool isValid = true;
                for (int x=i;x<i+3;x++)
                {
                    for (int y=j;y<j+3;y++)
                    {
                        if(static_cast<char>(gamemap[x][y]) != firstChar3)
                        {
                            isValid = false;
                            break;
                        }
                    }
                    if (!isValid)
                    {
                        break;
                    }
                }
                if(isValid)
                {
                    int distance1=std::abs(i + 1 - CenterStone_x1) + std::abs(j + 1 - CenterStone_y1);
                    if(distance1<=minDistance1)
                    {
                        minDistance1=distance1;
                        x=i;
                        y=j;
                    }
                }
            }
        }
    }
    if (x != -1 && y != -1)
    {
        qDebug()<<"最佳仓库位置: (" << x << ", " << y << ")" ;
        bestX_stock=x;
        bestY_stock=y;
        return true;
    }
    return false;
}

//寻找石头旁最佳位置建仓库
bool UsrAI::isFindBestStockLocation2(int x,int y)
{
    int minDistance2=12*MAP_SIZE;
    for (int i=0;i<MAP_SIZE - 3;i++)
    {
        for (int j=0;j<MAP_SIZE - 3;j++)
        {
            char firstChar4 = static_cast<char>(gamemap[i][j]);
            if(firstChar4 >= '0' && firstChar4 <= '3')
            {
                bool isValid = true;
                for (int x=i;x<i+3;x++)
                {
                    for (int y=j;y<j+3;y++)
                    {
                        if(static_cast<char>(gamemap[x][y]) != firstChar4)
                        {
                            isValid = false;
                            break;
                        }
                    }
                    if (!isValid)
                    {
                        break;
                    }
                }
                if(isValid)
                {
                    int distance2=std::abs(i + 1 - CenterStone_x2) + std::abs(j + 1 - CenterStone_y2);
                    if(distance2<=minDistance2)
                    {
                        minDistance2=distance2;
                        x=i;
                        y=j;
                    }
                }
            }
        }
    }
    if (x != -1 && y != -1)
    {
        qDebug()<<"最佳仓库位置: (" << x << ", " << y << ")" ;
        bestX_stock=x;
        bestY_stock=y;
        return true;
    }
    return false;
}

//寻找箭塔位置
void UsrAI::RangeToBuildArrowtower(int centerX, int centerY, int inR, int outR, int minDistance)
{
    for (int i = 0; i < MAP_SIZE; i++)
    {
        for (int j = 0; j < MAP_SIZE; j++)
        {
            int distanceToCenter=Distance(centerX,centerY,i,j);
            if(distanceToCenter>inR && distanceToCenter<outR)
            {
                bool tooCloseToTower = false;
                // 检查当前位置是否与已放置的箭塔太近
                for (const auto& pos : towerPositions)               //自动推导pos
                {
                    int distanceToTwoTower = Distance(i, j, pos.first, pos.second);
                    if (distanceToTwoTower < minDistance)
                    {
                        tooCloseToTower = true;
                        break;
                    }
                }
                if(!tooCloseToTower && isValidTowerLocation(i,j)==true && towerCount<=MAX_TOWER)
                {
                    towerPositions.push_back({i,j});
                    towerCount++;
                    // qDebug()<< "Position: (" << pos.first << ", " << pos.second << ")" ;
                }
            }

        }
    }
}
//计算两点间距离
double UsrAI::DistanceToPoints(int x1,int y1,int x2,int y2)
{
    return std::sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}
//检查箭塔的位置是否为平地
bool UsrAI::isValidTowerLocation(int x,int y)
{
    //检查边界
    if (x + 1 >= MAP_SIZE || y + 1 >= MAP_SIZE)
    {
        return false;
    }
    char TowerChar=static_cast<char>(gamemap[x][y]);
    if(TowerChar=='0'||TowerChar=='1'||TowerChar=='2'||TowerChar=='3')
    {
        if(gamemap[x][y] == TowerChar && gamemap[x + 1][y] == TowerChar && gamemap[x][y + 1]==TowerChar && gamemap[x + 1][y + 1]==TowerChar)
        {
            return true;
        }
    }
    return false;
}
//找兵营位置
bool UsrAI::isEmptyToBuildArmycamp(int x, int y, int length, int width)
{
    for (int i =x; i<MAP_SIZE-length;i++)
    {
        for (int j = y; j<MAP_SIZE-width;j++)
        {
            bool isEmpty = true;
            for(int k=0 ; k<width ; k++)
            {
                for(int l=0 ; l<length ; l++)
                {
                    char firstChar = static_cast<char>(gamemap[i+k][j+l]);
                    //qDebug() << "Checking sub-position: (" << i + k << ", " << j + l << ")";
                    if(firstChar !='0'&&firstChar !='1'&&firstChar !='2'&&firstChar !='3')
                    {
                        isEmpty = false;
                        break;
                    }
                }
                if (!isEmpty) { break; }
            }
            if (isEmpty)
            {
                startcamp_x=i;
                startcamp_y=j;
                //qDebug() << "兵营位置: (" << i << ", " << j << ")";
                return true;
            }
        }
    }
    return false;
}
//找市场位置
bool UsrAI::isEmptyToBuildMarket(int x, int y, int length, int width)
{
    for (int i =x; i<MAP_SIZE-length;i++)
    {
        for (int j = y; j<MAP_SIZE-width;j++)
        {
            bool isEmpty = true;
            for(int k=0 ; k<width ; k++)
            {
                for(int l=0 ; l<length ; l++)
                {
                    char firstChar = static_cast<char>(gamemap[i+k][j+l]);
                    if(firstChar !='0'&&firstChar !='1'&&firstChar !='2'&&firstChar !='3')
                    {
                        isEmpty = false;
                        break;
                    }
                }
                if (!isEmpty) { break; }
            }
            if (isEmpty)
            {
                startmarket_x=i;
                startmarket_y=j;
                //qDebug() << "市场位置: (" << i << ", " << j << ")";
                return true;
            }
        }
    }
    return false;
}
