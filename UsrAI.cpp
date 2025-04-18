#include "UsrAI.h"
tagGame tagUsrGame;
ins UsrIns;
/*##########璇峰嬁淇敼浠ヤ笂閮ㄥ垎##########*/
#include "set"
#include "QDebug"
#include<iostream>
#include <queue>
#include <utility>
using namespace std;
using Pos=pair<int,int>;
double mid=64*BLOCKSIDELENGTH;
int house;
int id_Wrong_lastFrame = -1;
int Map[128][128];
int CenterX,CenterY;
tagInfo info;
set<int>FarmerbeUsed;
//bool DockIsBuild;
bool IsCangkuBuild=false;
bool UpdateToFTB;
bool IsGuoHe;
bool IsGuCangBuilt;
int ret=999;
int home_x ,home_y;
int tmp = 1;
int max_human;
int SAIcnt=0,Shipcnt=0,WBcnt=0;
const int dx[4] = {0, 0, -1, 1};
const int dy[4] = {-1, 1, 0, 0};

static int dis_1=1e8-1;
static int dis_2=1e8-1;

void UsrAI::processData()
{
    info=getInfo();
    ditu();
    max_human=4;
    for(int i=0;i<info.buildings.size();i++){
        tagBuilding h=info.buildings[i];
        if(h.Type==BUILDING_CENTER){
            CenterX = h.BlockDR;
            CenterY = h.BlockUR;
        }
    }
    for(int i = 0;i<info.buildings.size();i++){
        tagBuilding h=info.buildings[i];
        if(h.Type==BUILDING_HOME){
            max_human+=4;
        }
    }
    BuildHome();
    CreateFarmer();
    BuildDock();
    CreatBoat();
   // CutDownTree();
   // Guohe();
    KillEnemy();
    Fish();
    KillFish();
    GoToBoat();
    //FindNearstBeach(CenterX,CenterY);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UsrAI::ditu(){
    static int x = 0;
    static int y = 0;
    for(;x<MAP_L;x++){
        for(;y<MAP_U;y++){
            Map[x][y]=info.theMap[x][y].height;
        }
    }
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
        for(int i=0;i<3;i++)
        {
            for(int j=0;j<3;j++)
            {
                Map[i+x][j+y]=5;
            }
        }
    }
    for(int i =0;i<info.armies.size();i++){
        tagArmy armies = info.armies[i];
        int x = armies.BlockDR,y=armies.UR;
        Map[x][y]=5;
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UsrAI::BuildHome(){
    static tagFarmer farmer;
    static int MaxCnt=10;
    static bool init=1;
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
    Pos pos=GetFittestBlockToBuildHouse(BUILDING_HOME,CenterX,CenterY);
    HumanBuild(farmer.SN,BUILDING_HOME,pos.first,pos.second);//pos.first,pos.second
}


void UsrAI::RecycleFarmer(tagFarmer &farmer)
{
    FarmerbeUsed.erase(farmer.SN);
    farmer.SN=-1;
}

void UsrAI::GetFarmer(tagFarmer &farmer)
{
    bool find=0;
    for(auto&f:info.farmers)
        if(f.SN==farmer.SN)
        {
            find=1;
            farmer=f;
            break;
        }
    if(!find)
        farmer=GetFarmer();
}

tagFarmer UsrAI::GetFarmer()
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

pair<int, int> UsrAI::GetFittestBlockToBuildHouse(int houseType, int x, int y)
{
   //int w=GetBuildingWidth(houseType);
   int dis=1e8+10;
   pair<int,int>res={-456,-789};
   for(int i=0;i<MAP_L;++i)
   {
       for(int j=0;j<MAP_U;++j)
       {
           bool t=CheckTaretBlockCanBuildTargetTypeBuilding(houseType,i,j);
           if((t==1))
           {
               //int a=i-x;//i+w/2-x
               //int b=j-y;//j+w/2-y
               int dd=abs(j-y)+abs(i-x);
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

/*int UsrAI::GetBuildingWidth(int Type)
{
    if(Type==BUILDING_DOCK)return 2;
    if(Type==BUILDING_HOME)return 2;
    return 3;
}*/

bool UsrAI::CheckTaretBlockCanBuildTargetTypeBuilding(int houseType, int x, int y)
{
    int h=info.theMap[x][y].height;
    if((h<0&&houseType!=BUILDING_DOCK)||h>4)return 0;
    for(int i=0;i<3;i++)
    {
        for(int j=0;j<3;j++)
        {
            int xx=x+i,yy=y+j;
            if(xx>=MAP_L)return 0;
            if(yy>=MAP_U)return 0;
            if(xx>=0&&yy>=0&&Map[xx][yy]!=h)return 0;
        }
    }
    return 1;
}

void UsrAI::CreateFarmer()
{
    for(int i=0;i<info.buildings.size();i++){
        tagBuilding h=info.buildings[i];
            if(h.Type==BUILDING_CENTER&&h.Project==ACT_NULL&&info.Meat>=BUILDING_CENTER_CREATEFARMER_FOOD&&info.farmers.size()<20){
                BuildingAction(h.SN,BUILDING_CENTER_CREATEFARMER);
        }
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UsrAI::BuildDock(){
    static tagFarmer farmer1;
    GetFarmer(farmer1);
    static int Max=5;
    int cnt=0;
    static bool init=1;
    if(init){
        init=0;
        farmer1.SN=-154;
    }


    for(int i=0;i<info.buildings.size();++i){
        tagBuilding h=info.buildings[i];
        if(h.Type==BUILDING_DOCK){
            if(h.Percent==100){
                //DockIsBuild=1;
                cnt++;
            }
        }
    }
    if(cnt>=Max){
        RecycleFarmer(farmer1);
        return;
    }
    if(info.Wood<BUILD_DOCK_WOOD)   return;
    //GetFarmer(farmer[0]);
    //cout<<farmer[0].SN<<endl;
    if(farmer1.WorkObjectSN!=-1)    return;
    if(farmer1.FarmerSort!=FARMERTYPE_FARMER)   return;
    Pos p=FindNearstBeachToBuild(BUILDING_DOCK,CenterX,CenterY);
    // cout<<p.first<<" "<<p.second<<endl;
    HumanBuild(farmer1.SN,BUILDING_DOCK,p.first,p.second);
}

pair<int,int> UsrAI::FindNearstBeachToBuild(int houseType,int x,int y){
        int dis = 1e8 + 10;
        pair<int, int> res = {-1, -1};
        for (int i = 0; i < MAP_L; ++i) {
            for (int j = 0; j < MAP_U; ++j) {
                bool f=CheckCanBuildDock(i,j);
                if((f==1)){
                    int dd=abs(j-y)+abs(i-x);
                    if (dd < dis) {
                        res = {i, j};
                        dis = dd;
                    }
                }
            }
        }
        return res;
}

bool UsrAI::CheckCanBuildDock(int x, int y){
    for(int i=0;i<2;i++){
        for(int j=0;j<2;j++){
            int ii=i+x,jj=j+y;
            if(ii<0||ii>=MAP_L||jj<0||jj>=MAP_U)return 0;
            if(info.theMap[ii][jj].type!=MAPPATTERN_OCEAN)return 0;
            if(Map[ii][jj]==5) return 0;
        }
    }
    for(int i=-1;i<3;i++){
        for(int j=-1;j<3;j++){
            int ii=i+x,jj=j+y;
            /*if(ii==x&&jj==y)continue;
            if(ii==x&&jj==y+1)continue;
            if(ii==x+1&&jj==y)continue;
            if(ii==x+1&&jj==y+1)continue;*/
            if(info.theMap[ii][jj].type==MAPPATTERN_GRASS)  return 1;
        }
    }
    return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UsrAI::CreatBoat(){
    /*for(int i = 0;i<info.farmers.size();i++){
        if(info.farmers[i].FarmerSort==FARMERTYPE_WOOD_BOAT){
            WBcnt++;
        }
        if(info.farmers[i].FarmerSort==FARMERTYPE_SAILING){
            SAIcnt++;
        }
    }
    for(int i = 0;i<info.armies.size();i++){
        if(info.armies[i].Sort==ARMY_SHIP){
            Shipcnt++;
        }
    }*/
    int left_human=max_human-info.farmers.size()-Shipcnt-SAIcnt-WBcnt-5;
    //cout<<"WBcut:"<<WBcnt<<"    ";
   // cout<<"saicnt:"<<SAIcnt<<"    ";
    //cout<<"shipcut:"<<Shipcnt<<"    ";

    for (int i = 0; i < info.buildings.size(); i++) {
        tagBuilding& h = info.buildings[i];
        if (h.Type == BUILDING_DOCK && h.Percent == 100) {
            if (h.Project == ACT_NULL&&left_human>0) {
                if (WBcnt < 5){
                    if(info.Wood >= BUILDING_DOCK_CREATE_WOOD_BOAT_WOOD) {
                        BuildingAction(h.SN, BUILDING_DOCK_CREATE_WOOD_BOAT);
                        WBcnt++;
                    }
                }
                else if (SAIcnt < 5 ){
                    if(info.Wood >= BUILDING_DOCK_CREATE_SAILING_WOOD) {
                        BuildingAction(h.SN, BUILDING_DOCK_CREATE_SAILING);
                        SAIcnt++;
                    }
                }
                else if (Shipcnt < 5){
                    if(info.Wood >= BUILDING_DOCK_CREATE_SHIP_WOOD) {
                        BuildingAction(h.SN, BUILDING_DOCK_CREATE_SHIP);
                        Shipcnt++;
                    }
                }
            }
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*void UsrAI::CutDownTree()
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
}*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
tagFarmer UsrAI::boat(){
    for(tagFarmer&farmer:info.farmers)
    {
        if(farmer.NowState==-1&&FarmerbeUsed.count(farmer.SN)==0)
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


bool UsrAI::CheckSea(int x,int y){
    if(info.theMap[x][y].type!=MAPPATTERN_OCEAN){
        return 0;
    }
    for(int i=-1;i<=1;++i){
        for(int j=-1;j<=1;++j){
            if(i==0&&j==0) continue;
            int nx=x+i;
            int ny=y+j;
            if (nx < 0 || nx >= MAP_L || ny < 0 || ny >= MAP_U) continue;
            if(Map[nx][ny]==5)  return 0;
            if(info.theMap[nx][ny].type!=MAPPATTERN_OCEAN)
            return 1;
        }
    }
    return 0;
}

pair<double,double>UsrAI::FindNearstBeach(int x,int y){
    pair<int,int>nearest={-789,-456};
    int mindis=1e8+10;
    for(int i=0;i<MAP_L-1;i++){
        for(int j=0;j<MAP_U-1;j++){
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UsrAI::Fish()
{

    vector<tagResource>fishs;
    for(auto&rsc:info.resources)
    {
        if(rsc.Type==RESOURCE_FISH)
        {
            fishs.push_back(rsc);
        }
    }
    sort(fishs.begin(),fishs.end(),[&](const tagResource&r0,const tagResource&r1)
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
        if(f.FarmerSort==FARMERTYPE_SAILING){
        if(FarmerbeUsed.count(f.SN)==0&&f.WorkObjectSN==-1)
        {
            HumanAction(f.SN,fishs[(idx++)].SN);
            if(idx>=fishs.size())idx=0;
        }
    }
    }

}
////////////////////////////////////////////////////////////////////
void UsrAI::KillEnemy()
{
    auto&enemy=info.enemy_armies;
    auto&army=info.armies;
    if(army.size()==0)return;
    if(enemy.size()==0)return;
    ////////////////////////////////
    for(auto&a:army)
    {
        if(a.WorkObjectSN==-1)
        {
            int tarGetSn=-1;
            float MinDis=1e8+10;
            for(auto&e:enemy)
            {
                float dd=calDistance(a.DR,a.UR,e.DR,e.UR);
                if(dd<MinDis)
                {
                    tarGetSn=e.SN;
                    MinDis=dd;
                }
            }
            HumanAction(a.SN,tarGetSn);
        }
    }
}

void UsrAI::KillFish(){
    vector<tagResource>fish;
    for(auto&rsc:info.resources)
    {
        if(rsc.Type==RESOURCE_FISH)
        {
            fish.push_back(rsc);
        }
    }
    sort(fish.begin(),fish.end(),[&](const tagResource&r0,const tagResource&r1)
    {
        float x=CenterX*BLOCKSIDELENGTH,y=CenterY*BLOCKSIDELENGTH;
        float x0=r0.DR-x,y0=r0.UR-y;
        float x1=r1.DR-x,y1=r1.UR-y;
        return x0*x0+y0*y0<x1*x1+y1*y1;
    });
    int idx=0;
    static tagFarmer farmer[4];
    static bool init=1;
    if(init){
        init=0;
        for(int i=0;i<info.farmers.size();i++){
            info.farmers[i].SN=-1;
        }
    }
        GetFarmer(farmer[0]);
        if(FarmerbeUsed.count(farmer[0].SN)==0&&farmer[0].WorkObjectSN==-1&&farmer[0].FarmerSort==FARMERTYPE_FARMER)
        {
            HumanAction(farmer[0].SN,fish[(idx++)].SN);
            if(idx>=fish.size())idx=0;
        }

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
pair<double,double>UsrAI::FindNearstGold(double x,double y){
    pair<double,double>nearest={-789,-456};
    int mindis=1e8+10;
    for(int i=0;i<info.resources.size();i++){
        if(info.resources[i].Type==RESOURCE_GOLD){
            int gold_x=info.resources[i].DR;
            int gold_y=info.resources[i].UR;
               int dis=abs(gold_x-x)+abs(gold_y-y);
                if(dis<mindis){
                    mindis=dis;
                    nearest={gold_x,gold_y};
                }
            }
        }
   return nearest;
}
void UsrAI::GoToBoat(){
    Pos nearestGold = FindNearstGold(CenterX ,CenterY);
    Pos nearestBeach = FindNearstBeach(CenterX,CenterY);
    static bool IsGuoHe=0;
    vector<int> farmersOnBoat;
    for(int i=0;i<info.farmers.size();i++){
        tagFarmer&boat = info.farmers[i];
        if (boat.FarmerSort == FARMERTYPE_WOOD_BOAT && boat.NowState == 0) {
            static  bool init=1;
            if(init){
                init=0;
            HumanMove(boat.SN,(nearestBeach.first+0.5)*BLOCKSIDELENGTH,(nearestBeach.second+0.5)*BLOCKSIDELENGTH);
            }
           // cout<<boat.NowState;
           // cout<<(nearestBeach.first+0.5)*BLOCKSIDELENGTH<<" "<<(nearestBeach.second+0.5)*BLOCKSIDELENGTH<<endl;
            for (int j = 0; j < info.farmers.size(); ++j) {
                tagFarmer& f = info.farmers[j];
                if (f.WorkObjectSN == -1&&f.FarmerSort == FARMERTYPE_FARMER ){
                    HumanAction(f.SN, boat.SN);
                    farmersOnBoat.push_back(f.SN);
                            //cout<<boat.Resource;
                    if (boat.Resource >=5){//计算船上的人数
                         HumanMove(boat.SN, 1943.98, 726.991);
                         IsGuoHe=1;
                         cout<<(nearestGold.first+0.5)*BLOCKSIDELENGTH<<" "<< (nearestGold.second+0.5)*BLOCKSIDELENGTH<<endl;
                      }
                    }
                }
            }
        //}
       if(boat.FarmerSort==FARMERTYPE_WOOD_BOAT) break;//把上船的人全都放到一个容器里，到了岸边再把容器里的人释放出来让他们去采金子
    }
    if(IsGuoHe){
        vector<tagResource>golds;
           for(auto&rsc:info.resources)
           {
               if(rsc.Type==RESOURCE_GOLD)
               {
                   golds.push_back(rsc);
               }
           }
           sort(golds.begin(),golds.end(),[&](const tagResource&r0,const tagResource&r1)
           {
               float x=CenterX*BLOCKSIDELENGTH,y=CenterY*BLOCKSIDELENGTH;
               float x0=r0.DR-x,y0=r0.UR-y;
               float x1=r1.DR-x,y1=r1.UR-y;
               return x0*x0+y0*y0<x1*x1+y1*y1;
           });
           int idx=0;
           for(int sn:farmersOnBoat)
           {
                 HumanAction(sn,golds[(idx++)].SN);
                   if(idx>=golds.size())idx=0;
               }
        }
    //}
}








