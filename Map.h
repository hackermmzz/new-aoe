#ifndef MAP_H
#define MAP_H

#include <Block.h>
#include <config.h>
#include <StaticRes.h>
#include <Animal.h>

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

    // 生成城镇中心附近11*11的部分
    void generateCenter();

    // 随机生成沙漠地貌
    void genDesert(int i, int j, int number, int Map[][74]);

    // 生成不同地貌
    void generateLandforms();

    void loadfindPathMap();
    void clearfindPathMap();

    int addStaticRes(int Num,double DR,double UR);

    int addStaticRes(int Num, int BlockDR, int BlockUR);

    int addAnimal(int Num,double DR,double UR);

    int loadResource();

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
    void GenerateTerrain();     // 用于生成地形（Block高度差）
    double smooth(double x);    // 用于平滑地图曲线
    void clearData();           // 用于清空所有地图创建时所建立的临时变量

};

#endif // MAP_H
