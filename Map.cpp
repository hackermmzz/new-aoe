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
#include<rectarea.h>
#include<linearea.h>
#include<circlearea.h>
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
//void Map::generateCenter()
//{
//    // 生成城镇中心，以3 * 3的左上角表示城镇中心真正的放置位置
//    Gamemap[MAP_L / 2 - 1][MAP_U / 2 - 1] = 9;
//    player[0]->addBuilding(BUILDING_CENTER,MAP_L / 2 - 1,MAP_L / 2 - 1,100);

//    //添加初始村民
//    player[0]->addFarmer((MAP_L / 2 - 1.5)*BLOCKSIDELENGTH,(MAP_L / 2 + 1.5)*BLOCKSIDELENGTH);
//    player[0]->addFarmer((MAP_L / 2 - 1.5)*BLOCKSIDELENGTH,(MAP_L / 2 - 1.5)*BLOCKSIDELENGTH);
//    player[0]->addFarmer((MAP_L / 2 + 2.5)*BLOCKSIDELENGTH,(MAP_L / 2 - 1.5)*BLOCKSIDELENGTH);

//    for(int i = MAP_L / 2 - 2; i <= MAP_L / 2 + 2; i++)
//    {
//        for(int j = MAP_U / 2 - 2; j <= MAP_U / 2 + 2; j++)
//        {
//            mapFlag[i][j] = true;
//        }
//    }
//}



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
void Map::drawEdge(int tempMap[MAP_L][MAP_U] ,std::map<int, int> codeToNum,int MapType1,int MapType2,int MapType3) {
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
    memset(TreeBlock,0,sizeof(TreeBlock));
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
    // 创建临时地图：海洋标记为1，陆地标记为0
    int tempMap[MAP_L][MAP_U] = {0};    //tempMap[左右][上下]
    for (int i = 0; i < MAP_L; ++i) {
        for (int j = 0; j < MAP_U; ++j) {
            tempMap[i][j] = (cell[i][j].getMapType() == MAPTYPE_OCEAN) ? 1 : 0;
        }
    }
    drawEdge(tempMap, OceanCodeToNum, 0, 1, 2);  // 绘制海洋边缘，陆地是0，海洋是1，海滩被标记为2
    drawEdge(tempMap, SandCodeToNum, 0, 2, 3);  // 绘制沙地边缘，陆地是0，沙地是2，边界被标记为2
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
    if(MAP_EXPLORE||EditorMode){
        for(int i=0;i<MAP_L;++i){
            for(int j=0;j<MAP_U;++j){
                cell[i][j].Explored=1;
            }
        }
    }
    if(MAP_VISIABLE||EditorMode){
        for(int i=0;i<MAP_L;++i){
            for(int j=0;j<MAP_U;++j){
                cell[i][j].Visible=1;
            }
        }
    }
}

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


Map::TypeRef Map::loadfindPathMap(MoveObject* moveOb)
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
    try {
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
                if(!map_Object[x][y].empty())
                {
                    siz = map_Object[x][y].size();
                    resMap_AI[x][y].clear_r();  //清除资源信息

                    //重新设置资源信息
                    for(int z = 0; z<siz; z++)
                    {
                        if(map_Object[x][y][z] != NULL)
                        {
                            sort = map_Object[x][y][z]->getSort();
                            Num = map_Object[x][y][z]->getNum();
                        }

                        if(sort == SORT_ANIMAL && map_Object[x][y][z] != NULL)
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
                            if(animalPrinter != NULL)
                            {
                                resMap_AI[x][y].fundation = animalPrinter->get_BlockSizeLen();
                                resMap_AI[x][y].SN = animalPrinter->getglobalNum();
                                resMap_AI[x][y].ResType = animalPrinter->get_ResourceSort();
                                resMap_AI[x][y].remain = animalPrinter->get_Cnt();
                            }
                        }
                        else if(sort == SORT_STATICRES && map_Object[x][y][z] != NULL)
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
                            if( stResPrinter != NULL)
                            {
                                resMap_AI[x][y].fundation = stResPrinter->get_BlockSizeLen();
                                resMap_AI[x][y].SN = stResPrinter->getglobalNum();
                                resMap_AI[x][y].ResType = stResPrinter->get_ResourceSort();
                                resMap_AI[x][y].remain = stResPrinter->get_Cnt();
                            }
                        }
                    }
                }
            }

        }

        reset_resMap_ForUserAndEnemy();
        return;
    }
    catch (std::bad_alloc& ba)
    {
        std::cerr << "std::bad_alloc caught: " << ba.what() << '\n';
    }
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
                        qWarning() << "Map::GenerateTerrain() ERROR：m_heightMap[" << i << "][" << j << "] == " << m_heightMap[i][j];
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
                qWarning() << "Map::GenerateTerrain() ERROR：m_heightMap[" << i << "][" << j << "] == " << m_heightMap;
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
    // 使用高精度时间为种子的真随机数生成器
    auto&&AllMapFile=GetAllTargetFiles(MAPFILE_SUFFIX);
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 gen(seed);
    std::uniform_int_distribution<> dis(0,AllMapFile.size()-1);
    int mapIdx = dis(gen); // 1~4
    auto mapPath=AllMapFile[mapIdx];
    QFile file(mapPath);
    MapFileName=mapPath.split("/").back();
