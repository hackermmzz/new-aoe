#include "Map.h"
#include <tuple>
#include <unordered_map>
#include <iostream>
#include <bits/stdc++.h>
#include <chrono>
#include<vector>
#include<array>
#include <random>
#include<map>
#include<algorithm>
#include<Rectarea.h>
#include<LineArea.h>
#include<CircleArea.h>

// 前向声明
class RectArea;
class CircleArea;
class LineArea;

// 外部声明全局区域对象（在MainWidget.cpp中定义）
extern RectArea* g_rectArea;
extern CircleArea* g_circleArea;
extern LineArea* g_lineArea;
Map*GlobalMap;
///////////////////////////
Map::Map()
{
    GlobalMap=this;
    for(int i = 0; i < MAP_L; i++)
    {
        cell[i] = new Block[MAP_U];
    }
    //
    findPathMapTemperature=vector<vector<vector<int>>>(MAXPLAYER,vector<vector<int>>(MAP_L,vector<int>(MAP_U)));
    blockIndex=decltype(blockIndex)(MAP_L,vector<int>(MAP_U));
    map_Vision=vector<vector<vector<Coordinate*>>>(MAP_L,vector<vector<Coordinate*>>(MAP_U));
    map_Object=vector<vector<vector<Coordinate*>>>(MAP_L,vector<vector<Coordinate*>>(MAP_U));
    map_Height=decltype(map_Height)(MAP_L,vector<int>(MAP_U));
    explored=decltype(explored)(MAP_L,vector<bool>(MAP_U));
    m_heightMap=decltype(m_heightMap)(GENERATE_L,vector<short>(GENERATE_U));
    TreeBlock=decltype(TreeBlock)(MAP_L,vector<bool>(MAP_U));
    barrierMap=decltype(barrierMap)(MAP_L,vector<int>(MAP_U));
}

Map::~Map()
{
    // 释放分配的内存
    for (int i = 0; i < MAP_L; i++)
    {
        delete[] cell[i];
    }
    delete[] cell;
}




// 海-陆边界 编码到地块类型的映射
// 海洋相对陆地的方向
const std::map<int, int> OceanCodeToNum = {
// 位模式: [左下角|右下角|右上角|左上角|下|右|上|左]
    // 基本方向
    {0b00000001, 37},     // 左
    {0b00000010, 29},     // 上
    {0b00000100, 38},     // 右
    {0b00001000, 39},     // 下
    // 组合方向
    {0b00000011, 30},     // 左上
    {0b00000110, 32},     // 右上
    {0b00001001, 40},     // 左下
    {0b00001100, 31},     // 右下
    // 单个角落
    {0b00010000, 34},     // 左上角
    {0b00100000, 36},     // 右上角
    {0b01000000, 35},     // 右下角
    {0b10000000, 33}      // 左下角
};
// 沙地-草地边界 编码到地块类型的映射
// 沙地相对草地的方向
const std::map<int, int> SandCodeToNum = {
// 位模式: [左下角|右下角|右上角|左上角|下|右|上|左]
    // 基本方向
    {0b00000001, 8},     // 左
    {0b00000010, 15},     // 上
    {0b00000100, 2},     // 右
    {0b00001000, 6},     // 下
    // 组合方向
    {0b00000011, 10},     // 左上
    {0b00000110, 5},     // 右上
    {0b00001001, 4},     // 左下
    {0b00001100, 13},     // 右下 TODO: 非完全匹配
    // 单个角落
    {0b00010000, 14},     // 左上角
    {0b00100000, 12},     // 右上角
    {0b01000000, 13},     // 右下角
    {0b10000000, 4}      // 左下角 TODO: 非完全匹配
};

/**
 * 函数：Map::drawEdge；
 * 参数：tempMap[MAP_L][MAP_U]——地形标记地图；
 *      codeToNum——编码到地块类型的映射；方向为类型2相对类型1的方位；
 *      MapType1——类型1；
 *      MapType2——类型2；
 *      MapType3——在tempMap中标记边界地形为MapType3；
 * 内容：绘制地形交界；
 * 返回值：空。
 */
void Map::drawEdge(vector<vector<int>>&tempMap ,std::map<int, int> codeToNum,int MapType1,int MapType2,int MapType3) {
    for (int i = 0; i < MAP_L; ++i) {
        for (int j = 0; j < MAP_U; ++j) {
            if (tempMap[i][j] == MapType1) { // 仅处理陆地单元格
                // 检查八个方向
                int left = (i > 0) ? tempMap[i-1][j]==MapType2 : 0;
                int up = (j+1 < MAP_U) ? tempMap[i][j+1]==MapType2 : 0;
                int right = (i+1 < MAP_L) ? tempMap[i+1][j]==MapType2 : 0;
                int down = (j > 0) ? tempMap[i][j-1]==MapType2 : 0;
                int upLeft = (i > 0 && j+1 < MAP_U) ? tempMap[i-1][j+1]==MapType2 : 0;
                int upRight = (i + 1 < MAP_L && j + 1 < MAP_U) ? tempMap[i + 1][j + 1]==MapType2 : 0;
                int downRight = (i + 1 < MAP_L && j - 1 >= 0) ? tempMap[i + 1][j - 1]==MapType2 : 0;
                int downLeft = (i - 1 >= 0 && j - 1 >= 0) ? tempMap[i - 1][j - 1]==MapType2 : 0;
                
                // 先只计算主方向（上下左右）的编码
                int mainDirCode = (left << 0) | (up << 1) | (right << 2) | (down << 3);
                
                // 先查找主方向的地块类型
                auto it = codeToNum.find(mainDirCode);
                if (it != codeToNum.end()) {
                    cell[i][j].Num = it->second;
                    tempMap[i][j] = MapType3; // 将更新过的tempMap设为MapType3
                } else {
                    // 如果主方向没有匹配，再考虑角落方向
                    int cornerDirCode = 0;
                    
                    // 检查角落方向
                    if (upLeft && !up && !left) cornerDirCode |= 0b00010000;  // 左上角
                    if (upRight && !up && !right) cornerDirCode |= 0b00100000;  // 右上角
                    if (downRight && !down && !right) cornerDirCode |= 0b01000000;  // 右下角
                    if (downLeft && !down && !left) cornerDirCode |= 0b10000000;  // 左下角
                    
                    // 查找角落方向的地块类型
                    it = codeToNum.find(cornerDirCode);
                    if (it != codeToNum.end()) {
                        cell[i][j].Num = it->second;
                        tempMap[i][j] = MapType3; // 将更新过的tempMap设为MapType3
                    }
                }
            } //if(tempMap[i][j] == 0)
        } //for(int j = 0; j < MAP_U; ++j)
    } //for(int i = 0; i < MAP_L; ++i)
}

void Map::MergeTrees()
{
    using PD=std::array<int,2>;
    static const int Dis=3;
    static const int therold=7;
    static std::vector<PD>offset;
    static vector<PD>blocked0,blocked1;//用于存储新增的被阻塞的位置
    //算出所有曼哈顿距离为Dis的偏移
    if(offset.empty()){
        for(int i=-Dis;i<=Dis;++i){
            for(int j=-Dis;j<=Dis;++j){
                if(abs(i)+abs(j)<=Dis){
                    offset.push_back(PD{i,j});
                }
            }
        }
    }
    //清空
    blocked0.clear();
    blocked1.clear();
    for(auto&v:TreeBlock)fill(v.begin(),v.end(),0);
    //获取所有的树
    set<PD>trees;
    for(auto&tree_:animal){
        Animal&tree=*tree_;
        if(tree.getNum()==ANIMAL_TREE&&!tree.isDie()){
            trees.insert({tree.getBlockDR(),tree.getBlockUR()});
        }
    }
    //查看邻接四个角有几个在trees里面
    auto GetAroundTreeCount=[&](int x,int y)->int{
        int cnt=0;
        for(auto&o:offset){
            int xx=x+o[0],yy=y+o[1];
            if(trees.count({xx,yy}))++cnt;
        }
        return cnt;
    };
    //对单个树进行检测
    for(auto&ele:trees){
        int x=ele[0],y=ele[1];
        //获取
        int cnt=GetAroundTreeCount(x,y);
        //默认被四棵树围住的树不能被砍
        if(cnt>=therold)
        {
            blocked0.push_back(PD{x,y});
            TreeBlock[x][y]=1;
        }
    }
    //对形成的block进一步封闭
    const static int off[][2]={{0,1},{0,-1},{-1,0},{1,0},{-1,-1},{-1,1},{1,1},{1,-1}};
    while(blocked0.size()){
        map<PD,int>counts;
        for(auto&pos:blocked0){
            int i=pos[0],j=pos[1];
            //如果周围有四个那么该点也为不可达
            for(auto*o:off){
                int ii=i+o[0],jj=j+o[1];
                if(ii>=0&&ii<MAP_L&&jj>=0&&jj<MAP_U&&!TreeBlock[ii][jj]){
                    if(++counts[PD{ii,jj}]==6){
                        blocked1.push_back({ii,jj});
                    }
                }
            }
        }
        //对新增的不可达区域进行mask
        for(auto&ele:blocked1){
            TreeBlock[ele[0]][ele[1]]=1;
        }
        //清空
        blocked0.clear();
        //
        swap(blocked0,blocked1);
    }

}

