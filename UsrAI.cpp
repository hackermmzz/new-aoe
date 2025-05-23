#include "UsrAI.h"
#include <iostream>
tagGame tagUsrGame;
ins UsrIns;
/*##########DO NOT MODIFY THE CODE ABOVE##########*/

// 全局变量定义
tagInfo info;
int building_num[20]; // 包括未完工的建筑
std::vector<int> building_sn[20]; // 只记录已完工的建筑
std::vector<int> idle_human_sn;
std::vector<int> human_sn;
std::vector<int> idle_ship_sn;
std::vector<int> ship_sn;
std::vector<int> idle_settler_sn;
std::vector<tagHuman> settler;
char mmap[MAP_L][MAP_U]; //-:空地 h:建筑 w:资源 i:人 o:海洋
int land_mask[MAP_L][MAP_U]; // 陆地标记：0-未标记/水域，1-中心陆地，2-非中心陆地
bool center_land_initialized = false; // 标记是否已经初始化过中心陆地
const int dx[4] = { 0, 0, -1, 1 };
const int dy[4] = { -1, 1, 0, 0 };
int center_x, center_y;
int obj_x, obj_y;
map<int, int> building_size{
    {BUILDING_HOME,2},
    {BUILDING_CENTER,4},
    {BUILDING_GRANARY,3},
    {BUILDING_MARKET,3},
    {BUILDING_STOCK,3},
    {BUILDING_DOCK,2},
};

inline int tran(int x) {
    return (double)x * BLOCKSIDELENGTH + BLOCKSIDELENGTH / 2;
}

inline int tran(double x) {
    return (int)(x / BLOCKSIDELENGTH);
}

// 找到离(x,y)最近的陆地，并返回其对岸的坐标
void find_opposite_land(int x, int y, int* x_out, int* y_out) {
    // 确定起点所在的陆地类型
    int current_land_type = 0;
    if (x >= 0 && x < MAP_L && y >= 0 && y < MAP_U) {
        current_land_type = land_mask[x][y];
    }

    // 目标陆地类型（寻找对岸）
    int target_land_type = 0;

    // 如果起点在中心陆地上，寻找非中心陆地
    if (current_land_type == 1) {
        target_land_type = 2;
    }
    // 如果起点在非中心陆地上或水域中，寻找中心陆地
    else {
        target_land_type = (current_land_type == 2) ? 1 : 0;

        // 如果在水域中且不知道要寻找哪种陆地，先找到最近的任何陆地
        if (current_land_type == 0 && target_land_type == 0) {
            // 使用BFS找到最近的任何陆地
            queue<pair<int, int>> q;
            bool visited[MAP_L][MAP_U];
            memset(visited, false, sizeof(visited));

            q.push(make_pair(x, y));
            visited[x][y] = true;

            bool found_land = false;
            while (!q.empty() && !found_land) {
                pair<int, int> curr = q.front();
                q.pop();

                int curr_x = curr.first;
                int curr_y = curr.second;

                // 如果当前位置是陆地，确定它的类型
                if (land_mask[curr_x][curr_y] > 0) {
                    current_land_type = land_mask[curr_x][curr_y];
                    // 确定目标陆地类型（寻找对岸）
                    target_land_type = (current_land_type == 1) ? 2 : 1;
                    found_land = true;
                    break;
                }

                // 向四个方向扩展
                for (int dir = 0; dir < 4; dir++) {
                    int nx = curr_x + dx[dir];
                    int ny = curr_y + dy[dir];

                    // 检查边界和是否已访问
                    if (nx >= 0 && nx < MAP_L && ny >= 0 && ny < MAP_U && !visited[nx][ny]) {
                        q.push(make_pair(nx, ny));
                        visited[nx][ny] = true;
                    }
                }
            }

            // 如果没找到任何陆地，返回原位置
            if (!found_land) {
                *x_out = x;
                *y_out = y;
                return;
            }
        }
    }

    // 使用BFS寻找目标类型的陆地
    queue<pair<int, int>> q;
    bool visited[MAP_L][MAP_U];
    memset(visited, false, sizeof(visited));

    q.push(make_pair(x, y));
    visited[x][y] = true;

    int target_x = -1, target_y = -1;
    int min_distance = INT_MAX;

    while (!q.empty()) {
        pair<int, int> curr = q.front();
        q.pop();

        int curr_x = curr.first;
        int curr_y = curr.second;

        // 向四个方向扩展
        for (int dir = 0; dir < 4; dir++) {
            int nx = curr_x + dx[dir];
            int ny = curr_y + dy[dir];

            // 检查边界和是否已访问
            if (nx >= 0 && nx < MAP_L && ny >= 0 && ny < MAP_U && !visited[nx][ny]) {
                visited[nx][ny] = true;

                // 如果找到目标类型的陆地
                if (land_mask[nx][ny] == target_land_type) {
                    int distance = abs(nx - x) + abs(ny - y);
                    if (distance < min_distance) {
                        target_x = nx;
                        target_y = ny;
                        min_distance = distance;
                    }
                }

                // 继续搜索
                q.push(make_pair(nx, ny));
            }
        }
    }

    // 如果找到目标陆地，返回其坐标
    if (target_x != -1) {
        *x_out = target_x;
        *y_out = target_y;
    }
    else {
        // 如果没找到，返回原位置
        *x_out = x;
        *y_out = y;
    }
}

