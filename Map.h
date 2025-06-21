#ifndef MAP_H
#define MAP_H

#include <cstring>

#include "Block.h"
#include "config.h"
#include "StaticRes.h"
#include "Animal.h"
#include "Player.h"


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

    void generateCenter();

    // 生成城镇中心附近13*13的部分
    void generateCenterAround();

    //生成敌人
    void generateEnemy();

    // 随机生成沙漠地貌
    void genDesert(int i, int j, int number, int Map[][74]);

    // 生成不同地貌
    void generateLandforms();
    // 重绘海岸
    void refineShore();

    // 判断地图块是否为斜坡
    bool isSlope(int BlockDR, int BlockUR);

    void JudegCellType(int BlockDR,int BlockUR);
    void CalCellOffset(int BlockDR,int BlockUR);
    //划分地图
     void divideTheMap();
    /*********************寻路相关*******************/
    //加载寻路用地图 视野+障碍物
    using Type=int[MAP_L][MAP_U];
    using TypeRef=Type&;
    TypeRef loadfindPathMap(MoveObject* moveOb);
    void loadfindPathMapTemperature();
    //加载障碍物地图
    void loadBarrierMap(bool absolute = false);
    void loadBarrierMap_ByObjectMap();
    bool CanCrush(Coordinate*object);
    /*********************寻路相关*******************/
    bool isBarrier(Point blockPoint,int blockSideLen = 1 );
    bool isBarrier( int blockDR , int blockUR, int &bDR_barrier , int &bUR_barrier ,int blockSideLen = 1 );
    bool isBarrier( int blockDR , int blockUR,int blockSideLen = 1 );

    bool isHaveObject(int blockDR , int blockUR, int &bDR_barrier , int &bUR_barrier ,int blockSideLen);
    bool isFlat(Coordinate* judOb);
    bool isFlat(int blockDR , int blockUR,int blockSideLen = 1);
    vector<pair<Point,int>> findBlock_Free(Coordinate* object , int disLen = 1 , bool mustFind = true);
    vector<Point>& findBlock_Free(Point blockPoint, int lenth,bool land);
    vector<Point> findBlock_Flat(int disLen = 1);

    bool isOverBorder(int blockDR, int blockUR){ return blockDR<0 || blockDR>=MAP_L || blockUR<0 ||blockUR>=MAP_U; }

    //用于查找Object视野范围内的格子，返回格子的列表容器
    vector<Point> get_ObjectVisionBlock(Coordinate* object);

    vector<Point> get_ObjectBlock(Coordinate* object);

    int get_MapHeight(int blockDR , int blockUR)
    {
        if(blockDR<0 || blockUR<0 || blockDR>=MAP_L || blockUR>=MAP_U)
        {
            qDebug()<<"get_MapHeight overborder"<<" blockDR:"<<blockDR<<", blockUR:"<<blockUR;
            return 0;
        }
        else return cell[blockDR][blockUR].getMapHeight();
    }

    //初始化视野地图
    void init_Map_Vision(){
        for(int x = 0; x<MAP_L;x++)
            for(int y = 0 ; y<MAP_U;y++) map_Vision[x][y].clear();
    }
    void init_Map_Object(){
        for(int x = 0; x<MAP_L;x++)
            for(int y = 0 ; y<MAP_U;y++) map_Object[x][y].clear();
    }
    void init_Map_UseToMonitor(){
        for(int x = 0; x<MAP_L;x++)
            for(int y = 0 ; y<MAP_U;y++) {
                map_Object[x][y].clear();
                map_Vision[x][y].clear();
            }
    }

    void init_Map_Height();

    void add_Map_Vision( Coordinate* object );
    void add_Map_Object( Coordinate* object ){
        for(int x = object->getBlockDR(); x<object->getBlockDR()+object->get_BlockSizeLen(); x++ )
            for(int y = object->getBlockUR(); y<object->getBlockUR()+object->get_BlockSizeLen(); y++)
                map_Object[x][y].push_back(object);
    }

    void reset_Map_Object_Resource();

    //更新用户视野状况
    void reset_CellExplore(Coordinate* eye,vector<Point>&store);
    void clear_CellVisible();

    void reset_ObjectExploreAndVisible();


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
    //每一个block所属的块的编号
    int blockIndex[MAP_L][MAP_U];
    int enemyBlockIdx;//敌人所属的大陆的区块编号
    bool enemyLandExplored;//标记地方大陆是否被探索
    // 用于存储地图
    Block **cell=new Block*[MAP_L];
    int intmap[72][72]={};

    std::list<StaticRes *> staticres={};
    std::list<Animal *> animal={};
