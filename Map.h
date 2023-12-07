#ifndef MAP_H
#define MAP_H

#include <Block.h>
#include <config.h>
//#include <Coores.h>
//#include <Animal.h>
//#include <Ruin.h>

class Map
{
public:
    Map();
    ~Map();
    void init(bool MapJudge);
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

    Block **cell=new Block*[MAP_L];

    int intmap[72][72]={};

//    std::list<Coores *> coores={};
//    std::list<Animal *> animal={};
//    std::list<Ruin *> ruin={};
};

#endif // MAP_H