// 判断(x,y)是否在市镇中心所在的陆地上
bool is_on_center_land(int x, int y) {
    // 检查边界
    if (x < 0 || x >= MAP_L || y < 0 || y >= MAP_U) {
        return false;
    }

    // 检查是否是中心陆地（值为1）
    return land_mask[x][y] == 1;
}

// 判断(x,y)是否在非中心陆地上
bool is_on_other_land(int x, int y) {
    // 检查边界
    if (x < 0 || x >= MAP_L || y < 0 || y >= MAP_U) {
        return false;
    }

    // 检查是否是非中心陆地（值为2）
    return land_mask[x][y] == 2;
}

void updateMap(int type, int sx, int sy) {
    int size = building_size[type];
    for (int i = sx;i < sx + size;i++) {
        for (int j = sy;j < sy + size;j++) {
            mmap[i][j] = 'h';
        }
    }
}

// 标记中心陆地和非中心陆地（只执行一次）
void updateCenterLandMask() {
    if (center_land_initialized) {
        return; // 如果已经初始化过，直接返回
    }

    // 只有当中心坐标有效时才进行初始化
    if (center_x >= 0 && center_y >= 0) {
        // 重置陆地标记
        memset(land_mask, 0, sizeof(land_mask));

        // 第一步：使用洪水填充算法标记所有与市镇中心相连的陆地为1
        queue<pair<int, int>> q;
        q.push(make_pair(center_x, center_y));
        land_mask[center_x][center_y] = 1;

        while (!q.empty()) {
            pair<int, int> curr = q.front();
            q.pop();

            int curr_x = curr.first;
            int curr_y = curr.second;

            // 向四个方向扩展
            for (int dir = 0; dir < 4; dir++) {
                int nx = curr_x + dx[dir];
                int ny = curr_y + dy[dir];

                // 检查边界和是否是陆地且未标记
                if (nx >= 0 && nx < MAP_L && ny >= 0 && ny < MAP_U && land_mask[nx][ny] == 0) {
                    // 陆地包括空地、建筑、人和资源
                    if (mmap[nx][ny] == '-' || mmap[nx][ny] == 'h' || mmap[nx][ny] == 'i' || mmap[nx][ny] == 'w') {
                        q.push(make_pair(nx, ny));
                        land_mask[nx][ny] = 1; // 中心陆地标记为1
                    }
                }
            }
        }

        // 第二步：直接遍历地图，把所有其他陆地标记为2
        for (int i = 0; i < MAP_L; i++) {
            for (int j = 0; j < MAP_U; j++) {
                // 如果是未标记的陆地，直接标记为非中心陆地
                if (land_mask[i][j] == 0 &&
                    (mmap[i][j] == '-' || mmap[i][j] == 'h' || mmap[i][j] == 'i' || mmap[i][j] == 'w')) {
                    land_mask[i][j] = 2; // 非中心陆地标记为2
                }
            }
        }

        center_land_initialized = true; // 标记为已初始化

    }
}

