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

    /*
    //写入你的代码
    myInFo=getInfo();
=======
//    info=getInfo();
//    tagBuilding center=info.buildings.front();
//        CenterX=center.BlockDR,CenterY=center.BlockUR;
//     ditu();
//     CreateFarmer();
>>>>>>> 37a1a2af495adb996219d9390e45aa515020703e

//     if(ret==999){
//         for (int i = 0; i < info.buildings.size(); ++i) {
//             tagBuilding& h = info.buildings[i];
//             if (h.Type == BUILDING_DOCK && h.Percent == 100){
//                 ret =BuildingAction(h.SN, BUILDING_DOCK_CREATE_WOOD_BOAT);
//                 break;
//             }
//         }

//     }
//     auto it = info.ins_ret.find(ret);
//     if (it!= info.ins_ret.end()) {
//         std::cout << it->second << std::endl;
//     } else {
//         std::cout << "未找到对应键值" << std::endl;
//     }

    // BuildHome();
    // CutDownTree();
     //CreateArmy();
    // GoToWork();

    // BuildDock();
     //PickGold();
     //GuoHe();
     //UpdateWeapon();
     //PickFish();
//     CreatBoat();
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
    static bool init=1;
    if(init){
        init=0;
        tagFarmer farmer;
        for(tagFarmer&f:info.farmers){
            if(f.FarmerSort==FARMERTYPE_SAILING){
                farmer=f;
                break;
            }
        }
        tagResource rsc;
        for(tagResource&res:info.resources){
            if(res.Type==RESOURCE_FISH){
                rsc=res;
                break;
            }
        }
        HumanAction(farmer.SN,rsc.SN);
    }
}

void UsrAI::BuildDock(){
    static tagFarmer farmer[1];
    static int Max=5;
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
            cout<<farmer[0].SN<<endl;
            if(farmer[0].WorkObjectSN!=-1) return;
        Pos p=FindNearstBeachToBuild(BUILDING_DOCK,CenterX,CenterY,1);
         cout<<p.first<<" "<<p.second<<endl;
        HumanBuild(farmer[0].SN,BUILDING_DOCK,p.first,p.second);
}

void UsrAI::GoToWork()
{
    vector<tagResource>trees;
    for(auto&rsc:info.resources)
    {
        if(rsc.Type==RESOURCE_TREE)
        {
            trees.push_back(rsc);
        }
    }
    sort(trees.begin(),trees.end(),[&](const tagResource&r0,const tagResource&r1)
    {
        float x=CenterX*BLOCKSIDELENGTH,y=CenterY*BLOCKSIDELENGTH;
        float x0=r0.DR-x,y0=r0.UR-y;
        float x1=r1.DR-x,y1=r1.UR-y;
        return x0*x0+y0*y0<x1*x1+y1*y1;
    });
    int idx=0;
    for(int i=0;i<info.farmers.size();++i)
    {
        tagFarmer&f=info.farmers[i];
        if(FarmerbeUsed.count(f.SN)==0&&f.WorkObjectSN==-1)
        {
            HumanAction(f.SN,trees[(idx++)].SN);
            if(idx>=trees.size())idx=0;
        }
    }
}

void UsrAI::CreateFarmer()
{
    tagBuilding center=info.buildings.front();
    if(center.Project==ACT_NULL&&info.Meat>=BUILDING_CENTER_CREATEFARMER_FOOD&&info.farmers.size()<25){
        BuildingAction(center.SN,BUILDING_CENTER_CREATEFARMER);
    }
}

pair<int,int>UsrAI::FindNearstBeach(int x,int y){
    pair<int,int>nearest={-789,-456};
    int mindis=1e8+10;
    for(int i=0;i<MAP_L;++i){
        for(int j=0;j<MAP_U;++j){
            bool t=CheckCanBuildDock(i,j);
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
                       cout<<"build wood boat"<<endl;
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
    static int MaxCnt=7;
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

void UsrAI::CutDownTree()
{
    static tagFarmer farmer[4];
    static bool init=1;
    static tagBuilding Stock;
    if(init)init=0,memset(farmer,-1,sizeof(farmer)),Stock.SN=-1;
    ////////////////////////////////
    if(Stock.SN==-1)
    {
        for(auto&house:info.buildings)
        {
            if(house.Type==BUILDING_STOCK)
            {
                Stock=house;
                break;
            }
        }
    }
    ////////////////////////////////
    vector<tagResource>trees;
    for(auto&rsc:info.resources)
    {
        if(rsc.Type==RESOURCE_TREE)
        {
            trees.push_back(rsc);
        }
    }
    if(trees.size()==0)
    {
        for(auto&f:farmer)
            RecycleFarmer(f);
        return;
    }
    sort(trees.begin(),trees.end(),[&](const tagResource&r0,const tagResource&r1)
    {
        float x=Stock.BlockDR*BLOCKSIDELENGTH,y=Stock.BlockUR*BLOCKSIDELENGTH;
        float x0=r0.DR-x,y0=r0.UR-y;
        float x1=r1.DR-x,y1=r1.UR-y;
        return x0*x0+y0*y0<x1*x1+y1*y1;
    });
    //////////////////////////
   for(auto&f:farmer)GetFarmer(f);
   ///////////////////////////
   set<int>BeCutTreeSn;
   for(tagFarmer&f:farmer)
       BeCutTreeSn.insert(f.WorkObjectSN);
    int idx=0;
    for(tagFarmer&f:farmer)
    {
        if(f.WorkObjectSN==-1)
        {
            while(idx<trees.size()&&BeCutTreeSn.count(trees[idx].SN))++idx;
            if(idx==trees.size())idx=0;
            HumanAction(f.SN,trees[idx].SN);
        }
    }
}
void UsrAI::PickFish(){
    static tagFarmer farmer[4];
    static bool init=1;
    static bool DockIsBuild;
        if(init)
        {
            init=0;
            for(auto&f:farmer)f.SN=-1;
        }
        ////////////////////////////
        bool IsFishLeft=0;
        for(tagResource&rsc:info.resources)
            if(rsc.Type==RESOURCE_FISH)
            {
                IsFishLeft=1;
                break;
            }
        if(!IsFishLeft){
            for(auto&f:farmer)
                RecycleFarmer(f);
           return;
        }
        ////////////////////////////
        GetFarmer(farmer[0]);
        GetFarmer(farmer[1]);
        if(DockIsBuild==0&&farmer[0].WorkObjectSN==-1)
        {

            vector<tagResource>rsc;
            for(auto&r:info.resources)
            {
                if(r.Type==RESOURCE_FISH)rsc.push_back(r);
            }
            Pos center=GetResourceCenter(rsc);
            Pos p=FindNearstBeachToBuild(BUILDING_DOCK,center.first,center.second,0);
            HumanBuild(farmer[0].SN,BUILDING_DOCK,p.first,p.second);
        }
        else if(DockIsBuild==0&&farmer[1].WorkObjectSN==-1)
            HumanAction(farmer[1].SN,farmer[0].WorkObjectSN);
        else{
            static int DockSn=-1;
            DockSn=farmer[0].WorkObjectSN;
            for(auto&house:info.buildings)
                if(house.SN==DockSn)
                {
                    if(house.Percent==100)DockIsBuild=1;
                    break;
                }
        }
        ///////////////////////////////
        if(DockIsBuild)
        {
            GetFarmer(farmer[2]);
            GetFarmer(farmer[3]);
            qDebug()<<farmer[2].SN;
            for(tagFarmer&farmer:info.farmers)
            {
                if(farmer.FarmerSort==FARMERTYPE_SAILING)
                if(farmer.WorkObjectSN==-1)
                {
                    float MinDis=1e8+10;
                    int targetSn=-1;
                    for(auto&rsc:info.resources)
                    {
                        if(rsc.Type!=RESOURCE_FISH)continue;
                        float dis=(rsc.DR-farmer.DR)*(rsc.DR-farmer.DR)+(rsc.UR-farmer.UR)*(rsc.UR-farmer.UR);
                        if(dis<MinDis)
                        {
                            targetSn=rsc.SN;
                            MinDis=dis;
                        }
                    }
                    if(targetSn!=-1)//找到目标资源，农民去采资源
                        HumanAction(farmer.SN,targetSn);
                }

            }
        }
    }

/*void UsrAI::GuoHe(){
    for(tagFarmer&farmer:info.farmers){
        if(farmer.FarmerSort==FARMERTYPE_WOOD_BOAT){
            if(farmer.WorkObjectSN==-1&&FarmerbeUsed.count(farmer.SN)==0){
                FarmerbeUsed.insert(farmer.SN);
                return farmer;
            }
        }
    }
        tagFarmer boat;
        boat.SN=-1;
        boat=farmer;
            IsGuoHe=1;
}
}*/