void Map::refineShore() {
    // 编辑器模式下也启用海岸线绘制
    extern bool EditorMode;
    // if (EditorMode) {
    //     return;
    // }
    
    // 创建临时地图：海洋标记为1，陆地标记为0
    vector<vector<int>> tempMap(MAP_L,vector<int>(MAP_U));    //tempMap[左右][上下]
    for (int i = 0; i < MAP_L; ++i) {
        for (int j = 0; j < MAP_U; ++j) {
            tempMap[i][j] = (cell[i][j].getMapType() == MAPTYPE_OCEAN) ? 1 : 0;
        }
    }
    drawEdge(tempMap, OceanCodeToNum, 0, 1, 2);  // 绘制海洋边缘，陆地是0，海洋是1，海滩被标记为2
    drawEdge(tempMap, SandCodeToNum, 0, 2, 3);  // 绘制沙地边缘，陆地是0，沙地是2，边界被标记为2
}

/*
 * 函数：Map::updateShoreArea
 * 参数：centerL, centerU - 修改中心位置的坐标
 *      radius - 影响半径（默认为2，确保包含所有可能受影响的区域）
 * 内容：实时更新指定区域周围的海滩绘制
 * 返回值：无
 */
void Map::updateShoreArea(int centerL, int centerU, int radius) {
    // 计算影响范围
    int minL = std::max(0, centerL - radius);
    int maxL = std::min(MAP_L - 1, centerL + radius);
    int minU = std::max(0, centerU - radius);
    int maxU = std::min(MAP_U - 1, centerU + radius);
    
    // 先将所有受影响区域重置为基础状态
    for (int i = minL; i <= maxL; i++) {
        for (int j = minU; j <= maxU; j++) {
            // 如果不是海洋，先重置为草地
            if (cell[i][j].getMapType() != MAPTYPE_OCEAN) {
                resetBlockToGrass(i, j);
            }
        }
    }
    
    // 创建局部临时地图，只处理影响区域
    int width = maxL - minL + 1;
    int height = maxU - minU + 1;
    std::vector<std::vector<int>> tempMap(width, std::vector<int>(height, 0));
    
    // 填充临时地图：海洋标记为1，陆地标记为0
    for (int i = minL; i <= maxL; i++) {
        for (int j = minU; j <= maxU; j++) {
            int localI = i - minL;
            int localJ = j - minU;
            tempMap[localI][localJ] = (cell[i][j].getMapType() == MAPTYPE_OCEAN) ? 1 : 0;
        }
    }
    
    // 对局部区域应用海滩绘制逻辑
    for (int localI = 0; localI < width; localI++) {
        for (int localJ = 0; localJ < height; localJ++) {
            if (tempMap[localI][localJ] == 0) { // 仅处理陆地单元格
                int globalI = localI + minL;
                int globalJ = localJ + minU;
                
                // 检查八个方向是否有海洋
                int left = (localI > 0) ? tempMap[localI-1][localJ] : 
                          (globalI > 0 && cell[globalI-1][globalJ].getMapType() == MAPTYPE_OCEAN ? 1 : 0);
                int up = (localJ+1 < height) ? tempMap[localI][localJ+1] : 
                        (globalJ+1 < MAP_U && cell[globalI][globalJ+1].getMapType() == MAPTYPE_OCEAN ? 1 : 0);
                int right = (localI+1 < width) ? tempMap[localI+1][localJ] : 
                           (globalI+1 < MAP_L && cell[globalI+1][globalJ].getMapType() == MAPTYPE_OCEAN ? 1 : 0);
                int down = (localJ > 0) ? tempMap[localI][localJ-1] : 
                          (globalJ > 0 && cell[globalI][globalJ-1].getMapType() == MAPTYPE_OCEAN ? 1 : 0);
                
                // 检查角落方向
                int upLeft = (localI > 0 && localJ+1 < height) ? tempMap[localI-1][localJ+1] : 
                            (globalI > 0 && globalJ+1 < MAP_U && cell[globalI-1][globalJ+1].getMapType() == MAPTYPE_OCEAN ? 1 : 0);
                int upRight = (localI+1 < width && localJ+1 < height) ? tempMap[localI+1][localJ+1] : 
                             (globalI+1 < MAP_L && globalJ+1 < MAP_U && cell[globalI+1][globalJ+1].getMapType() == MAPTYPE_OCEAN ? 1 : 0);
                int downRight = (localI+1 < width && localJ > 0) ? tempMap[localI+1][localJ-1] : 
                               (globalI+1 < MAP_L && globalJ > 0 && cell[globalI+1][globalJ-1].getMapType() == MAPTYPE_OCEAN ? 1 : 0);
                int downLeft = (localI > 0 && localJ > 0) ? tempMap[localI-1][localJ-1] : 
                              (globalI > 0 && globalJ > 0 && cell[globalI-1][globalJ-1].getMapType() == MAPTYPE_OCEAN ? 1 : 0);
                
                // 计算主方向编码
                int mainDirCode = (left << 0) | (up << 1) | (right << 2) | (down << 3);
                
                // 查找主方向的海滩类型
                auto it = OceanCodeToNum.find(mainDirCode);
                if (it != OceanCodeToNum.end()) {
                    cell[globalI][globalJ].Num = it->second;
                } else {
                    // 检查角落方向
                    int cornerDirCode = 0;
                    if (upLeft && !up && !left) cornerDirCode |= 0b00010000;  // 左上角
                    if (upRight && !up && !right) cornerDirCode |= 0b00100000;  // 右上角
                    if (downRight && !down && !right) cornerDirCode |= 0b01000000;  // 右下角
                    if (downLeft && !down && !left) cornerDirCode |= 0b10000000;  // 左下角
                    
                    it = OceanCodeToNum.find(cornerDirCode);
                    if (it != OceanCodeToNum.end()) {
                        cell[globalI][globalJ].Num = it->second;
                    }
                }
            }
        }
    }
    
    // 第二阶段：绘制沙地边界（海滩到草地的过渡）
    // 重新更新临时地图，现在海滩标记为2
    for (int i = minL; i <= maxL; i++) {
        for (int j = minU; j <= maxU; j++) {
            int localI = i - minL;
            int localJ = j - minU;
            // 如果是海洋保持为1，如果是海滩设为2，其他陆地设为0
            if (cell[i][j].getMapType() == MAPTYPE_OCEAN) {
                tempMap[localI][localJ] = 1;
            } else {
                // 检查是否是海滩纹理（海滩纹理的Num值在29-40范围内）
                bool isBeach = (cell[i][j].Num >= 29 && cell[i][j].Num <= 40);
                tempMap[localI][localJ] = isBeach ? 2 : 0;
            }
        }
    }
    
    // 对局部区域应用沙地边界绘制逻辑
    for (int localI = 0; localI < width; localI++) {
        for (int localJ = 0; localJ < height; localJ++) {
            if (tempMap[localI][localJ] == 0) { // 仅处理草地单元格
                int globalI = localI + minL;
                int globalJ = localJ + minU;
                
                // 检查八个方向是否有海滩
                int left = (localI > 0) ? (tempMap[localI-1][localJ] == 2 ? 1 : 0) : 
                          (globalI > 0 && cell[globalI-1][globalJ].Num >= 29 && cell[globalI-1][globalJ].Num <= 40 ? 1 : 0);
                int up = (localJ+1 < height) ? (tempMap[localI][localJ+1] == 2 ? 1 : 0) : 
                        (globalJ+1 < MAP_U && cell[globalI][globalJ+1].Num >= 29 && cell[globalI][globalJ+1].Num <= 40 ? 1 : 0);
                int right = (localI+1 < width) ? (tempMap[localI+1][localJ] == 2 ? 1 : 0) : 
                           (globalI+1 < MAP_L && cell[globalI+1][globalJ].Num >= 29 && cell[globalI+1][globalJ].Num <= 40 ? 1 : 0);
                int down = (localJ > 0) ? (tempMap[localI][localJ-1] == 2 ? 1 : 0) : 
                          (globalJ > 0 && cell[globalI][globalJ-1].Num >= 29 && cell[globalI][globalJ-1].Num <= 40 ? 1 : 0);
                
                // 检查角落方向
                int upLeft = (localI > 0 && localJ+1 < height) ? (tempMap[localI-1][localJ+1] == 2 ? 1 : 0) : 
                            (globalI > 0 && globalJ+1 < MAP_U && cell[globalI-1][globalJ+1].Num >= 29 && cell[globalI-1][globalJ+1].Num <= 40 ? 1 : 0);
                int upRight = (localI+1 < width && localJ+1 < height) ? (tempMap[localI+1][localJ+1] == 2 ? 1 : 0) : 
                             (globalI+1 < MAP_L && globalJ+1 < MAP_U && cell[globalI+1][globalJ+1].Num >= 29 && cell[globalI+1][globalJ+1].Num <= 40 ? 1 : 0);
                int downRight = (localI+1 < width && localJ > 0) ? (tempMap[localI+1][localJ-1] == 2 ? 1 : 0) : 
                               (globalI+1 < MAP_L && globalJ > 0 && cell[globalI+1][globalJ-1].Num >= 29 && cell[globalI+1][globalJ-1].Num <= 40 ? 1 : 0);
                int downLeft = (localI > 0 && localJ > 0) ? (tempMap[localI-1][localJ-1] == 2 ? 1 : 0) : 
                              (globalI > 0 && globalJ > 0 && cell[globalI-1][globalJ-1].Num >= 29 && cell[globalI-1][globalJ-1].Num <= 40 ? 1 : 0);
                
                // 计算主方向编码
                int mainDirCode = (left << 0) | (up << 1) | (right << 2) | (down << 3);
                
                // 查找主方向的沙地边界类型
                auto it = SandCodeToNum.find(mainDirCode);
                if (it != SandCodeToNum.end()) {
                    cell[globalI][globalJ].Num = it->second;
                } else {
                    // 检查角落方向
                    int cornerDirCode = 0;
                    if (upLeft && !up && !left) cornerDirCode |= 0b00010000;  // 左上角
                    if (upRight && !up && !right) cornerDirCode |= 0b00100000;  // 右上角
                    if (downRight && !down && !right) cornerDirCode |= 0b01000000;  // 右下角
                    if (downLeft && !down && !left) cornerDirCode |= 0b10000000;  // 左下角
                    
                    it = SandCodeToNum.find(cornerDirCode);
                    if (it != SandCodeToNum.end()) {
                        cell[globalI][globalJ].Num = it->second;
                    }
                }
            }
        }
    }
}

