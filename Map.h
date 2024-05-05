#ifndef MAP_H
#define MAP_H

#include <Block.h>
#include <config.h>
#include <StaticRes.h>
#include <Animal.h>
#include <Player.h>
#include <cstring>

class Map
{
public:
    Map();
    ~Map();
    void init(int MapJudge);
    // 随机生成资源（多个），并添加到地图中
    void generateResources();

    // 随机生成资源（单个），并添加到地图中
    void generateResource();

    // 生成城镇中心附近13*13的部分
    void generateCenter();

    // 随机生成沙漠地貌
    void genDesert(int i, int j, int number, int Map[][74]);

    // 生成不同地貌
    void generateLandforms();

    void loadfindPathMap();
    bool isBarrier( int blockDR , int blockUR, int &bDR_barrier , int &bUR_barrier ,int blockSideLen = 1 );

    void setPlayer(Player** player){ this->player = player; }

    int addStaticRes(int Num,double DR,double UR);

    int addStaticRes(int Num, int BlockDR, int BlockUR);

    bool addAnimal(int Num,double DR,double UR);

    bool loadResource();

    list<Animal*>::iterator deleteAnimal( list<Animal*>::iterator iterDele)
    {
        delete *iterDele;
        return animal.erase(iterDele);
    }

    list<StaticRes*>::iterator deleteStaticRes( list<StaticRes*>::iterator iterDele )
    {
        delete *iterDele;
        return staticres.erase(iterDele);
    }
    
    // 用于存储地图
    Block **cell=new Block*[MAP_L];

    int intmap[72][72]={};

    std::list<StaticRes *> staticres={};
    std::list<Animal *> animal={};
//    std::list<Ruin *> ruin={};
    int findPathMap[72][72];

private:
    int CheckNeighborHigher(int x, int y, int currentCalHeight);
    int CheckNeighborType(int x, int y, int selectType);
    int CheckNeighborForest(int x, int y, int forestCell[][FOREST_GENERATE_U]);
    bool CheckBorder(int x, int y, int currentCalHeight);
    bool GenerateTerrain();     // 用于生成地形高度（Block高度）
    void GenerateType();        // 依据高度生成地形图Block种类
    void CalOffset();           // 计算每个Block的偏移量
    void InitFaultHandle();     // 初始化错误处理
    void InitCell(int Num, bool isExplored, bool isVisible);
    void GenerateMapTxt(int MapJudge);

    double tranL(double BlockL);
    double tranU(double BlockU);

    //寻路障碍地图
    void clearfindPathMap(){memset(findPathMap,0,sizeof(findPathMap));}
    void clearBarrierMap(){ memset(barrierMap ,0 , sizeof(barrierMap)); }
    void setBarrier(int blockDR,int blockUR , int blockSideLen = 1 );

    Player** player;
    short m_heightMap[80][80];
    int Gamemap[MAP_L][MAP_U];  // 地图资源二维数组
    bool mapFlag[MAP_L][MAP_U] = {{false}}; // 地图标识二维数组，0为可放置，1为不可放置

    int barrierMap[MAP_L][MAP_U];

};

#endif // MAP_H
