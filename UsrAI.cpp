#include "UsrAI.h"
#include<set>
#include <iostream>
#include<unordered_map>
#include<list>
using namespace std;
tagGame tagUsrGame;
ins UsrIns;
//
//
/*##########DO NOT MODIFY THE CODE ABOVE##########*/
const int BarrierMask=-2;
const int goldWin=2000;
tagInfo info;
map<int,tagObj*>SnToObj;
set<int>lock_farmers;
set<int>idle_farmers;//所有空闲的农民
set<int>idle_armies;
tagBuilding Center;
int mapInfo[MAP_L][MAP_U];
int tmp[MAP_L][MAP_U];
bool upgrade=0;
unordered_map<int,int>buildingSize={
    {BUILDING_CENTER,3},
    {BUILDING_HOME,2},
    {BUILDING_GRANARY,3},
    {BUILDING_STOCK,3},
    {BUILDING_ARMYCAMP,3},
    {BUILDING_ARROWTOWER,2},
    {BUILDING_FARM,3},
    {BUILDING_MARKET,3},
    {BUILDING_STABLE,3},
    {BUILDING_RANGE,3},
    {BUILDING_DOCK,2},
};
int blockIndex[MAP_L][MAP_U];
vector<Point>enemyLand;
vector<Point>seaSide;
vector<Point>enemySeaSide;
vector<Point>unExplore;
map<array<int,2>,int>PosToBuilding;
list<tuple<vector<int>,int,array<int,2>,function<bool(void)>>>buildTask;
set<int>treeIsCutting;
set<string>buildingBuilt;
set<array<int,2>>buildingOcuppy;
set<array<int,2>>farmerPos;
/////////////////////////////
struct Node{
    Node*nxt;
    function<void(void)>action,init;
    function<bool(void)>condition;
    Node(
         const decltype(init)&init_,
         const decltype(action)&action_,
         const decltype(condition)&condition_,
         Node*nxt_=0
        )
        :
          isInit(0),
          nxt(nxt_),
          action(action_),
          init(init_),
          condition(condition_)
    {

    }
    Node* work(){
        if(!isInit){
            isInit=1;
            init();
        }
        //
        action();
        //
        bool cond=1;
        cond=cond&&condition();
        return cond?nxt:this;
    }
private:
    bool isInit;
};
vector<Node*>AllNodes;//当前所有的工作点
set<string>NodeMsg;
set<string>TheFrameNodeMsg;
/////////////////////////////
void AddMsg(string t){
    TheFrameNodeMsg.insert(t);
}
bool FindMsg(string t){
    return NodeMsg.count(t);
}
int randint(int a,int b){
    return rand()%(b-a+1)+a;
}
void UsrAI::processData()
{
    cheatAction();
    bool debug=0;
    if(debug){
        cheatAction();
        static bool f=0;
        if(!f)
        cheatRes(),cheatRes(),cheatRes(),cheatRes(),f=1;
        return;
    }
    info=getInfo();
    //初始化所有的工作点
    InitAll();
    //更新所有信息
    UpdateAll();
    //

}

void UsrAI::InitAll()
{
    static bool init=0;
    if(init)return;
    init=1;
    /////////////////建立所有起始工作点
    InitActionNode();
    //获取我方大陆岸边
    {
        for(int i=0;i<MAP_L;++i){
            for(int j=0;j<MAP_U;++j){
                if(checkIsSeaSide(i,j,0))seaSide.push_back({i,j});
            }
        }
    }
}