//    std::list<Ruin *> ruin={};


    int findPathMap[MAP_L][MAP_U] = {};

    //用于记录需要监视视野的Ob的视野格子和各Ob所在位置的地图
    vector<Coordinate*> map_Vision[MAP_L][MAP_U];   //对需要实时监视的ob所能看到的格子，填入ob相应的coordinate  实时监视是指瞪羚逃跑、狮子索敌等
    vector<Coordinate*> map_Object[MAP_L][MAP_U];   //对ob所在位置——有体积size，填入相应的coordinate

    //高层地图
    ///>=0为高度， = -1表示其为坡
    int map_Height[MAP_L][MAP_U] = {};


    /*************  取消使用，待删除   ***********************/
    //后续若需要给出障碍物地图，则修改该部分并启用

    tagMap resMap_UserAI[MAP_L][MAP_U];
    tagMap resMap_EnemyAI[MAP_L][MAP_U];
    //当前已经探索的区域
    bool explored[MAP_L][MAP_U];
    //更新用于AI的资源状况表
    void reset_resMap_AI();
    //传入player阵营，若是用户，传出已探索地图部分的地图资源信息，若是Enemy，传回完整的资源地图
    void reset_resMap_ForUserAndEnemy();
    /************************************/

public:
    int CheckNeighborHigher(int x, int y, int currentCalHeight);
    int CheckNeighborType(int x, int y, int selectType);
    int CheckNeighborForest(int x, int y, int forestCell[][FOREST_GENERATE_U]);
    bool CheckBorder(int x, int y, int currentCalHeight);
    bool GenerateTerrain();     // 用于生成地形高度（Block高度）
    void GenerateType();        // 依据高度生成地形图Block种类
    void CalOffset();           // 计算每个Block的偏移量
    void InitFaultHandle();     // 初始化错误处理
    void InitCell(int Num, bool isExplored, bool isVisible);
    void ResetMapType(int blockL, int blockU);
    void GenerateMapTxt(int MapJudge);
    void loadGenerateMapText(int MapJudge);
    bool  CheckIsNearOcean(int x,int y);
    double tranL(double BlockL);
    double tranU(double BlockU);

    //寻路障碍地图
    void clearfindPathMap(){memset(findPathMap,0,sizeof(findPathMap));}
    void clearfindPathMapTemperature(){memset(findPathMapTemperature,0,sizeof(findPathMapTemperature));}
    void clearBarrierMap(){ memset(barrierMap ,0 , sizeof(barrierMap)); }
    void setBarrier(int blockDR,int blockUR , int blockSideLen = 1 );

    void drawEdge(int tempMap[MAP_L][MAP_U],std::map<int, int> codeToNum,int MapType1,int MapType2,int MapType3);  // 绘制地形交界


    Player** player;
    short m_heightMap[GENERATE_L][GENERATE_L] = {{}};
    int Gamemap[MAP_L][MAP_U] = {};  // 地图资源二维数组
    bool mapFlag[MAP_L][MAP_U] = {{false}}; // 地图标识二维数组，0为可放置，1为不可放置

    int barrierMap[MAP_L][MAP_U];   //障碍物地图
    tagMap resMap_AI[MAP_L][MAP_U]; //为AI准备的资源地图
    int EL;
    int EU;

    //记录当前帧可见格子
    stack<Point> blockLab_Visible;

    int findPathMapTemperature[MAXPLAYER][MAP_L][MAP_U] = {};
};

#endif // MAP_H