// 更新游戏信息
void updateInfo() {
    // 初始化
    memset(building_num, 0, sizeof(building_num));
    for (int i = 0; i < 20; i++) {
        building_sn[i].clear();
    }
    // 更新地图信息
    for (int i = 0; i < MAP_L; i++) {
        for (int j = 0; j < MAP_U; j++) {
            if (info.theMap[i][j].type == MAPPATTERN_OCEAN) {
                mmap[i][j] = 'o';
            }
            else {
                mmap[i][j] = '-';
            }
        }
    }

    // 更新建筑信息
    for (auto& building : info.buildings) {
        building_num[building.Type]++;
        updateMap(building.Type, building.BlockDR, building.BlockUR);
        if (building.Percent == 100) {
            building_sn[building.Type].push_back(building.SN);
        }
        if (building.Type == BUILDING_CENTER) {
            center_x = building.BlockDR;
            center_y = building.BlockUR;
        }
    }
    // 更新中心陆地
    if (!center_land_initialized) {
        updateCenterLandMask();
    }
    // 更新村民信息
    idle_human_sn.clear();
    idle_ship_sn.clear();
    idle_settler_sn.clear();
    human_sn.clear();
    ship_sn.clear();
    settler.clear();
    for (auto& human : info.farmers) {
        mmap[human.BlockDR][human.BlockUR] = 'i';
        if (human.FarmerSort == 1) { //1表示运输船
            ship_sn.push_back(human.SN);
            continue;
        }
        human_sn.push_back(human.SN);
        if (!is_on_center_land(human.BlockDR, human.BlockUR)) {
            // cout<<"农民"<<human.SN<<"开始殖民"<<endl;
            settler.push_back(human);
        }
        if (human.NowState == HUMAN_STATE_IDLE && human.FarmerSort == 0) { //0表示普通农民
            if (is_on_center_land(human.BlockDR, human.BlockUR)) {
                idle_human_sn.push_back(human.SN);
            }
            else {
                idle_settler_sn.push_back(human.SN);
            }
        }

    }
    // 更新资源信息
    for (auto& resource : info.resources) {
        mmap[resource.BlockDR][resource.BlockUR] = 'w';
        if (resource.Type == RESOURCE_GOLD) {
            obj_x = resource.BlockDR;
            obj_y = resource.BlockUR;
        }
    }
}