/*
 * 函数：Map::resetBlockToGrass
 * 参数：blockL, blockU - block坐标
 * 内容：将指定block重置为草地状态
 * 返回值：无
 */
void Map::resetBlockToGrass(int blockL, int blockU) {
    if (blockL >= 0 && blockL < MAP_L && blockU >= 0 && blockU < MAP_U) {
        // 只有在不是海洋的情况下才重置为草地
        if (cell[blockL][blockU].getMapType() != MAPTYPE_OCEAN) {
            cell[blockL][blockU].Num = 0;  // 草地纹理编号
            cell[blockL][blockU].setMapPattern(MAPPATTERN_GRASS);
        }
    }
}

/*
 * 函数：Map::shouldBeBeach
 * 参数：blockL, blockU - block坐标
 * 内容：检查指定block是否应该绘制为海滩
 * 返回值：true表示应该为海滩，false表示应该为普通草地
 */
bool Map::shouldBeBeach(int blockL, int blockU) {
    if (blockL < 0 || blockL >= MAP_L || blockU < 0 || blockU >= MAP_U) {
        return false;
    }
    
    // 如果自己就是海洋，不需要绘制海滩
    if (cell[blockL][blockU].getMapType() == MAPTYPE_OCEAN) {
        return false;
    }
    
    // 检查八个方向是否有海洋
    for (int di = -1; di <= 1; di++) {
        for (int dj = -1; dj <= 1; dj++) {
            if (di == 0 && dj == 0) continue;  // 跳过自己
            
            int ni = blockL + di;
            int nj = blockU + dj;
            
            if (ni >= 0 && ni < MAP_L && nj >= 0 && nj < MAP_U) {
                if (cell[ni][nj].getMapType() == MAPTYPE_OCEAN) {
                    return true;  // 有相邻的海洋，应该绘制海滩
                }
            }
        }
    }
    
    return false;  // 没有相邻海洋，应该为普通草地
}

vector<QString> Map::GetAllTargetFiles(QString suffix)
{
    //获取当前目录路径
    QString currentDir = QDir::currentPath();
    // 创建迭代器（非递归模式）
    QDirIterator it(currentDir,                   // 当前目录
                    QStringList() << ("*." + suffix), // 后缀过滤器
                    QDir::Files);                 // 只遍历文件（不递归）
    vector<QString>ret;
    while (it.hasNext()) {
        ret.push_back(it.next());
    }
    return ret;
}

/*
 * 函数：Map::isSlope；
 * 参数：BlockDR，BlockUR——BlockX，BlockY；
 * 内容：判断地图块是否为斜坡；
 * 返回值：是/否。
 */
bool Map::isSlope(int BlockDR, int BlockUR)
{
    if(this->cell[BlockDR][BlockUR].getMapType() == MAPTYPE_FLAT ||
       this->cell[BlockDR][BlockUR].getMapType() == MAPTYPE_EMPTY) return false;
    return true;
}

void Map::JudegCellType(int BlockDR, int BlockUR)
{
    int i=BlockDR+4,j=BlockUR+4;
    Block&block=this->cell[i-4][j-4];
    //如果是海洋
    if(block.getMapHeight()==MAPHEIGHT_OCEAN){
        block.setMapType(MAPTYPE_OCEAN);
        return;
    }
    //判断是否是接壤海的区块,如果是直接返回，因为海洋边缘只允许是平地
    if(CheckIsNearOcean(i,j)){
        block.setMapHeight(MAPHEIGHT_FLAT);
        block.setMapType(MAPTYPE_OCEAN);
        return;
    }
    // 判断四个角与中间区块的高度差
    int heightDiffA = abs(m_heightMap[i][j] - m_heightMap[i - 1][j - 1]) + abs(m_heightMap[i][j] - m_heightMap[i + 1][j - 1]) + abs(m_heightMap[i][j] - m_heightMap[i - 1][j + 1]) + abs(m_heightMap[i][j] - m_heightMap[i + 1][j + 1]);
    // 判断四条临边与中间区块的高度差
    int heightDiffL = abs(m_heightMap[i][j] - m_heightMap[i - 1][j]) + abs(m_heightMap[i][j] - m_heightMap[i + 1][j]) + abs(m_heightMap[i][j] - m_heightMap[i][j - 1]) + abs(m_heightMap[i][j] - m_heightMap[i][j + 1]);
    switch(heightDiffA)
    {
    case 1:
        /*
            * X X X
            * X # X
            * X X 1
            */
        if(m_heightMap[i + 1][j + 1] - m_heightMap[i][j] == 1)
        {
            this->cell[i - 4][j - 4].setMapType(MAPTYPE_A3_UPTOR);
        }
        /*
            * X X X
            * X # X
            * 1 X X
            */
        else if(m_heightMap[i + 1][j - 1] - m_heightMap[i][j] == 1)
        {
            this->cell[i - 4][j - 4].setMapType(MAPTYPE_A2_DOWNTOU);
        }
        /*
            * X X 1
            * X # X
            * X X X
            */
        else if(m_heightMap[i - 1][j + 1] - m_heightMap[i][j] == 1)
        {
            this->cell[i - 4][j - 4].setMapType(MAPTYPE_A2_UPTOU);
        }
        /*
            * 1 X X
            * X # X
            * X X X
            */
        else if(m_heightMap[i - 1][j - 1] - m_heightMap[i][j] == 1)
        {
            this->cell[i - 4][j - 4].setMapType(MAPTYPE_A1_UPTOL);
        }
        break;
    }

    switch(heightDiffL)
    {
    case 2:
        /*
            * X 0 X
            * 0 # 1
            * X 1 X
            */
        if((m_heightMap[i + 1][j] - m_heightMap[i][j] == 1) && (m_heightMap[i][j + 1] - m_heightMap[i][j] == 1))
        {
            this->cell[i - 4][j - 4].setMapType(MAPTYPE_A1_DOWNTOL);
        }
        /*
             * X 1 X
             * 1 # 0
             * X 0 X
             */
        else if((m_heightMap[i - 1][j] - m_heightMap[i][j] == 1) && (m_heightMap[i][j - 1] - m_heightMap[i][j] == 1))
        {
            this->cell[i - 4][j - 4].setMapType(MAPTYPE_A3_DOWNTOR);
        }
        /*
             * X 1 X
             * 0 # 1
             * X 0 X
             */
        else if((m_heightMap[i - 1][j] - m_heightMap[i][j] == 1) && (m_heightMap[i][j + 1] - m_heightMap[i][j] == 1))
        {
            this->cell[i - 4][j - 4].setMapType(MAPTYPE_A0_DOWNTOD);
        }
        /*
             * X 0 X
             * 1 # 0
             * X 1 X
             */
        else if((m_heightMap[i + 1][j] - m_heightMap[i][j] == 1) && (m_heightMap[i][j - 1] - m_heightMap[i][j] == 1))
        {
            this->cell[i - 4][j - 4].setMapType(MAPTYPE_A0_DOWNTOD);
        }
        break;

    case 1:
        if(m_heightMap[i + 1][j] - m_heightMap[i][j] == 1)
        {
            this->cell[i - 4][j - 4].setMapType(MAPTYPE_L3_UPTORD);
        }
        else if(m_heightMap[i - 1][j] - m_heightMap[i][j] == 1)
        {
            this->cell[i - 4][j - 4].setMapType(MAPTYPE_L1_UPTOLU);
        }
        else if(m_heightMap[i][j + 1] - m_heightMap[i][j] == 1)
        {
            this->cell[i - 4][j - 4].setMapType(MAPTYPE_L2_UPTORU);
        }
        else if(m_heightMap[i][j - 1] - m_heightMap[i][j] == 1)
        {
            this->cell[i - 4][j - 4].setMapType(MAPTYPE_L0_UPTOLD);
        }
        break;
    }
}

