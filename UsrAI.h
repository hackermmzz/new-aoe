#ifndef USRAI_H
#define USRAI_H

#include "AI.h"
#include <QPair>
#include <QMutex> // 确保使用Qt的QMutex

extern tagGame tagUsrGame;
extern ins UsrIns;

/*##########请勿修改以上部分##########*/

class UsrAI : public AI {
public:
    UsrAI() { this->id = 0; }
    ~UsrAI() {}

private:
    /***************AI通信函数，请勿修改***************/
    void processData() override;

    tagInfo getInfo() { return tagUsrGame.getInfo(); }

    int AddToIns(instruction ins) override {
        UsrIns.lock.lock(); // 使用Qt的QMutex
        ins.id = UsrIns.g_id;
        UsrIns.g_id++;
        UsrIns.instructions.push(ins);
        UsrIns.lock.unlock();
        return ins.id;
    }

    void clearInsRet() override {
        tagUsrGame.clearInsRet();
    }
    /***************AI通信函数，请勿修改***************/

    // 辅助函数声明
    void initializeMap(const tagInfo& info); // 初始化地图
    void printMap(); // 打印地图

    bool canBuildHomeAtPosition(int startX, int startY); // 检查房屋建造位置

    double calculateDistance(double x1, double y1, double x2, double y2); // 计算距离

    QPair<int, int> findOptimalGranaryPosition(const tagInfo& info); // 找到最佳谷仓位置

    void initializeHomePositions(int centerX, int centerY, const tagInfo& info); // 初始化住房位置

    bool canBuildGranaryAtPosition(int startX, int startY, const tagInfo& info);
    bool canBuildHomePosition(int startX, int startY, const tagInfo& info);
    bool canBuildInRectangle(int startX, int startY, int width, int height, const tagInfo& info);
    int findClosestResource(const tagInfo& info, const tagFarmer& farmer, int resourceType);

    double mid = 36 * BLOCKSIDELENGTH; // 中间位置的坐标
    bool once = true;
    bool granaryBuilt = false;
    int tmpframe = (int)1e6;// 临时帧计数
    int House = 0;
    bool iscamp = false;
    bool iscampfinish = false;

    int id_Wrong_lastFrame = -1;


    static const int Home_MAX = 6; // 最大住房数量
    int HomeX[Home_MAX] = {0};
    int HomeY[Home_MAX] = {0};
    int homeToBuild = 0; // 当前正在建造的房子数量

    static const int MAP_SIZE = 72; // 地图尺寸
    int gameMap[MAP_SIZE][MAP_SIZE] = {{0}};
};

#endif // USRAI_H
