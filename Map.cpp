#include "Map.h"
///////////////////////////
Map*GlobalMap;
///////////////////////////
Map::Map()
{
    GlobalMap=this;
    for(int i = 0; i < MAP_L; i++)
    {
        cell[i] = new Block[MAP_U];
    }
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

/*
 * 函数：Map::generateResources；
 * 参数：无；
 * 内容：随机在地图上生成成片的资源；
 * 返回值：空。
 */
void Map::generateResources() {
    srand(time(NULL));
    // flag用于表示是否已经生成，若已生成则flag为1，否则为0
    int flag = 0;

    // 在45 * 45边缘生成两片森林
    for(int T = 0; T < ForestMAX; T ++) {
        flag = 0;
        int beginL = 0, beginU = 0;
        if(T) beginL = MAP_L / 2 - 25, beginU = MAP_U / 2 - 25;
        else beginL = MAP_L / 2 + 5, beginU = MAP_U / 2 + 5;

        // 初始化即将生成的森林
        int forestCell[FOREST_GENERATE_L][FOREST_GENERATE_U];

        // 树木数量计数
        int forestTreeCount = 0;

        // 如果树木数量到达目标值，则产生；否则重新生成
        while(forestTreeCount < FOREST_COUNT_MIN) {
            // 重置森林数组
            memset(forestCell, 0, sizeof(forestCell));
            forestTreeCount = 0;
            // 随机树木生成
            for(int fi = 0; fi < FOREST_GENERATE_L; fi ++)
                for(int fj = 0; fj < FOREST_GENERATE_U; fj ++)
                    if(rand() % 100 < FOREST_GENERATE_PERCENT) {
                        forestCell[fi][fj] = 1;
                        forestTreeCount ++;
                    }
            // 细胞自动机优化
            for(int T = 0; T < FOREST_GENERATE_OPTCOUNTER; T ++) {
                for(int fi = 0; fi < FOREST_GENERATE_L; fi ++) {
                    for(int fj = 0; fj < FOREST_GENERATE_U; fj ++) {
                        int forestCount = CheckNeighborForest(fi, fj, forestCell);
                        if(forestCell[fi][fj] == 1) {
                            if(forestCount < 4) {
                                forestCell[fi][fj] = 0;
                                forestTreeCount --;
                            }
                        }
                        else if(forestCount >= 5) {
                            forestCell[fi][fj] = 1;
                            forestTreeCount ++;
                        }
                    }
                }
            }
        }
        // 将生成的数组放置到对应地图坐标
        for(int i = beginL; i < beginL + FOREST_GENERATE_L; i ++) {
            for(int j = beginU; j < beginU + FOREST_GENERATE_U; j ++) {
                if(forestCell[i - beginL][j - beginU] == 1) {
                    mapFlag[i][j] = true;
                    Gamemap[i][j] = 11;
                }
            }
        }
    }

    // 在21 * 21边缘生成一堆浆果
    flag = 0;
    for (int i = MAP_L / 2 - 10; i <= MAP_L / 2 + 10; i++)
    {
        for (int j = MAP_U / 2 - 10; j <= MAP_U / 2 + 10; j++)
        {
            if(flag == 1) break;
            //刨除原本的部分
            if(i >= MAP_L / 2 - 9 && i <= MAP_L / 2 + 9 && j >= MAP_U / 2 - 9 && j <= MAP_U / 2 + 9) continue;

            if(rand() % 200 == 0)
            {
                bool BushJudge = true;
                for(int I = i - 1; I < i + 6; I++)
                {
                    for(int J = j - 1; J < j + 6; J++)
                    {
                        if(mapFlag[I][J] == true)
                        {
                            BushJudge = false;
                            break;
                        }
                    }
                    if(BushJudge == false) break;
                }

                if(BushJudge == true)
                {
                    int ti = 0, ResFlag = rand() % 5;
                    for(int I = i; I < i + 5; I++)
                    {
                        int tj = 0;
                        for(int J = j; J < j + 5; J++)
                        {
                            mapFlag[I][J] = true;
                            if(Food[ResFlag][ti][tj] == 1) Gamemap[I][J] = 2;
                            tj++;
                        }
                        ti++;
                    }
                    flag = 1;
                }
            }
            if(flag == 1) break;
        }
        if(flag == 1) break;
        if(flag == 0 && i == MAP_L / 2 + 10) i = MAP_L / 2 - 10;
    }

    // 在39 * 39边缘生成一堆石头
    flag = 0;
    for (int i = MAP_L / 2 - 19; i <= MAP_L / 2 + 19; i++)
    {
        for (int j = MAP_U / 2 - 19; j <= MAP_U / 2 + 19; j++)
        {
            if(flag == 1) break;
            //刨除原本的部分
            if(i >= MAP_L / 2 - 18 && i <= MAP_L / 2 + 18 && j >= MAP_U / 2 - 18 && j <= MAP_U / 2 + 18) continue;

            if(rand() % 500 == 0)
            {
                bool StoneJudge = true;
                for(int I = i - 1; I < i + 6; I++)
                {
                    for(int J = j - 1; J < j + 6; J++)
                    {
                        if(mapFlag[I][J] == true)
                        {
                            StoneJudge = false;
                            break;
                        }
                    }
                    if(StoneJudge == false) break;
                }

                if(StoneJudge == true)
                {
                    int ti = 0, ResFlag = rand() % 5;
                    for(int I = i; I < i + 5; I++)
                    {
                        int tj = 0;
                        for(int J = j; J < j + 5; J++)
                        {
                            mapFlag[I][J] = true;
                            if(Stone[ResFlag][ti][tj] == 1) Gamemap[I][J] = 4;
                            tj++;
                        }
                        ti++;
                    }
                    flag = 1;
                }
            }
        }
        if(flag == 1) break;
        if(flag == 0 && i == MAP_L / 2 + 19) i = MAP_L / 2 - 19;
    }

    // 在61 * 61边缘生成一堆石头
    flag = 0;
    for (int i = MAP_L / 2 - 30; i <= MAP_L / 2 + 30; i++)
    {
        for (int j = MAP_U / 2 - 30; j <= MAP_U / 2 + 30; j++)
        {
            if(flag == 1) break;
            //刨除原本的部分
            if(i >= MAP_L / 2 - 29 && i <= MAP_L / 2 + 29 && j >= MAP_U / 2 - 29 && j <= MAP_U / 2 + 29) continue;

            if(rand() % 1000 == 0)
            {
                bool StoneJudge = true;
                for(int I = i - 1; I < i + 6; I++)
                {
                    for(int J = j - 1; J < j + 6; J++)
                    {
                        if(mapFlag[I][J] == true)
                        {
                            StoneJudge = false;
                            break;
                        }
                    }
                    if(StoneJudge == false) break;
                }

                if(StoneJudge == true)
                {
                    int ti = 0, ResFlag = rand() % 5;
                    for(int I = i; I < i + 5; I++)
                    {
                        int tj = 0;
                        for(int J = j; J < j + 5; J++)
                        {
                            mapFlag[I][J] = true;
                            if(Stone[ResFlag][ti][tj] == 1) Gamemap[I][J] = 4;
                            tj++;
                        }
                        ti++;
                    }
                    flag = 1;
                }
            }
        }
        if(flag == 1) break;
        if(flag == 0 && i == MAP_L / 2 + 30) i = MAP_L / 2 - 30;
    }

    // 在27 * 27边缘生成一堆瞪铃
    flag = 0;
    for (int i = MAP_L / 2 - 13; i <= MAP_L / 2 + 13; i++)
    {
        for (int j = MAP_U / 2 - 13; j <= MAP_U / 2 + 13; j++)
        {
            if(flag == 1) break;
            //刨除原本的部分
            if(i >= MAP_L / 2 - 12 && i <= MAP_L / 2 + 12 && j >= MAP_U / 2 - 12 && j <= MAP_U / 2 + 12) continue;

            if(rand() % 400 == 0)
            {
                bool GazelleJudge = true;
                for(int I = i - 1; I < i + 6; I++)
                {
                    for(int J = j - 1; J < j + 6; J++)
                    {
                        if(mapFlag[I][J] == true)
                        {
                            GazelleJudge = false;
                            break;
                        }
                    }
                    if(GazelleJudge == false) break;
                }

                if(GazelleJudge == true)
                {
                    int ti = 0, ResFlag = rand() % 5;
                    for(int I = i; I < i + 5; I++)
                    {
                        int tj = 0;
                        for(int J = j; J < j + 5; J++)
                        {
                            mapFlag[I][J] = true;
                            if(Food[ResFlag][ti][tj] == 1) Gamemap[I][J] = 3;
                            tj++;
                        }
                        ti++;
                    }
                    flag = 1;
                }
            }
        }
        if(flag == 1) break;
        if(flag == 0 && i == MAP_L / 2 + 13) i = MAP_L / 2 - 13;
    }

}

/*
 * 函数：Map::generateResource；
 * 参数：无；
 * 内容：随机在地图上生成单独的资源；
 * 返回值：空。
 */
void Map::generateResource() {
    srand(time(NULL));

    int CountTree = 0;
    int flag = 0;

    // 在主营范围外随机生成单棵树
    for(int i = 0; i < MAP_L; i++)
    {
        for(int j = 0; j < MAP_U; j++)
        {
            if(i >= MAP_L / 2 - 13 && i <= MAP_L / 2 + 13 && j >= MAP_U / 2 - 13 && j <= MAP_U / 2 + 13)
            {
                continue;
            }
            if(rand() % 1260 == 0 && mapFlag[i][j] == false && mapFlag[i - 1][j] == false && mapFlag[i + 1][j] == false && mapFlag[i][j + 1] == false && mapFlag[i - 1][j - 1] == false && mapFlag[i][j - 1] == false && mapFlag[i + 1][j - 1] == false && mapFlag[i - 1][j + 1] == false && mapFlag[i + 1][j + 1] == false)
            {
                //Tree
                Gamemap[i][j] = 1;
                mapFlag[i][j] = true;
                CountTree ++;
            }
        }
        if(CountTree >= TREEMAX) break;
        if(CountTree < TREEMAX && i == MAP_L - 1) i = 0;
    }

    // 在61 * 61范围内生成4只狮子
    flag = 0;
    while(flag < 4)
    {
        int i = 0, j = 0;
        switch(flag)
        {
        case 0:
            for(int i = MAP_L / 2 - 26; i <= MAP_L / 2 + 26; i++)
            {
                j = MAP_U / 2 - 26;
                if(flag == 1) break;
                if(rand() % 200 == 0 && mapFlag[i][j] == false && mapFlag[i - 1][j] == false && mapFlag[i + 1][j] == false && mapFlag[i][j + 1] == false && mapFlag[i - 1][j - 1] == false && mapFlag[i][j - 1] == false && mapFlag[i + 1][j - 1] == false && mapFlag[i - 1][j + 1] == false && mapFlag[i + 1][j + 1] == false)
                {
                    Gamemap[i][j] = 6;
                    mapFlag[i][j] = true;
                    flag ++;
                    break;
                }
            }
            break;
        case 1:
            for(int i = MAP_L / 2 - 26; i <= MAP_L / 2 + 26; i++)
            {
                j = MAP_U / 2 + 26;
                if(flag == 2) break;
                if(rand() % 200 == 0 && mapFlag[i][j] == false && mapFlag[i - 1][j] == false && mapFlag[i + 1][j] == false && mapFlag[i][j + 1] == false && mapFlag[i - 1][j - 1] == false && mapFlag[i][j - 1] == false && mapFlag[i + 1][j - 1] == false && mapFlag[i - 1][j + 1] == false && mapFlag[i + 1][j + 1] == false)
                {
                    Gamemap[i][j] = 6;
                    mapFlag[i][j] = true;
                    flag ++;
                    break;
                }
            }
            break;
        case 2:
            for(int j = MAP_U / 2 - 26; j <= MAP_U / 2 + 26; j++)
            {
                i = MAP_L / 2 - 26;
                if(flag == 3) break;
                if(rand() % 200 == 0 && mapFlag[i][j] == false && mapFlag[i - 1][j] == false && mapFlag[i + 1][j] == false && mapFlag[i][j + 1] == false && mapFlag[i - 1][j - 1] == false && mapFlag[i][j - 1] == false && mapFlag[i + 1][j - 1] == false && mapFlag[i - 1][j + 1] == false && mapFlag[i + 1][j + 1] == false)
                {
                    Gamemap[i][j] = 6;
                    mapFlag[i][j] = true;
                    flag ++;
                    break;
                }
            }
            break;
        case 3:
            for(int j = MAP_U / 2 - 26; j <= MAP_U / 2 + 26; j++)
            {
                i = MAP_L / 2 + 26;
                if(flag == 4) break;
                if(rand() % 200 == 0 && mapFlag[i][j] == false && mapFlag[i - 1][j] == false && mapFlag[i + 1][j] == false && mapFlag[i][j + 1] == false && mapFlag[i - 1][j - 1] == false && mapFlag[i][j - 1] == false && mapFlag[i + 1][j - 1] == false && mapFlag[i - 1][j + 1] == false && mapFlag[i + 1][j + 1] == false)
                {
                    Gamemap[i][j] = 6;
                    mapFlag[i][j] = true;
                    flag ++;
                    break;
                }
            }
            break;
        default:
            break;
        }
    }

    //test

    // 在51 * 51范围内生成一堆象
    flag = 0;
    int tmpL[3] = {-2, 0, 2}, tmpU[2] = {-2, 2};
    for(int i = MAP_L / 2 - 25; i <= MAP_L / 2 + 25; i++)
    {
        for(int j = MAP_U / 2 - 25; j <= MAP_U / 2 + 25; j++)
        {
            if(flag == 1) break;
            if(i >= MAP_L / 2 - 23 && i <= MAP_L / 2 + 23 && j >= MAP_U / 2 - 23 && j <= MAP_U / 2 + 23) continue;
            if(rand() % 500 == 0 && mapFlag[i][j] == false && mapFlag[i - 1][j] == false && mapFlag[i + 1][j] == false && mapFlag[i][j + 1] == false && mapFlag[i - 1][j - 1] == false && mapFlag[i][j - 1] == false && mapFlag[i + 1][j - 1] == false && mapFlag[i - 1][j + 1] == false && mapFlag[i + 1][j + 1] == false && mapFlag[i - 2][j - 2] == false&& mapFlag[i - 2][j - 1] == false && mapFlag[i - 2][j] == false && mapFlag[i - 2][j + 1] == false && mapFlag[i - 2][j + 2] == false && mapFlag[i + 2][j - 2] == false&& mapFlag[i + 2][j - 1] == false && mapFlag[i + 2][j] == false && mapFlag[i + 2][j + 1] == false && mapFlag[i + 2][j + 2] == false && mapFlag[i - 1][j + 2] == false && mapFlag[i - 1][j - 2] == false && mapFlag[i + 1][j + 2] == false && mapFlag[i + 1][j - 2] == false && mapFlag[i][j + 2] == false && mapFlag[i][j - 2] == false)
            {
                int I = i + tmpL[rand() % 3], J = j + tmpU[rand() % 2];
                Gamemap[i][j] = 7;
                EL=i;
                EU=j;
                Gamemap[I][J] = 7;
                mapFlag[i][j] = true;
                mapFlag[I][J] = true;
                flag = 1;
            }
        }
        if(flag == 1) break;
        if(flag == 0 && i == MAP_L / 2 + 25) i = MAP_L / 2 - 25;
    }

    // 在61 * 61范围内生成三个废墟用于标出箭塔的建设地点
    flag = 0;
    for(int i = MAP_L / 2 - 30; i <= MAP_L / 2 + 30; i++)
    {
        int j = MAP_U / 2 - 30 + rand() % 6;
        if(rand() % 500 == 0 && mapFlag[i][j] == false && mapFlag[i - 1][j] == false && mapFlag[i + 1][j] == false && mapFlag[i][j + 1] == false && mapFlag[i - 1][j - 1] == false && mapFlag[i][j - 1] == false && mapFlag[i + 1][j - 1] == false && mapFlag[i - 1][j + 1] == false && mapFlag[i + 1][j + 1] == false && mapFlag[i - 2][j - 2] == false&& mapFlag[i - 2][j - 1] == false && mapFlag[i - 2][j] == false && mapFlag[i - 2][j + 1] == false && mapFlag[i - 2][j + 2] == false && mapFlag[i + 2][j - 2] == false&& mapFlag[i + 2][j - 1] == false && mapFlag[i + 2][j] == false && mapFlag[i + 2][j + 1] == false && mapFlag[i + 2][j + 2] == false && mapFlag[i - 1][j + 2] == false && mapFlag[i - 1][j - 2] == false && mapFlag[i + 1][j + 2] == false && mapFlag[i + 1][j - 2] == false && mapFlag[i][j + 2] == false && mapFlag[i][j - 2] == false)
        {
            Gamemap[i][j] = 10;
            mapFlag[i][j] = true;
            flag = 1;
        }
        if(flag == 1) break;
        if(flag == 0 && i == MAP_L / 2 + 30) i = MAP_L / 2 - 30;
    }

    flag = 0;
    for(int i = MAP_L / 2 - 30; i <= MAP_L / 2 + 30; i++)
    {
        int j = MAP_U / 2 + 30 - rand() % 6;
        if(rand() % 500 == 0 && mapFlag[i][j] == false && mapFlag[i - 1][j] == false && mapFlag[i + 1][j] == false && mapFlag[i][j + 1] == false && mapFlag[i - 1][j - 1] == false && mapFlag[i][j - 1] == false && mapFlag[i + 1][j - 1] == false && mapFlag[i - 1][j + 1] == false && mapFlag[i + 1][j + 1] == false && mapFlag[i - 2][j - 2] == false&& mapFlag[i - 2][j - 1] == false && mapFlag[i - 2][j] == false && mapFlag[i - 2][j + 1] == false && mapFlag[i - 2][j + 2] == false && mapFlag[i + 2][j - 2] == false&& mapFlag[i + 2][j - 1] == false && mapFlag[i + 2][j] == false && mapFlag[i + 2][j + 1] == false && mapFlag[i + 2][j + 2] == false && mapFlag[i - 1][j + 2] == false && mapFlag[i - 1][j - 2] == false && mapFlag[i + 1][j + 2] == false && mapFlag[i + 1][j - 2] == false && mapFlag[i][j + 2] == false && mapFlag[i][j - 2] == false)
        {
            Gamemap[i][j] = 10;
            mapFlag[i][j] = true;
            flag = 1;
        }
        if(flag == 1) break;
        if(flag == 0 && i == MAP_L / 2 + 30) i = MAP_L / 2 - 30;
    }

    flag = 0;
    int i = MAP_U / 2;
    if(rand() % 2 == 1)
    {
        i += 30 - rand() % 6;
    }
    else
    {
        i -= 30 - rand() % 6;
    }
    for(int j = MAP_L / 2 - 30; j <= MAP_L / 2 + 30; j++)
    {
        if(rand() % 500 == 0 && mapFlag[i][j] == false && mapFlag[i - 1][j] == false && mapFlag[i + 1][j] == false && mapFlag[i][j + 1] == false && mapFlag[i - 1][j - 1] == false && mapFlag[i][j - 1] == false && mapFlag[i + 1][j - 1] == false && mapFlag[i - 1][j + 1] == false && mapFlag[i + 1][j + 1] == false && mapFlag[i - 2][j - 2] == false&& mapFlag[i - 2][j - 1] == false && mapFlag[i - 2][j] == false && mapFlag[i - 2][j + 1] == false && mapFlag[i - 2][j + 2] == false && mapFlag[i + 2][j - 2] == false&& mapFlag[i + 2][j - 1] == false && mapFlag[i + 2][j] == false && mapFlag[i + 2][j + 1] == false && mapFlag[i + 2][j + 2] == false && mapFlag[i - 1][j + 2] == false && mapFlag[i - 1][j - 2] == false && mapFlag[i + 1][j + 2] == false && mapFlag[i + 1][j - 2] == false && mapFlag[i][j + 2] == false && mapFlag[i][j - 2] == false)
        {
            Gamemap[i][j] = 10;
            mapFlag[i][j] = true;
            flag = 1;
        }
        if(flag == 1) break;
        if(flag == 0 && j == MAP_L / 2 + 30) j = MAP_L / 2 - 30;
    }

}


/*
 * 函数：Map::generateCenter
 * 参数：无；
 * 内容：在地图中央生产市镇中心及初始村民；
 * 返回值：空。
 */
void Map::generateCenter()
{
    // 生成城镇中心，以3 * 3的左上角表示城镇中心真正的放置位置
    Gamemap[MAP_L / 2 - 1][MAP_U / 2 - 1] = 9;
    player[0]->addBuilding(BUILDING_CENTER,MAP_L / 2 - 1,MAP_L / 2 - 1,100);

    //添加初始村民
    player[0]->addFarmer((MAP_L / 2 - 1.5)*BLOCKSIDELENGTH,(MAP_L / 2 + 1.5)*BLOCKSIDELENGTH);
    player[0]->addFarmer((MAP_L / 2 - 1.5)*BLOCKSIDELENGTH,(MAP_L / 2 - 1.5)*BLOCKSIDELENGTH);
    player[0]->addFarmer((MAP_L / 2 + 2.5)*BLOCKSIDELENGTH,(MAP_L / 2 - 1.5)*BLOCKSIDELENGTH);

    for(int i = MAP_L / 2 - 2; i <= MAP_L / 2 + 2; i++)
    {
        for(int j = MAP_U / 2 - 2; j <= MAP_U / 2 + 2; j++)
        {
            mapFlag[i][j] = true;
        }
    }
}



/*
 * 函数：Map::generateCenterAround
 * 参数：无；
 * 内容：在地图中央13 * 13的部分单独生成市镇中心附近的资源；
 * 返回值：空。
 */
void Map::generateCenterAround() {
    srand(time(NULL));

    // 生成城镇中心附近9 * 9部分的1棵随机位置的树
    // flag：0表示未生成，1表示已生成
    int flag = 0;
    for (int i = MAP_L / 2 - 4; i <= MAP_L / 2 + 4; i++)
    {
        for (int j = MAP_U / 2 - 4; j <= MAP_U / 2 + 4; j++)
        {
            if(flag == 1) break;
            if(rand() % 80 == 0 && mapFlag[i][j] != 1)
            {
                Gamemap[i][j] = 1;
                mapFlag[i][j] = true;
                flag = 1;
            }
        }
        if(flag == 1) break;
        if(flag == 0 && i == MAP_L / 2 + 4) i = MAP_L / 2 - 4;
    }

    // 生成城镇中心附近13 * 13部分的3棵随机位置的树
    // flag：0表示未生成，1表示已生成
    for(int T = 0; T < 3; T++)
    {
        flag = 0;
        for (int i = MAP_L / 2 - 6; i <= MAP_L / 2 + 6; i++)
        {
            for (int j = MAP_U / 2 - 6; j <= MAP_U / 2 + 6; j++)
            {
                if(flag == 1) break;
                //刨除原本的9 * 9部分
                if(i >= MAP_L / 2 - 4 && i <= MAP_L / 2 + 4 && j >= MAP_U / 2 - 4 && j <= MAP_U / 2 + 4) continue;

                if(rand() % 80 == 0 && mapFlag[i][j] != 1)
                {
                    Gamemap[i][j] = 1;
                    mapFlag[i][j] = true;
                    flag = 1;
                }
            }
            if(flag == 1) break;
            if(flag == 0 && i == MAP_L / 2 + 4) i = MAP_L / 2 - 4;
        }
    }

    return ;
}
/*
 * 函数 Map::generateEnemy;
 * 参数：无；
 * 内容：在开始时生成三组敌人；
 * 返回值：无；
 */
void Map::generateEnemy(){
    //寻找三处可以生成敌人的地方
    double pos_L[4]={0};
    double pos_U[4]={0};
    int I=0;
    int J=0;
    int num=0;
    int dir=0;
    if(EL>=MAP_L/2&&EU>=MAP_U/2) dir=1;
    else if(EL>=MAP_L/2&&EU<MAP_U/2) dir=2;
    else if(EL<MAP_L/2&&EU<MAP_U/2) dir=3;
    else if(EL<MAP_L/2&&EU>=MAP_U/2) dir=4;
    for(int i=MAP_L-10;i>MAP_L-20&&dir!=2;i--){
        for(int j=10;j<20;j++){
            if (mapFlag[i][j]==false&&mapFlag[i+1][j+1]==false&&mapFlag[i-1][j-1]==false&&mapFlag[i+1][j]==false&&mapFlag[i][j+1]==false&&mapFlag[i][j-1]==false
                    &&mapFlag[i-1][j]==false&&mapFlag[i-1][j+1]==false&&mapFlag[i+1][j-1]==false&&mapFlag[i+2][j+2]==false&&mapFlag[i-2][j-2]==false&&mapFlag[i+2][j]==false&&mapFlag[i][j+2]==false&&mapFlag[i][j-2]==false
                    &&mapFlag[i-2][j]==false&&mapFlag[i-2][j+2]==false&&mapFlag[i+2][j-2]==false&&mapFlag[i-2][j+1]==false&&mapFlag[i+2][j-1]==false&&mapFlag[i+2][j+1]==false&&mapFlag[i-2][j-1]==false&&mapFlag[i-1][j+2]==false&&mapFlag[i+1][j-2]==false&&mapFlag[i+1][j+2]==false&&mapFlag[i-1][j-2]==false)
            {pos_L[I]=i*BLOCKSIDELENGTH;
             pos_U[J]=j*BLOCKSIDELENGTH;
             num=1;
             I++;
             J++;
             break;
            }
        }if(num==1) break;
    }
    num=0;
    for(int i=10;i<20&&dir!=3;i++){
        for(int j=10;j<20;j++){
            if (mapFlag[i][j]==false&&mapFlag[i+1][j+1]==false&&mapFlag[i-1][j-1]==false&&mapFlag[i+1][j]==false&&mapFlag[i][j+1]==false&&mapFlag[i][j-1]==false
                    &&mapFlag[i-1][j]==false&&mapFlag[i-1][j+1]==false&&mapFlag[i+1][j-1]==false)
            {pos_L[I]=i*BLOCKSIDELENGTH;
             pos_U[J]=j*BLOCKSIDELENGTH;
             num=1;
             I++;
             J++;
             break;
            }
        }if(num==1) break;
    }
    num=0;
    for(int i=MAP_L-10;i>MAP_L-20&&dir!=1;i--){
        for(int j=MAP_U-10;j>MAP_U-20;j--){
            if (mapFlag[i][j]==false&&mapFlag[i+1][j+1]==false&&mapFlag[i-1][j-1]==false&&mapFlag[i+1][j]==false&&mapFlag[i][j+1]==false&&mapFlag[i][j-1]==false
                    &&mapFlag[i-1][j]==false&&mapFlag[i-1][j+1]==false&&mapFlag[i+1][j-1]==false)
            {pos_L[I]=i*BLOCKSIDELENGTH;
             pos_U[J]=j*BLOCKSIDELENGTH;
             num=1;
             I++;
             J++;
             break;
            }
        }if(num==1) break;
    }
    num=0;
    for(int i=10;i<20&&dir!=4;i++){
        for(int j=MAP_U-10;j>MAP_U-20;j--){
            if (mapFlag[i][j]==false&&mapFlag[i+1][j+1]==false&&mapFlag[i-1][j-1]==false&&mapFlag[i+1][j]==false&&mapFlag[i][j+1]==false&&mapFlag[i][j-1]==false
                    &&mapFlag[i-1][j]==false&&mapFlag[i-1][j+1]==false&&mapFlag[i+1][j-1]==false)
            {pos_L[I]=i*BLOCKSIDELENGTH;
             pos_U[J]=j*BLOCKSIDELENGTH;
             num=1;
             I++;
             J++;
             break;
            }
        }if(num==1) break;
    }
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    int rand=qrand()%6;
    int rand2=qrand()%3;
    double TL=0,TU=0;
    if(rand==1){
        TL=pos_L[1];TU=pos_U[1];
        pos_L[1]=pos_L[2];pos_U[1]=pos_U[2];
        pos_L[2]=TL;pos_U[2]=TU;
    }
    else if(rand==2){
        TL=pos_L[0];TU=pos_U[0];
        pos_L[0]=pos_L[1];pos_U[0]=pos_U[1];
        pos_L[1]=TL;pos_U[1]=TU;
    }
    else if(rand==3){
        TL=pos_L[0];TU=pos_U[0];
        pos_L[0]=pos_L[1];pos_U[0]=pos_U[1];
        pos_L[1]=TL;pos_U[1]=TU;
        TL=pos_L[1];TU=pos_U[1];
        pos_L[1]=pos_L[2];pos_U[1]=pos_U[2];
        pos_L[2]=TL;pos_U[2]=TU;
    }
    else if(rand==4){
        TL=pos_L[0];TU=pos_U[0];
        pos_L[0]=pos_L[2];pos_U[0]=pos_U[2];
        pos_L[2]=TL;pos_U[2]=TU;
    }
    else if(rand==5){
        TL=pos_L[0];TU=pos_U[0];
        pos_L[0]=pos_L[2];pos_U[0]=pos_U[2];
        pos_L[2]=TL;pos_U[2]=TU;
        TL=pos_L[1];TU=pos_U[1];
        pos_L[1]=pos_L[2];pos_U[1]=pos_U[2];
        pos_L[2]=TL;pos_U[2]=TU;
    }
    //第一组
    player[1]->addArmy(0,pos_L[0]-10,pos_U[0]-10);
    player[1]->addArmy(0,pos_L[0]+10,pos_U[0]+10);
    //第二组
    player[1]->addArmy(1,pos_L[1]-15,pos_U[1]-15);
    player[1]->addArmy(1,pos_L[1]+15,pos_U[1]+15);
    player[1]->addArmy(3,pos_L[1]-30,pos_U[1]-30);
    player[1]->addArmy(3,pos_L[1]-30,pos_U[1]+30);
    player[1]->addArmy(2,pos_L[1]+45,pos_U[1]-45);
    player[1]->addArmy(2,pos_L[1]+45,pos_U[1]+45);
    //第三组
    player[1]->addArmy(0,pos_L[2]-15,pos_U[2]-15);
    player[1]->addArmy(0,pos_L[2]+15,pos_U[2]+15);
    player[1]->addArmy(0,pos_L[2]+30,pos_U[2]-30);
    player[1]->addArmy(0,pos_L[2]-30,pos_U[2]+30);
    player[1]->addArmy(1,pos_L[2]-45,pos_U[2]+45);
    player[1]->addArmy(1,pos_L[2]+45,pos_U[2]-45);
    player[1]->addArmy(1,pos_L[2]+60,pos_U[2]-60);
    player[1]->addArmy(1,pos_L[2]-60,pos_U[2]+60);
    player[1]->addArmy(2,pos_L[2]+75,pos_U[2]+75);
    player[1]->addArmy(2,pos_L[2]-75,pos_U[2]-75);
    player[1]->addArmy(2,pos_L[2]+90,pos_U[2]-90);
    player[1]->addArmy(2,pos_L[2]+90,pos_U[2]-90);
    player[1]->addArmy(rand2+4,pos_L[2]-80,pos_U[2]+80);
}

/*
 * 函数：Map::genDesert；
 * 参数：i, j——对应xy坐标；
 *      number——最大生成半径；
 *      Map[][74]——记录生成沙漠的地块；
 * 内容：随机在地图上绘制沙漠地貌；
 * 返回值：空。
 */
void Map::genDesert(int i, int j, int number, int Map[][74]) {
    if(number > 0 && i < 73 && i >= 1 && j < 73 && j >= 1 && Map[i][j] == 0)
    {
        Map[i][j] = number;
        genDesert(i - 1, j, number - 1, Map);
        genDesert(i + 1, j, number - 1, Map);
        genDesert(i, j - 1, number - 1, Map);
        genDesert(i, j + 1, number - 1, Map);
    }
    return ;
}

/*
 * 函数：generateLandforms；
 * 参数：无；
 * 内容：绘制沙漠地貌的边缘；
 * 返回值：空。
 */
void Map::generateLandforms() {
    int Map[74][74] = {0};
    srand(time(nullptr));
    for(int i = 0; i < GENERATELANDFORMS_NUM; i++) {
        int randX = rand() % 72, randY = rand() % 72;
        genDesert(randX + 1, randY + 1, 25 + rand() % 5, Map);
    }

    // 沙地赋值为1，草地赋值为0
    for(int i = 0; i < 74; i++) {
        for(int j = 0; j < 74; j++) {
            if(i == 0 || i == 73) Map[i][j] = 1;
            else {
                if(j == 0 || j == 73) Map[i][j] = 1;
            }
            if(Map[i][j] != 0) {
                Map[i][j] = 1;
            }
        }
    }

    for (int i = 1; i < 73; i++) {
        for (int j = 1; j < 73; j++) {
            int TmpCal[4][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
            if (Map[i][j] != 0) {
                int ZeroCnt = 0;
                int FlagK = 0;
                int tmpk[4] = {0};
                for (int k = 0; k < 4; k++) {
                    // 如果沙地旁有草地，则记录草地相对沙地的位置和草地数量（0 <= cnt <= 4）
                    if (Map[i + TmpCal[k][0]][j + TmpCal[k][1]] == 0) {
                        ZeroCnt++;
                        tmpk[k] = 1;
                    }
                }
                Map[i][j] = Map[i][j] * 10 + ZeroCnt;
                FlagK = tmpk[0] * 1000 + tmpk[1] * 100 + tmpk[2] * 10 + tmpk[3];

                // 判断FlagK的值
                switch (ZeroCnt) {
                    case 1:
                        FlagK = (FlagK == 1000) ? 1 : (FlagK == 100) ? 2 : (FlagK == 10) ? 3 : (FlagK == 1) ? 4 : 0;
                        break;
                    case 2:
                        FlagK = (FlagK == 1001) ? 4 : (FlagK == 1010) ? 6 : (FlagK == 1100) ? 1 : (FlagK == 101) ? 5 : (FlagK == 110) ? 2 : (FlagK == 11) ? 3 : 0;
                        break;
                    case 3:
                        FlagK = (FlagK == 1011) ? 4 : (FlagK == 1101) ? 1 : (FlagK == 1110) ? 2 : (FlagK == 111) ? 3 : 0;
                        break;
                    default:
                        FlagK = 0;
                        break;
                }
                Map[i][j] = Map[i][j] * 10 + FlagK;
            }
        }
    }

    for(int i = 1; i < 73; i++) {
        for(int j = 1; j < 73; j++) {
            if(Map[i][j] == 0) {
                this->cell[i - 1][j - 1].Num = 0;
            }
            else {
                switch(Map[i][j]) {
                    case 100:
                        this->cell[i - 1][j - 1].Num = 1;
                        break;
                    case 111:
                        this->cell[i - 1][j - 1].Num = 2;
                        break;
                    case 112:
                        this->cell[i - 1][j - 1].Num = 3;
                        break;
                    case 113:
                        this->cell[i - 1][j - 1].Num = 4;
                        break;
                    case 114:
                        this->cell[i - 1][j - 1].Num = 5;
                        break;
                    case 121:
                        this->cell[i - 1][j - 1].Num = 6;
                        break;
                    case 122:
                        this->cell[i - 1][j - 1].Num = 7;
                        break;
                    case 123:
                        this->cell[i - 1][j - 1].Num = 8;
                        break;
                    case 124:
                        this->cell[i - 1][j - 1].Num = 9;
                        break;
                    case 125:
                        this->cell[i - 1][j - 1].Num = 10;
                        break;
                    case 126:
                        this->cell[i - 1][j - 1].Num = 11;
                        break;
                    case 131:
                        this->cell[i - 1][j - 1].Num = 12;
                        break;
                    case 132:
                        this->cell[i - 1][j - 1].Num = 13;
                        break;
                    case 133:
                        this->cell[i - 1][j - 1].Num = 14;
                        break;
                    case 134:
                        this->cell[i - 1][j - 1].Num = 15;
                        break;
                    case 140:
                        this->cell[i - 1][j - 1].Num = 16;
                        break;
                }
            }
        }
    }
    return ;
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

void Map::CalCellOffset(int BlockDR, int BlockUR)
{
    int i=BlockDR,j=BlockUR;
    Block&block=cell[i][j];
    // 偏移
    if(block.getMapHeight()>0)
    {
        this->cell[i][j].setOffsetY(DRAW_OFFSET * this->cell[i][j].getMapHeight());
    }
    if(block.getMapType() == 2 || block.getMapType() == 3 || block.getMapType() == 4 || block.getMapType() == 5 || block.getMapType() == 8 || block.getMapType() == 9)
    {
        this->cell[i][j].setOffsetY(this->cell[i][j].getOffsetY() + DRAW_OFFSET);
    }

    // 修整边界
    if(this->cell[i][j].getMapType() == 10)
    {
        int t = this->cell[i][j].getOffsetX();
        this->cell[i][j].setOffsetX(t - 1);
    }
    if(this->cell[i][j].getMapType() == 11)
    {
        int t = this->cell[i][j].getOffsetX();
        this->cell[i][j].setOffsetX(t + 1);
    }
    if(this->cell[i][j].getMapType() == 13)
    {
        int t = this->cell[i][j].getOffsetY();
        this->cell[i][j].setOffsetY(t + 1);
    }
    //如果是海洋，按照wlh的方式来偏移
    if(block.getMapType()==MAPTYPE_OCEAN)
        block.setOffsetY(2);
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
                object = map_Object[x][y][i];
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
    if(object->getSort()==SORT_FARMER||object->getSort()==SORT_ARMY||object->getSort()==SORT_HUMAN){
        Human*human=(Human*)object;
        if(human->getTransported())
            return 0;
    }
    return 1;
}


void Map::loadfindPathMap(MoveObject* moveOb)
{
    int represent = moveOb->getPlayerRepresent();

    if(represent == MAXPLAYER)
        represent = NOWPLAYER-1;

    memcpy(findPathMap, findPathMapTemperature[represent], sizeof(findPathMapTemperature[represent]));

    return;
}

void Map::loadfindPathMapTemperature()
{
    clearfindPathMapTemperature();
    for(int represent = 0; represent<NOWPLAYER; represent++)
        for(int x=0; x<MAP_L; x++)
            for(int y=0; y<MAP_U; y++)
            {
                if(barrierMap[x][y] || represent == NOWPLAYERREPRESENT && !cell[x][y].Explored)
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

vector<Point> Map::findBlock_Free(Point blockPoint, int lenth)
{
    int blockDR = blockPoint.x, blockUR = blockPoint.y;

    int bDRL = max(0,blockDR-lenth) , bURD = max(0,blockUR - lenth);
    int bDRR = min(blockDR+1+lenth, MAP_L) , bURU = min(blockUR+1+lenth, MAP_U);

    vector<Point> Block_Free;
    Point tempPoint;

    //在给定范围内找寻没有障碍物的格子
    for(int x = bDRL; x<bDRR; x++)
    {
        for(int y = bURD; y<bURU;y++)
        {
            if(x == blockDR && y==blockUR) continue;

            if(map_Object[x][y].empty())
            {
                tempPoint.x = x;
                tempPoint.y = y;
                Block_Free.push_back(tempPoint);
            }
        }
    }

    return Block_Free;
}

vector<Point> Map::findBlock_Flat(int disLen )
{
    /**
    * 仅在地图资源初始化时可使用，用于寻找建筑可建筑地点
    * 输入：建筑的边长
    * 输出：建筑可建的位置表，从中心开始往外扩展
    *
    */
    int blockDR = MAP_L / 2 - 2, blockUR = MAP_L / 2 - 2 , blockSideLen = 4;

    int sideRM = blockDR+blockSideLen, sideUM = blockUR+blockSideLen;
    int sideRL = blockDR - disLen , sideUL = blockUR - disLen;

    int bDRL,bURD,bDRR,bURU;
    vector<Point> Block_Flat;
    Point tempPoint;
    int maxh,minh,temph;
    bool isfalse;

    do{
        bDRL = max(0, sideRL++) , bURD = max(0, sideUL++) , bDRR = min(sideRM++, MAP_L - disLen) , bURU = min(sideUM++, MAP_U  - disLen);
        //在给定范围内找寻指定区域平坦的格子
        for(int x = bDRL; x<bDRR; x++)
        {
            for(int y = bURD; y<bURU;y++)
            {
                maxh = minh = map_Height[x][y];
                if(mapFlag[x][y]) continue;

                isfalse = false;
                for(int i = 0 ; i < disLen; i++)
                {
                    for(int j = 0 ; j<disLen; j++)
                    {
                        temph = map_Height[x+i][y+j];
                        if(temph>maxh) maxh = temph;
                        else if(temph < minh) minh = temph;

                        if(mapFlag[x+i][y+j] || minh == -1 || maxh!=minh)
                        {
                            isfalse = true;
                            break;
                        }
                    }
                    if(isfalse) break;
                }
                if(isfalse) continue;
                tempPoint.x = x;
                tempPoint.y = y;
                Block_Flat.push_back(tempPoint);
            }
        }
    }while(Block_Flat.empty());

    return Block_Flat;
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

//void Map::clearfindPathMap() {
//    for (int i = 0; i < MAP_L; ++i)
//    {
//        for (int j = 0; j < MAP_U; ++j)
//        {
//            findPathMap[i][j] = 0;
//        }
//    }
//}


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

//更新的resMap_AI是模板，对userAI，需要传入其于视野地图的&，对Enemy，直接使用resMap_AI（全视野）
void Map::reset_resMap_AI()
{
    int siz;
    int sort,Num;
    Animal* animalPrinter = NULL;
    StaticRes* stResPrinter = NULL;
    for(int x = 0;x<MAP_L;x++)
    {
        for(int y = 0 ; y<MAP_U;y++)
        {
            if(!resMap_AI[x][y].explore)
            {
                resMap_AI[x][y].explore = true;
                resMap_AI[x][y].high = cell[x][y].getMapHeight();
            }

            siz = map_Object[x][y].size();
            resMap_AI[x][y].clear_r();  //清除资源信息

            //重新设置资源信息
            for(int z = 0; z<siz; z++)
            {
                sort = map_Object[x][y][z]->getSort();
                Num = map_Object[x][y][z]->getNum();

                if(sort == SORT_ANIMAL)
                {
                    switch (Num) {
                    case ANIMAL_GAZELLE:
                        resMap_AI[x][y].type = RESOURCE_GAZELLE;
                        break;
                    case ANIMAL_LION:
                        resMap_AI[x][y].type = RESOURCE_LION;
                        break;
                    case ANIMAL_ELEPHANT:
                        resMap_AI[x][y].type = RESOURCE_ELEPHANT;
                        break;
                    case ANIMAL_TREE:
                    case ANIMAL_FOREST:
                        resMap_AI[x][y].type = RESOURCE_TREE;
                        break;
                    default:
                        break;
                    }

                    map_Object[x][y][z]->printer_ToAnimal((void**)&animalPrinter);

                    resMap_AI[x][y].fundation = animalPrinter->get_BlockSizeLen();
                    resMap_AI[x][y].SN = animalPrinter->getglobalNum();
                    resMap_AI[x][y].ResType = animalPrinter->get_ResourceSort();
                    resMap_AI[x][y].remain = animalPrinter->get_Cnt();
                }
                else if(sort == SORT_STATICRES)
                {
                    switch (Num) {
                    case NUM_STATICRES_Bush:
                        resMap_AI[x][y].type = RESOURCE_BUSH;
                        break;
                    case NUM_STATICRES_Stone:
                        resMap_AI[x][y].type = RESOURCE_STONE;
                        break;
                    default:
                        break;
                    }

                    map_Object[x][y][z]->printer_ToStaticRes((void**)&stResPrinter);

                    resMap_AI[x][y].fundation = stResPrinter->get_BlockSizeLen();
                    resMap_AI[x][y].SN = stResPrinter->getglobalNum();
                    resMap_AI[x][y].ResType = stResPrinter->get_ResourceSort();
                    resMap_AI[x][y].remain = stResPrinter->get_Cnt();
                }
            }
        }

    }

    reset_resMap_ForUserAndEnemy();
    return;
}

void Map::reset_resMap_ForUserAndEnemy()
{
    for(int x = 0; x<MAP_L;x++)
        for(int y = 0 ; y<MAP_U;y++)
        {
            if(cell[x][y].Explored) resMap_UserAI[x][y] = resMap_EnemyAI[x][y] = resMap_AI[x][y];
            else resMap_EnemyAI[x][y] = resMap_AI[x][y];
        }

    return;
}

//更新用户视野
void Map::reset_CellExplore(Coordinate* eye)
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
        if(!cell[blockLab[i].x][blockLab[i].y].Explored)
            cell[blockLab[i].x][blockLab[i].y].Explored = true;

        if(!cell[blockLab[i].x][blockLab[i].y].Visible)
        {
            blockLab_Visible.push(blockLab[i]);
            cell[blockLab[i].x][blockLab[i].y].Visible = true;
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
 * 函数：Map::loadResource；
 * 参数：无；
 * 内容：随机生成资源；
 * 返回值：生成成功返回true。
 */
bool Map::loadResource() {
    for(int i = 0; i < MAP_U; i++)
    {
        for(int j = 0; j < MAP_L; j++)
        {
            if((Gamemap[i][j] == 1 || Gamemap[i][j] == 11) && this->cell[i][j].getMapType() != MAPTYPE_FLAT) continue;
            int tOffsetX = 0, tOffsetY = 0;
            if(Gamemap[i][j] == 7) addAnimal(2, tranL(i) + BLOCKSIDELENGTH / 2, tranU(j)+BLOCKSIDELENGTH / 2); // 大象
            else if(Gamemap[i][j] == 5) addStaticRes(2, i, j); // 金矿
            else if(Gamemap[i][j] == 4) addStaticRes(1, i, j); // 石头
            else if(Gamemap[i][j] == 3) addAnimal(1, tranL(i) + BLOCKSIDELENGTH / 2 + tOffsetX, tranU(j)+BLOCKSIDELENGTH / 2 + tOffsetY); // 瞪羚
            else if(Gamemap[i][j] == 2) addStaticRes(0, tranL(i) + BLOCKSIDELENGTH / 2 + tOffsetX, tranU(j) + BLOCKSIDELENGTH / 2 + tOffsetY); // 浆果
            else if(Gamemap[i][j] == 1) {
                addAnimal(0, tranL(i) + BLOCKSIDELENGTH / 2 + tOffsetX, tranU(j) + BLOCKSIDELENGTH/2 + tOffsetY); // 树
            }
            else if(Gamemap[i][j] == 11) addAnimal(0, tranL(i) + BLOCKSIDELENGTH/2 + tOffsetX, tranU(j) + BLOCKSIDELENGTH/2 + tOffsetY);
            /*
                    种类：
                    0为空地；
                    1为树木；
                    2为浆果；
                    3为瞪羚；
                    4为石头；
                    5为金矿；
                    6为狮子；
                    7为大象；
                    9为主营；
                    10为箭塔废墟；
                    11为树林。
                */
            Gamemap[i][j] = 0;  // :)
            mapFlag[i][j] = 0;
        }
    }
    return true;
}

/*
 * 函数：Map::checkNeighborHigher；
 * 参数：heightMap[][80]——临时高度数组；
 *      x——x坐标；y——y坐标；
 *      currentCalHeight——当前正在生成的高度;
 * 内容：计算block(x, y)周围8个block有几个高度是高出来的；
 * 返回值：返回数量。
 */
int Map::CheckNeighborHigher(int x, int y, int currentCalHeight) {
    int count = 0;
    int dx[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
    int dy[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
    for(int i = 0; i < 8; i ++) {
        int tx = x + dx[i], ty = y + dy[i];
        if(tx < 0 || ty < 0 || tx > GENERATE_L || ty > GENERATE_U) continue;
        if(m_heightMap[tx][ty] == currentCalHeight) count ++;
    }
    return count;
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
 * 函数：Map::CheckNeighborForest；
 * 参数：x——x坐标；y——y坐标；
 *      forestCell[][FOREST_GENERATE_U]——临时森林数组；
 * 内容：计算block(x, y)周围8个block里可能生成树木的数量；
 * 返回值：返回可能生成树木的数量。
 */
int Map::CheckNeighborForest(int x, int y, int forestCell[][FOREST_GENERATE_U])
{
    int count = 0;
    int dx[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
    int dy[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
    for(int i = 0; i < 8; i ++) {
        int tx = x + dx[i], ty = y + dy[i];
        if(tx < 0 || ty < 0 || tx > FOREST_GENERATE_L || ty > FOREST_GENERATE_U) continue;
        if(forestCell[tx][ty] == 1) count ++;
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
 * 函数：Map::GenerateTerrain；
 * 参数：无；
 * 内容：*细胞自动机*逐层生成地形高度；
 * 返回值：是否生成成功。
 */
bool Map::GenerateTerrain() {
    memset(m_heightMap, 0, sizeof(m_heightMap));
    srand(time(NULL));
    for(int height = MAPHEIGHT_FLAT + 1; height < MAPHEIGHT_PERCENT; height ++) {
        // 每层微调生成概率
        int percent = MAPHEIGHT_PERCENT;
        switch (height) {
        case 1:
            percent -= 3;   // 57
            break;
        case 2:
            percent += 13;  // 70
            break;
        case 3:
            percent -= 10;  // 60
            break;
        case 4:
            percent += 40;  // 100
            break;
        default:
            break;
        }
        for(int i = 0; i < GENERATE_L; i ++)
            for(int j = 0; j < GENERATE_U; j ++) {
                if(rand() % 100 < percent && m_heightMap[i][j] == height - 1 && CheckBorder(i, j, height))
                    m_heightMap[i][j] ++;
            }

        // 特判市镇中心附近
        if(height == 1) {
            for(int i = MAP_L / 2 - CENTER_RADIUS - 1 + CENTER_DEVIATION; i < MAP_L / 2 + CENTER_RADIUS + CENTER_DEVIATION; i ++) {
                for(int j = MAP_U / 2 - CENTER_RADIUS - 1 + CENTER_DEVIATION; j < MAP_U / 2 + CENTER_RADIUS + CENTER_DEVIATION; j ++) {
                    m_heightMap[i][j] = 0;
                }
            }
        }

        for(int optCounter = 0; optCounter <= MAPHEIGHT_OPTCOUNT; optCounter ++)    // optCounter：当前优化次数
        {
            for(int i = 0; i < GENERATE_L; i ++)
                for(int j = 0; j < GENERATE_U; j ++) {
                    int count = CheckNeighborHigher(i, j, height);
                    if(m_heightMap[i][j] == height && count < 4) m_heightMap[i][j] --;
                    else if(m_heightMap[i][j] == height - 1 && count >= 5) m_heightMap[i][j] ++;
                    if(m_heightMap[i][j] > 5 || m_heightMap[i][j] < 0) {
                        qDebug() << "Map::GenerateTerrain() ERROR：m_heightMap[" << i << "][" << j << "] == " << m_heightMap[i][j];
                        return false;
                    }
                }
            if(optCounter == MAPHEIGHT_OPTCOUNT / 2)
                for(int i = 2; i < GENERATE_L-2; i ++)
                    for(int j = 2; j < GENERATE_U-2; j ++) {
                        int count = CheckNeighborHigher(i, j, height);
                        if(m_heightMap[i][j] == height - 1 && count >= 2) {
                            if(m_heightMap[i - 1][j - 1] == height && m_heightMap[i + 1][j + 1] == height) {
                                m_heightMap[i - 1][j - 1] --;
                                m_heightMap[i + 1][j + 1] --;
                            }
                            if(m_heightMap[i - 1][j + 1] == height && m_heightMap[i + 1][j - 1] == height) {
                                m_heightMap[i - 1][j + 1] --;
                                m_heightMap[i + 1][j - 1] --;
                            }
                        }
                    }
        }
    }

    for(int i = 2; i < GENERATE_L-2; i ++) {
        for(int j = 2; j < GENERATE_U-2; j ++) {
            if(m_heightMap[i][j] > 5 || m_heightMap[i][j] < 0) {
                qDebug() << "Map::GenerateTerrain() ERROR：m_heightMap[" << i << "][" << j << "] == " << m_heightMap;
                return false;
            }
        }
    }

    // 取中心高度值存入cell
    for(int i = 4; i < GENERATE_L-4; i ++)
        for(int j = 4; j < GENERATE_U-4; j ++)
            this->cell[i - 4][j - 4].setMapHeight(m_heightMap[i][j]);
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
                // 设置平地全部为草地
                this->cell[i][j].Num = MAPPATTERN_GRASS;
                // 测试时设置平地全部为沙漠，便于调试
                // this->cell[i][j].Num = MAPPATTERN_DESERT;
            }
            else if(this->cell[i][j].getMapType() == 0)
            {
                qDebug() << "Map::InitFaultHandle() ERROR：第" << i << "行第" << j <<"列的地块MapType未定义！";
            }
            if(this->cell[i][j].Num >= 29) qDebug() << "Map::InitFaultHandle() ERROR：cell[" << i << "][" << j << "].Num >= 29";
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
 * 函数：Map::GenerateMapTxt；
 * 参数：获取启动参数，判断是否输出地图；
 * 内容：导出地图txt文件；
 * 返回值：空。
 */
void Map::GenerateMapTxt(int MapJudge) {
    if(MapJudge == 0)       // 随机产生地图
    {
        std::ofstream outMapFile("tmpMap.txt");
        if (outMapFile.is_open())
        {
            for(int i = 0 ; i < MAP_L; i++)
            {
                outMapFile<<Gamemap[i][0];

                for(int j = 1; j < MAP_U; j++)
                {
                    outMapFile<<' '<<Gamemap[i][j];
                }
                outMapFile<<"\n";
            }
            outMapFile << "\n"; //隔三行
            outMapFile << "\n";
            outMapFile << "\n";

            for (int i = 0; i < MAP_L; i++)
            {
                outMapFile<<this->cell[i][0].getMapHeight();

                for (int j = 1; j < MAP_U; j++)
                {
                    outMapFile<< ' ' << this->cell[i][j].getMapHeight();
                }
                outMapFile << "\n";
            }
            outMapFile << "\n";
            outMapFile << "\n";
            outMapFile << "\n";

            for (int i = 0; i < GENERATE_L; i++)
            {
                outMapFile<<m_heightMap[i][0];

                for (int j = 1; j < GENERATE_U; j++)
                {
                    outMapFile << ' ' << m_heightMap[i][j];
                }
                outMapFile << "\n";
            }

            outMapFile.close();
            qDebug() << "地图数据已导出为文件";
        }
        else
        {
            qDebug() << "无法导出地图文件";
        }
    }
}

/*
 * 函数：Map::loadGenerateMapText
 * 参数：获取启动参数，判断是否载入地图；
 * 内容：读取地图txt文件；
 * 返回值：空。
 */
void Map::loadGenerateMapText(int MapJudge)
{
    QFile file("map.txt");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug()<<"文件打开错误";
        return;
    }
    QJsonParseError * error = nullptr;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(),error);
    if(error){
        qDebug()<<"json error";
        return;
    }
    file.close();
    /////////////////////////////////开始解析
    QJsonObject root=doc.object();
    QStringList allKeys=root.keys();
    for(QString&key:allKeys){
        QJsonObject obj=root[key].toObject();
        if(key.contains("Cell")){
            int blockL=obj["BlockDR"].toInt(),blockU=obj["BlockUR"].toInt();
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
        }
        else if(key.contains("Building")){
            Player&me=*(player[0]),&enemy=(*player[1]);
            Player&cur=obj["Own"].toString()=="WLH"?me:enemy;
            cur.addBuilding(obj["Num"].toInt(),obj["BlockDR"].toInt(),obj["BlockUR"].toInt(),100);
        }else if(key.contains("Human")){
            Player&me=*(player[0]),&enemy=(*player[1]);
            Player&cur=obj["Own"].toString()=="WLH"?me:enemy;
            double UR=obj["UR"].toDouble(),DR=obj["DR"].toDouble();
            if(obj["Sort"].toString()=="Farmer")
            {
                int FarmerType=obj["FarmerType"].toInt();
                if(FarmerType==FARMERTYPE_FARMER)
                cur.addFarmer(DR,UR);
                else cur.addShip(FarmerType,DR,UR);
            }
            else cur.addArmy(obj["Num"].toInt(),DR,UR);
        }else if(key.contains("Animal")){
            addAnimal(obj["Num"].toInt(),obj["DR"].toDouble(),obj["UR"].toDouble());
        }else if(key.contains("StaticRes")){
            addStaticRes(obj["Num"].toInt(),obj["BlockDR"].toInt(),obj["BlockUR"].toInt());
        }
    }
    //赋值m_heightMap
    memset(m_heightMap,0,sizeof(m_heightMap));
    for(int i=0;i<MAP_L;++i){
        for(int j=0;j<MAP_U;++j){
            m_heightMap[i+4][j+4]=cell[i][j].getMapHeight();
        }
    }
}

bool Map::CheckIsNearOcean(int x, int y)
{
    //检查周围3*3区域是不是有海洋
    for(int i=-1;i<=1;++i){
        for(int j=-1;j<=1;++j){
            int ii=i+x,jj=j+y;
            if(ii<0||jj<0||ii>=GENERATE_L||jj>=GENERATE_U)continue;
            if(m_heightMap[i][j]==MAPHEIGHT_OCEAN)return 1;
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
void Map::init(int MapJudge) {
    InitCell(0, MAP_EXPLORE, true);    // 第二个参数修改为true时可令地图全部可见
    loadGenerateMapText(MapJudge);  //载入地图
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