array<int, 2> Map::GetCellOffset(int BlockDR, int BlockUR)
{
    int i=BlockDR,j=BlockUR;
    Block&block=cell[i][j];
    int ox=0,oy=0;
    // 偏移
    if(block.getMapHeight()>0)
    {
        oy=DRAW_OFFSET * this->cell[i][j].getMapHeight();
    }
    if(block.getMapType() == 2 || block.getMapType() == 3 || block.getMapType() == 4 || block.getMapType() == 5 || block.getMapType() == 8 || block.getMapType() == 9)
    {
        oy=this->cell[i][j].getOffsetY() + DRAW_OFFSET;
    }

    // 修整边界
    if(this->cell[i][j].getMapType() == 10)
    {
        ox = this->cell[i][j].getOffsetX()-1;
    }
    else if(this->cell[i][j].getMapType() == 11)
    {
        ox= this->cell[i][j].getOffsetX()+1;
    }
    else if(this->cell[i][j].getMapType() == 13)
    {
       oy = this->cell[i][j].getOffsetY()+1;
    }
    //如果是海洋，按照wlh的方式来偏移
    if(block.getMapType()==MAPTYPE_OCEAN)
        oy=2;
    return {ox,oy};
}

void Map::CalCellOffset(int BlockDR, int BlockUR)
{
    auto&&ret=GetCellOffset(BlockDR,BlockUR);
    auto&cell=this->cell[BlockDR][BlockUR];
    cell.setOffsetX(ret[0]);
    cell.setOffsetY(ret[1]);
}

void Map::divideTheMap()
{
    vector<vector<bool>>vis(MAP_L,vector<bool>(MAP_U));
    int idx=0;
    function<void(int i,int j)>dfs=[&](int i,int j)->void{
            queue<array<int,2>>q;
            vis[i][j]=1;
            blockIndex[i][j]=idx;
            q.push({i,j});
            while(!q.empty()){
                auto&e=q.front();
                int i=e[0],j=e[1];
                q.pop();
                bool flag=cell[i][j].getMapType()==MAPTYPE_OCEAN;
                static const int off[][2]={{0,1},{0,-1},{1,0},{-1,0}};
                for(auto*o:off){
                    int ii=o[0]+i,jj=o[1]+j;
                    if(ii>=0&&ii<MAP_L&&jj>=0&&jj<MAP_U){
                        if(!vis[ii][jj]){
                            bool flag1=cell[ii][jj].getMapType()==MAPTYPE_OCEAN;
                            if(flag1==flag){
                                vis[ii][jj]=1;
                                blockIndex[ii][jj]=idx;
                                q.push({ii,jj});
                            }
                        }
                    }
                }
           }
    };
    
    for(int i=0;i<MAP_L;++i){
        for(int j=0;j<MAP_U;++j){
            if(!vis[i][j]){
                dfs(i,j);
                ++idx;
            }
        }
    }
    //找到地方陆地目标所在的大陆
    {
        enemyBlockIdx=-1;
        enemyLandExplored=0;
        map<int,int>idxCnt;
        for(auto*human:player[1]->human){
            int x = human->getBlockDR();
            int y = human->getBlockUR();
            // 添加边界检查
            if(x >= 0 && x < MAP_L && y >= 0 && y < MAP_U) {
                ++idxCnt[blockIndex[x][y]];
            }
        }
        for(auto&ele:idxCnt){
            int idx=ele.first,cnt=ele.second;
            if(enemyBlockIdx==-1||cnt>idxCnt[enemyBlockIdx]){
                enemyBlockIdx=idx;
            }
        }
    }
    //找到市镇中心所在起始位置
    Point centerPos;
    for(auto&build:player[0]->build){
        if(build->getNum()==BUILDING_CENTER){
            centerPos={build->getBlockDR(),build->getBlockUR()};
            break;
        }
    }
    // 添加边界检查
    int mask = -1;
    if(centerPos.x >= 0 && centerPos.x < MAP_L && centerPos.y >= 0 && centerPos.y < MAP_U) {
        mask = blockIndex[centerPos.x][centerPos.y];
    }
    //如果是海洋并且与己方大陆相距两格，那么可见
    auto checkLand=[&](int x,int y)->bool{
        if(x>=0&&x<MAP_L&&y>=0&&y<MAP_U){
            return cell[x][y].getMapType()!=MAPTYPE_OCEAN&&blockIndex[x][y]==mask;
        }
        return false;
    };
    auto checkOceanNeedSetExplored=[&](int i,int j)->bool{
        //检测2*2的正方形内是否有内陆
        for(int x=-2;x<=2;++x){
            for(int y=-2;y<=2;++y){
                int ii=i+x,jj=j+y;
                if(checkLand(ii,jj))return 1;
            }
        }
        return 0;
    };
    //将己方地图可见化
    for(int i=0;i<MAP_L;++i){
        for(int j=0;j<MAP_U;++j){
            //
            auto&block=cell[i][j];
            if(blockIndex[i][j]==mask){
                block.Visible=0;
                block.Explored=1;
            }
            else if(block.getMapType()==MAPTYPE_OCEAN&&checkOceanNeedSetExplored(i,j)){
                    block.Visible=0;
                    block.Explored=1;
            }
            else{
                block.Visible=0;
                block.Explored=0;
            }
        }
    }
    //根据预定义参数进行设置可见
    if(MAP_EXPLORE||EditorMode||GlobalVision){
        for(int i=0;i<MAP_L;++i){
            for(int j=0;j<MAP_U;++j){
                cell[i][j].Explored=1;
            }
        }
    }
    if(MAP_VISIABLE||EditorMode||GlobalVision){
        for(int i=0;i<MAP_L;++i){
            for(int j=0;j<MAP_U;++j){
                cell[i][j].Visible=1;
            }
        }
    }
}

/*void Map::divideTheMap()
{
    Point centerPos;
    for(auto&build:player[0]->build){
        if(build->getNum()==BUILDING_CENTER){
            centerPos={build->getBlockDR(),build->getBlockUR()};
            break;
        }
    }
    for(int i=0;i<MAP_L;++i){
        for(int j=0;j<MAP_U;++j){
            auto&block=cell[i][j];
            block.Visible=0;
            block.Explored=0;
        }
    }
    for(int i=-8;i<=8;++i){
        for(int j=-8;j<=8;++j){
            int ii=centerPos.x+i,jj=centerPos.y+j;
            if(ii>=0&&ii<MAP_L&&jj>=0&&jj<MAP_U){
                cell[ii][jj].Visible=1;
                cell[ii][jj].Explored=1;
            }
        }
    }
    if(MAP_EXPLORE||EditorMode||GlobalVision){
        for(int i=0;i<MAP_L;++i){
            for(int j=0;j<MAP_U;++j){
                cell[i][j].Explored=1;
            }
        }
    }
    if(MAP_VISIABLE||EditorMode||GlobalVision){
        for(int i=0;i<MAP_L;++i){
            for(int j=0;j<MAP_U;++j){
                cell[i][j].Visible=1;
            }
        }
    }
}*/

int Map::getCellOffsetX(int l, int u)
{
    if(l>=0&&u>=0&&l<MAP_L&&u<MAP_U){
        return cell[l][u].getOffsetX();
    }
    return 0;
}

int Map::getCellOffsetY(int l, int u)
{
    if(l>=0&&u>=0&&l<MAP_L&&u<MAP_U){
        return cell[l][u].getOffsetY();
    }
    return 0;
}

void Map::loadBarrierMap(bool absolute)
{
    clearBarrierMap();

    for(int i=0;i<MAXPLAYER;i++)
    {
        //设置建筑为障碍
        if(!player[i]->build.empty())
        {
            std::list<Building *>::iterator iter=player[i]->build.begin() , iterend = player[i]->build.end();
            while(iter!=iterend)
            {
                setBarrier((*iter)->getBlockDR() , (*iter)->getBlockUR() , (*iter)->get_BlockSizeLen());
                iter++;
            }
        }
        //设置人为障碍物
        if(!player[i]->human.empty())
        {
            std::list<Human *>::iterator iter=player[i]->human.begin(), iterend = player[i]->human.end();
            while(iter!=iterend)
            {
//                CollisionObject.push_back((*ite));
                if(!(*iter)->isDie())
                    setBarrier((*iter)->getBlockDR(),(*iter)->getBlockUR(),(*iter)->get_BlockSizeLen());
                iter++;
            }
        }
    }

    //设置不可移动资源为障碍物
    if(!staticres.empty())
    {
        std::list<StaticRes *>::iterator iter=staticres.begin() , iterend = staticres.end();
        while(iter!=iterend)
        {
            if( absolute || (*iter)->getNum() != NUM_STATICRES_Bush )
                setBarrier((*iter)->getBlockDR() , (*iter)->getBlockUR() , (*iter)->get_BlockSizeLen());
            iter++;
        }
    }

    //设置动物为障碍物
    if(!animal.empty())
    {
        std::list<Animal *>::iterator iter=animal.begin(),iterend = animal.end();
        while(iter!=iterend)
        {
            /*if((*iter)->isTree()) */setBarrier((*iter)->getBlockDR() , (*iter)->getBlockUR() , (*iter)->get_BlockSizeLen());
            iter++;
        }
    }
}

void Map::loadBarrierMap_ByObjectMap()
{
    clearBarrierMap();

    int size;
    Coordinate* object;
    for(int x = 0; x<MAP_L; x++)
        for(int y = 0 ; y<MAP_U; y++)
        {
            size = map_Object[x][y].size();
            for(int i = 0; i<size; i++)
            {
                //
                object=map_Object[x][y][i];
                if(CanCrush(object)==0)continue;
                barrierMap[x][y] = 1;
                break;
            }
        }
    return;
}

bool Map::CanCrush(Coordinate *object)
{
    if(object->getSort() == SORT_STATICRES && (object->getNum() == NUM_STATICRES_Bush||object->getNum()==NUM_STATICRES_Fish))
        return 0;
    Human*human=0;
    object->printer_ToHuman((void**)&human);
    if(human){
        if(human->getTransported())
            return 0;
    }
    return 1;
}


