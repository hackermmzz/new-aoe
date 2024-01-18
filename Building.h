#ifndef BUILDING_H
#define BUILDING_H

#include <Coordinate.h>

class Building:public Coordinate
{
public:
    Building();
    Building(int Num, int BlockDR, int BlockUR, int civ, int Percent=100);
    int getSort();

    static std::list<ImageResource>* getBuild(int i) {
        return build[i];
    }
    static std::list<ImageResource>* getBuilt(int i,int j) {
        return built[i][j];
    }
    static std::string getBuildingname(int index)
    {
        return Buildingname[index];
    }
    static std::string getBuiltname(int index1,int index2)
    {
        return Builtname[index1][index2];
    }

    static void allocatebuild(int i)
    {
        build[i]=new std::list<ImageResource>;
    }
    static void allocatebuilt(int i,int j)
    {
        built[i][j]=new std::list<ImageResource>;
    }
    static void deallocatebuild(int i) {
        delete build[i];
        build[i] = nullptr;
    }
    static void deallocatebuilt(int i,int j) {
        delete built[i][j];
        built[i][j] = nullptr;
    }
    int* getActions()
    {
        return actions;
    }

private:
    static std::list<ImageResource> *build[4];
    //建设list

    static std::list<ImageResource> *built[3][7];
    //建设完成的list

    int Num;
    //建筑编号

    double Percent = 0;
    //完成百分比 100时表示建筑已经被建造完成 根据完成度有不同的贴图

    int Finish=0;//0为未完成 1为完成

    int civ;
    //建筑所处时代 来确定不同时代建筑有何变化 ？时代要不要用player类下的

    double hpPercent;

    int Foundation;
    //地基类型

    static std::string Buildingname[4];
    static std::string Builtname[3][7];

    std::string BuildDisplayName[7]={"房屋","谷仓","市镇中心","仓库","农场","市场","箭塔"};
    int BuildingMaxBlood[7]={600,600,600,600,600,600,600};
    int actions[ACT_WINDOW_NUM_FREE] = {0};
};

#endif // BUILDING_H