void UsrAI::InitActionNode()
{
    map<string,Node*>nodes;
    decltype(Node::action)emptyAction=[&](){};
    decltype(Node::condition)emptyCond=[&](){return false;};//形成自循环
    decltype(Node::init)emptyInit=[&](){};
    //建造任务
    {
        nodes["BuildTask"]=new Node(
                    emptyInit,
                    [&](){
                        for(auto itr=buildTask.begin();itr!=buildTask.end();){
                            auto&farmer=get<0>(*itr);
                            int tp=get<1>(*itr);
                            auto pos=get<2>(*itr);
                            auto&check=get<3>(*itr);
                            //如果不存在
                            if(PosToBuilding.count(pos)==0){
                                if(check()){
                                    int idx=randint(0,farmer.size()-1);
                                    HumanBuild(farmer[idx],tp,pos[0],pos[1]);
                                }
                                else
                                {
                                    for(auto sn:farmer)
                                    GoToCutTree(sn);
                                }
                            }
                            else{
                                //看看是否已经建好
                                int sn=PosToBuilding[pos];
                                tagBuilding build=*(tagBuilding*)GetObj(sn);
                                if(build.Percent>=100){
                                    buildingBuilt.insert("pos("+to_string(pos[0])+","+to_string(pos[1])+")built");
                                    itr=buildTask.erase(itr);
                                    continue;
                                }
                                else{
                                    for(auto sn_:farmer){
                                        if(SnToObj.count(sn_)){
                                            auto&f=*(tagFarmer*)SnToObj[sn_];
                                            if(f.WorkObjectSN!=sn){
                                                HumanAction(sn_,sn);
                                            }
                                        }
                                    }
                                }
                            }
                            itr=next(itr);
                        }
                    },
                    [&](){
                        return 0;
                    }


                    );
    }
    //生成农民
    {
        Node*farmerGenerate=new Node(
                    emptyInit,
                    [&](){
            //
            int cnt=Count(info.farmers,[&](tagFarmer f){return f.FarmerSort==FARMERTYPE_FARMER;});
            if(cnt>=20)return;
            //生产农民
            if(Center.Project==0)BuildingAction(Center.SN,BUILDING_CENTER_CREATEFARMER);
        },
        emptyCond
        );
        nodes["FarmerGenerate"]=farmerGenerate;
    }
    //建造房子
    {
        static int farmerSN=-1;
        Node*homeBuild=new Node(
                    emptyInit,
                    [&](){
                        if(farmerSN==-1)
                        {
                            auto&&v=AllocateFarmers(1,Center.BlockDR,Center.BlockUR);
                            if(v.size())farmerSN=v.front();
                            else return;
                        }
                        tagFarmer farmer=*(tagFarmer*)GetObj(farmerSN);
                        int cnt=Count(info.buildings,[&](tagBuilding obj){return obj.Type==BUILDING_HOME;});
                        if(cnt>=12)return;
                        if((cnt>=5&&info.Wood>=200)||(cnt<5&&info.Wood>=BUILDING_CENTER_CREATEFARMER_FOOD)){
                            if(!checkWorkObjIsTargetType((*(tagFarmer*)GetObj(farmerSN)).WorkObjectSN,info.buildings)&&info.Wood>=BUILD_HOUSE_WOOD){
                                static array<int,2>pre={Center.BlockDR,Center.BlockUR};
                                array<int,2>place=FindForBuilding(BUILDING_HOME,BUILDING_CENTER,pre[0],pre[1],1);
                                if(place[0]!=INT_MAX){
                                    pre=place;
                                    HumanBuild(farmerSN,BUILDING_HOME,place[0],place[1]);
                                }
                            }
                        }
                        else{
                            GoToCutTree(farmerSN);
                        }
                    },
        [&](){
            int cnt=Count(info.buildings,[&](tagBuilding obj)->bool{return obj.Type==BUILDING_HOME;});
            if(cnt>=12){
                FreeFarmers({farmerSN});
                return 1;
            }
            return 0;
        }

        );
        nodes["homeBuild"]=homeBuild;
    }
    //人去捕鱼
    {
        static vector<int>farmer;
        static list<int>fish;
        nodes["fishing"]=new Node(
            [&](){
                //找到所有靠岸渔场
                for(auto obj:info.resources){
                    if(obj.Type!=RESOURCE_FISH)continue;
                    int ele=obj.SN;
                    tagResource res=*(tagResource*)GetObj(ele);
                    //判断是否靠岸
                    static const int off[][2]={ {0,-1},{1,-1},{0,2},{1,2},{-1,0},{-1,1},{2,0},{2,1} };
                    bool flag=IsYanAn(res.BlockDR,res.BlockUR);
                    if(flag){
                        fish.push_back(ele);
                    }
                }
            },
            [&](){
                AllocateFarmers(farmer,2,Center.BlockDR,Center.BlockUR,1);
                //更新list
                for(auto itr=fish.begin();itr!=fish.end();){
                    if(SnToObj.count(*itr)==0)itr=fish.erase(itr);
                    else itr=next(itr);
                }
                //寻找一个靠我方大陆且可以人采集到的最近渔场
                int tarsn=-1;
                for(auto ele:fish){
                    tagResource res=*(tagResource*)GetObj(ele);
                    if(tarsn==-1)tarsn=ele;
                    else
                    {
                        tagResource tar=*(tagResource*)GetObj(tarsn);
                        if(abs(tar.BlockDR-Center.BlockDR)+abs(tar.BlockUR-Center.BlockUR)>abs(res.BlockDR-Center.BlockDR)+abs(res.BlockUR-Center.BlockUR))
                        tarsn=ele;
                    }
                }
                //所有人直接对同一个渔场进行采集
                if(tarsn!=-1){
                    for(auto sn:farmer){
                        tagFarmer f=*(tagFarmer*)GetObj(sn);
                        if(f.WorkObjectSN==-1){
                            HumanAction(sn,tarsn);
                        }
                    }
                }
            },
            [&](){
                if(fish.size()==0){
                    FreeFarmers(farmer);
                    return 1;
                }
                return 0;
            }
        );
    }
    //砍树
    {
        static vector<int>farmer;
        nodes["cutTree"]=new Node(
                    emptyInit,
                    [&](){
                        list<int>trees;
                        //获取所有树的位置
                        for(tagResource res:info.resources){
                            if(res.Type!=RESOURCE_TREE||!InMyLand(res.BlockDR,res.BlockUR))continue;
                            trees.push_back(res.SN);
                        }
                        //分配农民
                        AllocateFarmers(farmer,3,Center.BlockDR,Center.BlockUR);
                        //去砍树
                        for(auto sn:farmer){
                            GoToCutTree(sn);
                        }
                    },
                    [&](){
                        if(info.Wood>=400)
                        {
                            AddMsg("WoodIsEnough");

                        }
                        if(info.Wood>=1000){
                            FreeFarmers(farmer);
                            return 1;
                        }
                        return 0;
                    }
        );
    }
    //猎杀羚羊
    {
        static vector<int>farmers;
        static bool buildSock=0;
        static int stockSN=-1;
        static array<int,2>pos={INT_MAX,INT_MAX};
        nodes["killSheep"]=new Node(
                    emptyCond,
                    [&](){
                        AllocateFarmers(farmers,2,Center.BlockDR,Center.BlockUR);
                        if(farmers.size()!=2)return;
                        list<tagResource>sheep;
                        set<int>allSN;
                        bool alive=0;
                        for(auto&obj:info.resources)
                        {
                            if(obj.Type!=RESOURCE_GAZELLE)continue;
                            if(blockIndex[obj.BlockDR][obj.BlockUR]!=blockIndex[Center.BlockDR][Center.BlockUR])continue;
                            if(obj.Blood>0)alive=1;
                            sheep.push_back(obj);
                            allSN.insert(obj.SN);
                        }
                        //先杀掉所有🐏
                        if(alive){
                            tagFarmer f0=*(tagFarmer*)GetObj(farmers[0]),f1=*(tagFarmer*)GetObj(farmers[1]);
                            if(f0.WorkObjectSN!=-1&&(*(tagResource*)GetObj(f0.WorkObjectSN)).Blood>0)return;
                            //找到离二者最近的🐏
                            int tar=-1;
                            for(auto&res:sheep){
                                if(res.Blood<=0)continue;
                                int dis=Dis(f0,res)+Dis(f1,res);
                                if(tar==-1)tar=res.SN;
                                else{
                                    tagResource tt=*(tagResource*)GetObj(tar);
                                    int dis1=Dis(f0,tt)+Dis(f1,tt);
                                    if(dis1>dis)tar=res.SN;
                                }
                            }
                            //杀了他
                            if(~~~~~tar){
                                HumanAction(farmers[0],tar);
                                HumanAction(farmers[1],tar);
                            }
                        }
                        //建仓库
                        else if(!IsBuilt(pos[0],pos[1])){
                            //如果存在这个任务那么直接返回
                            if(IsExistBuildTask(pos[0],pos[1]))return;
                            //计算出一个最佳点，这里我简单的取中点
                            int x=0,y=0;
                            for(auto&res:sheep){
                                x+=res.BlockDR,y+=res.BlockUR;
                            }
                            x/=sheep.size(),y/=sheep.size();
                            pos=FindForBuilding(BUILDING_STOCK,x,y);
                            BuildBuilding(farmers,pos[0],pos[1],BUILDING_STOCK,[&](){return info.Wood>=BUILD_STOCK_WOOD;});
                        }
                        else if(sheep.size()){
                            //发送消息
                            AddMsg("pickGazalle");
                            //采集🐏🥩根据距离仓库得距离来算(两个人对不同肉堆采集)
                            if(PosToBuilding.count(pos)==0)return;
                            stockSN=PosToBuilding[pos];
                            tagFarmer f0=*(tagFarmer*)GetObj(farmers[0]),f1=*(tagFarmer*)GetObj(farmers[1]);
                            tagBuilding stock=*(tagBuilding*)GetObj(stockSN);
                            vector<pair<int,int>>dis;
                            for(auto&res:sheep){
                                int dis0=Dis(res,stock);
                                dis.push_back({dis0,res.SN});
                            }
                            sort(dis.begin(),dis.end());
                            //
                            if(dis.size())
                            {
                                for(int i=0,j=0;i<farmers.size();++i,(++j)%=dis.size()){
                                    auto&f=*(tagFarmer*)GetObj(farmers[i]);
                                    if(f.WorkObjectSN==-1)
                                        HumanAction(f.SN,dis[j].second);
                                }
                            }
                        }
                    },
                    [&](){
                        for(auto&obj:info.resources)
                            if(obj.Type==RESOURCE_GAZELLE)
                                return 0;
                        FreeFarmers(farmers);
                        return 1;
                    }
        );
    }
    //猎杀大象
    {
        static vector<int>farmers;
        static int elephantSN=-1;
        static array<int,2>place{INT_MAX,INT_MAX};
        static bool flag=0;
        nodes["killElephant"]=new Node(
                        emptyInit,
                    [&](){
                        if(!(flag||(flag=FindMsg("WoodIsEnough"))))return;
                        AllocateFarmers(farmers,2,Center.BlockDR,Center.BlockUR,1);
                        if(farmers.empty())return;
                        auto&&elephants=findTargetObj(info.resources,[&](tagResource res){return res.Type==RESOURCE_ELEPHANT;});
                        //两个人同时杀大象,边走边杀
                        if(elephantSN==-1||(*(tagResource*)GetObj(elephantSN)).Blood<=0){
                            elephantSN=-1;
                            for(tagResource obj:elephants)
                                if(obj.Type==RESOURCE_ELEPHANT&&obj.Blood>0)
                                    elephantSN=obj.SN;
                        }
                        //
                        if(elephantSN==-1&&!IsExistBuildTask(place[0],place[1])){
                            //计算位置
                            int x=0,y=0;
                            for(auto obj:elephants)x+=obj.BlockDR,y+=obj.BlockUR;
                            x/=elephants.size(),y/=elephants.size();
                            //全死了,那就建造仓库
                            place=FindForBuilding(BUILDING_STOCK,x,y);
                            if(place[0]!=INT_MAX)BuildBuilding(farmers,place[0],place[1],BUILDING_STOCK,[&](){return info.Wood>=BUILD_STOCK_WOOD;});
                            return;
                        }
                        if(IsExistBuildTask(place[0],place[1])&&!IsBuilt(place[0],place[1]))return;
                        //采肉
                        if(elephantSN==-1){
                            tagBuilding&stock=*(tagBuilding*)GetObj(PosToBuilding[place]);
                            vector<array<int,2>>dis;
                            for(tagResource res:elephants){
                                dis.push_back({Dis(stock,res),res.SN});
                            }
                            sort(dis.begin(),dis.end());
                            if(dis.empty())return;
                            int idx=0;
                            for(auto sn:farmers){
                                tagFarmer f=*(tagFarmer*)GetObj(sn);
                                if(f.WorkObjectSN==-1){
                                    HumanAction(f.SN,dis[idx][1]);
                                }
                                (++idx)%=dis.size();
                            }
                        }
                        //猎杀
                        else
                        {
                            tagResource ele=*(tagResource*)GetObj(elephantSN);
                            for(auto sn:farmers){
                                tagFarmer f=*(tagFarmer*)GetObj(sn);
                                if(f.Blood<=0)continue;
                                bool flag=1;
                                if(abs(f.BlockDR-ele.BlockDR)<3&&abs(f.BlockUR-ele.BlockUR)<3)flag=0;
                                if(flag){
                                    if(f.WorkObjectSN!=elephantSN){
                                        HumanAction(sn,ele.SN);
                                    }
                                }
                                else{
                                    if(f.NowState!=HUMAN_STATE_WALKING){
                                        HumanMove(sn,(0.5+Center.BlockDR)*BLOCKSIDELENGTH,(0.5+Center.BlockUR)*BLOCKSIDELENGTH);
                                    }
                                }
                            }
                        }
                    },
                    [&](){
                        bool flag=findTargetObj(info.resources,[&](tagResource res){return res.Type==RESOURCE_ELEPHANT;}).empty();
                        if(flag){
                            //
                            FreeFarmers(farmers);
                        }
                        return flag;
                    }
                    );
    }
    //在沿岸建造船坞
    {
        static int farmerSN=-1;
        static vector<array<int,2>>edge;
        static vector<tagResource>fish;
        static bool flag=0;
        nodes["buildDock"]=new Node(
                    [&](){
                        //找到所有沿我方大陆的岸(可以建造船坞)(dis=2)
                        auto&theMap=*info.theMap;
                        for(int i=0;i<MAP_L;++i){
                            for(int j=0;j<MAP_U;++j){
                                bool flag=1;
                                if(theMap[i][j].type==MAPPATTERN_OCEAN){
                                    for(int l0=0;l0<buildingSize[BUILDING_DOCK]&&flag;++l0){
                                        for(int l1=0;l1<buildingSize[BUILDING_DOCK]&&flag;++l1){
                                            int ii=i+l0,jj=j+l1;
                                            if(!(ii>=0&&ii<MAP_L&&jj>=0&&jj<MAP_U&&theMap[ii][jj].type==MAPPATTERN_OCEAN)){
                                                flag=0;
                                            }
                                        }
                                    }
                                }
                                if(flag&&IsYanAn(i,j))edge.push_back({i,j});
                            }
                        }
                        //找到所有渔场
                        for(auto&obj:info.resources)
                            if(obj.Type==RESOURCE_FISH)
                                fish.push_back(obj);
                        //排序
                        vector<int>Indices(edge.size());
                        iota(Indices.begin(),Indices.end(),0);
                        sort(Indices.begin(),Indices.end(),[&](int i,int j){
                            auto&p0=edge[i],&p1=edge[j];
                            auto Dis0=[&](int x,int y)->int{
                              int ret=0;
                              for(auto&obj:fish){
                                  ret+=abs(obj.BlockDR-x)+abs(obj.BlockUR-y);
                              }
                              return ret;
                            };
                            return Dis0(p0[0],p0[1])<Dis0(p1[0],p1[1]);
                        });
                        decltype(edge)tmp(edge.size());
                        for(int i=0;i<edge.size();++i)tmp[i]=edge[Indices[i]];
                        swap(tmp,edge);
                    },
                    [&](){
                        if(!(flag||(flag=FindMsg("pickGazalle"))))return;
                        if(farmerSN==-1){
                            auto&&v=AllocateFarmers(1,Center.BlockDR,Center.BlockUR);
                            if(v.size())farmerSN=v[0];
                            else return;
                        }
                        if(info.Wood<BUILD_DOCK_WOOD){
                            GoToCutTree(farmerSN);
                            return;
                        }
                        //先找到所有的船坞
                        auto&&dock=findTargetObj(info.buildings,[&](tagBuilding obj){return obj.Type==BUILDING_DOCK;});
                        //
                        tagFarmer farmer=*(tagFarmer*)GetObj(farmerSN);
                        if(!checkCondition(info.buildings,[&](tagBuilding obj){return obj.Type==BUILDING_DOCK&&farmer.WorkObjectSN==obj.SN;})){
                            static int idx=-1;
                            ++idx;
                            //确保当前位置与所有船坞有一定间隔
                            bool flag=1;
                            for(auto&obj:dock){
                                tagObj obj1;obj1.BlockDR=edge[idx][0],obj1.BlockUR=edge[idx][1];
                                if(Dis(obj,obj1)<=6){
                                    flag=0;
                                    return;
                                }
                            }
                            //
                            if(flag)
                            HumanBuild(farmerSN,BUILDING_DOCK,edge[idx][0],edge[idx][1]);
                        }
                    },
                    [&](){
                        int cnt=0;
                        for(tagBuilding obj:info.buildings){
                            if(obj.Percent>=100&&obj.Type==BUILDING_DOCK)++cnt;
                        }
                        if(cnt>=2){
                            FreeFarmers({farmerSN});
                            return 1;
                        }
                        return 0;
                    }
        );
    }
    //建船
    {
        nodes["buildFishBoat"]=new Node(
                    emptyInit,
                    [&](){
                        if(FindMsg("pickGazalle")==0)return;
                        int cnt=Count(info.farmers,[&](tagFarmer f){return f.FarmerSort==FARMERTYPE_SAILING;});
                        for(auto&obj:info.buildings){
                            if(obj.Type==BUILDING_DOCK&&obj.Project==BUILDING_DOCK_CREATE_SAILING){
                                ++cnt;
                            }
                        }
                        if(cnt>=3)return;
                        //找到一个船坞
                        int tarsn=-1;
                        for(auto&obj:info.buildings){
                            if(obj.Type==BUILDING_DOCK&&obj.Percent==100&&obj.Project==0){
                                tarsn=obj.SN;
                                break;
                            }
                        }
                        if((~~~~~tarsn)&&info.Wood>=BUILDING_DOCK_CREATE_SAILING_WOOD){
                            BuildingAction(tarsn,BUILDING_DOCK_CREATE_SAILING);
                        }
                    },
                    [&](){
                        int cnt=0;
                        for(auto&b:info.farmers){
                            if(b.FarmerSort==FARMERTYPE_SAILING)++cnt;
                        }
                        return cnt>=3;
                    }
                    );
    }
    //命令所有的空闲人员立刻去砍树
    {
        nodes["idle_curTree"]=new Node(
                    emptyInit,
                    [&](){
                        list<int>trees;
                        //获取所有树的位置
                        for(tagResource res:info.resources){
                            if(res.Type!=RESOURCE_TREE||!InMyLand(res.BlockDR,res.BlockUR))continue;
                            trees.push_back(res.SN);
                        }
                        for(auto sn:idle_farmers){
                            GoToCutTree(sn);
                        }
                    },
                    [&](){
                        return 0;
                    }
                    );
    }
    //命令所有渔船去打鱼
    {
        //
        static set<int>fish;
        nodes["catchFish"]=new Node(
                    emptyInit,
                    [&](){
                        fish.clear();
                        for(auto&obj:info.resources){
                            if(obj.Type==RESOURCE_FISH){
                                fish.insert(obj.SN);
                            }
                        }
                        for(auto&obj:info.farmers){
                            if(obj.FarmerSort==FARMERTYPE_SAILING){
                                if(fish.count(obj.WorkObjectSN)==0){
                                    int tarsn=-1;
                                    for(auto sn:fish){
                                        if(tarsn==-1)tarsn=sn;
                                        else if(Dis(obj,*GetObj(tarsn))>Dis(obj,*GetObj(sn)))tarsn=sn;
                                    }
                                    if(~tarsn)
                                    HumanAction(obj.SN,tarsn);
                                }
                            }
                        }
                    },
                    [&](){
                        if(fish.size()==0){
                            for(auto f:findTargetObj(info.farmers,[&](tagFarmer f){return f.FarmerSort==FARMERTYPE_SAILING;})){
                                HumanAction(f.SN,f.SN);//自杀
                            }
                        }
                        return fish.size()==0;
                    }
                  );
    }
    //升级时代
    {
        nodes["upgradeTime"]=new Node(
                    emptyInit,
                    [&](){
                        if(info.farmers.size()>=15&&info.Meat>=500&&Center.Project==0){
                            BuildingAction(Center.SN,BUILDING_CENTER_UPGRADE);
                        }
                        if(Center.Project==BUILDING_CENTER_UPGRADE&&Center.ProjectPercent>=100){
                            upgrade=1;
                        }
                    },
                    [&](){
                        return upgrade;
                    }
                    );
    }
    //建造谷仓
    {
        static int dis=7;
        static int farmer=-1;
        static array<int,2>pos={INT_MAX,INT_MAX};
        nodes["buildGranary"]=new Node(
                    emptyInit,
                    [&](){
                        if(!upgrade)return;
                        if(farmer==-1){
                            auto&&v=AllocateFarmers(1,Center.BlockDR,Center.BlockUR,1);
                            if(v.size())farmer=v.front();
                        }
                        if(!IsExistBuildTask(pos[0],pos[1])){
                            pos=FindForBuilding(BUILDING_GRANARY,BUILDING_CENTER,Center.BlockDR,Center.BlockUR,dis);
                            if(pos[0]!=INT_MAX)
                            {
                                BuildBuilding({farmer},pos[0],pos[1],BUILDING_GRANARY,[&](){return info.Wood>=BUILD_GRANARY_WOOD;});
                            }
                            else dis=max(--dis,1);
                        }
                    },
                    [&](){
                        bool f=IsBuilt(pos[0],pos[1]);
                        if(f)
                        {
                            FreeFarmers({farmer});
                            AddMsg("GranaryBuilt");
                        }
                        return f;
                    }
                    );
    }
    //建造市场
    {
        static int farmer=-1;
        static bool flag=0;
        static array<int,2>pos{INT_MAX,INT_MAX};
        nodes["buildMarket"]=new Node(
                    emptyInit,
                    [&](){
                        if(!(flag||(flag=FindMsg("GranaryBuilt"))))return;
                        if(farmer==-1){
                            auto&&v=AllocateFarmers(1,Center.BlockDR,Center.BlockUR,1);
                            if(v.size())farmer=v.front();
                        }
                        if(farmer!=-1&&pos[0]==INT_MAX){
                            pos=FindForBuilding(BUILDING_MARKET,Center.BlockDR,Center.BlockUR);
                            if(pos[0]!=INT_MAX)
                            {
                                BuildBuilding({farmer},pos[0],pos[1],BUILDING_MARKET,[&](){return info.Wood>=BUILD_MARKET_WOOD;});
                            }
                        }
                    },
                    [&](){
                        bool f=IsBuilt(pos[0],pos[1]);
                        if(f)
                        {
                            FreeFarmers({farmer});
                            AddMsg("MarketBuilt");
                        }
                        return f;
                    }
                    );
    }
    //种地
    {
        static bool flag=0;
        static const int maxCnt=4;
        static set<int>farmers;
        static map<int,array<int,2>>record;
        nodes["Farming"]=new Node(
                    emptyInit,
                    [&](){
                        if(!(flag||(flag=FindMsg("MarketBuilt"))))return;
                        //计算当前有多少个农场
                        int cnt=Count(info.buildings,[&](tagBuilding obj){return obj.Type==BUILDING_FARM;});
                        if(cnt>=maxCnt)return;
                        //分配农民去建
                        auto gra=findTargetObj(info.buildings,[&](tagBuilding obj){return obj.Type==BUILDING_GRANARY;}).front();
                        auto place=FindForBuilding(BUILDING_FARM,gra.BlockDR,gra.BlockUR);
                        if(place[0]!=INT_MAX){
                            int f=-1;
                            //指派农民去
                            if(farmers.size()<maxCnt){
                                auto&&farmer=AllocateFarmers(1,place[0],place[1]);
                                if(farmer.empty())return;
                                f=farmer.front();
                                farmers.insert(f);
                            }
                            //从现有的农民中拿出一个
                            else{
                                for(auto sn:farmers){
                                    tagFarmer f_=*(tagFarmer*)GetObj(sn);
                                    if(!record.count(sn)&&!checkCondition(info.buildings,[&](tagBuilding obj){return obj.Type==BUILDING_FARM&&obj.SN==f_.WorkObjectSN;})){
                                        f=sn;
                                        break;
                                    }
                                }
                            }
                            if(f!=-1){
                                record[f]=place;
                                BuildBuilding({f},place[0],place[1],BUILDING_FARM,[&](){return info.Wood>=BUILD_FARM_WOOD;});
                            }
                            //对于已经建好的,从映射集合移除
                            for(auto itr=record.begin();itr!=record.end();){
                                auto&p=itr->second;
                                if(IsBuilt(p[0],p[1])){
                                    itr=record.erase(itr);
                                }
                                else{
                                    itr=next(itr);
                                }
                            }
                        }
                        //

                    },
                    [&](){
                        return 0;
                    }
                    );
    }
    //造战船
    {
        nodes["buildShip"]=new Node(
                    emptyInit,
                    [&](){
                        if(upgrade==0)return;
                        int cnt=Count(info.armies,[&](tagArmy f){return f.Sort==AT_SHIP;});
                        for(auto&obj:info.buildings){
                            if(obj.Type==BUILDING_DOCK&&obj.Project==BUILDING_DOCK_CREATE_SHIP){
                                ++cnt;
                            }
                        }
                        if(info.Wood<BUILDING_DOCK_CREATE_SHIP_WOOD||cnt>=5)return;
                        if(FindMsg("shipIsReady"))return;
                        int tarsn=-1;
                        for(auto&obj:info.buildings){
                            if(obj.Type==BUILDING_DOCK&&obj.Project==0&&obj.Percent>=100){
                                tarsn=obj.SN;
                                break;
                            }
                        }
                        if(~~~~~~~tarsn){
                            BuildingAction(tarsn,BUILDING_DOCK_CREATE_SHIP);
                        }
                    },
                    [&](){
                        //造五个战舰
                        int cnt=0;
                        for(auto obj:info.armies){
                            if(obj.Sort==AT_SHIP)++cnt;
                        }
                        if(cnt>=5){
                            AddMsg("shipIsReady");
                        }
                        return FindMsg("safeOcean");
                    }
                    );
    }
    //巡逻并且杀掉敌方巡逻舰
    {
        static bool ready=0;
        nodes["killEnemyShip"]=new Node(
                    emptyInit,
                    [&](){
                        ready=ready||FindMsg("shipIsReady");
                        if(!ready)return;
                        vector<array<int,2>>unexplored;
                        //获取地方大陆可能在的几个角落
                        auto&theMap=*(info.theMap);
                        if(theMap[0][0].type==MAPPATTERN_UNKNOWN)unexplored.push_back({0,0});
                        if(theMap[0][MAP_U-1].type==MAPPATTERN_UNKNOWN)unexplored.push_back({0,MAP_U-1});
                        if(theMap[MAP_L-1][0].type==MAPPATTERN_UNKNOWN)unexplored.push_back({MAP_L-1,0});
                        if(theMap[MAP_L-1][MAP_U-1].type==MAPPATTERN_UNKNOWN)unexplored.push_back({MAP_L-1,MAP_U-1});
                        if(unexplored.size()==0)return;
                        //获取所有的战船
                        vector<tagArmy>ship;
                        for(auto&obj:info.armies){
                            if(obj.Sort==AT_SHIP)ship.push_back(obj);
                        }
                        //如果发现敌方战舰,立刻击杀，力斩不饶
                        auto&&enemy_ship=findTargetObj(info.enemy_armies,[&](tagArmy obj){return obj.Sort==AT_SHIP;});
                        auto&&enemy_army=findTargetObj(info.enemy_armies,[&](tagArmy obj){return obj.Sort!=AT_SHIP;});
                        if(info.enemy_armies.size()){
                            //找到离我方战舰最近的战舰击杀
                            auto&enemy=enemy_ship.size()?enemy_ship:enemy_army;
                            int tarsn=-1;
                            for(auto&obj:enemy){
                                if(tarsn==-1)tarsn=obj.SN;
                                else{
                                    tagObj f0=*GetObj(tarsn);
                                    auto Dis0=[&](tagObj tt)->int{
                                      int ret=0;
                                      for(auto s:ship)
                                          ret+=Dis(s,tt);
                                      return ret;
                                    };
                                    if(Dis0(f0)>Dis0(obj))tarsn=obj.SN;
                                }
                            }
                            if(~~~~~tarsn){
                                for(auto s:ship){
                                    if(s.WorkObjectSN!=tarsn){
                                        HumanAction(s.SN,tarsn);
                                    }
                                }
                            }
                            return;
                        }
                        //如果已经探索到一个角,那么让战舰沿着新探索出来的边界去探索
                        if(!enemyLand.empty()){
                            for(tagArmy s:ship){
                                if(s.NowState==HUMAN_STATE_IDLE){
                                    auto&&v=GetUnExplore(s.BlockDR,s.BlockUR,0);
                                    if(v.size()){
                                        int idx=randint(0,v.size()-1);
                                        auto&p=v[idx];
                                        HumanMove(s.SN,(p.x+0.5)*BLOCKSIDELENGTH,(p.y+0.5)*BLOCKSIDELENGTH);
                                    }
                                    //如果全部探索过了，那么直接自尽
                                    else{
                                        for(auto s:ship)HumanAction(s.SN,s.SN);
                                        AddMsg("safeOcean");
                                        return;
                                    }
                                }
                            }
                            return;
                        }
                        //寻找一个未探索的边角
                        long long tx=INT_MAX,ty=INT_MAX;
                        for(auto p:unexplored){
                            auto Dis=[&](int tx,int ty)->long long{
                                long long ret=0;
                                for(auto&obj:ship){
                                    ret+=abs(obj.BlockDR*1ll-tx)+abs(obj.BlockUR*1ll-ty);
                                }
                                return ret;
                            };
                            if(Dis(tx,ty)>Dis(p[0],p[1]))tx=p[0],ty=p[1];
                        }
                        //巡逻
                        if(tx!=INT_MAX){
                            for(auto&obj:ship){
                                if(obj.NowState==HUMAN_STATE_IDLE)
                                HumanMove(obj.SN,(tx+0.5)*BLOCKSIDELENGTH,(ty+0.5)*BLOCKSIDELENGTH);
                            }
                        }

                    },
                    [&](){
                        return FindMsg("safeOcean");
                    }
                    );
    }
    //造运输船
    {
        nodes["buildWoodBoat"]=new Node(
                    emptyInit,
                    [&](){
                        if(enemyLand.empty())return;
                        int cnt=Count(info.farmers,[&](tagFarmer f){return f.FarmerSort==FARMERTYPE_WOOD_BOAT;});
                        for(auto&obj:info.buildings){
                            if(obj.Type==BUILDING_DOCK&&obj.Project==BUILDING_DOCK_CREATE_WOOD_BOAT){
                                ++cnt;
                            }
                        }
                        if(cnt>=1)return;
                        vector<tagBuilding>dock;
                        for(auto&obj:info.buildings){
                            if(obj.Type==BUILDING_DOCK){
                                dock.push_back(obj);
                            }
                        }
                        //生产木船
                        if(info.Wood>=BUILDING_DOCK_CREATE_WOOD_BOAT_WOOD)
                        for(auto&obj:dock){
                            if(obj.Project==0){
                                BuildingAction(obj.SN,BUILDING_DOCK_CREATE_WOOD_BOAT);
                                break;
                            }
                        }
                    },
                    [&](){

                        return 0;
                    }
                    );
    }
    //载人去对岸
    {
        nodes["transport"]=new Node(
                    emptyCond,
                    [&](){
                        //找到所有的船
                        vector<tagFarmer>boat=findTargetObj(info.farmers,[&](tagFarmer obj){return obj.FarmerSort==FARMERTYPE_WOOD_BOAT;});
                        //运输船靠岸
                        for(auto&b:boat){
                            if(b.NowState==HUMAN_STATE_IDLE&&!checkIsKaoAn(b.BlockDR,b.BlockUR,blockIndex[Center.BlockDR][Center.BlockUR])){
                                //选择最近的点作为一个点作为着陆点
                                int tx=MAP_L,ty=MAP_U;
                                for(auto&p:seaSide){
                                    int d0=abs(p.x-b.BlockDR)+abs(p.y-b.BlockUR);
                                    int d1=abs(tx-b.BlockDR)+abs(ty-b.BlockUR);
                                    if(d0<d1&&mapInfo[p.x][p.y]!=BarrierMask)
                                        tx=p.x,ty=p.y;
                                }
                                HumanMove(b.SN,(tx+0.5)*BLOCKSIDELENGTH,(ty+0.5)*BLOCKSIDELENGTH);
                            }
                            //运往对面
                            static set<int>gogogo;
                            static set<int>targeted;
                            if(b.Resource==5&&!gogogo.count(b.SN)){
                                //找到一个离该船最近的敌方大陆岸边
                                int tx=1e6,ty=1e6;
                                for(auto&p:enemySeaSide){
                                    int dis0=abs(tx-b.BlockDR)+abs(tx-b.BlockUR);
                                    int dis1=abs(p.x-b.BlockDR)+abs(p.y-b.BlockUR);
                                    if(dis1<dis0)tx=p.x,ty=p.y;
                                }
                                HumanMove(b.SN,(tx+0.5)*BLOCKSIDELENGTH,(ty+0.5)*BLOCKSIDELENGTH);
                                gogogo.insert(b.SN);
                            }
                            //对所有运输的船进行检查,并且安排他们回来
                            for(auto sn:gogogo){
                                tagFarmer boat=*(tagFarmer*)GetObj(sn);
                                if(boat.Resource==0){
                                    gogogo.erase(sn);
                                    targeted.erase(sn);
                                    return;
                                }
                            }
                            //农民上船
                            for(auto&b:boat){
                                if(targeted.count(b.SN))continue;
                                auto&&farmers=AllocateFarmers(5,b.BlockDR,b.BlockUR,1,1);
                                if(farmers.size()==5){
                                    targeted.insert(b.SN);
                                    for(auto sn:farmers)
                                        HumanAction(sn,b.SN);
                                }
                            }
                        }
                        //运输农民

                    },
                    [&](){
                        return 0;
                    }

                    );
    }
    //让所有农民去探索，找到一个可以建造兵营的地方
    {
        static array<int,2>  place={INT_MAX,INT_MAX};
        static int cnt=0;
        static vector<int>buildType{BUILDING_ARMYCAMP,BUILDING_RANGE,BUILDING_STABLE};
        static vector<int>condition{BUILD_ARMYCAMP_WOOD,BUILD_RANGE_WOOD,BUILD_STABLE_WOOD};
        nodes["explore"]=new Node(
                    emptyInit,
                    [&](){
                        //找到所有在敌方大陆的农民
                        if(enemyLand.empty())return;
                        auto&v=enemyLand.front();
                        vector<tagFarmer>farmer;
                        for(auto&f:info.farmers){
                            if(blockIndex[f.BlockDR][f.BlockUR]==blockIndex[v.x][v.y])
                            {
                                farmer.push_back(f);
                            }
                        }
                        //
                        if(farmer.empty())return;
                        //如果找到一块可以建造兵营的敌方,那么就建造兵营
                        if(cnt<buildType.size()){
                            if(place[0]==INT_MAX)
                            {
                                place=FindForBuildingInEnemyLand(buildType[cnt],0,0);
                            }
                            else if(place[0]!=INT_MAX&&!IsExistBuildTask(place[0],place[1])){
                                vector<int>f;
                                for(auto&obj:farmer)f.push_back(obj.SN);
                                BuildBuilding(f,place[0],place[1],buildType[cnt],[&](){return info.Wood>=condition[cnt];});
                                return;
                            }
                            else if(IsExistBuildTask(place[0],place[1])&&!IsBuilt(place[0],place[1]))return;
                            //
                            else if(IsBuilt(place[0],place[1]))
                            {
                                ++cnt;
                                if(cnt==buildType.size())
                                AddMsg("ArmyCampBuilt");
                                else {
                                    place={INT_MAX,INT_MAX};
                                    return;
                                }
                            }
                        }
                        //让农民去探索
                        for(int i=0;i<farmer.size();++i){
                            auto&f=farmer[i];
                            //如果他的目的地是不可达的，重新选定方向
                            if(f.NowState==HUMAN_STATE_IDLE)
                            {
                                auto&&v=GetUnExplore(f.BlockDR,f.BlockUR);
                                if(v.size()){
                                    int idx=randint(0,v.size()-1);
                                    auto&p=v[idx];
                                    HumanMove(f.SN,(p.x+0.5)*BLOCKSIDELENGTH,(p.y+0.5)*BLOCKSIDELENGTH);
                                }
                            }
                        }
                    },
                    [&](){
                        return FindMsg("ArmyCampBuilt");
                    }

                    );

    }
    //生产兵力和升级兵器伤害
    {
        static bool upgrade1=0;
        static bool upgrade0=0;
        static bool upgrade2=0;
        static bool upgrade3=0;
        static bool upgrade4=0;
        static bool upgrade5=0;
        static bool upgrade6=0;
        nodes["ClubManGenerate"]=new Node(
                    emptyInit,
                    [&](){
                        static bool flag=0;
                        if(!(flag||(flag=(FindMsg("ArmyCampBuilt")||(info.Meat>=500&&upgrade)))))return;
                        //升级板斧兵
                        if(!upgrade0){
                            for(auto&obj:info.buildings){
                                if(obj.Type==BUILDING_STOCK&&obj.Percent>=100){
                                    if(obj.Project==0){
                                        if(info.Meat>=BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY_FOOD){
                                            BuildingAction(obj.SN,BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY);
                                            break;
                                        }
                                    }
                                    else if(obj.Project==BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY){
                                        upgrade0=1;
                                        break;
                                    }
                                }
                            }
                        }
                        if(!upgrade1){
                            for(auto&obj:info.buildings){
                                if(obj.Type==BUILDING_STOCK&&obj.Percent>=100){
                                    if(obj.Project==0){
                                        if(info.Meat>=BUILDING_STOCK_UPGRADE_USETOOL_FOOD){
                                            BuildingAction(obj.SN,BUILDING_STOCK_UPGRADE_USETOOL);
                                            break;
                                        }
                                    }
                                    else if(obj.Project==BUILDING_STOCK_UPGRADE_USETOOL){
                                        upgrade1=1;
                                        break;
                                    }
                                }
                            }
                            return;
                        }
                        if(!upgrade2){
                            for(auto&obj:info.buildings){
                                if(obj.Type==BUILDING_ARMYCAMP&&obj.Percent>=100){
                                    if(obj.Project==0){
                                        if(info.Meat>=BUILDING_ARMYCAMP_UPGRADE_CLUBMAN_FOOD){
                                            BuildingAction(obj.SN,BUILDING_ARMYCAMP_UPGRADE_CLUBMAN);
                                            break;
                                        }
                                    }
                                    else if(obj.Project==BUILDING_ARMYCAMP_UPGRADE_CLUBMAN){
                                        upgrade2=1;
                                        break;
                                    }
                                }
                            }
                        }
                        if(!upgrade3){
                            for(auto&obj:info.buildings){
                                if(obj.Type==BUILDING_STOCK&&obj.Percent>=100){
                                    if(obj.Project==0){
                                        if(info.Meat>=BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER_FOOD){
                                            BuildingAction(obj.SN,BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER);
                                            break;
                                        }
                                    }
                                    else if(obj.Project==BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER){
                                        upgrade3=1;
                                        break;
                                    }
                                }
                            }
                        }
                        if(!upgrade4){
                            for(auto&obj:info.buildings){
                                if(obj.Type==BUILDING_MARKET&&obj.Percent>=100){
                                    if(obj.Project==0){
                                        if(info.Meat>=BUILDING_MARKET_WOOD_UPGRADE_FOOD&&info.Wood>=BUILDING_MARKET_WOOD_UPGRADE_WOOD){
                                            BuildingAction(obj.SN,BUILDING_MARKET_WOOD_UPGRADE);
                                            break;
                                        }
                                    }
                                    else if(obj.Project==BUILDING_MARKET_WOOD_UPGRADE){
                                        upgrade4=1;
                                        break;
                                    }
                                }
                            }
                        }
                        if(!upgrade5){
                            for(auto&obj:info.buildings){
                                if(obj.Type==BUILDING_MARKET&&obj.Percent>=100){
                                    if(obj.Project==0){
                                        if(info.Meat>=BUILDING_MARKET_GOLD_UPGRADE_FOOD&&info.Wood>=BUILDING_MARKET_GOLD_UPGRADE_WOOD){
                                            BuildingAction(obj.SN,BUILDING_MARKET_GOLD_UPGRADE);
                                            break;
                                        }
                                    }
                                    else if(obj.Project==BUILDING_MARKET_GOLD_UPGRADE){
                                        upgrade5=1;
                                        break;
                                    }
                                }
                            }
                        }
                        if(!upgrade6){
                            for(auto&obj:info.buildings){
                                if(obj.Type==BUILDING_STOCK&&obj.Percent>=100){
                                    if(obj.Project==0){
                                        if(info.Meat>=BUILDING_STOCK_UPGRADE_DEFENSE_RIDER_FOOD){
                                            BuildingAction(obj.SN,BUILDING_STOCK_UPGRADE_DEFENSE_RIDER);
                                            break;
                                        }
                                    }
                                    else if(obj.Project==BUILDING_STOCK_UPGRADE_DEFENSE_RIDER){
                                        upgrade6=1;
                                        break;
                                    }
                                }
                            }
                        }
                        //如果任意所有项均完成,可以出击
                        if(upgrade0&&upgrade1&&upgrade2&&upgrade3&&upgrade4&&upgrade5&&upgrade6){
                            AddMsg("I am hero!");
                        }
                        //如果当前兵力达到15,那么让农民主动探索更大区域
                        int cnt=Count(info.armies,[&](tagArmy army){return army.Sort!=AT_SHIP;});
                        if(cnt>=10){
                            AddMsg("exploreMore");
                            return;
                        }
                        //生产投石兵和刀斧手
                        int SN=-1;
                        for(auto&obj:info.buildings){
                            if(obj.Type==BUILDING_ARMYCAMP&&obj.Project==0&&obj.Percent>=100){
                                SN=obj.SN;
                                break;
                            }
                        }
                        if(SN==-1)return;
                        int clubManCnt=Count(info.armies,[&](tagArmy obj){return obj.Sort==AT_CLUBMAN;});
                        //int stoneManCnt=Count(info.armies,[&](tagArmy obj){return obj.Sort==AT_SLINGER;});
                        if(clubManCnt<=8){
                            if(info.Meat>=BUILDING_ARMYCAMP_CREATE_CLUBMAN_FOOD)
                                BuildingAction(SN,BUILDING_ARMYCAMP_CREATE_CLUBMAN);
                        }
                        /*
                        if(stoneManCnt<=5){
                            if(info.Meat>=BUILDING_ARMYCAMP_CREATE_SLINGER_FOOD)
                                BuildingAction(SN,BUILDING_ARMYCAMP_CREATE_SLINGER);
                        }
                        */
                        //生产弓箭手
                        SN=-1;
                        for(auto&obj:info.buildings){
                            if(obj.Type==BUILDING_RANGE&&obj.Project==0&&obj.Percent>=100){
                                SN=obj.SN;
                                break;
                            }
                        }
                        if(SN==-1)return;
                        int bowManCnt=Count(info.armies,[&](tagArmy obj){return obj.Sort==AT_BOWMAN;});
                        if(bowManCnt<=5)
                        {
                            if(info.Meat>=BUILDING_RANGE_CREATE_BOWMAN_FOOD)
                                BuildingAction(SN,BUILDING_RANGE_CREATE_BOWMAN);
                        }
                        //生产骑士
                        SN=-1;
                        for(auto&obj:info.buildings){
                            if(obj.Type==BUILDING_STABLE&&obj.Project==0&&obj.Percent>=100)
                            {
                                SN=obj.SN;
                                break;
                            }
                        }
                        if(SN==-1)return;
                        int scout=Count(info.armies,[&](tagArmy obj){return obj.Sort==AT_SCOUT;});
                        if(scout<=5){
                            if(info.Meat>=BUILDING_STABLE_CREATE_SCOUT_FOOD)
                                BuildingAction(SN,BUILDING_STABLE_CREATE_SCOUT);
                        }
                    },
                    [&](){
                        return 0;
                    }
                    );
    }
    //士兵巡逻，坑杀敌人
    {
        nodes["killArmy"]=new Node(
                    emptyInit,
                    [&](){
                        //if(!FindMsg("I am hero!"))return;
                        auto&&army=findTargetObj(info.armies,[&](const tagArmy&obj)->bool{return obj.Sort!=AT_SHIP;});
                        auto&&bowMan=findTargetObj(info.enemy_armies,[&](const tagArmy&obj)->bool{return obj.Sort==AT_BOWMAN;});
                        auto&&other=findTargetObj(info.enemy_armies,[&](const tagArmy&obj)->bool{return obj.Sort!=AT_SHIP&&obj.Sort!=AT_BOWMAN;});
                        //如果发现敌人,力斩不饶
                        if(bowMan.size()||other.size()){
                            //三个围殴一个杀，我们就简单得选取三个杀他一个，不做优化
                            map<int,tagArmy>a;
                            for(tagArmy&obj:army)if(obj.WorkObjectSN==-1)a[obj.SN]=obj;
                            vector<tagArmy>enemy=findTargetObj(info.enemy_armies,[&](const tagArmy&obj)->bool{return obj.Sort!=AT_SHIP;});
                            for(int i=0;i<enemy.size()&&a.size();++i){
                                vector<pair<int,double>>dis;
                                for(auto&ele:a){
                                    dis.push_back({ele.first,Distance(enemy[i],ele.second)});
                                }
                                sort(dis.begin(),dis.end(),[&](pair<int,double> t0,pair<int,double> t1){return t0.second<t1.second;});
                                for(int j=0;j<3&&a.size();++j){
                                    HumanAction(dis[j].first,enemy[i].SN);
                                    a.erase(dis[j].first);
                                }
                            }
                        }

                    },
                    [&](){
                        return 0;
                    }

             );
    }
    //农民开始疯狂探索
    {
        static bool flag=0;
        nodes["exploreMore"]=new Node(
                emptyInit,
                    [&](){
                        if(!(flag||(flag=FindMsg("exploreMore"))))return;
                        vector<tagFarmer>farmer=findTargetObj(info.farmers,[&](tagFarmer f){return f.FarmerSort==FARMERTYPE_FARMER&&blockIndex[f.BlockDR][f.BlockUR]==blockIndex[enemyLand.front().x][enemyLand.front().y];});
                        for(int i=0;i<farmer.size();++i){
                            auto&f=farmer[i];
                            //如果他的目的地是不可达的，重新选定方向
                            if(f.NowState==HUMAN_STATE_IDLE)
                            {
                                auto&&v=GetUnExplore(f.BlockDR,f.BlockUR);
                                if(v.size()){
                                    int idx=randint(0,v.size()-1);
                                    auto&p=v[idx];
                                    HumanMove(f.SN,(p.x+0.5)*BLOCKSIDELENGTH,(p.y+0.5)*BLOCKSIDELENGTH);
                                }
                            }
                        }
                        //
                    },
                    [&](){
                        int goldCnt=0;
                        for(auto&res:info.resources){
                            if(res.Type==RESOURCE_GOLD){
                                goldCnt+=res.Cnt;
                            }
                        }
                        if(goldCnt>=goldWin){
                           // AddMsg("crazyTime");
                        }
                        return goldCnt>=goldWin;
                    }
                    );
    }
    //调动所有人来采矿，盖仓库
    {
        static bool flag=0;
        static vector<vector<tagResource>>golds;
        nodes["crazy"]=new Node(
                    [&](){
                        //将金矿分类
                        vector<tagResource>gold=findTargetObj(info.resources,[&](tagResource res){return res.Type==RESOURCE_GOLD;});
                        set<tagResource>all{gold.begin(),gold.end()};
                        //
                        while(all.empty()==0){
                            golds.emplace_back();
                            auto&v=golds.back();
                            v.push_back(*all.begin());
                            all.erase(all.begin());
                            //
                            for(auto itr=all.begin();itr!=all.end();){
                                bool flag=0;
                                for(auto ele:v){
                                    int dis=Dis(ele,*itr);
                                    if(dis<=2){
                                        flag=1;
                                        break;
                                    }
                                }
                                if(flag){
                                    v.push_back(*itr);
                                    itr=all.erase(itr);
                                }
                                else itr=next(itr);
                            }
                        }
                        sort(golds.begin(),golds.end(),[&](vector<tagResource>&v0,vector<tagResource>&v1){
                            int d0=0,d1=0;
                            for(tagResource&ele:v0)d0+=ele.Cnt;
                            for(tagResource&ele:v1)d1+=ele.Cnt;
                            return d0>d1;
                        });
                        //回收所有我方大陆农民，一律过来淘金
                        for(tagFarmer f:info.farmers){
                            if(blockIndex[f.BlockDR][f.BlockUR]==blockIndex[Center.BlockDR][Center.BlockUR]){
                                if(f.FarmerSort==FARMERTYPE_FARMER){
                                    lock_farmers.erase(f.SN);
                                }
                            }
                        }
                    },
                    [&](){
                        //if(!(flag||(flag=FindMsg("crazyTime"))))return;
                        //这里我把它当作是exploreMore的下一个节点,所以我直接就不像之前那么写了
                        //只对第一大金矿附近建仓库
                        vector<tagFarmer>farmers=findTargetObj(info.farmers,[&](tagFarmer f){return f.FarmerSort==FARMERTYPE_FARMER&&blockIndex[f.BlockDR][f.BlockUR]==blockIndex[enemyLand.front().x][enemyLand.front().y];});
                        static array<int,2>pos={INT_MAX,INT_MAX};
                        if(pos[0]==INT_MAX){
                            pos=FindForBuilding(BUILDING_STOCK,golds.front().front().BlockDR,golds.front().front().BlockUR,blockIndex[enemyLand.front().x][enemyLand.front().y]);
                            if(pos[0]!=INT_MAX){
                                vector<int>f;
                                for(auto&obj:farmers)
                                    f.push_back(obj.SN);
                                BuildBuilding(f,pos[0],pos[1],BUILDING_STOCK,[&](){return info.Wood>=BUILD_STOCK_WOOD;});
                                return;
                            }
                        }else if(flag||(flag=IsBuilt(pos[0],pos[1]))){
                            vector<tagResource>gold=findTargetObj(info.resources,[&](tagResource res){return res.Type==RESOURCE_GOLD;});
                            tagBuilding stock=*(tagBuilding*)GetObj(PosToBuilding[pos]);
                            //已经被农民锁定的金块堆我也要剔除掉
                            set<int>locked;
                            for(tagFarmer&f:info.farmers){
                                if(f.WorkObjectSN!=-1){
                                    locked.insert(f.WorkObjectSN);
                                }
                            }

                            //分配农民去挖矿
                            for(tagFarmer f:farmers){
                                if(f.WorkObjectSN==-1){
                                    //寻找离他最近的矿去采
                                    sort(gold.begin(),gold.end(),[&](tagResource r0,tagResource r1)->bool{
                                        if(locked.size()!=gold.size()&&locked.count(r0.SN))return 0;
                                        return Distance(r0,f)<Distance(r1,f);
                                    });
                                    HumanAction(f.SN,gold[0].SN);
                                    locked.insert(gold[0].SN);
                                }
                            }
                        }
                    },
                    [&](){
                        return 0;
                    }

                    );
    }
    //
    nodes["exploreMore"]->nxt=nodes["crazy"];
    //
    AllNodes.push_back(nodes["BuildTask"]);
    AllNodes.push_back(nodes["killElephant"]);
    AllNodes.push_back(nodes["homeBuild"]);
    AllNodes.push_back(nodes["FarmerGenerate"]);
    AllNodes.push_back(nodes["killSheep"]);
    AllNodes.push_back(nodes["fishing"]);
    AllNodes.push_back(nodes["cutTree"]);
    AllNodes.push_back(nodes["buildDock"]);
    AllNodes.push_back(nodes["buildFishBoat"]);
    AllNodes.push_back(nodes["idle_curTree"]);
    AllNodes.push_back(nodes["catchFish"]);
    AllNodes.push_back(nodes["upgradeTime"]);
    AllNodes.push_back(nodes["buildShip"]);
    AllNodes.push_back(nodes["killEnemyShip"]);
    AllNodes.push_back(nodes["buildWoodBoat"]);
    AllNodes.push_back(nodes["transport"]);
    AllNodes.push_back(nodes["explore"]);
    AllNodes.push_back(nodes["ClubManGenerate"]);
    AllNodes.push_back(nodes["killArmy"]);
    AllNodes.push_back(nodes["buildGranary"]);
    AllNodes.push_back(nodes["buildMarket"]);
    AllNodes.push_back(nodes["Farming"]);
    AllNodes.push_back(nodes["exploreMore"]);
    ////////////////

}