Map::TypeRef& Map::loadfindPathMap(MoveObject* moveOb)
{
    int represent = moveOb->getPlayerRepresent();

    if(represent == MAXPLAYER)
        represent = NOWPLAYER-1;
    return findPathMapTemperature[represent];
   // memcpy(findPathMap, findPathMapTemperature[represent], sizeof(findPathMapTemperature[represent]));
}

void Map::loadfindPathMapTemperature()
{
    clearfindPathMapTemperature();
    for(int represent = 0; represent<NOWPLAYER; represent++)
        for(int x=0; x<MAP_L; x++)
            for(int y=0; y<MAP_U; y++)
            {
                if(TreeBlock[x][y] || barrierMap[x][y] || (represent == NOWPLAYERREPRESENT && !cell[x][y].Explored))
                    findPathMapTemperature[represent][x][y] = 1;
            }
    return;
}

//设置障碍物
void Map::setBarrier(int blockDR,int blockUR , int blockSideLen )
{
    int bDRR = min(blockDR+blockSideLen, MAP_L),bURU = min(blockUR+blockSideLen,MAP_U);

    for(int i = blockDR; i<bDRR; i++)
        for(int j = blockUR;j<bURU;j++) barrierMap[i][j] = 1;

    return;
}

bool Map::isBarrier(Point blockPoint,int blockSideLen)
{
    return isBarrier(blockPoint.x,blockPoint.y,blockSideLen);
}

bool Map::isBarrier( int blockDR , int blockUR, int blockSideLen)
{
    if(isOverBorder(blockDR,blockUR)) return true;
    int bDRR = min(blockDR+blockSideLen, MAP_L),bURU = min(blockUR+blockSideLen,MAP_U);

    for(int i = blockDR; i<bDRR; i++)
        for(int j = blockUR;j<bURU;j++)
            if(barrierMap[i][j] == 1) return true;

    return false;
}

bool Map::isBarrier( int blockDR , int blockUR, int &bDR_barrier , int &bUR_barrier ,int blockSideLen )
{
/**
传入：
blockDR:起始BlockDR；
blockUR:起始BlockUR；
&bDR_barrier: 返回第一个找到的障碍物点BlockDR；
&bUR_barrier: 返回第一个找到的障碍物点的BlockUR；
blockSideLen: 返回查找的边大小；
返回：
true：指定范围内有障碍物；
false：指定范围内无障碍物；
*/
    int bDRR = min(blockDR+blockSideLen, MAP_L),bURU = min(blockUR+blockSideLen,MAP_U);
    bDR_barrier=blockDR;
    bUR_barrier = blockUR;
    if(isOverBorder(blockDR,blockUR)) return true;

    for(int i = blockDR; i<bDRR; i++)
        for(int j = blockUR;j<bURU;j++)
        {
            if(barrierMap[i][j] == 1)
            {
                bDR_barrier = i;
                bUR_barrier = j;

                return true;
            }
        }

    return false;
}

bool Map::isHaveObject(int blockDR , int blockUR, int &bDR_barrier , int &bUR_barrier ,int blockSideLen)
{
    int bDRR = min(blockDR+blockSideLen, MAP_L),bURU = min(blockUR+blockSideLen,MAP_U);
    bDR_barrier=blockDR;
    bUR_barrier = blockUR;
    if(isOverBorder(blockDR,blockUR)) return true;

    for(int i = blockDR; i<bDRR; i++)
        for(int j = blockUR;j<bURU;j++)
        {
            if(map_Object[i][j].size())
            {
                bDR_barrier = i;
                bUR_barrier = j;

                return true;
            }
        }

    return false;
}



bool Map::isFlat(Coordinate* judOb)
{
    int blockDR = judOb->getBlockDR(),blockUR = judOb->getBlockUR() , blockSideLen = judOb->get_BlockSizeLen();
    return isFlat(blockDR,blockUR,blockSideLen);
}

bool Map::isFlat(int blockDR , int blockUR,int blockSideLen)
{
    int sideR = blockDR+blockSideLen, sideU = blockUR+blockSideLen;
    int standard = map_Height[blockDR][blockUR] , tempHight;

    if(standard == -1) return false;

    for(int x = blockDR; x<sideR; x++)
    {
        for(int y = blockUR; y<sideU;y++)
        {
            tempHight = map_Height[x][y];

            if(tempHight != standard) return false;
        }
    }

    return true;
}

//该函数调用必须在barrierMap数组更新后
vector<pair<Point,int>> Map::findBlock_Free(Coordinate* object , int disLen, bool mustFind)
{
    int blockDR = object->getBlockDR(),blockUR = object->getBlockUR() , blockSideLen = object->get_BlockSizeLen();
    int sideR = blockDR+blockSideLen, sideU = blockUR+blockSideLen;
    int bDRL = max(0,blockDR-disLen) , bURD = max(0,blockUR - disLen) , bDRR = min(blockDR+blockSideLen+disLen , MAP_L) , bURU = min(blockUR+blockSideLen+disLen,MAP_U);
    vector<Point> Block_Free;
    Point tempPoint;

    //在给定范围内找寻没有障碍物的格子
    for(int x = bDRL; x<bDRR; x++)
    {
        for(int y = bURD; y<bURU;y++)
        {
            if(x>=blockDR && x<sideR && y>=blockUR && y<sideU) continue;

            if(map_Object[x][y].empty())
            {
                tempPoint.x = x;
                tempPoint.y = y;
                Block_Free.push_back(tempPoint);
            }
        }
    }

    //如果一次找寻后为空,再次查询
    while(Block_Free.empty() && mustFind)
    {
        bDRL = max(0,bDRL-1);
        bURD = max(0,bURD-1);
        bDRR = min(bDRR+1, MAP_L);
        bURU = min(bURU+1 , MAP_U);

        if(bDRL == 0 && bURD == 0 && bDRR == MAP_L && bURU == MAP_U)
        {
            qDebug()<<"findFalse";
            break;
        }

        //查找边界是否有无障碍空位
        for(int x = bDRL; x<bDRR;x++)
        {
            if(map_Object[x][bURD].empty())
            {
                tempPoint.x = x;
                tempPoint.y = bURD;
                Block_Free.push_back(tempPoint);
            }
            if(map_Object[x][bURU-1].empty())
            {
                tempPoint.x = x;
                tempPoint.y = bURU-1;
                Block_Free.push_back(tempPoint);
            }
        }

        for(int y = bURD+1; y<bURU;y++)
        {
            if(map_Object[bDRL][y].empty())
            {
                tempPoint.x = bDRL;
                tempPoint.y = y;
                Block_Free.push_back(tempPoint);
            }
            if(map_Object[bDRR-1][y].empty())
            {
                tempPoint.x = bDRR-1;
                tempPoint.y = y;
                Block_Free.push_back(tempPoint);
            }
        }
    }
    vector<pair<Point,int>>ans;
    for(auto&v:Block_Free){
        ans.push_back({v,cell[v.x][v.y].getMapType()});
    }
    return ans;
}

vector<Point>& Map::findBlock_Free(Point blockPoint, int lenth,bool landUnit)
{
    int blockDR = blockPoint.x, blockUR = blockPoint.y;

    int bDRL = max(0,blockDR-lenth) , bURD = max(0,blockUR - lenth);
    int bDRR = min(blockDR+1+lenth, MAP_L) , bURU = min(blockUR+1+lenth, MAP_U);

    static vector<Point> Block_Free;
    Block_Free.clear();
    Point tempPoint;

    //在给定范围内找寻没有障碍物的格子
    for(int x = bDRL; x<bDRR; x++)
    {
        for(int y = bURD; y<bURU;y++)
        {
            if(x == blockDR && y==blockUR) continue;
            bool ocean=cell[x][y].getMapType()==MAPTYPE_OCEAN;
            if(map_Object[x][y].empty()&&(ocean^landUnit))
            {
                tempPoint.x = x;
                tempPoint.y = y;
                Block_Free.push_back(tempPoint);
            }
        }
    }

    return Block_Free;
}


vector<Point> Map::get_ObjectVisionBlock(Coordinate* object)
{
    Point position(object->getBlockDR() , object->getBlockUR());
    Point visionPoint;
    vector<Point> blockLab = object->getViewLab();
    vector<Point> visionLab;
    int labSize = blockLab.size();
    for(int i = 0 ; i<labSize; i++)
    {
        visionPoint = position+blockLab[i];
        if(visionPoint.x<0 || visionPoint.x>=MAP_L || visionPoint.y<0 || visionPoint.y>=MAP_U) continue;
        visionLab.push_back(visionPoint);
    }

    return visionLab;
}

vector<Point> Map::get_ObjectBlock(Coordinate* object)
{
    /**
    * 获取指定目标在地图上所占格子
    * 输入:指定的object的Coorinate指针
    * 传出:其所占格子的vector列表
    */

    int blockSidelen = object->get_BlockSizeLen();
    Point position;
    vector<Point> blockLab;
    position.x = object->getBlockDR();
    position.y = object->getBlockUR();

    for(int x = 0 ; x < blockSidelen; x++)
        for(int y = 0 ; y < blockSidelen; y++)
            blockLab.push_back(Point(position.x+x , position.y + y));

    return blockLab;
}

