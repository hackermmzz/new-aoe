#include "UsrAI.h"
tagGame tagUsrGame;
ins UsrIns;
/*##########请勿修改以上部分##########*/
#include "set"
#include "QDebug"
#include<iostream>
using namespace std;
using Pos=pair<int,int>;
double mid=64*BLOCKSIDELENGTH;
int house;
int id_Wrong_lastFrame = -1;
int Map[128][128];
int CenterX,CenterY;
tagInfo info;
set<int>FarmerbeUsed;
bool DockIsBuild;
bool IsCangkuBuild=false;
bool UpdateToFTB;
bool IsGuoHe;
bool IsGuCangBuilt;
int ret=999;

void UsrAI::processData()
{
        info=getInfo();
        tagBuilding center=info.buildings.front();
            CenterX=center.BlockDR,CenterY=center.BlockUR;
         ditu();
         CreateFarmer();

         BuildHome();
         //CutDownTree();
         //CreateArmy();
         //GoToWork();

         BuildDock();
         PickGold();
         GuoHe();
         //UpdateWeapon();
         //PickFish();
         CreatBoat();
         //SailingAttack();
         Fish();
}
    void UsrAI::ditu(){
        memcpy(Map,info.theMap,sizeof(Map));
        for(int i=0;i<info.resources.size();++i)
        {
            tagResource rsc=info.resources[i];
                   tagBuilding e;
                   int x=rsc.BlockDR,y=rsc.BlockUR;
                           Map[x][y]=5;
        }
        for(int i=0;i<info.farmers.size();++i)
            {
                tagFarmer farmer=info.farmers[i];
                int x=farmer.BlockDR,y=farmer.BlockUR;
                Map[x][y]=5;
            }
        for(int i=0;i<info.buildings.size();++i)
            {
                tagBuilding building=info.buildings[i];
                int x=building.BlockDR,y=building.BlockUR;
                int w=GetBuildingWidth(building.Type,124234);
                for(int i=0;i<w;++i)
                {
                    for(int j=0;j<w;++j)
                    {
                        Map[i+x][j+y]=5;
                    }
                }
            }

    }
    tagFarmer UsrAI::GetFarmer()//找到空闲且未被使用的农民
    {
        for(tagFarmer&farmer:info.farmers)
        {
            if(farmer.WorkObjectSN==-1&&FarmerbeUsed.count(farmer.SN)==0)
            {
                FarmerbeUsed.insert(farmer.SN);
                return farmer;
            }
        }
        tagFarmer farmer;
        farmer.SN=-1;
        return farmer;
    }
    void UsrAI::GetFarmer(tagFarmer &farmer)
    {
        bool find=0;
        for(auto&f:info.farmers)//便利容器内的每个元素
            if(f.SN==farmer.SN)
            {
                find=1;
                farmer=f;
                break;
            }
        if(!find)
            farmer=GetFarmer();
    }
    void UsrAI::RecycleFarmer(tagFarmer &farmer)
    {
        FarmerbeUsed.erase(farmer.SN);
        farmer.SN=-1;
    }
    int UsrAI::GetBuildingWidth(int Type, int reservced)
    {
        if(Type==BUILDING_DOCK)return 2;
        if(Type==BUILDING_HOME)return 2;
        return 3;
    }
    pair<int, int> UsrAI::GetFittestBlockToBuildHouse(int houseType, int x, int y, int interval, int reserved0, int reserved1)//妫€鏌ユ渶鍚堥€傜殑寤洪€犱綅缃?
    {
       int w=GetBuildingWidth(houseType,1234509);
       int dis=1e8+10;
       pair<int,int>res={-456,-789};
       for(int i=0;i<MAP_L;++i)
       {
           for(int j=0;j<MAP_U;++j)
           {
               bool t=CheckTaretBlockCanBuildTargetTypeBuilding(houseType,i,j,interval);
               if((t&1))
               {
                   int a=i+w/2-x;
                   int dd=abs(a);
                   int b=j+w/2-y;
                   dd+=abs(b);
                   if(dd<dis)
                   {
                       res={i,j};
                       dis=dd;
                   }
               }
           }
       }
       return res;
    }
    bool UsrAI::CheckTaretBlockCanBuildTargetTypeBuilding(int houseType, int x, int y,int interval=0)
    {//检查是否可以建造指定类型的建筑物
        int w=GetBuildingWidth(houseType,75);
        int h=Map[x][y];
        if((h<0&&houseType!=BUILDING_DOCK)||h>4)return 0;
        for(int i=0-interval;i<w-(-interval);++i)
        {
            for(int j=0-interval;j<w-(-interval);++j)
            {
                int xx=x+i,yy=y+j;
                if(xx>=MAP_L)return 0;
                if(yy>=MAP_U)return 0;
                if(xx>=0&&yy>=0&&Map[xx][yy]!=h)return 0;
            }
        }
        return 1;
    }
    pair<int, int> UsrAI::GetResourceCenter(vector<tagResource> &rsc)
    {
        int x=0,y=0;
        for(auto&r:rsc)
            x+=r.BlockDR,y+=r.BlockUR;
        return Pos{x/rsc.size(),y/rsc.size()};
    }
    void UsrAI::PickGold(){
        static tagFarmer farmer[4];
         static bool init=1;
         if(init)
             {
                 init=0;
                 for(auto&f:farmer)f.SN=-1;
             }
         bool IsGoldLeft=0;
         for(tagResource&rsc:info.resources)
                 if(rsc.Type==RESOURCE_GOLD)
                 {
                     IsGoldLeft=1;
                     break;
                 }
         if(!IsGoldLeft){
                for(auto&f:farmer)
                    RecycleFarmer(f);
               return;
            }/*判断农民是否过河，如果过了河选择合适的位置建造仓库
               然后再根据仓库是否建成选择采金子*/
         if(IsGuoHe){
             if(IsGuCangBuilt==0&&farmer[0].WorkObjectSN==-1)
                 {
                     vector<tagResource>rsc;
                     for(auto&r:info.resources)
                     {
                         if(r.Type==RESOURCE_GOLD)rsc.push_back(r);
                     }
                     Pos center=GetResourceCenter(rsc);
                     Pos p=GetFittestBlockToBuildHouse(BUILDING_GRANARY,center.first,center.second,0,1,1);
                     HumanBuild(farmer[0].SN,BUILDING_GRANARY,p.first,p.second);
                 }
             else if(IsGuCangBuilt==0&&farmer[1].WorkObjectSN==-1)
                     HumanAction(farmer[1].SN,farmer[0].WorkObjectSN);
                 else{
                     static int GuCangSn=-1;
                     GuCangSn=farmer[0].WorkObjectSN;
                     for(auto&house:info.buildings)
                         if(house.SN==GuCangSn)
                         {
                             if(house.Percent==100)IsGuCangBuilt=1;
                             break;
                         }
                 }
             if(IsGuCangBuilt)
                {
                    GetFarmer(farmer[2]);
                    GetFarmer(farmer[3]);
                    for(tagFarmer&f:farmer)
                    {
                        if(f.WorkObjectSN==-1)
                        {
                            float MinDis=1e8+10;
                            int targetSn=-1;
                            for(auto&rsc:info.resources)
                            {
                                if(rsc.Type!=RESOURCE_GOLD)continue;
                                float dis=(rsc.DR-f.DR)*(rsc.DR-f.DR)+(rsc.UR-f.UR)*(rsc.UR-f.UR);
                                if(dis<MinDis)
                                {
                                    targetSn=rsc.SN;
                                    MinDis=dis;
                                }
                            }
                            if(targetSn!=-1)//找到目标资源，农民去采资源
                                HumanAction(f.SN,targetSn);
                        }

                    }
                }
         }
    }
    bool UsrAI::CheckCanBuildDock(int x, int y){
        const static int w=GetBuildingWidth(BUILDING_DOCK,0);
        //只能建在海洋里面
        for(int i=0;i<w;++i){
            for(int j=0;j<w;++j){
                int ii=i+x,jj=j+y;
                if(ii<0||ii>=MAP_L||jj<0||jj>=MAP_U)return 0;
                if(info.theMap[ii][jj].type!=MAPPATTERN_OCEAN)return 0;
            }
        }
        return 1;
    }
    pair<int,int> UsrAI::FindNearstBeachToBuild(int houseType,int x,int y,int reserved){
            int w = GetBuildingWidth(houseType, 1234509);
            int dis = 1e8 + 10;
            pair<int, int> res = {-1, -1};
            for (int i = 0; i < MAP_L; ++i) {
                for (int j = 0; j < MAP_U; ++j) {
                        bool f=CheckCanBuildDock(i,j);
                       if(f){
                        int a = i + w / 2 - x;
                        int dd = abs(a);
                        int b = j + w / 2 - y;
                        dd += abs(b);
                        if (dd < dis) {
                            res = {i, j};
                            dis = dd;
                     }
                }
            }
            }
            return res;

    }

    void UsrAI::Fish()
    {
        static bool init = 1;
           if (init) {
               init = 0;
               std::vector<tagFarmer> sailingFarmers;
               for (const tagFarmer& f : info.farmers) {
                          if (f.FarmerSort == FARMERTYPE_SAILING) {
                              sailingFarmers.push_back(f);  // 将渔船加入列表
                          }
                      }
                std::vector<tagResource> fishResources;
               for (const tagResource& res : info.resources) {
                          if (res.Type == RESOURCE_FISH) {
                              fishResources.push_back(res);  // 将鱼资源加入列表
                          }
                      }
               for (const tagFarmer& farmer : sailingFarmers) {
                           if (!fishResources.empty()) {
                               // 假设每艘船都去最近的鱼资源点打鱼
                               HumanAction(farmer.SN, fishResources[0].SN);  // 让渔船去打鱼
                           }
                       }
                   }
    }

    void UsrAI::BuildDock(){
        static tagFarmer farmer[1];
        static int Max=3;
        int cnt=0;
        static bool init=1;
        if(init){
            init=0;
            farmer[1].SN=-154;
        }
        for(int i=0;i<info.buildings.size();++i){
            tagBuilding h=info.buildings[i];
            if(h.Type==BUILDING_DOCK){
                if(h.Percent==100){
                    DockIsBuild=1;
                    cnt++;
                }
            }
        }
        if(cnt>=Max){
            RecycleFarmer(farmer[0]);
                    return;
        }
            if(info.Wood<BUILD_DOCK_WOOD)return;
                GetFarmer(farmer[0]);
                //cout<<farmer[0].SN<<endl;
                if(farmer[0].WorkObjectSN!=-1) return;
                if(farmer[0].FarmerSort!=FARMERTYPE_FARMER) return;
            Pos p=FindNearstBeachToBuild(BUILDING_DOCK,CenterX,CenterY,1);
            // cout<<p.first<<" "<<p.second<<endl;
            HumanBuild(farmer[0].SN,BUILDING_DOCK,p.first,p.second);
    }


    tagFarmer UsrAI::boat(){
        for(tagFarmer&farmer:info.farmers)
        {
            if(farmer.WorkObjectSN==-1&&FarmerbeUsed.count(farmer.SN)==0)
            {
                if(farmer.FarmerSort==FARMERTYPE_WOOD_BOAT){
                    FarmerbeUsed.insert(farmer.SN);
                    return farmer;
                }
            }
        }
        tagFarmer farmer;
        farmer.SN=-1;
        return farmer;
    }
    void UsrAI::boat(tagFarmer&farmer){
        bool find=0;
        for(auto&f:info.farmers)
            if(f.SN==farmer.SN){
                find=1;
                farmer=f;
                break;
            }
        if(!find){
            farmer=boat();
        }
    }
    /*pair<double,double>UsrAI::FindNearstGold(double x,double y){
        double MinDis=1e8+10;
        pair<double,double>nearest={-1,-1};
        for(auto&rsc:info.resources)
                 {
            if(rsc.Type==RESOURCE_BUSH){
                    double dis=(rsc.BlockDR-x)*(rsc.BlockDR-x)+(rsc.BlockUR-y)*(rsc.BlockUR-y);
                    if(dis<MinDis)
                          {
                           MinDis=dis;
                           nearest={rsc.BlockDR,rsc.BlockUR};
                           }
                       }
        }
         return nearest;
    }*/
    void UsrAI::GuoHe(){

        static bool init=1;
        static tagFarmer farmer[1];
        static bool IsGuoHe=0;
        if(init){
            init=0;
            for(int i=0;i<info.farmers.size();i++){
                farmer[i].SN=-1;//对农民序列号初始化
            }
        }
        if(IsGuoHe){
            for(auto&f:farmer)
                RecycleFarmer(f);
            return;//如果过了河就回收船
        }
        boat(farmer[0]);//获得船
        static bool t=1;
        if(t){
            t=0;
        Pos p=FindNearstBeach(farmer[0].BlockDR,farmer[0].BlockUR);//找到离船最近的海岸边

         HumanMove(farmer[0].SN,(p.first+0.5)*BLOCKSIDELENGTH,(p.second+0.5)*BLOCKSIDELENGTH);//靠岸
    }
        int assignedFarmers=0;
        vector<tagFarmer> availableFarmers;
         for(int i=0;i<info.farmers.size();i++){
             tagFarmer&f=info.farmers[i];
                 if(f.WorkObjectSN==-1&&FarmerbeUsed.count(f.SN)==0){
                         if(f.FarmerSort==FARMERTYPE_FARMER){
                 availableFarmers.push_back(f);
                    //cout<<availableFarmers.size()<<endl;
                 }
             }

                 //if(availableFarmers.size()<5){
                  for(int i=0;i<availableFarmers.size()&assignedFarmers<5;i++){
                 //if(assignedFarmers<5){

                        HumanAction(availableFarmers[i].SN,farmer[0].SN);
                        FarmerbeUsed.insert(availableFarmers[i].SN);
                        assignedFarmers++;
                        cout<<assignedFarmers<<endl;
                        //}
                       // }


                     //else{
                           //cout<<"abc";
                           std::vector<tagResource> goldResources;
                           for (const tagResource& res : info.resources) {
                           if (res.Type == RESOURCE_GOLD) {
                           //cout<<res.SN<<endl;
                           goldResources.push_back(res);
                           //cout<<goldResources.size()<<endl;//寻找到金子
                            }
                          }
                            Pos pos=FindNearstBeach(goldResources[0].BlockDR,goldResources[0].BlockUR);
                            //船移动到金子的位置
                                cout<<goldResources[0].SN<<endl;
                              // cout<<goldResources[0].BlockDR<<" "<<goldResources[0].BlockUR<<endl;
                              cout<<pos.first<<" "<<pos.second<<endl;
                           //HumanAction(farmer[0].SN,goldResources[0].SN);
                              HumanMove(farmer[0].SN,(pos.first*0.5)*BLOCKSIDELENGTH,(pos.second*0.5)*BLOCKSIDELENGTH);
                         }

                 }
    }
    //}





    void UsrAI::CreateFarmer()
    {
        tagBuilding center=info.buildings.front();
        if(center.Project==ACT_NULL&&info.Meat>=BUILDING_CENTER_CREATEFARMER_FOOD&&info.farmers.size()<25){
            BuildingAction(center.SN,BUILDING_CENTER_CREATEFARMER);
        }
    }

    bool UsrAI::CheckSea(int x,int y){
        if(info.theMap[x][y].type==MAPPATTERN_OCEAN){
            return 0;
        }
        for(int i=-1;i<=1;++i){
            for(int j=-1;j<=1;++j){
                if(i==0&&j==0) continue;
                int nx=x+i;
                int ny=y+j;
                  if (nx < 0 || nx >= MAP_L || ny < 0 || ny >= MAP_U) continue;
                  if(info.theMap[nx][ny].type==MAPPATTERN_OCEAN)
                  return 1;
            }
        }
        return 0;
    }
    pair<int,int>UsrAI::FindNearstBeach(int x,int y){
        pair<int,int>nearest={-789,-456};
        int mindis=1e8+10;
        for(int i=0;i<MAP_L;++i){
            for(int j=0;j<MAP_U;++j){
                bool t=CheckSea(i,j);
                if(t){
                   int dis=abs(i-x)+abs(j-y);
                    if(dis<mindis){
                        mindis=dis;
                        nearest={i,j};
                    }
                    }
                }
            }
       return nearest;
    }

    void UsrAI::CreatBoat(){
           int SAIcnt=0,Shipcnt=0,WBcnt=0;
           for (int i = 0; i < info.buildings.size(); ++i) {
               tagBuilding& h = info.buildings[i];
               if (h.Type == BUILDING_DOCK && h.Percent == 100) {
                   if (h.Project == ACT_NULL) {
                       if (WBcnt < 8){
                           if(info.Wood >= BUILDING_DOCK_CREATE_WOOD_BOAT_WOOD) {//运输船
                           BuildingAction(h.SN, BUILDING_DOCK_CREATE_WOOD_BOAT);
                           //cout<<"build wood boat"<<endl;
                           WBcnt++;
                           }
                       }else
                           if (Shipcnt < 8){
                               if(info.Wood >= BUILDING_DOCK_CREATE_SHIP_WOOD) {//渔船
                               BuildingAction(h.SN, BUILDING_DOCK_CREATE_SHIP);
                               Shipcnt++;
                               }
                       }else
                       if (SAIcnt < 8 ){
                           if(info.Wood >= BUILDING_DOCK_CREATE_SAILING_WOOD) {//侦察船
                           BuildingAction(h.SN, BUILDING_DOCK_CREATE_SAILING);
                           SAIcnt++;
                           }
                       }
                   }
               }
           }
    }
    void UsrAI::BuildHome()
    {
        static tagFarmer farmer;
        static int MaxCnt=3;
        static  bool init=1;
        if(init)init=0,farmer.SN=-154;
        int curcnt=0;
        for(int i=0;i<info.buildings.size();++i)
        {
            tagBuilding home=info.buildings[i];
            if(home.Percent==100&&home.Type==BUILDING_HOME)++curcnt;
        }
        if(curcnt>=MaxCnt)
        {
            RecycleFarmer(farmer);
            return;
        }
        if(info.Wood<BUILD_HOUSE_WOOD)return;
        ////////////////////////////////
        GetFarmer(farmer);
        if(farmer.WorkObjectSN!=-1)return;
        Pos pos=GetFittestBlockToBuildHouse(BUILDING_HOME,CenterX,CenterY,1,1,2);
        HumanBuild(farmer.SN,BUILDING_HOME,pos.first,pos.second);
    }