void UsrAI::UpdateAll()
{
    //建立所有的sn对应的对象表
    {
        SnToObj.clear();
        for(auto&obj:info.armies)SnToObj[obj.SN]=&obj;
        for(auto&obj:info.buildings)SnToObj[obj.SN]=&obj;
        for(auto&obj:info.enemy_armies)SnToObj[obj.SN]=&obj;
        for(auto&obj:info.enemy_buildings)SnToObj[obj.SN]=&obj;
        for(auto&obj:info.enemy_farmers)SnToObj[obj.SN]=&obj;
        for(auto&obj:info.farmers)SnToObj[obj.SN]=&obj;
        for(auto&obj:info.resources)SnToObj[obj.SN]=&obj;
    }
    //建立建筑与坐标的映射
    {
        PosToBuilding.clear();
        for(auto&obj:info.buildings){
            PosToBuilding[array<int,2>{obj.BlockDR,obj.BlockUR}]=obj.SN;
        }
    }
    //找到市镇中心
    {
        for(auto&obj:info.buildings)
            if(obj.Type==BUILDING_CENTER)
            {
                Center=obj;
                break;
            }
    }
    //更新地图
    {
        auto&theMap=*info.theMap;
        for(int i=0;i<MAP_L;++i){
            for(int j=0;j<MAP_U;++j){
                tagTerrain terrain=theMap[i][j];
                if(terrain.type==MAPPATTERN_GRASS){
                    mapInfo[i][j]=terrain.height;
                }else{
                    mapInfo[i][j]=-1;
                }
            }
        }
        //标记斜坡
        for(int i=0;i<MAP_L;++i){
            for(int j=0;j<MAP_U;++j){
                tmp[i][j]=mapInfo[i][j];
                const static int off[][2]={{0,1},{0,-1},{1,0},{-1,0},{1,-1},{1,1},{-1,1},{-1,-1}};
                bool flag=1;
                if(mapInfo[i][j]>=0){
                    for(auto*o:off){
                        int ii=i+o[0],jj=j+o[1];
                        if(ii>=0&&i<MAP_L&&jj>=0&&jj<MAP_U){
                            if(mapInfo[i][j]!=mapInfo[ii][jj]){
                                flag=0;
                                break;
                            }
                        }
                    }
                }
                if(!flag)tmp[i][j]=-1;
            }
        }
        memcpy(mapInfo,tmp,sizeof(tmp));
        //将障碍物标记到地图
        for(auto&ele:SnToObj){
            tagObj&obj=*ele.second;
            mapInfo[obj.BlockDR][obj.BlockUR]=BarrierMask;
        }
        //将建筑标记到地图
        auto help=[&](vector<tagBuilding>&v)->void{
            for(auto&ele:v){
                int size=buildingSize[ele.Type];
                for(int i=0;i<size;++i)
                    for(int j=0;j<size;++j)
                        mapInfo[i+ele.BlockDR][j+ele.BlockUR]=BarrierMask;
            }
        };
        help(info.buildings);
        help(info.enemy_buildings);
        //将待建的建筑标记到地图
        for(auto&task:buildTask){
            int tp=get<1>(task);
            auto&pos=get<2>(task);
            int size=buildingSize[tp];
            for(int i=0;i<size;++i){
                for(int j=0;j<size;++j){
                    mapInfo[i+pos[0]][j+pos[1]]=BarrierMask;
                }
            }
        }
    }
    //更新各区块所属的大陆
    {
        memset(blockIndex,0,sizeof(blockIndex));
        int idx=0;
        auto&theMap=*(info.theMap);
        //一共两个大陆，先更新我方所在大陆(永远为1)
        function<void(int,int)>dfs=[&](int i,int j)->void{
            queue<array<int,2>>q;
            blockIndex[i][j]=idx;
            q.push({i,j});
            while(!q.empty()){
                int siz=q.size();
                while(siz--){
                    auto&e=q.front();
                    int i=e[0],j=e[1];
                    q.pop();
                    const static int off[][2]={{0,-1},{0,1},{-1,0},{1,0}};
                    for(auto*o:off){
                        int ii=o[0]+i,jj=o[1]+j;
                        if(ii>=0&&jj>=0&&ii<MAP_L&&jj<MAP_U&&!blockIndex[ii][jj]){
                            if(theMap[i][j].type==theMap[ii][jj].type){
                                blockIndex[ii][jj]=idx;
                                q.push({ii,jj});
                            }
                        }
                    }
                }
            }
        };
        ++idx;
        dfs(Center.BlockDR,Center.BlockUR);
        //更新其他区域
        for(int i=0;i<MAP_L;++i){
            for(int j=0;j<MAP_U;++j){
                if(!blockIndex[i][j]){
                    ++idx;
                    dfs(i,j);
                }
            }
        }
       //看看有没有找到敌方大陆
        enemyLand.clear();
        enemySeaSide.clear();
        for(int i=0;i<MAP_L;++i){
            for(int j=0;j<MAP_U;++j){
                if(theMap[i][j].type==MAPPATTERN_GRASS&&blockIndex[i][j]!=blockIndex[Center.BlockDR][Center.BlockUR]){
                    enemyLand.push_back({i,j});
                    blockIndex[i][j]=119;
                    if(checkIsSeaSide(i,j))enemySeaSide.push_back({i,j});
                }
            }
        }
        //将所有未探索的区域记下来
        unExplore.clear();
        for(int i=0;i<MAP_L;++i){
            for(int j=0;j<MAP_U;++j){
                if(theMap[i][j].type==MAPPATTERN_UNKNOWN){
                    unExplore.push_back({i,j});
                }
            }
        }
    }
    //获取所有空闲的农民和军队
    {
        idle_farmers.clear();
        idle_armies.clear();
        for(auto&obj:info.farmers)
            if(!lock_farmers.count(obj.SN)&&obj.FarmerSort==FARMERTYPE_FARMER&&InMyLand(obj.BlockDR,obj.BlockUR))//不把船和在地方大陆的农民算进去
                idle_farmers.insert(obj.SN);
        for(auto&obj:info.armies)
            if(obj.WorkObjectSN==-1)
                idle_armies.insert(obj.SN);
        //更新所有农民的位置
        farmerPos.clear();
        for(tagFarmer&f:info.farmers)farmerPos.insert({f.BlockDR,f.BlockUR});
    }
    //更新被砍得树
    {
        for(tagFarmer obj:info.farmers){
            if(obj.WorkObjectSN!=-1)treeIsCutting.insert(obj.WorkObjectSN);
        }
    }
    //清空所有节点消息
    {
        NodeMsg.swap(TheFrameNodeMsg);
        TheFrameNodeMsg.clear();
    }
    //更新所有工作点
    {
        decltype(AllNodes) updates;
        for(auto*node:AllNodes){
            auto*nxt=node->work();
            if(nxt)
            updates.push_back(nxt);
        }
        AllNodes.swap(updates);
    }
}

