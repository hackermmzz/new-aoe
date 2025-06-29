#include "UsrAI.h"
#include<set>
#include <iostream>
#include<unordered_map>
using namespace std;
tagGame tagUsrGame;
ins UsrIns;
//
//
/*##########DO NOT MODIFY THE CODE ABOVE##########*/
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

}

void UsrAI::InitActionNode()
{
    map<string,Node*>nodes;
    decltype(Node::action)emptyAction=[&](){};
    decltype(Node::condition)emptyCond=[&](){return false;};//形成自循环
    decltype(Node::init)emptyInit=[&](){};
    //生成农民
    {
        Node*farmerGenerate=new Node(
                    emptyInit,
                    [&](){
            //
            if(info.farmers.size()>=20)return;
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
                            auto&&v=AllocateFarmers(1,Center.BlockDR,Center.BlockUR,1);
                            if(v.size())farmerSN=v.front();
                            else return;
                        }
                        tagFarmer farmer=*(tagFarmer*)SnToObj[farmerSN];
                        if(farmer.WorkObjectSN==-1&&info.Wood>=BUILD_HOUSE_WOOD){
                            static array<int,2>pre={Center.BlockDR,Center.BlockUR};
                            array<int,2>place=FindForBuilding(BUILDING_HOME,BUILDING_CENTER,pre[0],pre[1],1);
                            if(place[0]!=INT_MAX){
                                pre=place;
                                HumanBuild(farmerSN,BUILDING_HOME,place[0],place[1]);
                            }
                        }
                    },
        [&](){
            int cnt=0;
            for(tagBuilding&obj:info.buildings){
                if(obj.Type==BUILDING_HOME&&obj.Percent>=100)++cnt;
            }
            if(cnt>=6){
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
                    tagResource res=*(tagResource*)SnToObj[ele];
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
                    tagResource res=*(tagResource*)SnToObj[ele];
                    if(tarsn==-1)tarsn=ele;
                    else
                    {
                        tagResource tar=*(tagResource*)SnToObj[tarsn];
                        if(abs(tar.BlockDR-Center.BlockDR)+abs(tar.BlockUR-Center.BlockUR)>abs(res.BlockDR-Center.BlockDR)+abs(res.BlockUR-Center.BlockUR))
                        tarsn=ele;
                    }
                }
                //所有人直接对同一个渔场进行采集
                if(tarsn!=-1){
                    for(auto sn:farmer){
                        tagFarmer f=*(tagFarmer*)SnToObj[sn];
                        if(f.WorkObjectSN==-1){
                            HumanAction(sn,tarsn);
                        }
                    }
                }
            },
            [&](){
                return fish.size()==0;
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
                        AllocateFarmers(farmer,5,Center.BlockDR,Center.BlockUR);
                        //去砍树
                        for(auto sn:farmer){
                            tagFarmer f=*(tagFarmer*)SnToObj[sn];
                            //寻找离他最近的树
                            if(f.WorkObjectSN==-1){
                                int tar=-1;
                                for(auto treeSN:trees){
                                    auto tree=*(tagResource*)SnToObj[treeSN];
                                    if(tar==-1)tar=treeSN;
                                    else if(Dis(*SnToObj[tar],f)>Dis(*SnToObj[treeSN],f))
                                        tar=treeSN;
                                }
                                if(~tar){
                                    HumanAction(sn,tar);
                                }
                            }
                        }
                    },
                    [&](){
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
        nodes["killSheep"]=new Node(
                    emptyCond,
                    [&](){
                        AllocateFarmers(farmers,2,Center.BlockDR,Center.BlockUR);
                        if(farmers.size()!=2)return;
                        list<tagResource>sheep;
                        bool alive=0;
                        for(auto&obj:info.resources)
                        {
                            if(obj.Type!=RESOURCE_GAZELLE)continue;
                            if(obj.Blood>0)alive=1;
                            sheep.push_back(obj);
                        }
                        //先杀掉所有🐏
                        if(alive){
                            tagFarmer f0=*(tagFarmer*)SnToObj[farmers[0]],f1=*(tagFarmer*)SnToObj[farmers[1]];
                            if(f0.WorkObjectSN!=-1&&(*(tagResource*)SnToObj[f0.WorkObjectSN]).Blood>0)return;
                            //找到离二者最近的🐏
                            int tar=-1;
                            for(auto&res:sheep){
                                if(res.Blood<=0)continue;
                                int dis=Dis(f0,res)+Dis(f1,res);
                                if(tar==-1)tar=res.SN;
                                else{
                                    tagResource tt=*(tagResource*)SnToObj[tar];
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
                        else if(!buildSock){
                            static bool first=1;
                            tagFarmer f0=*(tagFarmer*)SnToObj[farmers[0]],f1=*(tagFarmer*)SnToObj[farmers[1]];
                            if(f0.WorkObjectSN!=-1&&!first){
                                tagBuilding stock=*(tagBuilding*)SnToObj[f0.WorkObjectSN];
                                if(stock.Percent>=100){
                                    buildSock=1;
                                }
                                stockSN=f0.WorkObjectSN;
                                if(f1.WorkObjectSN!=f0.WorkObjectSN)
                                {
                                    HumanAction(f1.SN,f0.WorkObjectSN);
                                }
                                return;
                            }
                            if(info.Wood<BUILD_STOCK_WOOD)return;
                            first=0;
                            //计算出一个最佳点，这里我简单的取中点
                            int x=0,y=0;
                            for(auto&res:sheep){
                                x+=res.BlockDR,y+=res.BlockUR;
                            }
                            x/=sheep.size(),y/=sheep.size();
                            auto place=FindForBuilding(BUILDING_STOCK,x,y);
                            HumanBuild(f0.SN,BUILDING_STOCK,place[0],place[1]);
                        }
                        else if(sheep.size()){
                            //发送消息
                            AddMsg("pickGazalle");
                            //采集🐏🥩根据距离仓库得距离来算
                            if(stockSN==-1)return;
                            tagFarmer f0=*(tagFarmer*)SnToObj[farmers[0]],f1=*(tagFarmer*)SnToObj[farmers[1]];
                            if(f0.WorkObjectSN!=-1)return;
                            int tarsn=-1;
                            tagBuilding stock=*(tagBuilding*)SnToObj[stockSN];
                            for(auto&res:sheep){
                                if(tarsn==-1)tarsn=res.SN;
                                else{
                                    int dis0=Dis(res,stock),dis1=Dis(*(tagResource*)SnToObj[tarsn],stock);
                                    if(dis0<dis1)tarsn=res.SN;
                                }
                            }
                            //
                            if(tarsn!=-1)
                            for(auto sn:farmers)HumanAction(sn,tarsn);
                        }
                    },
                    [&](){
                        for(auto&obj:info.resources)if(obj.Type==RESOURCE_GAZELLE)return 0;
                        return 1;
                    }
        );
    }
    //在沿岸建造船坞
    {
        static int farmerSN=-1;
        static vector<array<int,2>>edge;
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
                                            if(!(ii>=0&&i<MAP_L&&jj>=0&&jj<MAP_U&&theMap[ii][jj].type==MAPPATTERN_OCEAN)){
                                                flag=0;
                                            }
                                        }
                                    }
                                }
                                if(flag&&IsYanAn(i,j))edge.push_back({i,j});
                            }
                        }
                    },
                    [&](){
                        if(farmerSN==-1){
                            auto&&v=AllocateFarmers(1,Center.BlockDR,Center.BlockUR);
                            if(v.size())farmerSN=v[0];
                            else return;
                        }
                        if(info.Wood<BUILD_DOCK_WOOD)return;
                        //随机选择一个地点进行建筑船坞(2个）
                        int idx=randint(0,edge.size()-1);
                        tagFarmer farmer=*(tagFarmer*)SnToObj[farmerSN];
                        if(farmer.WorkObjectSN==-1){
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
                            tagFarmer f=*(tagFarmer*)SnToObj[sn];
                            if(f.WorkObjectSN==-1){
                                int tar=-1;
                                for(auto treeSN:trees){
                                    auto tree=*(tagResource*)SnToObj[treeSN];
                                    if(tar==-1)tar=treeSN;
                                    else if(Dis(*SnToObj[tar],f)>Dis(*SnToObj[treeSN],f))
                                        tar=treeSN;
                                }
                                if(~tar){
                                    HumanAction(sn,tar);
                                }
                            }
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
                                        else if(Dis(obj,*SnToObj[tarsn])>Dis(obj,*SnToObj[sn]))tarsn=sn;
                                    }
                                    if(~tarsn)
                                    HumanAction(obj.SN,tarsn);
                                }
                            }
                        }
                    },
                    [&](){
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
    //造战船
    {
        nodes["buildShip"]=new Node(
                    emptyInit,
                    [&](){
                        if(upgrade==0)return;
                        if(info.Wood<BUILDING_DOCK_CREATE_SHIP_WOOD)return;
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
                        if(cnt>=4){
                            AddMsg("shipIsReady");
                            return 1;
                        }
                        return 0;
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
                        //寻找一个未探索的边角
                        long long tx=INT_MAX/2,ty=INT_MAX/2;
                        for(auto p:unexplored){
                            auto Dis=[&](int tx,int ty)->int{
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
                                if(obj.status==HUMAN_STATE_IDLE)
                                HumanMove(obj.SN,tx*BLOCKSIDELENGTH,ty*BLOCKSIDELENGTH);
                            }
                        }
                    },
                    [&](){
                        return 0;
                    }
                    );
    }
    //
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
    //获取所有空闲的农民和军队
    {
        idle_farmers.clear();
        idle_armies.clear();
        for(auto&obj:info.farmers)
            if(obj.WorkObjectSN==-1&&!lock_farmers.count(obj.SN)&&obj.FarmerSort==FARMERTYPE_FARMER)//不把船算进去
                idle_farmers.insert(obj.SN);
        for(auto&obj:info.armies)
            if(obj.WorkObjectSN==-1)
                idle_armies.insert(obj.SN);
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
                const static int off[][2]={{0,1},{0,-1},{1,0},{-1,0}};
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
            mapInfo[obj.BlockDR][obj.BlockUR]=-1;
        }
        //将建筑标记到地图
        auto help=[&](vector<tagBuilding>&v)->void{
            for(auto&ele:v){
                int size=buildingSize[ele.Type];
                for(int i=0;i<size;++i)
                    for(int j=0;j<size;++j)
                        mapInfo[i+ele.BlockDR][j+ele.BlockUR]=-1;
            }
        };
        help(info.buildings);
        help(info.enemy_buildings);

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

vector<int> UsrAI::AllocateFarmers(int n, int x, int y, bool lock)
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

array<int, 2> UsrAI::FindSpace(int size, int x, int y,bool land)
{
    if(size<=0)return {INT_MAX,INT_MAX};
    //
    vector<array<int,2>>fit;
    for(int i=0;i<MAP_L;++i){
        for(int j=0;j<MAP_U;++j){
            bool ava=1;
            for(int l0=0;l0<size&&ava;++l0){
                for(int l1=0;l1<size&&ava;++l1){
                    int ii=i+l0,jj=j+l1;
                    if(ii>=0&&jj>=0&&ii<MAP_L&&jj<MAP_U){
                        int h=mapInfo[ii][jj];
                        if(land&&h>=0&&h<=MAPHEIGHT_MAX&&h==mapInfo[i][j])continue;
                        if(!land&&h==mapInfo[i][j])continue;
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
    return ret;
}

array<int, 2> UsrAI::FindForBuilding(int type, int x, int y)
{
     auto&&ret=FindSpace(buildingSize[type]+1,x,y,type!=BUILDING_DOCK);
     return ret;
}

array<int, 2> UsrAI::FindForBuilding(int tar, int src, int x, int y,int dis)
{
    int size=buildingSize[tar];
    if(size<=0)return {INT_MAX,INT_MAX};
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
                        int h=mapInfo[ii][jj];
                        if(land&&h>=0&&h<=MAPHEIGHT_MAX&&h==mapInfo[i][j])continue;
                        if(!land&&h==MAPHEIGHT_OCEAN)continue;
                    }
                    ava=0;
                }
            }
            if(ava)fit.push_back({i,j});
        }
    }
    //寻找最合适的
    int size0=buildingSize[src];
    int xx=size0+x-1,yy=size0+y-1;
    array<int,2> ret={INT_MAX,INT_MAX};
    for(array<int,2>ele:fit){
        //这里就简单以中心点为目标点
        long long tx=x+size/2,ty=y+size/2;
        if(abs(tx-ret[0])+abs(ty-ret[1])>abs(tx-ele[0])+abs(ty-ele[1])){
            //保证距离src建筑距离至少为dis
            if(abs(ele[0]+size-x)>=dis&&abs(ele[0]-xx-1)>=dis
                    &&abs(ele[1]+size-y)>=dis&&abs(ele[1]-yy-1)>=dis)
                ret=ele;
        }
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

int UsrAI::Dis(tagObj &t0, tagObj &t1)
{
    return abs(t0.BlockDR-t1.BlockDR)+abs(t0.BlockUR-t1.BlockUR);
}