void Map::add_Map_Vision( Coordinate* object )
{
    vector<Point> blockLab = get_ObjectVisionBlock(object);
    int size = blockLab.size();

    for(int i = 0 ;i<size ; i++)
        map_Vision[blockLab[i].x][blockLab[i].y].push_back(object);

    return;
}

 void Map::init_Map_Height()
 {
     for(int x = 0; x<MAP_L; x++)
     {
         for(int y = 0; y<MAP_U; y++)
         {
            if(isSlope(x,y))
                map_Height[x][y] = -1;
            else map_Height[x][y] = cell[x][y].getMapHeight();
         }
     }

     return;
 }

 void Map::reset_Map_Object_Resource()
 {
    std::list<Animal*>::iterator animaliter=animal.begin(), animalitere = animal.end();
    std::list<StaticRes*>::iterator SRiter=staticres.begin(), SRitere = staticres.end();

    while(animaliter!=animalitere)
    {
        if((*animaliter)->is_Surplus())
            add_Map_Object(*animaliter);
        animaliter++;
    }

    while(SRiter != SRitere)
    {
        if((*SRiter)->is_Surplus())
            add_Map_Object(*SRiter);
        SRiter++;
    }

    return;
 }


QString Map::GetMapFileName()
{
    return MapFileName;
}

//更新用户视野
void Map::reset_CellExplore(Coordinate* eye,vector<Point>&store)
{
    /**
    * 输入：用户的控制对象，如Human、Building
    * 操作：根据用户输入的对象，设置视野内格子为已探索
    */
    vector<Point> blockLab;
    int size;
    Building* buildPrinter = NULL;

    eye->printer_ToBuilding((void**)& buildPrinter);

    if(!(buildPrinter == NULL || buildPrinter->isFinish()))
        blockLab = get_ObjectBlock(eye);
    else
        blockLab = get_ObjectVisionBlock(eye);

    size = blockLab.size();

    for(int i = 0 ; i<size; i++)
    {
        int x=blockLab[i].x,y=blockLab[i].y;
        if(!cell[x][y].Explored){
            cell[x][y].Explored = true;
            store.push_back({x,y});
        }
        if(!cell[x][y].Visible)
        {
            blockLab_Visible.push(blockLab[i]);
            cell[x][y].Visible = true;
        }
    }

    return ;
}

void Map::clear_CellVisible()
{
    while(blockLab_Visible.size())
    {
        cell[blockLab_Visible.top().x][blockLab_Visible.top().y].Visible = false;
        blockLab_Visible.pop();
    }

    return;
}

//用于更新每个object可见性与是否在已探索格子
void Map::reset_ObjectExploreAndVisible()
{
    int size;
    for(int x = 0 ; x<MAP_L;x++)
    {
        for(int y = 0 ; y<MAP_U;y++)
        {
            size = map_Object[x][y].size();
            for(int z = 0; z<size;z++)
            {
                map_Object[x][y][z]->setExplored(cell[x][y].Explored);
                map_Object[x][y][z]->setvisible(cell[x][y].Visible);
            }
        }
    }
    return;
}

int Map::addStaticRes(int Num, double DR, double UR) {
    StaticRes *newstaticres=new StaticRes(Num,DR,UR);
    this->staticres.push_back(newstaticres);
    return 0;
}

int Map::addStaticRes(int Num, int BlockDR, int BlockUR) {
    StaticRes *newstaticres=new StaticRes(Num,BlockDR,BlockUR);
    this->staticres.push_back(newstaticres);
    return 0;
}

/*
 * 函数：Map::addAnimal；
 * 参数：Num——动物种类；
 *      DR，UR——动物生成的xy坐标；
 * 内容：在对应坐标生成一个动物；
 * 返回值：生成成功返回true。
 */
bool Map::addAnimal(int Num, double DR, double UR) {
    Animal *newanimal=new Animal(Num,DR,UR);
    this->animal.push_back(newanimal);
    return true;
}


/*
 * 函数：Map::CheckNeighborType
 * 参数：x：x坐标；
 *      y：y坐标；
 *      selectType：需要搜索的blockType类型;
 * 内容：统计block(x, y)周围8个block有几个是selectType类型；
 * 返回值：返回数量。
 */