vector<int> UsrAI::AllocateFarmers(int n, int x, int y, bool lock,bool force)
{
    vector<int>ret;
    //
    struct Data{
        double dis;
        int sn;
    };
    vector<Data>all;
    for(int sn:idle_farmers){
        auto&ele=*(tagFarmer*)SnToObj[sn];
        double dis=abs(ele.BlockDR-x)+abs(ele.BlockUR-y);
        all.push_back({dis,ele.SN});
    }
    if(force&&all.size()<n)return {};
    sort(all.begin(),all.end(),[&](Data&d0,Data&d1){return d0.dis<d1.dis;});
    for(int i=0;i<min(int(all.size()),n);++i)ret.push_back(all[i].sn);
    //
    if(lock)
    for(auto sn:ret)
    {
        lock_farmers.insert(sn);
        idle_farmers.erase(sn);
    }
    //
    return ret;
}

void UsrAI::AllocateFarmers(vector<int> &v, int n, int x, int y, bool lock)
{
    //把死去的农民移除
    if(v.size()){
        int i=0,j=int(v.size())-1;
        while(i<j){
            if(SnToObj.count(v[i])==0){
                swap(v[i],v[j]);
                --j;
            }
            else ++i;
        }
        if(SnToObj.count(v[j])==0)--j;
        v.resize(j+1);
    }
    //
    if(v.size()>=n)return;
    auto&&ret=AllocateFarmers(n-v.size(),x,y,lock);
    for(auto sn:ret)v.push_back(sn);
}