//    QFile file("map3.txt");

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
            if(blockL >= 0 && blockL < MAP_L && blockU >= 0 && blockU < MAP_U) {
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
            if(blockDR >= 0 && blockDR < MAP_L && blockUR >= 0 && blockUR < MAP_U) {
                cur.addBuilding(obj["Num"].toInt(), blockDR, blockUR, 100);
            } else {
                qWarning() << "Building坐标超出范围:" << blockDR << "," << blockUR;
            }
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
            else
            {
                //默认敌方只有军队需要须臾限制
                int sn=cur.addArmy(obj["Num"].toInt(),DR,UR)->getglobalNum();
                //读取区域限制
                if(obj.contains("AreaLimit")){
                    QJsonObject area=obj["AreaLimit"].toObject();
                    string type=area["Type"].toString().toStdString();
                    ParseArea(sn,type,area);
                }
            }
        }else if(key.contains("Animal")){
            double dr=obj["DR"].toDouble(),ur=obj["UR"].toDouble();
            addAnimal(obj["Num"].toInt(),dr,ur);
        }else if(key.contains("StaticRes")){
            int blockDR = obj["BlockDR"].toInt();
            int blockUR = obj["BlockUR"].toInt();
            // 添加边界检查
            if(blockDR >= 0 && blockDR < MAP_L && blockUR >= 0 && blockUR < MAP_U) {
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

                if(type == "AT_SHIP") {
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

                if(type == "AT_SCOUT") {
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

                enemy.addArmyDEFENSE(type, x, y, status);
            }
        }

        qDebug() << "敌人配置读取完成";
    }

    //赋值m_heightMap
    memset(m_heightMap,0,sizeof(m_heightMap));
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
    memset(explored,0,sizeof(explored));
    InitCell(0, MAP_EXPLORE, false);    // 第二个参数修改为true时可令地图全部可见
    loadGenerateMapText(MapJudge);  //载入地图
    divideTheMap();                 //把地图化分成一个一个连通块,并且对己方地图可见化

    // 敌人配置现在从map.txt文件中读取，不再硬编码
    // 如果没有从文件中读取到敌人配置，则使用默认配置
//    if(player[1]->human.empty()) {
//        Player& enemy = (*player[1]);

//        //巡逻船
//        enemy.addArmyAROUND(AT_SHIP,100,1500,ARMY_STATE_AROUND,300,10000,2000,20);
//        enemy.addArmyAROUND(AT_SHIP,500,1600,ARMY_STATE_AROUND,300,10000,2000,400);
//        enemy.addArmyAROUND(AT_SHIP,1200,1350,ARMY_STATE_AROUND,300,10000,2400,1300);
//        enemy.addArmyAROUND(AT_SHIP,1700,1000,ARMY_STATE_AROUND,300,10000,2500,1900);
//        enemy.addArmyAROUND(AT_SHIP,2400,1000,ARMY_STATE_AROUND,300,10000,2000,2500);
//        enemy.addArmyAROUND(AT_SHIP,3100,1000,ARMY_STATE_AROUND,300,10000,2000,2900);
//        enemy.addArmyAROUND(AT_SHIP,3150,1700,ARMY_STATE_AROUND,300,10000,1700,3400);
//        enemy.addArmyAROUND(AT_SHIP,3700,1900,ARMY_STATE_AROUND,300,10000,1200,3900);
//        enemy.addArmyAROUND(AT_SHIP,4100,1000,ARMY_STATE_AROUND,300,10000,500,4300);
//        enemy.addArmyAROUND(AT_SHIP,4500,1000,ARMY_STATE_AROUND,300,10000,100,4300);


//        // 内陆巡逻骑兵
//        enemy.addArmyAROUND(AT_SCOUT,4000,700,ARMY_STATE_AROUND,300,10000,2100,4550);
//        enemy.addArmyAROUND(AT_SCOUT,3300,750,ARMY_STATE_AROUND,300,10000,3800,3000);
//        enemy.addArmyAROUND(AT_SCOUT,1800,50,ARMY_STATE_AROUND,300,10000,4450,3100);

//        //  内陆进攻兵(基准点700 2500替换为2000 500)
//        enemy.addArmyATTACK(0,2000,525,3,3000,37500);
//        enemy.addArmyATTACK(0,2000,550,3,3000,37500);
//        enemy.addArmyATTACK(0,2000,575,3,3000,37500);
//        enemy.addArmyATTACK(0,2000,600,3,3000,37500);
//        enemy.addArmyATTACK(1,2025,500,3,3000,37500);
//        enemy.addArmyATTACK(1,2050,500,3,3000,37500);
//        enemy.addArmyATTACK(1,2075,500,3,3000,37500);
//        enemy.addArmyATTACK(1,2100,500,3,3000,37500);
//        enemy.addArmyATTACK(2,2025,525,3,3000,37500);
//        enemy.addArmyATTACK(2,2025,550,3,3000,37500);
//        enemy.addArmyATTACK(2,2050,525,3,3000,37500);
//        enemy.addArmyATTACK(2,2050,550,3,3000,37500);

//        //  内陆防守兵（基准点500,1600替换为3000 400）
//        enemy.addArmyDEFENSE(0,3000,400,2);
//        enemy.addArmyDEFENSE(0,3000,425,2);
//        enemy.addArmyDEFENSE(0,3000,450,2);
//        enemy.addArmyDEFENSE(0,3000,475,2);
//        enemy.addArmyDEFENSE(1,3025,400,2);
//        enemy.addArmyDEFENSE(1,3050,400,2);
//        enemy.addArmyDEFENSE(1,3075,400,2);
//        enemy.addArmyDEFENSE(1,3100,400,2);
//        enemy.addArmyDEFENSE(2,3025,425,2);
//        enemy.addArmyDEFENSE(2,3025,450,2);
//        enemy.addArmyDEFENSE(2,3050,425,2);
//        enemy.addArmyDEFENSE(2,3050,450,2);

//        qDebug() << "使用默认敌人配置";
//    }

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