int Map::CheckNeighborType(int x, int y, int selectType) {
    int count = 0;
    int dx[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
    int dy[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
    for(int i = 0; i < 8; i ++) {
        int tx = x + dx[i], ty = y + dy[i];
        if(tx < 0 || ty < 0 || tx > 71 || ty > 71) continue;
        if(this->cell[tx][ty].getMapType() == selectType) count ++;
    }
    return count;
}


/*
 * 函数：Map::checkBorder；
 * 参数：heightMap[][80]——临时高度数组；
 *      x——x坐标；y——y坐标；
 *      currentCalHeight——当前正在生成的高度；
 * 内容：检查block(x, y)是否为当前高度的边界；
 * 返回值：如果是，返回false；如果不是，返回true。
 */
bool Map::CheckBorder(int x, int y, int currentCalHeight) {
    int dx[24] = {-2, -1, 0, 1, 2, -2, -1, 0, 1, 2, -2, -1, 1, 2, -2, -1, 0, 1, 2, -2, -1, 0, 1, 2};
    int dy[24] = {-2, -2, -2, -2, -2, -1, -1, -1, -1, -1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2};
    for(int i = 0; i < 24; i ++) {
        int tx = x + dx[i], ty = y + dy[i];
        if(tx < 0 || ty < 0 || tx > GENERATE_L || ty > GENERATE_L) continue;
        if(m_heightMap[tx][ty] <= currentCalHeight - 2) return false;
    }
    return true;
}

/*
 * 函数：Map::GenerateType；
 * 参数：无；
 * 内容：依据附近地形差确定对应坐标需要绘制的block类型；
 * 返回值：空。
 */
void Map::GenerateType() {
    /*
     * 坐标系映射如下：
     * —————→ y
     * |
     * |
     * ↓
     * x
     */
    for(int i = 4; i < MAP_L + 4; i++)
    {
        for(int j = 4; j < MAP_U + 4; j++)
        {
            JudegCellType(i-4,j-4);
        }
    }

    // 特殊处理（直接处理cell）
    for(int i = 0; i < MAP_L; i ++) {
        for(int j = 0; j < MAP_U; j ++) {
            Block&block=cell[i][j];
            if(block.getMapType()==MAPTYPE_OCEAN||CheckIsNearOcean(i+4,j+4))continue;//如果是海洋或者周围是海洋就不要检测了
            int count = CheckNeighborType(i, j, MAPTYPE_A2_UPTOU) +
                    CheckNeighborType(i, j, MAPTYPE_L3_DOWNTORD) +
                    CheckNeighborType(i, j, MAPTYPE_L0_DOWNTOLD)+
            CheckNeighborType(i, j, MAPTYPE_A1_DOWNTOL) +
                    CheckNeighborType(i, j, MAPTYPE_A3_DOWNTOR);
            if(this->cell[i][j].getMapType() == MAPTYPE_A0_DOWNTOD && count == 0) {
                if(j + 1 < MAP_U && this->cell[i][j + 1].getMapType() != MAPTYPE_FLAT)
                    this->cell[i][j].setMapType(MAPTYPE_A0_UPTOD);
            }
            if(i + 1 < MAP_L && j + 1 < MAP_U && i > 0 &&
                    (this->cell[i - 1][j].getMapType() == MAPTYPE_A0_DOWNTOD || this->cell[i - 1][j].getMapType() == MAPTYPE_L2_UPTORU) &&
                    this->cell[i][j + 1].getMapType() == MAPTYPE_A0_DOWNTOD &&
                    this->cell[i + 1][j].getMapType() == MAPTYPE_FLAT)
                this->cell[i][j].setMapType(MAPTYPE_A2_UPTOU);
            count = CheckNeighborType(i, j, MAPTYPE_L0_UPTOLD) + CheckNeighborType(i, j, MAPTYPE_L3_UPTORD);
            if(this->cell[i][j].getMapType() == MAPTYPE_A0_DOWNTOD && count > 0)
                this->cell[i][j].setMapType(MAPTYPE_A0_UPTOD);
        }
    }
}

/*
 * 函数：Map::CalOffset；
 * 参数：无；
 * 内容：计算每种block不同高度对应的偏移量；
 * 返回值：空。
 */
void Map::CalOffset() {
    for(int i = 0; i < MAP_L; i ++) {
        for(int j = 0; j < MAP_U; j ++) {
            CalCellOffset(i,j);
        }
    }
    return ;
}

/*
 * 函数：Map::InitFaultHandle；
 * 参数：无；
 * 内容：确认地图块样式，并在 debug 模式下输出地图生成中的错误；
 * 返回值：空。
 */
void Map::InitFaultHandle() {
    for(int i = 0; i < MAP_L; i++)
    {
        for(int j = 0; j < MAP_U; j++)
        {
            this->cell[i][j].setMapPattern(0);
            //如果是海洋直接设置Num为0
            if(this->cell[i][j].getMapType()==MAPTYPE_OCEAN)this->cell[i][j].Num=0;
            //
            else if(this->cell[i][j].getMapType() != 0 && this->cell[i][j].getMapType() != 1)
            {
                this->cell[i][j].Num = (this->cell[i][j].getMapPattern() + 1) * 15 + this->cell[i][j].getMapType();
            }
            else if(this->cell[i][j].getMapType() == 1)
            {
                // 检查是否已经是海岸线纹理，如果是则不要覆盖
                bool isShoreTexture = (this->cell[i][j].Num >= 29 && this->cell[i][j].Num <= 40) || 
                                     (this->cell[i][j].Num >= 2 && this->cell[i][j].Num <= 16);
                if (!isShoreTexture) {
                    // 设置平地全部为草地，但不覆盖海岸线纹理
                    this->cell[i][j].Num = 0;        // 默认地形为 Grass，值为 0
                }
                // 测试时设置平地全部为沙漠，便于调试
                // this->cell[i][j].Num = MAPPATTERN_DESERT;
            }
            else if(this->cell[i][j].getMapType() == 0)
            {
                qDebug() << "Map::InitFaultHandle() ERROR：第" << i << "行第" << j <<"列的地块MapType未定义！";
            }
            if(this->cell[i][j].Num >= 41) qDebug() << "Map::InitFaultHandle() ERROR：cell[" << i << "][" << j << "].Num >= 41";
        }
    }
}

/*
 * 函数：Map::InitCell；
 * 参数：Num——初始化地图块类型；
 *      isExplored——地图块是否被探索过；
 *      isVisible——地图块是否可见；
 * 内容：初始化cell数组；
 * 返回值：空。
 */
void Map::InitCell(int Num, bool isExplored, bool isVisible) {
    for(int i = 0; i < MAP_L; i ++) {
        for(int j = 0; j < MAP_U;j ++) {
            this->cell[i][j].Num = Num;
            this->cell[i][j].Explored = isExplored;
            this->cell[i][j].Visible = isVisible;    // 地图可见度
            this->cell[i][j].setBlockDRUR(i,j);
            this->cell[i][j].setDRUR(i*BLOCKSIDELENGTH,j*BLOCKSIDELENGTH);
        }
    }
}

/*
 * 函数：Map::loadGenerateMapText
 * 参数：获取启动参数，判断是否载入地图；
 * 内容：读取地图txt文件；
 * 返回值：空。
 */

void Map::loadGenerateMapText()
{
    // 使用高精度时间为种子的真随机数生成器
    auto AllMapFile=GetAllTargetFiles(MAPFILE_SUFFIX.c_str());
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 gen(seed);
    std::uniform_int_distribution<> dis(0,AllMapFile.size()-1);
    int mapIdx = dis(gen); // 1~4
    auto mapPath=AllMapFile[mapIdx];
    QFile file(mapPath);
    MapFileName=mapPath.split("/").back();

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qWarning() << "文件打开错误\n";
        return;
    }
    QJsonParseError * error = nullptr;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(),error);
    if(error){
        qWarning()<<"json error";
        return;
    }
    file.close();
    /////////////////////////////////判断位置是否合法
    auto checkBlockLegal=[&](int dr,int ur)->bool{return dr>=0&&dr<MAP_L&&ur>=0&&ur<MAP_U;};
    auto checkDetailBlockLegal=[&](double dr,double ur)->bool{return checkBlockLegal(floor(dr/BLOCKSIDELENGTH),floor(ur/BLOCKSIDELENGTH));};
    /////////////////////////////////解析区域到relation映射的函数
    auto ParseAreaToRelation=[&](Coordinate* unit, const string& type, QJsonObject& obj, int areaType) -> void {
        // 只有在编辑器模式下才建立relation映射
        if (!g_rectArea || !g_circleArea || !g_lineArea) {
            return;  // 非编辑器模式，跳过
        }

        if(type == LineArea::Name()){
            LineAreaData areaData;
            areaData.areaType = areaType;
            QJsonArray points = obj["Point"].toArray();
            for(auto ele : points){
                QJsonArray point = ele.toArray();
                areaData.data.push_back({point[0].toDouble(), point[1].toDouble()});
            }
            g_lineArea->relation.insert({unit, areaData});
        }
        else if(type == CircleArea::Name()){
            CircleAreaData areaData;
            areaData.areaType = areaType;
            areaData.dr = obj["DR"].toDouble();
            areaData.ur = obj["UR"].toDouble();
            areaData.rad = obj["R"].toDouble();
            g_circleArea->relation.insert({unit, areaData});
        }
        else if(type == RectArea::Name()){
            RectAreaData areaData;
            areaData.areaType = areaType;
            areaData.dr = obj["DR"].toDouble();
            areaData.ur = obj["UR"].toDouble();
            areaData.w = obj["W"].toDouble();
            areaData.h = obj["H"].toDouble();
            g_rectArea->relation.insert({unit, areaData});
        }
    };

    /////////////////////////////////解析区域的函数
    auto ParseArea=[&](int sn,const string&type,QJsonObject&obj)->void{
        void*data=0;
        if(type==LineArea::Name()){
            auto*block=new vector<array<double,2>>();
            QJsonArray points=obj["Point"].toArray();
            for(auto ele:points){
                QJsonArray point=ele.toArray();
                block->push_back({point[0].toDouble(),point[1].toDouble()});
            }
            data=block;
        }else if(type==CircleArea::Name()){
            double dr=obj["DR"].toDouble(),ur=obj["UR"].toDouble(),r=obj["R"].toDouble();
            auto*dat=new array<double,3>{dr,ur,r};
            data=dat;
        }else if(type==RectArea::Name()){
            double dr=obj["DR"].toDouble(),ur=obj["UR"].toDouble(),w=obj["W"].toDouble(),h=obj["H"].toDouble();
            auto*dat=new array<double,4>{dr,ur,w,h};
            data=dat;
        }
        //
        // 检查是否已经有区域限制，如果有，先释放内存
        if(enemyAreaLimit.find(sn) != enemyAreaLimit.end()){
            auto& existing = enemyAreaLimit[sn];
            if(existing.second != nullptr){
                // 根据类型释放内存
                if(existing.first == LineArea::Name()){
                    delete (vector<array<double,2>>*)existing.second;
                } else if(existing.first == CircleArea::Name()){
                    delete (array<double,3>*)existing.second;
                } else if(existing.first == RectArea::Name()){
                    delete (array<double,4>*)existing.second;
                }
            }
        }
        enemyAreaLimit[sn]=pair<string,void*>{type,data};
    };
    /////////////////////////////////开始解析
    QJsonObject root=doc.object();
    QStringList allKeys=root.keys();
    for(QString&key:allKeys){
        QJsonObject obj=root[key].toObject();
        if(key.contains("Cell")){
            int blockL=obj["BlockDR"].toInt(),blockU=obj["BlockUR"].toInt();
            // 添加边界检查
            if(checkBlockLegal(blockL,blockU)) {
                Block&block=cell[blockL][blockU];
                block.Num=obj["Num"].toInt();
                block.Visible=obj["Visible"].toBool();
                block.Explored=obj["Explored"].toBool();
                block.setMapType(obj["Type"].toInt());
                block.setMapPattern(obj["Pattern"].toInt());
                block.setMapHeight(obj["Height"].toInt());
                block.setOffsetX(obj["OffsetX"].toInt());
                block.setOffsetY(obj["OffsetY"].toInt());
                block.setMapResource(obj["Resource"].toInt());
            } else {
                qWarning() << "Cell坐标超出范围:" << blockL << "," << blockU;
            }
        }
        else if(key.contains("Building")){
            Player&me=*(player[0]),&enemy=(*player[1]);
            Player&cur=obj["Own"].toString()=="WLH"?me:enemy;
            int blockDR = obj["BlockDR"].toInt();
            int blockUR = obj["BlockUR"].toInt();
            // 添加边界检查
            if(checkBlockLegal(blockDR,blockUR)){
                cur.addBuilding(obj["Num"].toInt(), blockDR, blockUR, 100);
            } else {
                qWarning() << "Building坐标超出范围:" << blockDR << "," << blockUR;
            }
        }else if(key.contains("Human")){
            Player&me=*(player[0]),&enemy=(*player[1]);
            Player&cur=obj["Own"].toString()=="WLH"?me:enemy;
            double UR=obj["UR"].toDouble(),DR=obj["DR"].toDouble();
            if(!checkDetailBlockLegal(DR,UR))continue;
            if(obj["Sort"].toString()=="Farmer")
            {
                int FarmerType=obj["FarmerType"].toInt();
                if(FarmerType==FARMERTYPE_FARMER)
                cur.addFarmer(DR,UR);
                else cur.addShip(FarmerType,DR,UR);
            }
            else
            {
                //默认敌方只有军队需要须臾限制
                Army* army = cur.addArmy(obj["Num"].toInt(),DR,UR);
                int sn = army->getglobalNum();
                //读取区域限制
                if(obj.contains("AreaLimit")){
                    QJsonObject area=obj["AreaLimit"].toObject();
                    string type=area["Type"].toString().toStdString();
                    ParseArea(sn,type,area);
                    ParseAreaToRelation(army, type, area, 0);  // 0=普通区域
                }
                // 读取多个区域限制 (AreaLimits 数组)
                if(obj.contains("AreaLimits")){
                    QJsonArray areas=obj["AreaLimits"].toArray();
                    for(auto areaValue : areas){
                        QJsonObject area=areaValue.toObject();
                        string type=area["Type"].toString().toStdString();
                        ParseArea(sn,type,area);
                        ParseAreaToRelation(army, type, area, 0);  // 0=普通区域
                    }
                }
                //读取巡逻区域 (Beatarea)
                if(obj.contains("Beatarea")){
                    QJsonObject area=obj["Beatarea"].toObject();
                    string type=area["Type"].toString().toStdString();
                    ParseArea(sn,type,area);
                    ParseAreaToRelation(army, type, area, 1);  // 1=巡逻区域
                }
                // 读取多个巡逻区域 (BeatAreas 数组)
                if(obj.contains("BeatAreas")){
                    QJsonArray areas=obj["BeatAreas"].toArray();
                    for(auto areaValue : areas){
                        QJsonObject area=areaValue.toObject();
                        string type=area["Type"].toString().toStdString();
                        ParseArea(sn,type,area);
                        ParseAreaToRelation(army, type, area, 1);  // 1=巡逻区域
                    }
                }

                // 读取敌人状态属性
                if(obj.contains("statu") && cur.getRepresent() == 1){  // 只对敌方单位处理状态
                    string status = obj["statu"].toString().toStdString();
                    if(status == "attack" || status == "defend"){
                        // 将敌人状态存储到Map的内部存储中，稍后应用到MainWidget
                        int globalNum = army->getglobalNum();
                        enemyStatusMap[globalNum] = status;

                        // 调试信息
                        QString debugInfo = QString("读取敌人状态: GlobalNum=%1, Status=%2")
                            .arg(globalNum).arg(QString::fromStdString(status));
                        qDebug() << debugInfo;
                    }
                }
            }
        }else if(key.contains("Animal")){
            double dr=obj["DR"].toDouble(),ur=obj["UR"].toDouble();
            if(!checkDetailBlockLegal(ur,dr))continue;
            addAnimal(obj["Num"].toInt(),dr,ur);
        }else if(key.contains("StaticRes")){
            int blockDR = obj["BlockDR"].toInt();
            int blockUR = obj["BlockUR"].toInt();
            // 添加边界检查
            if(checkBlockLegal(blockDR,blockUR)) {
                addStaticRes(obj["Num"].toInt(), blockDR, blockUR);
            } else {
                qWarning() << "StaticRes坐标超出范围:" << blockDR << "," << blockUR;
            }
        }
    }
    //
    //
    // 读取敌人配置信息
    if(root.contains("EnemyConfig")) {
        QJsonObject enemyConfig = root["EnemyConfig"].toObject();
        Player& enemy = (*player[1]);

        // 读取巡逻船配置
        if(enemyConfig.contains("patrolShips")) {
            QJsonArray patrolShips = enemyConfig["patrolShips"].toArray();
            for(const QJsonValue& shipValue : patrolShips) {
                QJsonObject ship = shipValue.toObject();
                QString type = ship["type"].toString();
                double x = ship["x"].toDouble();
                double y = ship["y"].toDouble();
                int status = ship["status"].toString() == "ARMY_STATE_AROUND" ? ARMY_STATE_AROUND : 0;
                int starttime = ship["starttime"].toInt();
                int finishtime = ship["finishtime"].toInt();
                double destX = ship["destX"].toDouble();
                double destY = ship["destY"].toDouble();

                if(type == "AT_SHIP"&&checkDetailBlockLegal(x,y)&&checkDetailBlockLegal(destX,destY)) {
                    enemy.addArmyAROUND(AT_SHIP, x, y, status, starttime, finishtime, destX, destY);
                }
            }
        }

        // 读取内陆巡逻骑兵配置
        if(enemyConfig.contains("patrolScouts")) {
            QJsonArray patrolScouts = enemyConfig["patrolScouts"].toArray();
            for(const QJsonValue& scoutValue : patrolScouts) {
                QJsonObject scout = scoutValue.toObject();
                QString type = scout["type"].toString();
                double x = scout["x"].toDouble();
                double y = scout["y"].toDouble();
                int status = scout["status"].toString() == "ARMY_STATE_AROUND" ? ARMY_STATE_AROUND : 0;
                int starttime = scout["starttime"].toInt();
                int finishtime = scout["finishtime"].toInt();
                double destX = scout["destX"].toDouble();
                double destY = scout["destY"].toDouble();

                if(type == "AT_SCOUT"&&checkDetailBlockLegal(x,y)&&checkDetailBlockLegal(destX,destY)) {
                    enemy.addArmyAROUND(AT_SCOUT, x, y, status, starttime, finishtime, destX, destY);
                }
            }
        }

        // 读取内陆进攻兵配置
        if(enemyConfig.contains("attackArmies")) {
            QJsonArray attackArmies = enemyConfig["attackArmies"].toArray();
            for(const QJsonValue& armyValue : attackArmies) {
                QJsonObject army = armyValue.toObject();
                int type = army["type"].toInt();
                double x = army["x"].toDouble();
                double y = army["y"].toDouble();
                int status = army["status"].toInt();
                int starttime = army["starttime"].toInt();
                int finishtime = army["finishtime"].toInt();
                if(checkDetailBlockLegal(x,y))
                enemy.addArmyATTACK(type, x, y, status, starttime, finishtime);
            }
        }

        // 读取内陆防守兵配置
        if(enemyConfig.contains("defenseArmies")) {
            QJsonArray defenseArmies = enemyConfig["defenseArmies"].toArray();
            for(const QJsonValue& armyValue : defenseArmies) {
                QJsonObject army = armyValue.toObject();
                int type = army["type"].toInt();
                double x = army["x"].toDouble();
                double y = army["y"].toDouble();
                int status = army["status"].toInt();
                if(checkDetailBlockLegal(x,y))
                enemy.addArmyDEFENSE(type, x, y, status);
            }
        }

        qDebug() << "敌人配置读取完成";
    }

    //赋值m_heightMap
    for(auto&v:m_heightMap)fill(v.begin(),v.end(),0);
    for(int i=0;i<MAP_L;++i){
        for(int j=0;j<MAP_U;++j){
            m_heightMap[i+4][j+4]=cell[i][j].getMapHeight();
        }
    }

    // 确保所有cell元素都被正确初始化
    for(int i=0;i<MAP_L;++i){
        for(int j=0;j<MAP_U;++j){
            // 如果cell没有被读取过，设置默认值
            if(cell[i][j].getMapType() == 0 && cell[i][j].getMapHeight() == 0) {
                cell[i][j].setMapType(MAPTYPE_FLAT);
                cell[i][j].setMapHeight(MAPHEIGHT_FLAT);
                cell[i][j].setMapPattern(MAPPATTERN_GRASS);
                cell[i][j].setMapResource(RESOURCE_EMPTY);
                cell[i][j].Explored = false;
                cell[i][j].Visible = false;
                cell[i][j].setBlockDRUR(i,j);
                cell[i][j].setDRUR(i*BLOCKSIDELENGTH,j*BLOCKSIDELENGTH);
            }
        }
    }

    qDebug() << "地图文件解析完成，共处理" << allKeys.size() << "个对象";
}