void UsrAI::FreeFarmers(const vector<int> sn)
{
    for(auto sn_:sn)
    {
        lock_farmers.erase(sn_);
    }
}


array<int, 2> UsrAI::FindSpace(int size, int x, int y,bool land,int index)
{
    if(size<=0)return {INT_MAX,INT_MAX};
    if(index==-1)index=blockIndex[Center.BlockDR][Center.BlockUR];
    //
    vector<array<int,2>>fit;
    for(int i=0;i<MAP_L;++i){
        for(int j=0;j<MAP_U;++j){
            bool ava=1;
            for(int l0=0;l0<size&&ava;++l0){
                for(int l1=0;l1<size&&ava;++l1){
                    int ii=i+l0,jj=j+l1;
                    if(ii>=0&&jj>=0&&ii<MAP_L&&jj<MAP_U){
                        if(blockIndex[ii][jj]==index){
                            int h=mapInfo[ii][jj];
                            if(land&&h>=0&&h<=MAPHEIGHT_MAX&&h==mapInfo[i][j])continue;
                            if(!land&&h==mapInfo[i][j])continue;
                        }
                    }
                    ava=0;
                }
            }
            if(ava)
                fit.push_back({i,j});
        }
    }
    //寻找最合适的
    array<int,2> ret={INT_MAX,INT_MAX};
    for(array<int,2>ele:fit){
        //这里就简答以中心点为目标点
        long long tx=x+size/2,ty=y+size/2;
        if(abs(tx-ret[0])+abs(ty-ret[1])>abs(tx-ele[0])+abs(ty-ele[1]))
            ret=ele;
    }
    if(PosToBuilding.count(ret)){
        cout<<"Why ?"<<endl;
    }
    return ret;
}

