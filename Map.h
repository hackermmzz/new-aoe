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
    void init();

    // 重绘海岸
    void refineShore();
    //重绘基础地形
    void refineBaseTerrain();
    // 实时更新指定区域的海滩绘制
    void updateShoreArea(int centerL, int centerU, int radius = 2);
    //判断某一块土地是否是沙滩
    __forceinline bool IsBeach(int x,int y){return (cell[x][y].Num >= 29 && cell[x][y].Num <= 40);}
    //判断某一块土地是否是海洋
    __forceinline bool IsOcean(int x,int y){  return cell[x][y].getMapType() == MAPTYPE_OCEAN;}
    // 重置block为草地__forceinline bool Map::IsBeach(int x, int y)
    void resetBlockToGrass(int blockL, int blockU);
    // 检查block是否应该为海滩
    bool shouldBeBeach(int blockL, int blockU);
    //
    vector<QString> GetAllTargetFiles(QString suffix);
    // 判断地图块是否为斜坡
    bool isSlope(int BlockDR, int BlockUR);

    void JudegCellType(int BlockDR,int BlockUR);
    array<int,2> GetCellOffset(int BlockDR, int BlockUR);
    void CalCellOffset(int BlockDR,int BlockUR);
    //划分地图
     void divideTheMap_oceanPlay();
     void divideTheMap_commonPlay();
    //获取指定cell的偏移
    int getCellOffsetX(int l,int u);
    int getCellOffsetY(int l,int u);
    /*********************寻路相关*******************/
    //加载寻路用地图 视野+障碍物
    using TypeRef=vector<vector<int>>;
    TypeRef& loadfindPathMap(MoveObject* moveOb);
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
    vector<Point>& findBlock_Free(Point blockPoint, int lenth,bool landUnit);

    //判断指定格是否符合移动对象的地形类型；landUnit=true表示只能位于非海洋格
    bool isTerrainValidForMove(const Point& block, bool landUnit);
    //用于恢复已经位于错误地形中的对象；优先返回未被占用的最近合法格
    Point findNearestValidTerrainBlock(const Point& start, bool landUnit);

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
        for(int x = object->getBlockDR(); x<int(object->getBlockDR()+object->get_BlockSizeLen()); x++ )
            for(int y = object->getBlockUR(); y<int(object->getBlockUR()+object->get_BlockSizeLen()); y++)
                map_Object[x][y].push_back(object);
    }

    void reset_Map_Object_Resource();

    //更新用户视野状况
    void reset_CellExplore(Coordinate* eye,vector<Point>&store);
    void clear_CellVisible();

    void reset_ObjectExploreAndVisible();


    void setPlayer(Player** player){ this->player = player; }

    int addStaticRes(int Num,Double DR,Double UR);

    int addStaticRes(int Num, int BlockDR, int BlockUR);

    bool addAnimal(int Num,Double DR,Double UR);

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
    vector<vector<int>> blockIndex;
    int enemyBlockIdx;//敌人所属的大陆的区块编号
    bool enemyLandExplored;//标记地方大陆是否被探索
    // 用于存储地图
    Block **cell=new Block*[MAP_L];
    int intmap[72][72]={};

    std::list<StaticRes *> staticres={};
    std::list<Animal *> animal={};
//    std::list<Ruin *> ruin={};

    //打开的地图文件
    QString MapFileName;
    //
    //用于记录需要监视视野的Ob的视野格子和各Ob所在位置的地图
    vector<vector<vector<Coordinate*>>> map_Vision;   //对需要实时监视的ob所能看到的格子，填入ob相应的coordinate  实时监视是指瞪羚逃跑、狮子索敌等
    vector<vector<vector<Coordinate*>>> map_Object;   //对ob所在位置——有体积size，填入相应的coordinate

    //高层地图
    ///>=0为高度， = -1表示其为坡
    vector<vector<int>> map_Height;


    /*************  取消使用，待删除   ***********************/
    //当前已经探索的区域
    vector<vector<bool>> explored;
    /************************************/

public:
    int mapIdx = Rand.nextRaw()%4 + 1;
    QString GetMapFileName();
    int CheckNeighborType(int x, int y, int selectType);
    bool CheckBorder(int x, int y, int currentCalHeight);
    void GenerateType();        // 依据高度生成地形图Block种类
    void CalOffset();           // 计算每个Block的偏移量
    void InitFaultHandle();     // 初始化错误处理
    void InitCell(int Num, bool isExplored, bool isVisible);
    void ResetMapType(int blockL, int blockU);
    void loadGenerateMapText();
    
    // 应用敌人状态到MainWidget
    void applyEnemyStatusToMainWidget(class MainWidget* mainWidget);
    bool  CheckIsNearOcean(int x,int y);
    Double tranL(Double BlockL);
    Double tranU(Double BlockU);

    //寻路障碍地图
    void clearfindPathMapTemperature();
    void clearBarrierMap();
    void setBarrier(int blockDR,int blockUR , int blockSideLen = 1 );

    void drawEdge(vector<vector<int>>&tempMap,std::map<int, int> codeToNum,int MapType1,int MapType2,int MapType3);  // 绘制地形交界
    //合并森林
    void MergeTrees();

    Player** player;
    vector<vector<short>> m_heightMap;
    vector<vector<bool>> TreeBlock;//将森林按所处位置合并成森林
    vector<vector<int>> barrierMap;   //障碍物地图
    map<int,pair<string,void*>>enemyAreaLimit;//记录敌人能活动的区域限制
    map<int,string>enemyStatusMap;//记录敌人的状态(attack/defend)
    int EL;
    int EU;

    //记录当前帧可见格子
    stack<Point> blockLab_Visible;

    vector<vector<vector<int>>>findPathMapTemperature;
};

#endif // MAP_H