// 以sx，sy为中心向外搜索面积为size*size的空地，x,y返回空地左下角坐标
int getEmptyBlock(int sx, int sy, int* x, int* y, int type) {
    // 获取建筑大小
    if (building_size.find(type) == building_size.end()) {
        return -1; // 不支持的建筑类型
    }
    int size = building_size[type];
    bool need_ocean = (type == BUILDING_DOCK);

    int buffer = 4; // 缓冲区大小，对所有建筑都适用
    int searchSize = size + buffer; // 建筑大小加缓冲区

    // 螺旋式向外搜索，最大搜索半径为maxRadius
    int maxRadius = 40;
    for (int radius = 0; radius <= maxRadius; radius++) {
        // 遍历当前半径的正方形圈
        for (int offsetX = -radius; offsetX <= radius; offsetX++) {
            for (int offsetY = -radius; offsetY <= radius; offsetY++) {
                // 仅检查当前圈的边缘
                if (radius > 0 && abs(offsetX) < radius && abs(offsetY) < radius) {
                    continue;
                }

                int startX = sx + offsetX;
                int startY = sy + offsetY;

                // 检查边界
                if (startX < 0 || startX + searchSize > MAP_L || startY < 0 || startY + searchSize > MAP_U) {
                    continue;
                }

                bool isAvailable = true;

                // 计算建筑本体的起始坐标（加上缓冲区的一半）
                int buildingStartX = startX + buffer / 2;
                int buildingStartY = startY + buffer / 2;

                if (need_ocean) {
                    // 对于码头：首先检查建筑本体是否全部在海洋中
                    for (int i = 0; i < size; i++) {
                        for (int j = 0; j < size; j++) {
                            int checkX = buildingStartX + i;
                            int checkY = buildingStartY + j;
                            // 检查是否是海洋
                            if (mmap[checkX][checkY] != 'o') {
                                isAvailable = false;
                                break;
                            }
                        }
                        if (!isAvailable) break;
                    }

                    // 如果建筑本体在海洋中，检查是否至少有一格与陆地相邻
                    if (isAvailable) {
                        bool adjacent_to_land = false;

                        // 只检查建筑本体的边界格子
                        for (int i = 0; i < size && !adjacent_to_land; i++) {
                            for (int j = 0; j < size && !adjacent_to_land; j++) {
                                // 只检查边界格子
                                if (i > 0 && i < size - 1 && j > 0 && j < size - 1) {
                                    continue; // 跳过内部格子
                                }

                                int bx = buildingStartX + i;
                                int by = buildingStartY + j;

                                // 检查当前格子的四个方向
                                for (int dir = 0; dir < 4; dir++) {
                                    int ni = bx + dx[dir];
                                    int nj = by + dy[dir];

                                    if (ni >= 0 && ni < MAP_L && nj >= 0 && nj < MAP_U) {
                                        // 检查是否是可用的陆地
                                        if (mmap[ni][nj] == '-') {
                                            adjacent_to_land = true;
                                            break;
                                        }
                                    }
                                }
                            }
                        }

                        // 如果没有相邻的陆地，则该位置不可用
                        if (!adjacent_to_land) {
                            isAvailable = false;
                        }
                    }

                    // 对于码头，最后检查整个区域（包括缓冲区）是否没有被占用
                    if (isAvailable) {
                        for (int i = 0; i < searchSize; i++) {
                            for (int j = 0; j < searchSize; j++) {
                                int checkX = startX + i;
                                int checkY = startY + j;
                                // 检查此位置是否已被占用（除了海洋外）
                                if (mmap[checkX][checkY] != 'o' && mmap[checkX][checkY] != '-') {
                                    isAvailable = false;
                                    break;
                                }
                            }
                            if (!isAvailable) break;
                        }
                    }
                }
                else {
                    // 对于普通建筑，检查是否所有格子及缓冲区都是可用的陆地
                    for (int i = 0; i < searchSize; i++) {
                        for (int j = 0; j < searchSize; j++) {
                            int checkX = startX + i;
                            int checkY = startY + j;
                            if (info.theMap[checkX][checkY].height == -1 || mmap[checkX][checkY] != '-') {
                                isAvailable = false;
                                break;
                            }
                        }
                        if (!isAvailable) break;
                    }
                }

                if (isAvailable) {
                    // 返回建筑的实际放置位置（去掉缓冲区的一半）
                    *x = startX + buffer / 2;
                    *y = startY + buffer / 2;
                    return 1; // 返回1表示成功找到空地
                }
            }
        }
    }

    // 未找到合适的空地
    return -1;
}

void outputMap() {
    //输出高程图到文件test/hmap.txt
    FILE* f = fopen("test/hmap.txt", "w");
    if (f) {
        for (int i = 0;i < MAP_L;i++) {
            for (int j = 0;j < MAP_U;j++) {
                fprintf(f, "%d", info.theMap[i][j].height);
            }
            fprintf(f, "\n");
        }
        fclose(f);
    }

    //输出mmap到文件test/mmap.txt
    f = fopen("test/mmap.txt", "w");
    if (f) {
        for (int i = 0;i < MAP_L;i++) {
            for (int j = 0;j < MAP_U;j++) {
                fprintf(f, "%c", mmap[i][j]);
            }
            fprintf(f, "\n");
        }
        fclose(f);
    }

}