array<int, 2> UsrAI::FindForBuilding(int type, int x, int y,int index)
{
     auto&&ret=FindSpace(buildingSize[type]+1,x,y,type!=BUILDING_DOCK,index);
     return ret;
}

array<int, 2> UsrAI::FindForBuilding(int tar, int src, int x, int y,int dis,int index)
{
    int size=buildingSize[tar];
    if(size<=0)return {INT_MAX,INT_MAX};
    if(index==-1)index=blockIndex[Center.BlockDR][Center.BlockUR];
    //
    bool land=tar!=BUILDING_DOCK;
    vector<array<int,2>>fit;
    for(int i=0;i<MAP_L;++i){
        for(int j=0;j<MAP_U;++j){
            bool ava=1;
            for(int l0=0;l0<size&&ava;++l0){
                for(int l1=0;l1<size&&ava;++l1){
                    int ii=i+l0,jj=j+l1;
                    if(ii>=0&&jj>=0&&ii<MAP_L&&jj<MAP_U){
                        if(blockIndex[ii][jj]==index){
                            int h=mapInfo[ii][jj];
                            if(land&&h>=0&&h<=MAPHEIGHT_MAX&&h==mapInfo[i][j])continue;
                            if(!land&&h==MAPHEIGHT_OCEAN)continue;
                        }
                    }
                    ava=0;
                }
            }
            if(ava)fit.push_back({i,j});
        }
    }
    //寻找最合适的
    int size0=buildingSize[src];
    int xx=size0+x,yy=size0+y;
    array<int,2> ret={INT_MAX,INT_MAX};
    for(array<int,2>ele:fit){
        //这里就简单以中心点为目标点
        long long tx=x+size/2,ty=y+size/2;
        if(abs(tx-ret[0])+abs(ty-ret[1])>abs(tx-ele[0])+abs(ty-ele[1])){
            //保证距离src建筑距离至少为dis
            if((x-(ele[0]+size))>=dis||(ele[0]-xx)>=dis||(y-(ele[1]+size))>=dis||(ele[1]-yy)>=dis)
                ret=ele;
        }
    }
    //
    if(PosToBuilding.count(ret)){
        cout<<"Why ?"<<endl;
    }
    return ret;
}