bool Map::CheckIsNearOcean(int x, int y)
{
    //检查周围3*3区域是不是有海洋
    for(int i=-1;i<=1;++i){
        for(int j=-1;j<=1;++j){
            int ii=i+x,jj=j+y;
            if(ii<0||jj<0||ii>=GENERATE_L||jj>=GENERATE_U)continue;
            if(m_heightMap[ii][jj]==MAPHEIGHT_OCEAN)return 1;
        }
    }
    return 0;
}


/*
 * 函数：Map::init；
 * 参数：无；
 * 内容：初始化地图的总函数；
 * 返回值：空。
 */
void Map::init() {
    InitCell(0, MAP_EXPLORE, false);    // 第二个参数修改为true时可令地图全部可见
    loadGenerateMapText();  //载入地图
    divideTheMap();                 //把地图化分成一个一个连通块,并且对己方地图可见化
    refineShore();
}

void Map::ResetMapType(int blockL, int blockU)
{
    int i = blockL,j = blockU;
    Block& block = cell[i][j];
    this->cell[i][j].setMapType(MAPTYPE_FLAT);
}

double Map::tranL(double BlockL)
{
    double L;
    L = BlockL * BLOCKSIDELENGTH;
    return L;
}

double Map::tranU(double BlockU)
{
    double U;
    U = BlockU * BLOCKSIDELENGTH;
    return U;
}


void Map::clearfindPathMapTemperature()
{
    for(auto&v0:findPathMapTemperature){
        for(auto&v1:v0){
            fill(v1.begin(),v1.end(),0);
        }
    }
}

void Map::clearBarrierMap(){
    for(auto&v:barrierMap){
        fill(v.begin(),v.end(),0);
    }
}

// 应用敌人状态到MainWidget
void Map::applyEnemyStatusToMainWidget(MainWidget* mainWidget) {
    if (!mainWidget) return;
    
    int appliedCount = 0;
    // 遍历所有敌方单位，根据globalNum匹配并应用状态
    for (Human* human : player[1]->human) {
        int globalNum = human->getglobalNum();
        auto it = enemyStatusMap.find(globalNum);
        if (it != enemyStatusMap.end()) {
            mainWidget->setEnemyStatus(human, it->second);
            appliedCount++;
            
            // 调试信息
            QString debugInfo = QString("应用敌人状态: GlobalNum=%1, Status=%2")
                .arg(globalNum).arg(QString::fromStdString(it->second));
            qDebug() << debugInfo;
        }
    }
    
    // 输出总结信息
    if (appliedCount > 0) {
        QString summaryInfo = QString("总共应用了 %1 个敌人状态").arg(appliedCount);
        qDebug() << summaryInfo;
    }
}
