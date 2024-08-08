#include"UsrAI.h"
tagGame tagUsrGame;
ins UsrIns;
/*##########请勿修改以上部分##########*/
#include<fstream>
#include<Map.h>
#include <QVector>
#include <algorithm>
#include <cmath>
#include <QDebug>
#include <QtMath>
tagInfo myInfo;
double mid=36*BLOCKSIDELENGTH;  // 中间位置的坐标
struct point {
    int x;
    int y;
    double distance() const {
        return qSqrt(qPow(x - 32, 2) + qPow(y - 32, 2));
    }
};
// 按离从近到远排序的比较函数


bool isCamp=false;  // 是否有军营
bool isCampFinish=false;  // 军营是否建造完成
vector<point> camp_Flat;
int camp_flag=0;
int tmpFrame=(int)1e6;  // 临时帧计数
int house;  // 统计房子的数量
int house_max=0;
int farm=0;//统计农田数量
int farm_num=0;
int FARM_UPGRADE=0;
vector<point> house_Flat;
int house_flag=0;
bool isGranary=false;//记录谷仓数量
bool isGranaryFinish=false;
bool isArrowtower=false;
int granary_flag=0;
int granary_max =0;
int arrowtower_flag=0;
int market_flag=0;
int farm_flag=0;
int stable_flag=0;
vector<point> arrowtower_Flat;
vector<point> granary_Flat;
vector<point> market_Flat;
vector<point> farm_Flat;
vector<point> stable_Flat;
bool isMarket=false;
int human_num=0;//记录总人数
int id_Wrong_lastFrame = -1;
int UPGRADE_CLUBMAN=0;//是否升级刀斧兵
int init=0;
static int t=0;
int temp_map[72][72];
int flag_map[72][72];
vector<tagResource> bush;
vector<tagResource> tree;
vector<tagResource> gazelle;
void UsrAI::processData()
{
    int nowState_Farmer;
    int SN_res;
    double dis,temp_dis;
    int order_Record;
    static int timers = 1;
    house = 0;
    myInfo=getInfo(); //每帧获取新的tagInfo
    //初始化
    if(init==0){
        for(int i=0;i<72;i++){
            for(int j=0;j<72;j++){
                temp_map[i][j]=myInfo.theMap[i][j];
            }
        }
        for(int i=0;i<myInfo.buildings.size();i++){
            int x=myInfo.buildings[i].BlockDR;
            int y=myInfo.buildings[i].BlockUR;
            for(int k=0;k<3;k++){
                for(int j=0;j<3;j++){
                    flag_map[k+x][y+j]=1;
                }
            }
        }
        //存储浆果信息
        for(tagResource res:getInfo().resources){
            if(res.Type==RESOURCE_BUSH){
                bush.push_back(res);
            }
        }

        //存储树木信息
        for(tagResource res:getInfo().resources){
            if(res.Type==RESOURCE_TREE){
                tree.push_back(res);
            }
        }
        auto distanceComparator = [](const tagResource& a, const tagResource& b) {
            double distA = std::sqrt(std::pow(a.BlockDR - 34, 2) + std::pow(a.BlockUR - 34, 2));
            double distB = std::sqrt(std::pow(b.BlockDR - 34, 2) + std::pow(b.BlockUR - 34, 2));
            return distA < distB;
        };

        // 排序
        std::sort(tree.begin(), tree.end(), distanceComparator);
        //存储鹿信息
        for(tagResource res:getInfo().resources){
            if(res.Type==RESOURCE_GAZELLE){
                gazelle.push_back(res);
            }
        }
        //   for(int i=0;i<myInfo.buildings.size();i++){
        //       int x=myInfo.buildings[i].BlockDR;
        //       int y=myInfo.buildings[i].BlockUR;
        //       for(int k=0;k<3;k++){
        //           for(int j=0;j<3;j++){
        //               DebugText( ("111"+ QString::number(x+k) +","+QString::number(y+j)+":"+ QString::number(flag_map[k+x][y+j])).toStdString());

        //           }
        //       }
        //   }
        for(tagResource res:getInfo().resources){
            int x=res.BlockDR;
            int y=res.BlockUR;
            if(flag_map[x][y]==0){
                flag_map[x][y]=1;
            }
        }
        if (house_flag == 0) {
            int disLen = 3;  // 指定区域的大小为2×2
            int center = MAP_L / 2;  // 搜索范围中心
            int blockSideLen = 20;  // 定义搜索范围的总大小

            // 计算搜索范围的边界
            int right = center + blockSideLen / 2;  // 右边界初始位置
            int top = center + blockSideLen / 2;  // 上边界初始位置
            int left = center - blockSideLen / 2;  // 左边界初始位置
            int low = center - blockSideLen / 2;  // 下边界初始位置
            int maxh=-2, minh=100, temph=0;
            bool isfalse;
            // 固定搜索范围，不改变边界
            low = max(0, low);
            left = max(0, left);
            right = min(right, MAP_L - disLen);
            top = min(top, MAP_U - disLen);
            while (house_Flat.size() < 10) {
                // 在给定范围内找寻指定区域平坦的格子
                for (int x = right; x >=left; x--) {
                    for (int y = top; y > low; y--) {
                        point tempPoint;
                        if (x + disLen > right || y + disLen > top) continue;  // 确保范围不超出边界
                        maxh = minh = temp_map[x][y];
                        if (flag_map[x][y]) continue;
                        isfalse = false;
                        for (int i = 0; i < disLen; i++) {
                            for (int j = 0; j < disLen; j++) {
                                temph = temp_map[x + i][y + j];
                                if (temph > maxh) maxh = temph;
                                else if (temph < minh) minh = temph;
                                if (flag_map[x + i][y + j] || minh == -1 || maxh != minh) {
                                    isfalse = true;
                                    break;
                                }
                            }
                            if (isfalse) break;
                        }
                        if (isfalse) continue;
                        // 标记已找到的平坦区域
                        tempPoint.x = x;
                        tempPoint.y = y;
                        for (int i = 0; i < disLen; i++) {
                            for (int j = 0; j < disLen; j++) {
                                flag_map[x + i][y + j] = 1;
                            }
                        }
                        house_Flat.push_back(tempPoint);
                        if(house_Flat.size()>=10){
                            break;
                        }
                        DebugText( ("命令"+ QString::number(x) +","+ QString::number(y)).toStdString());
                    }
                    if(house_Flat.size()>=10){
                        break;
                    }
                }

            }

            house_flag++;
        }
        if (granary_flag == 0) {
            int dr = 0, ur = 0;
            for (int j = 0; j < myInfo.resources.size(); j++) {
                if (myInfo.resources[j].Type == RESOURCE_BUSH) {
                    dr = myInfo.resources[j].BlockDR;
                    ur = myInfo.resources[j].BlockUR;
                    DebugText(("浆果位置" + QString::number(dr) + "," + QString::number(ur)).toStdString());
                    break;
                }
            }

            int disLen = 4;  // 指定区域的大小为4×4
            int blockSideLen = 20;  // 定义搜索范围的总大小
            // 计算搜索范围的边界
            int right = dr + blockSideLen / 2;  // 右边界初始位置
            int top = ur + blockSideLen / 2;  // 上边界初始位置
            int left = dr - blockSideLen / 2;  // 左边界初始位置
            int low = ur - blockSideLen / 2;  // 下边界初始位置

            // 固定搜索范围，不改变边界
            low = max(0, low);
            left = max(0, left);
            right = min(right, 72 - disLen);
            top = min(top, 72 - disLen);

            int maxh = -2, minh = 100, temph = 0;
            bool isfalse;
            point bestPoint;
            int minDist = INT_MAX;  // 用于记录最小距离

            while (granary_Flat.size() < 1) {
                // 在给定范围内找寻指定区域平坦的格子
                for (int x = left; x < right; x++) {
                    for (int y = low; y < top; y++) {
                        if (x + disLen > right || y + disLen > top) continue;  // 确保范围不超出边界
                        point tempPoint;
                        maxh = minh = temp_map[x][y];
                        if (flag_map[x][y]) continue;
                        isfalse = false;
                        for (int i = 0; i < disLen; i++) {
                            for (int j = 0; j < disLen; j++) {
                                temph = temp_map[x + i][y + j];
                                if (temph > maxh) maxh = temph;
                                else if (temph < minh) minh = temph;
                                if (flag_map[x + i][y + j] || minh == -1 || maxh != minh) {
                                    isfalse = true;
                                    break;
                                }
                            }
                            if (isfalse) break;
                        }
                        if (isfalse) continue;

                        // 计算当前点到 (dr, ur) 的距离
                        int dist = (x - dr) * (x - dr) + (y - ur) * (y - ur);
                        if (dist < minDist) {
                            minDist = dist;
                            bestPoint.x = x;
                            bestPoint.y = y;
                        }
                    }
                }

                // 如果找到合适的位置，标记并添加到结果中
                if (minDist != INT_MAX) {
                    for (int i = 0; i < disLen; i++) {
                        for (int j = 0; j < disLen; j++) {
                            flag_map[bestPoint.x + i][bestPoint.y + j] = 1;
                        }
                    }
                    granary_Flat.push_back(bestPoint);
                }
                break;  // 找到一个合适的位置后退出循环
            }
            granary_flag++;
        }

        if (camp_flag == 0) {
            int disLen = 4;  // 指定区域的大小为3*3
            int center = MAP_L / 2;  // 搜索范围中心
            int blockSideLen = 26;  // 定义搜索范围的总大小

            // 计算搜索范围的边界
            int right = center + blockSideLen / 2;  // 右边界初始位置
            int top = center + blockSideLen / 2;  // 上边界初始位置
            int left = center - blockSideLen / 2;  // 左边界初始位置
            int low = center - blockSideLen / 2;  // 下边界初始位置
            int maxh=-2, minh=100, temph=0;
            bool isfalse;
            // 固定搜索范围，不改变边界
            low = max(0, low);
            left = max(0, left);
            right = min(right, MAP_L - disLen);
            top = min(top, MAP_U - disLen);
            while (camp_Flat.size() < 1) {

                // 在给定范围内找寻指定区域平坦的格子
                for (int x = right; x >= left; x--) {
                    for (int y = low; y < top; y++) {
                        if (x + disLen > right || y + disLen > top) continue;  // 确保范围不超出边界
                        point tempPoint;
                        maxh = minh = temp_map[x][y];
                        if (flag_map[x][y]) continue;
                        isfalse = false;
                        for (int i = 0; i < disLen; i++) {
                            for (int j = 0; j < disLen; j++) {
                                temph = temp_map[x + i][y + j];
                                if (temph > maxh) maxh = temph;
                                else if (temph < minh) minh = temph;
                                if (flag_map[x + i][y + j] || minh == -1 || maxh != minh) {
                                    isfalse = true;
                                    break;
                                }
                            }
                            if (isfalse) break;
                        }
                        if (isfalse) continue;
                        // 标记已找到的平坦区域
                        tempPoint.x = x;
                        tempPoint.y = y;
                        for (int i = 0; i < disLen; i++) {
                            for (int j = 0; j < disLen; j++) {
                                flag_map[x + i][y + j] = 1;
                            }
                        }
                        camp_Flat.push_back(tempPoint);
                        if(granary_Flat.size()>=1){
                            break;
                        }
                    }
                    if(camp_Flat.size()>=1){
                        break;
                    }
                }
            }
            camp_flag++;
        }
        if (arrowtower_flag == 0) {
            int disLen = 4;  // 指定区域的大小为3*3
            int center = MAP_L / 2;  // 搜索范围中心
            int blockSideLen = 10;  // 定义搜索范围的总大小

            // 计算搜索范围的边界
            int right = center + blockSideLen / 2;  // 右边界初始位置
            int top = center + blockSideLen / 2;  // 上边界初始位置
            int left = center - blockSideLen / 2;  // 左边界初始位置
            int low = center - blockSideLen / 2;  // 下边界初始位置
            int maxh=-2, minh=100, temph=0;
            bool isfalse;
            // 固定搜索范围，不改变边界
            low = max(0, low);
            left = max(0, left);
            right = min(right, MAP_L - disLen);
            top = min(top, MAP_U - disLen);
            while (arrowtower_Flat.size() < 1) {

                // 在给定范围内找寻指定区域平坦的格子
                for (int x = left; x < right; x++) {
                    for (int y = low; y < top; y++) {
                        if (x + disLen > right || y + disLen > top) continue;  // 确保范围不超出边界
                        point tempPoint;
                        maxh = minh = temp_map[x][y];
                        if (flag_map[x][y]) continue;
                        isfalse = false;
                        for (int i = 0; i < disLen; i++) {
                            for (int j = 0; j < disLen; j++) {
                                temph = temp_map[x + i][y + j];
                                if (temph > maxh) maxh = temph;
                                else if (temph < minh) minh = temph;
                                if (flag_map[x + i][y + j] || minh == -1 || maxh != minh) {
                                    isfalse = true;
                                    break;
                                }
                            }
                            if (isfalse) break;
                        }
                        if (isfalse) continue;
                        // 标记已找到的平坦区域
                        tempPoint.x = x;
                        tempPoint.y = y;
                        for (int i = 0; i < disLen; i++) {
                            for (int j = 0; j < disLen; j++) {
                                flag_map[x + i][y + j] = 1;
                            }
                        }
                        arrowtower_Flat.push_back(tempPoint);
                        if(granary_Flat.size()>=1){
                            break;
                        }
                    }
                    if(arrowtower_Flat.size()>=1){
                        break;
                    }
                }
            }
            arrowtower_flag++;
        }
        if (market_flag == 0) {
            int disLen = 4;  // 指定区域的大小为2×2
            int center = MAP_L / 2;  // 搜索范围中心
            int blockSideLen = 34;  // 定义搜索范围的总大小

            // 计算搜索范围的边界
            int right = center + blockSideLen / 2;  // 右边界初始位置
            int top = center + blockSideLen / 2;  // 上边界初始位置
            int left = center - blockSideLen / 2;  // 左边界初始位置
            int low = center - blockSideLen / 2;  // 下边界初始位置
            int maxh=-2, minh=100, temph=0;
            bool isfalse;
            // 固定搜索范围，不改变边界
            low = max(0, low);
            left = max(0, left);
            right = min(right, MAP_L - disLen);
            top = min(top, MAP_U - disLen);
            while (market_Flat.size() < 1) {
                // 在给定范围内找寻指定区域平坦的格子
                for (int x = right; x >=left; x--) {
                    for (int y = top; y>=low; y--) {
                        point tempPoint;
                        if (x + disLen > right || y + disLen > top) continue;  // 确保范围不超出边界
                        maxh = minh = temp_map[x][y];
                        if (flag_map[x][y]) continue;
                        isfalse = false;
                        for (int i = 0; i < disLen; i++) {
                            for (int j = 0; j < disLen; j++) {
                                temph = temp_map[x + i][y + j];
                                if (temph > maxh) maxh = temph;
                                else if (temph < minh) minh = temph;
                                if (flag_map[x + i][y + j] || minh == -1 || maxh != minh) {
                                    isfalse = true;
                                    break;
                                }
                            }
                            if (isfalse) break;
                        }
                        if (isfalse) continue;
                        // 标记已找到的平坦区域
                        tempPoint.x = x;
                        tempPoint.y = y;
                        for (int i = 0; i < disLen; i++) {
                            for (int j = 0; j < disLen; j++) {
                                flag_map[x + i][y + j] = 1;
                            }
                        }
                        market_Flat.push_back(tempPoint);
                        if(market_Flat.size()>=1){
                            break;
                        }
                    }
                    if(market_Flat.size()>=1){
                        break;
                    }
                }

            }

            market_flag++;
        }
        if (stable_flag == 0) {
            int disLen = 4;  // 指定区域的大小为2×2
            int center = MAP_L / 2;  // 搜索范围中心
            int blockSideLen = 30;  // 定义搜索范围的总大小

            // 计算搜索范围的边界
            int right = center + blockSideLen / 2;  // 右边界初始位置
            int top = center + blockSideLen / 2;  // 上边界初始位置
            int left = center - blockSideLen / 2;  // 左边界初始位置
            int low = center - blockSideLen / 2;  // 下边界初始位置
            int maxh=-2, minh=100, temph=0;
            bool isfalse;
            // 固定搜索范围，不改变边界
            low = max(0, low);
            left = max(0, left);
            right = min(right, MAP_L - disLen);
            top = min(top, MAP_U - disLen);
            while (stable_Flat.size() < 1) {
                // 在给定范围内找寻指定区域平坦的格子
                for (int x = right; x >=left; x--) {
                    for (int y = top; y>=low; y--) {
                        point tempPoint;
                        if (x + disLen > right || y + disLen > top) continue;  // 确保范围不超出边界
                        maxh = minh = temp_map[x][y];
                        if (flag_map[x][y]) continue;
                        isfalse = false;
                        for (int i = 0; i < disLen; i++) {
                            for (int j = 0; j < disLen; j++) {
                                temph = temp_map[x + i][y + j];
                                if (temph > maxh) maxh = temph;
                                else if (temph < minh) minh = temph;
                                if (flag_map[x + i][y + j] || minh == -1 || maxh != minh) {
                                    isfalse = true;
                                    break;
                                }
                            }
                            if (isfalse) break;
                        }
                        if (isfalse) continue;
                        // 标记已找到的平坦区域
                        tempPoint.x = x;
                        tempPoint.y = y;
                        for (int i = 0; i < disLen; i++) {
                            for (int j = 0; j < disLen; j++) {
                                flag_map[x + i][y + j] = 1;
                            }
                        }
                        stable_Flat.push_back(tempPoint);
                        if(stable_Flat.size()>=1){
                            break;
                        }
                    }
                    if(stable_Flat.size()>=1){
                        break;
                    }
                }

            }

            stable_flag++;
        }
        auto compare=[](const point& a, const point& b) {
            return a.distance() < b.distance();
        };
        if (farm_flag == 0) {
            int disLen = 4;
            int center = MAP_L / 2+2;
            int blockSideLen = 18;


            int right = center + blockSideLen / 2;
            int top = center + blockSideLen / 2;
            int left = center - blockSideLen / 2;
            int low = center - blockSideLen / 2;
            int maxh = -2, minh = 100, temph = 0;
            bool isfalse;
            low = std::max(0, low);
            left = std::max(0, left);
            right = std::min(right, MAP_L - disLen);
            top = std::min(top, MAP_U - disLen);
            std::vector<point> closestPoints;

            for (int x = left; x < right; x++) {
                for (int y = low; y < top; y++) {
                    point tempPoint;
                    if (x + disLen > right || y + disLen > top) continue;
                    maxh = minh = temp_map[x][y];
                    if (flag_map[x][y]) continue;
                    isfalse = false;
                    for (int i = 0; i < disLen; i++) {
                        for (int j = 0; j < disLen; j++) {
                            temph = temp_map[x + i][y + j];
                            if (temph > maxh) maxh = temph;
                            else if (temph < minh) minh = temph;
                            if (flag_map[x + i][y + j] || minh == -1 || maxh != minh) {
                                isfalse = true;
                                break;
                            }
                        }
                        if (isfalse) break;
                    }
                    if (isfalse) continue;
                    tempPoint.x = x;
                    tempPoint.y = y;
                    for (int i = 0; i < disLen; i++) {
                        for (int j = 0; j < disLen; j++) {
                            flag_map[x + i][y + j] = 1;
                        }
                    }
                    closestPoints.push_back(tempPoint);
                    std::sort(closestPoints.begin(), closestPoints.end(), compare);
                    if (closestPoints.size() > 3) {
                        closestPoints.pop_back();
                    }
                }
            }
            for (const point& p : closestPoints) {
                farm_Flat.push_back(p);
            }

            farm_flag++;
        }
        init++;
    }
    if(id_Wrong_lastFrame>=0&&timers>0) //记录了上一帧下达命令的编号
    {

        order_Record = myInfo.ins_ret[id_Wrong_lastFrame];  //获取上一帧下达命令的结果
        DebugText( ("命令"+ QString::number(id_Wrong_lastFrame) +"的返回为"+ QString::number(order_Record)).toStdString());
        id_Wrong_lastFrame = -1;
        timers--;  //只获取并打印一次
    }
    //for(int i=0;i<farm_Flat.size();i++){
    //      DebugText( ("农场坐标"+ QString::number(farm_Flat[i].x) +","+ QString::number(farm_Flat[i].y)).toStdString());
    //}
    human_num=myInfo.armies.size()+myInfo.farmers.size();
    // 首先遍历城市中心，标记各类建筑
    for(tagBuilding building : getInfo().buildings)
    {
        if(building.Type==BUILDING_HOME&&building.Percent==100)//如果建筑类型是房子且建造完成，增加房子数量计数
        {
            house++;
            house_max=max(house,house_max);//记录最大值
        }
        else if( building.Type==BUILDING_ARMYCAMP &&building.Percent==100)// 如果建筑类型是军营
        {

            isCamp=true;    //标记有军营
            if(building.Percent==100&&!isCampFinish)// 如果军营建造完成且尚未标记完成
            {
                isCampFinish=true;  // 标记军营建造完成
                tmpFrame=tagUsrGame.getInfo().GameFrame;  // 记录下当前帧数
                DebugText("我有兵营啦"); //打印信息到debugText窗口
            }
            if(myInfo.armies.size()>0){
                BuildingAction(building.SN, BUILDING_ARMYCAMP_UPGRADE_CLUBMAN);
            }
            if(myInfo.GameFrame>tmpFrame+10) // 军营建好后过一会，执行创建棍棒兵的指令
            {
                BuildingAction(building.SN, BUILDING_ARMYCAMP_CREATE_CLUBMAN);
            }

        }
        else if( building.Type==BUILDING_CENTER && building.Project==ACT_NULL && getInfo().farmers.size()<=house_max*2+4)
        {
            if( getInfo().farmers.size()<=20){
                if(!(myInfo.enemy_armies.size()>0&&myInfo.armies.size()<2)){
                    BuildingAction(building.SN,BUILDING_CENTER_CREATEFARMER);
                }

            }
        }
        else if( building.Type==BUILDING_GRANARY&&building.Percent==100){// 如果建筑类型是谷仓
            isGranary=true;
        }
        else if(building.Type==BUILDING_MARKET&&building.Percent==100){
            isMarket=true;
        }
        else if(building.Type==BUILDING_ARROWTOWER&&building.Percent==100){
            isArrowtower=true;
        }
        else if(building.Type==BUILDING_FARM&&building.Percent==100){
            farm++;
            farm_num=max(farm,farm_num);
        }
    }
    // 遍历所有农民（此处即传统遍历方式）
    for(int i = 0 ; i<myInfo.farmers.size() ; i++)
    {
        nowState_Farmer = myInfo.farmers[i].NowState;
        if( i==0&& ( nowState_Farmer==HUMAN_STATE_IDLE || nowState_Farmer ==HUMAN_STATE_STOP||nowState_Farmer==HUMAN_STATE_GATHERING)&&house_max*4+2<=human_num&&house_max<=5&&myInfo.Wood>=30){
            // 如果是第一个农民且处于空闲或停止状态，建造房子
            //DebugText( ("村民1的状态"+ QString::number(nowState_Farmer)).toStdString());
            int x1=house_Flat[house_max].x;
            int y1=house_Flat[house_max].y;
            HumanBuild(myInfo.farmers[i].SN, BUILDING_HOME ,x1, y1);
            continue;
        }
        else if( (i==1 && (nowState_Farmer==HUMAN_STATE_IDLE)&&!isGranary&&myInfo.Wood>=120 ) ){
            // DebugText( ("村民2的状态"+ QString::number(nowState_Farmer)).toStdString());
            int x1=granary_Flat[0].x;
            int y1=granary_Flat[0].y;
            HumanBuild(myInfo.farmers[i].SN, BUILDING_GRANARY ,x1, y1);
            continue;
        }
        else if(i==2  && (nowState_Farmer==HUMAN_STATE_IDLE  )&&!isArrowtower && isGranary&&myInfo.Stone>=150&& i<myInfo.farmers.size()){
            int x1=arrowtower_Flat[0].x;
            int y1=arrowtower_Flat[0].y;
            DebugText( ("isArrowtower"+ QString::number(isArrowtower)).toStdString());
            HumanBuild(myInfo.farmers[i].SN, BUILDING_ARROWTOWER ,x1, y1);
            continue;
        }
        else if(i==3&&myInfo.farmers[3].Blood<=0&&!isArrowtower&& i<myInfo.farmers.size()){
            int x1=market_Flat[0].x;
            int y1=market_Flat[0].y;
            HumanBuild(myInfo.farmers[i].SN, BUILDING_ARROWTOWER ,x1, y1);
            continue;
        }
        else if(i==4&&(nowState_Farmer==HUMAN_STATE_IDLE  )&&isGranary&&myInfo.Wood>=150&&!isMarket&& i<myInfo.farmers.size()){
            int x1=market_Flat[0].x;
            int y1=market_Flat[0].y;
            HumanBuild(myInfo.farmers[i].SN, BUILDING_MARKET ,x1, y1);
            continue;
        }else if(i==5&&myInfo.farmers[4].Blood<=0&&!isMarket&& i<myInfo.farmers.size()){
            int x1=market_Flat[0].x;
            int y1=market_Flat[0].y;
            HumanBuild(myInfo.farmers[i].SN, BUILDING_MARKET ,x1, y1);
            continue;
        }
        else if(i<6 && (nowState_Farmer == HUMAN_STATE_IDLE )&&i<myInfo.farmers.size())
        {
            //DebugText( (QString::number(i)+"浆果剩余"+ QString::number(bush[i].Cnt)).toStdString());
            if(gazelle[i].Cnt>0&&bush[i].Cnt<=0){
                HumanAction(myInfo.farmers[i].SN,gazelle[i].SN);
                for(tagResource res:getInfo().resources){
                    if(res.Type==RESOURCE_GAZELLE){
                        for(int k=0;k<gazelle.size();k++){
                            if(gazelle[k].SN==res.SN){
                                gazelle[k].Cnt=res.Cnt;
                                break;
                            }
                        }
                    }
                }
                continue;
            }
            // 如果是前6个农民且处于空闲状态，采集浆果资源并且没有农场
            else if(bush[i].Cnt>0){
                HumanAction(myInfo.farmers[i].SN,bush[i].SN);
                for(tagResource res:getInfo().resources){
                    if(res.Type==RESOURCE_BUSH){
                        for(int k=0;k<bush.size();k++){
                            if(bush[k].SN==res.SN){
                                bush[k].Cnt=res.Cnt;
                                break;
                            }
                        }
                    }
                }
                continue;
            }
            continue;
        }
        else if( i==6&&!isCamp&&myInfo.Wood>=125 )
        {   // 如果是第7个农民且处于空闲或停止状态，建造军营
            //DebugText( ("村民3的状态"+ QString::number(nowState_Farmer)).toStdString());
            int x1=camp_Flat[0].x;
            int y1=camp_Flat[0].y;
            HumanBuild(myInfo.farmers[i].SN, BUILDING_ARMYCAMP ,x1, y1);
            continue;
        }
        else if(i==6&&isMarket&&myInfo.Wood>=75&& i<myInfo.farmers.size()){
            int x1=farm_Flat[0].x;
            int y1=farm_Flat[0].y;
            HumanBuild(myInfo.farmers[i].SN, BUILDING_FARM ,x1, y1);
            continue;
        }
        else if(i==7&&isMarket&&myInfo.Wood>=75&& i<myInfo.farmers.size()){
            int x1=farm_Flat[1].x;
            int y1=farm_Flat[1].y;
            HumanBuild(myInfo.farmers[i].SN, BUILDING_FARM ,x1, y1);
            continue;
        }
        else if(i==8&&isMarket&&myInfo.Wood>=75&& i<myInfo.farmers.size()){
            int x1=farm_Flat[2].x;
            int y1=farm_Flat[2].y;
            HumanBuild(myInfo.farmers[i].SN, BUILDING_FARM ,x1, y1);
            continue;
        }
        else if( i<11&&i>=6&&(nowState_Farmer==HUMAN_STATE_IDLE)&&i<myInfo.farmers.size()&&t<15&& i<myInfo.farmers.size())
        {
            int ret=HumanAction(myInfo.farmers[i].SN,tree[t].SN);
            if(ret>=0){
                DebugText("树木ret:"+ QString::number(ret).toStdString());
                t++;
            }
            continue;
        }
        else if(i==11&&isMarket&&myInfo.Wood>=150&& i<myInfo.farmers.size()){
            int x1=stable_Flat[0].x;
            int y1=stable_Flat[0].y;
            HumanBuild(myInfo.farmers[i].SN, BUILDING_STABLE ,x1, y1);
            continue;
        }
        if( i==12&&house_max*4+2<=human_num&&house_max<=5&&myInfo.Wood>=30){
            // 如果是第12个农民且处于空闲或停止状态，建造房子
            //DebugText( ("村民1的状态"+ QString::number(nowState_Farmer)).toStdString());
            int x1=house_Flat[house_max].x;
            int y1=house_Flat[house_max].y;
            HumanBuild(myInfo.farmers[i].SN, BUILDING_HOME ,x1, y1);
            continue;
        }
        else if(i<15 && (nowState_Farmer==HUMAN_STATE_IDLE)&&i<myInfo.farmers.size())
        {

            for(int j = 0 ; j<myInfo.resources.size() ; j++)
            {
                temp_dis = calDistance(mid,mid,myInfo.resources[j].DR, myInfo.resources[j].UR);
                if(myInfo.resources[j].Type==RESOURCE_STONE &&temp_dis<dis )
                {
                    SN_res = myInfo.resources[j].SN;
                    dis = temp_dis;
                }
            }
            HumanAction(myInfo.farmers[i].SN,SN_res);
            continue;
        }
        else if(nowState_Farmer== HUMAN_STATE_IDLE&&i<myInfo.farmers.size())
        {
            SN_res = -1;
            dis = 1e6;
            for(int j = 0 ; j<myInfo.resources.size() ; j++)
            {
                temp_dis = calDistance(mid, mid, myInfo.resources[j].DR, myInfo.resources[j].UR);
                if(myInfo.resources[j].Type==RESOURCE_ELEPHANT&&temp_dis<dis)
                {
                    SN_res = myInfo.resources[j].SN;
                    dis = temp_dis;
                }
            }
            HumanAction(myInfo.farmers[i].SN,SN_res);
        }
    }
    // 遍历所有军队
    for(int i = 0 ; i<myInfo.armies.size(); i++)
    {
        dis = 1e6;
        {
            if(myInfo.enemy_armies.size()>0&&(myInfo.armies[i].NowState==HUMAN_STATE_IDLE)){
                int n=myInfo.enemy_armies.size();
                for(int j = 0; j<myInfo.enemy_armies.size(); j++){
                    temp_dis = calDistance(mid, mid, myInfo.enemy_armies[j].DR, myInfo.enemy_armies[j].UR);
                    if(myInfo.enemy_armies[j].Blood>0&&temp_dis<dis)
                    {
                        SN_res = myInfo.enemy_armies[j].SN;
                        dis = temp_dis;
                    }
                }
                HumanAction(myInfo.armies[i].SN,SN_res);
            }
        }
    }
    for(tagBuilding building : getInfo().buildings){
        if(building.Type==BUILDING_STOCK&&building.Percent==100){
            BuildingAction(building.SN,  BUILDING_STOCK_UPGRADE_USETOOL);
            if(myInfo.Meat>=75){
                BuildingAction(building.SN,  BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY);
            }
            continue;
        }
        if(building.Type==BUILDING_GRANARY&&building.Percent==100){
            BuildingAction(building.SN, BUILDING_GRANARY_ARROWTOWER);
            continue;
        }
        if(building.Type==BUILDING_ARROWTOWER&&building.Percent==100){
            int dis=8;
            for(int j = 0; j<myInfo.enemy_armies.size(); j++){
                temp_dis = calDistance(building.BlockDR, building.BlockUR, myInfo.enemy_armies[j].DR, myInfo.enemy_armies[j].UR);
                if(myInfo.enemy_armies[j].Blood>0&&temp_dis<dis)
                {
                    SN_res = myInfo.enemy_armies[j].SN;
                }
            }
            HumanAction(building.SN,SN_res);
            continue;
        }
        if(building.Type==BUILDING_STABLE&&building.Percent==100&&myInfo.Meat>=50){
            BuildingAction(building.SN, BUILDING_STABLE_CREATE_SCOUT);
        }
    }
}