array<int, 2> UsrAI::FindForBuildingInEnemyLand(int type, int x, int y)
{
    int size=buildingSize[type];
    auto index=blockIndex[enemyLand.front().x][enemyLand.front().y];
    //
    vector<array<int,2>>fit;
    for(int i=0;i<MAP_L;++i){
        for(int j=0;j<MAP_U;++j){
            bool ava=1;
            for(int l0=0;l0<size&&ava;++l0){
                for(int l1=0;l1<size&&ava;++l1){
                    int ii=i+l0,jj=j+l1;
                    if(ii>=0&&jj>=0&&ii<MAP_L&&jj<MAP_U){
                        //检查四个块，不能有未探索的区域
                        const static int off[][2]={{0,-1},{0,1},{-1,0},{1,0}};
                        for(auto*o:off){
                            int i3=ii+o[0],j3=jj+o[1];
                            if(i3>=0&&j3>=0&&i3<MAP_L&&j3<MAP_U){
                                if(mapInfo[i3][j3]<0){
                                    ava=0;
                                }
                            }
                        }
                        if(blockIndex[ii][jj]==index){
                            int h=mapInfo[ii][jj];
                            if(h>=0&&h<=MAPHEIGHT_MAX&&h==mapInfo[i][j])continue;
                        }
                    }
                    ava=0;
                }
            }
            if(ava)
                fit.push_back({i,j});
        }
    }
    //寻找最合适的
    array<int,2> ret={INT_MAX,INT_MAX};
    for(array<int,2>ele:fit){
        //这里就简答以中心点为目标点
        long long tx=x+size/2,ty=y+size/2;
        if(abs(tx-ret[0])+abs(ty-ret[1])>abs(tx-ele[0])+abs(ty-ele[1]))
            ret=ele;
    }
    if(PosToBuilding.count(ret)){
        cout<<"Why ?"<<endl;
    }
    return ret;
}