// 尝试建造建筑
bool UsrAI::tryBuildBuilding(int buildingType, int requiredWood, int maxCount, int reqiredBuilding, int is_settler) {
    if (reqiredBuilding != -1 && building_sn[reqiredBuilding].empty()) {
        return false;
    }
    if (is_settler) {
        if (building_num[buildingType] < maxCount && info.Wood >= requiredWood && !idle_settler_sn.empty()) {
            // 找出一个空闲村民
            int settlerSN = idle_settler_sn.back();
            // 寻找一块适合建造的空地
            int x, y;
            if (getEmptyBlock(settler[0].BlockDR, settler[0].BlockUR, &x, &y, buildingType) > 0) {
                HumanBuild(settlerSN, buildingType, x, y);
                idle_settler_sn.pop_back();
                updateMap(buildingType, x, y);
                return true;
            }
        }
    }
    else {
        // 检查建筑数量、木材和空闲村民
        if (building_num[buildingType] < maxCount && info.Wood >= requiredWood && !idle_human_sn.empty()) {
            // 找出一个空闲村民
            int humanSN = idle_human_sn.back();
            // 寻找一块适合建造的空地
            int x, y;
            if (getEmptyBlock(center_x, center_y, &x, &y, buildingType) > 0) {
                // 向村民发出建造指令
                HumanBuild(humanSN, buildingType, x, y);
                idle_human_sn.pop_back();
                updateMap(buildingType, x, y);
                return true;
            }
        }
    }
    return false;
}

bool near_land(int x, int y) {
    // 只检查四个方向（上、下、左、右）
    for (int dir = 0; dir < 4; dir++) {
        int nx = x + dx[dir];
        int ny = y + dy[dir];

        // 检查边界和是否是陆地
        if (nx >= 0 && nx < MAP_L && ny >= 0 && ny < MAP_U && mmap[nx][ny] == '-') {
            return true;
        }
    }
    return false;
}

// 使用BFS找到最近的符合条件的水域
void find_near_land(int x, int y, int* x_out, int* y_out) {
    queue<pair<int, int>> q;
    bool visited[MAP_L][MAP_U];
    memset(visited, false, sizeof(visited));

    // 从起点开始
    q.push(make_pair(x, y));
    visited[x][y] = true;

    while (!q.empty()) {
        pair<int, int> curr = q.front();
        q.pop();

        int curr_x = curr.first;
        int curr_y = curr.second;

        // 如果当前格子是水域，检查周围是否有陆地
        if (mmap[curr_x][curr_y] == 'o') {
            bool has_land_nearby = false;

            // 检查四个方向
            for (int dir = 0; dir < 4; dir++) {
                int nx = curr_x + dx[dir];
                int ny = curr_y + dy[dir];

                if (nx >= 0 && nx < MAP_L && ny >= 0 && ny < MAP_U) {
                    // 如果旁边有陆地，标记找到了
                    if (mmap[nx][ny] == '-') {
                        has_land_nearby = true;
                        break;
                    }
                }
            }

            // 如果这个水域旁边有陆地，返回该位置
            if (has_land_nearby) {
                *x_out = curr_x;
                *y_out = curr_y;
                return;
            }
        }

        // 向四个方向扩展
        for (int dir = 0; dir < 4; dir++) {
            int nx = curr_x + dx[dir];
            int ny = curr_y + dy[dir];

            // 检查边界和是否已访问
            if (nx >= 0 && nx < MAP_L && ny >= 0 && ny < MAP_U && !visited[nx][ny]) {
                q.push(make_pair(nx, ny));
                visited[nx][ny] = true;
            }
        }
    }

    // 如果没找到，返回原位置
    *x_out = x;
    *y_out = y;
}



