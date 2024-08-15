#ifndef USRAI_H
#define USRAI_H
#include "AI.h"
extern tagGame tagUsrGame;
extern ins UsrIns;
/*##########请勿修改以上部分##########*/
struct Position
{
    int x;
    int y;
};
struct Bush
{
    int SN;
    int x;
    int y;
    bool isCollected;
};
struct VillagerTask
{
    int villagerSN; // 村民序列号
    int bushSN;    // 浆果资源序列号
};
class UsrAI:public AI
{
public:
    UsrAI(){this->id=0;}
    ~UsrAI(){}
    int Distance(int x1,int y1,int x2,int y2);                    //计算曼哈顿距离
    double DistanceToPoints(int x1,int y1,int x2,int y2);         //计算两点间距离
    void InitializeMap();                                         //初始化地图数组函数
    void DrawMap();                                               //把地图里的资源绘制到地图中
    void writeMapToFile();
    void HandOutBerry();                                          //分配浆果函数
    void RangeToBuildArrowtower(int centerX, int centerY, int inR, int outR, int minDistance);
        //建箭塔范围

    bool isFindBestGranaryLocation(int bestX,int bestY);          //判断是否为最佳谷仓位置
    bool isFindBestStockLocation1(int x,int y);
    bool isFindBestStockLocation2(int x,int y);
    bool isEmptyToBuildHome(int x,int y,int length,int width);    //判断是否为建房空地
    bool isEmptyToBuildArmycamp(int x,int y,int length,int width);//判断是否为建兵营空地
    bool isEmptyToBuildMarket(int x,int y,int length,int width);  //判断是否为市场空地
    bool isValidTowerLocation(int x,int y);                       //判断箭塔位置是否有效

    std::vector<Bush> bushes;                                     //建立浆果数组
    std::vector<VillagerTask> villagerTasks;
    std::vector<int> unassignedBushes;                            // 未分配的浆果序列号向量
    std::vector<int> aliveGazelles;                               // 存活的羊的序列号
    std::vector<int> aliveGazelles_Blood;                         // 存活的羊剩余血的数组
    std::vector<std::pair<int, int>> towerPositions;              // 存储已放置箭塔的位置


private:
    /***************AI通信函数，请勿修改***************/
    void processData() override;
    tagInfo getInfo(){return tagUsrGame.getInfo();}
    int AddToIns(instruction ins) override
    {
        UsrIns.lock.lock();
        ins.id=UsrIns.g_id;
        UsrIns.g_id++;
        UsrIns.instructions.push(ins);
        UsrIns.lock.unlock();
        return ins.id;
    }
    void clearInsRet() override
    {
        tagUsrGame.clearInsRet();
    }
    /***************AI通信函数，请勿修改***************/

};


#endif // USRAI_H