bool UsrAI::InMyLand(int x, int y)
{
    return blockIndex[x][y]==blockIndex[Center.BlockDR][Center.BlockUR];
}

bool UsrAI::IsYanAn(int x, int y)
{
    static const int off[][2]={ {0,-1},{1,-1},{0,2},{1,2},{-1,0},{-1,1},{2,0},{2,1} };
    bool flag=0;
    for(auto*o:off){
        int ii=o[0]+x,jj=o[1]+y;
        if(ii>=0&&jj>=0&&ii<MAP_L&&jj<MAP_U&&InMyLand(ii,jj)){
            flag=1;
            break;
        }
    }
    return flag;
}

int UsrAI::Dis(const tagObj &t0,const  tagObj &t1)
{
    return abs(t0.BlockDR-t1.BlockDR)+abs(t0.BlockUR-t1.BlockUR);
}

double UsrAI::Distance(const tagObj &t0, const tagObj &t1)
{
    auto d0=(t0.BlockDR-t1.BlockDR)*BLOCKSIDELENGTH;
    auto d1=(t0.BlockUR-t1.BlockUR)*BLOCKSIDELENGTH;
    return d0*d0+d1*d1;
}

double UsrAI::Distance(const array<double, 2>&p0, const array<double, 2>&p1)
{
    auto d0=p0[0]-p1[0],d1=p0[1]-p1[1];
    return sqrt(d0*d0+d1*d1);
}

bool UsrAI::checkIsSeaSide(int i, int j,bool landF)
{
    auto f0=landF?MAPPATTERN_GRASS:MAPPATTERN_OCEAN;
    auto f1=!landF?MAPPATTERN_GRASS:MAPPATTERN_OCEAN;
    if((*info.theMap)[i][j].type!=f0)return 0;
    const static int off[][2]={{0,1},{0,-1},{-1,0},{1,0}};
    bool flag=0;
    for(auto*o:off){
        int ii=o[0]+i,jj=o[1]+j;
        if(ii>=0&&ii<MAP_L&&jj>=0&&jj<MAP_U){
            flag=(*info.theMap)[ii][jj].type==f1;
        }
        if(flag)break;
    }
    return flag;
}

bool UsrAI::checkTargetBlockIsEnemySideSea(double ii, double jj)
{
    int i=ii/BLOCKSIDELENGTH,j=jj/BLOCKSIDELENGTH;
    if((*info.theMap)[i][j].type!=MAPPATTERN_OCEAN)return 0;
    const static int off[][2]={{0,1},{0,-1},{-1,0},{1,0}};
    bool flag=0;
    for(auto*o:off){
        int ii=o[0]+i,jj=o[1]+j;
        if(ii>=0&&ii<MAP_L&&jj>=0&&jj<MAP_U){
            flag=(*info.theMap)[ii][jj].type==MAPPATTERN_GRASS;
            flag=flag&&blockIndex[ii][jj]==blockIndex[enemyLand.front().x][enemyLand.front().y];
        }
        if(flag)break;
    }
    return flag;
}

void UsrAI::GoToCutTree(int sn)
{
    tagFarmer f=*(tagFarmer*)GetObj(sn);
    vector<tagResource>&&trees=findTargetObj(info.resources,[&](tagResource res){
            return res.Type==RESOURCE_TREE&&blockIndex[res.BlockDR][res.BlockUR]==blockIndex[Center.BlockDR][Center.BlockUR];
});
    //寻找离他最近的树
    if(f.WorkObjectSN==-1){
        int tar=-1;
        for(auto tree:trees){
            if(treeIsCutting.count(tree.SN))continue;
            if(tar==-1)tar=tree.SN;
            else if(Distance(*SnToObj[tar],f)>Distance(tree,f))
                tar=tree.SN;
        }
        if(~tar){
            HumanAction(sn,tar);
        }
}
}

bool UsrAI::checkIsKaoAn(int i, int j,int index)
{
    const static int off[][2]={{0,1},{0,-1},{-1,0},{1,0}};
    bool flag=0;
    for(auto*o:off){
        int ii=o[0]+i,jj=o[1]+j;
        if(ii>=0&&ii<MAP_L&&jj>=0&&jj<MAP_U){
            if(blockIndex[ii][jj]==index)
            flag=(*info.theMap)[ii][jj].type==MAPPATTERN_GRASS;
        }
        if(flag)break;
    }
    return flag;
}

void UsrAI::BuildBuilding(const vector<int> &farmer, int x, int y, int type,const function<bool(void)>&check)
{
    using Type=remove_reference<decltype(*buildTask.begin())>::type;
    Type tp;
    get<0>(tp)=farmer;
    get<1>(tp)=type;
    get<2>(tp)={x,y};
    get<3>(tp)=check;
    buildTask.emplace_back(tp);
    buildingOcuppy.insert({x,y});
}

bool UsrAI::IsBuilt(int x, int y)
{
    return buildingBuilt.count("pos("+to_string(x)+","+to_string(y)+")built");
}

bool UsrAI::IsExistBuildTask(int x, int y)
{
    return buildingOcuppy.count({x,y});
}

vector<Point> UsrAI::GetUnExplore(int x, int y,bool land)
{
    int mnd=INT_MAX;
    vector<Point>ret;
    for(auto&p:unExplore){
        if(land&&!CheckLandPosition(x,y,p.x,p.y))continue;
        else if(!land&&!CheckOceanPosition(x,y,p.x,p.y))continue;
        int d=abs(x-p.x)+abs(y-p.y);
        if(d<mnd){
            mnd=d;
            ret.clear();
            ret.push_back(p);
        }
        else if(mnd==d){
            ret.push_back(p);
        }
    }
    return ret;
}

bool UsrAI::CheckLandPosition(int i, int j, int ti, int tj)
{
    auto&theMap=*(info.theMap);
    //如果与目的地夹着海洋,则不可达
    int itr=abs(ti-i)+abs(tj-j);
    for(int k=1;k<=itr;++k){
        float fac=k*1.0/itr;
        int ii=i*(1-fac)+fac*ti;
        int jj=j*(1-fac)+fac*tj;
        if(ii>=0&&ii<MAP_L&&jj>=0&&jj<MAP_U){
            if(theMap[ii][jj].type==MAPPATTERN_OCEAN)
                return 0;
        }
    }
    return 1;
}

bool UsrAI::CheckOceanPosition(int i, int j, int ti, int tj)
{
    auto&theMap=*(info.theMap);
    //如果与目的地夹着海洋,则不可达
    int itr=abs(ti-i)+abs(tj-j);
    for(int k=1;k<=itr;++k){
        float fac=k*1.0/itr;
        int ii=i*(1-fac)+fac*ti;
        int jj=j*(1-fac)+fac*tj;
        if(ii>=0&&ii<MAP_L&&jj>=0&&jj<MAP_U){
            if(theMap[ii][jj].type==MAPPATTERN_GRASS)
                return 0;
        }
    }
    return 1;
}

bool UsrAI::CheckGoldReach(int x, int y)
{
    //就单纯检测金矿四周是否有其他障碍物
    const static int off[][2]={{0,1},{0,-1},{1,0},{-1,0},{1,1},{1,-1},{-1,1},{-1,-1}};
    for(auto*o:off){
        int i=x+o[0],j=y+o[1];
        if(i>=0&&j>=0&&i<MAP_L&&j<MAP_U){
            if(mapInfo[i][j]!=-1||farmerPos.count({i,j}))return 1;
        }
    }
    return 0;
}

tagObj *UsrAI::GetObj(int sn)
{
    static vector<int>buf(1024*1024);
    if(SnToObj.count(sn))return SnToObj[sn];
    return (tagObj*)&buf[0];
}



template<class T>
bool UsrAI::checkWorkObjIsTargetType(int sn, vector<T> &v)
{
    for(auto&obj:v){
        if(obj.SN==sn)
            return 1;
    }
    return 0;
}



template<class T, class U>
int UsrAI::Count(const vector<T> &v, U op)
{
    int ans=0;
    for(auto&obj:v){
        ans+=op(obj);
    }
    return ans;
}

template<class T, class U>
vector<T> UsrAI::findTargetObj(const vector<T> &v, U op)
{
    vector<T>ret;
    for(auto&obj:v){
        if(op(obj))ret.push_back(obj);
    }
    return ret;
}

template<class U, class T>
bool UsrAI::checkCondition(const U &buf, const T &op)
{
    for(auto&obj:buf){
        if(op(obj))return 1;
    }
    return 0;
}