void UsrAI::processData()
{
  //  return;
    info = getInfo();
    // 获取地图信息
    if (info.GameFrame % 5 != 0) {
        return;
    }
    if(g_frame==15)qDebug()<<"1";
    updateInfo();
    // outputMap();

    // 按优先级尝试建造各种建筑
    tryBuildBuilding(BUILDING_HOME, BUILD_HOUSE_WOOD, 5, -1);
    tryBuildBuilding(BUILDING_GRANARY, BUILD_GRANARY_WOOD, 1, -1);
    tryBuildBuilding(BUILDING_MARKET, BUILD_MARKET_WOOD, 1, BUILDING_GRANARY);
    tryBuildBuilding(BUILDING_STOCK, BUILD_STOCK_WOOD, 1, -1); //对岸已经有一个仓库了
    tryBuildBuilding(BUILDING_DOCK, BUILD_DOCK_WOOD, 2, -1); // 尝试建造码头

    //处理殖民行为
    if (settler.size() > 0) {
        //建造仓库
        tryBuildBuilding(BUILDING_STOCK, BUILD_STOCK_WOOD, 2, -1, 1);
        // 寻找最近的金矿
        int min_distance = INT_MAX;
        int target_gold_sn = -1;
        if (building_sn[BUILDING_STOCK].size() >= 2) {
            for (auto& res : info.resources) {
                if (res.Type == RESOURCE_GOLD) {
                    // 计算距离
                    int distance = abs(settler[0].BlockDR - res.BlockDR) +
                        abs(settler[0].BlockUR - res.BlockUR);

                    if (distance < min_distance) {
                        min_distance = distance;
                        target_gold_sn = res.SN;
                    }
                }
            }

            // 如果找到金矿，派遣殖民者去采集
            if (target_gold_sn != -1) {
                while (idle_settler_sn.size() > 0) {
                    HumanAction(idle_settler_sn.back(), target_gold_sn);
                    idle_settler_sn.pop_back();
                }
            }
        }
    }

    // 处理建筑行为
    for (auto& building : info.buildings) {
        if (building.Type == BUILDING_CENTER) {
            if (building.Project == 0 && human_sn.size() < 6 && info.Meat >= BUILDING_CENTER_CREATEFARMER_FOOD) {
                BuildingAction(building.SN, BUILDING_CENTER_CREATEFARMER);
            }
        }
        if (building.Type == BUILDING_DOCK) {
            if (building.Project == 0 && ship_sn.size() < 1 && info.Wood >= BUILDING_DOCK_CREATE_WOOD_BOAT_WOOD) {
                BuildingAction(building.SN, BUILDING_DOCK_CREATE_WOOD_BOAT);
            }
        }
    }
    // cout<<"村民数量"<<info.farmers.size()<<endl;
    // cout<<"村民状态"<<info.farmers[0].NowState<<endl;

    for (auto& ship : info.farmers) {
        if (ship.FarmerSort == 1 && ship.NowState == HUMAN_STATE_IDLE) {

            if (near_land(ship.BlockDR, ship.BlockUR) && ship.Resource < 5 && idle_human_sn.size()>0) {
                // 如果船靠岸，并且载人小于5，则让村民上船
                HumanAction(idle_human_sn.back(), ship.SN);
                idle_human_sn.pop_back();
            }
            else if (ship.Resource == 5) {
                // 如果船载满5人，则让船去目的地
                int x, y;
                find_opposite_land(ship.BlockDR, ship.BlockUR, &x, &y);  // 找到对岸
                HumanMove(ship.SN, tran(x), tran(y));
            }
            else {
                // 如果船载人小于5，并且不在岸上，则让船靠岸
                if (!near_land(ship.BlockDR, ship.BlockUR)) {
                    int x, y;
                    find_near_land(center_x, center_y, &x, &y);
                    HumanMove(ship.SN, tran(x), tran(y));
                }
            }
        }
    }
}
