#include "Map.h"

Map::Map()
{

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

    //    // 清空 coores 列表并调用析构函数释放内存
    //    for (auto it = coores.begin(); it != coores.end(); ++it)
    //    {
    //        delete *it;
    //    }
    //    coores.clear();

    //    // 清空 animal 列表并调用析构函数释放内存
    //    for (auto it = animal.begin(); it != animal.end(); ++it)
    //    {
    //        delete *it;
    //    }
    //    animal.clear();

    //    // 清空 ruin 列表并调用析构函数释放内存
    //    for (auto it = ruin.begin(); it != ruin.end(); ++it)
    //    {
    //        delete *it;
    //    }
    //    ruin.clear();
}

// 随机生成资源（多个），并添加到地图中
void Map::generateResources()
{
    srand(time(NULL));
    // flag用于表示是否已经生成，若已生成则flag为1，否则为0
    int flag = 0;

    // 在45 * 45边缘生成两片森林
    for(int T = 0; T < ForestMAX; T++)
    {
        flag = 0;
        for (int i = MAP_L / 2 - 25; i <= MAP_L / 2 + 10; i++)
        {
            for (int j = MAP_U / 2 - 25; j <= MAP_U / 2 + 10; j++)
            {
                if(flag == 1) break;
                if(i >= MAP_L / 2 - 24 && i <= MAP_L / 2 + 9 && j >= MAP_U / 2 - 24 && j <= MAP_U / 2 + 9) continue;
                if(rand() % 1000 == 0 && mapFlag[i][j] != 1 && mapFlag[i + 7][j + 7] != 1 && mapFlag[i + 14][j + 14] != 1 && mapFlag[i][j + 14] != 1 && mapFlag[i + 14][j] != 1 && mapFlag[i + 3][j + 3] != 1 && mapFlag[i + 10][j + 10] != 1 && mapFlag[i + 3][j + 10] != 1 && mapFlag[i + 10][j + 3] != 1)
                {
                    flag = 1;
                    int ForestFlag = rand() % 3;

                    int ti = 0;
                    for(int I = i; I < i + 15; I++)
                    {
                        int tj = 0;
                        for(int J = j; J < j + 15; J++)
                        {
                            mapFlag[I][J] = true;
                            if(Forest[ForestFlag][ti][tj] == 1)
                            {
                                Gamemap[I][J] = 11;
                            }
                            tj++;
                        }
                        ti++;
                    }
                }
            }
            if(flag == 1) break;
            if(flag == 0 && i == MAP_L / 2 + 10) i = MAP_L / 2 - 25;
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

// 随机生成资源（单个），并添加到地图中
void Map::generateResource()
{
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

// 生成城镇中心13 * 13的部分
void Map::generateCenter()
{
    srand(time(NULL));

    // 生成城镇中心，以3 * 3的左上角表示城镇中心真正的放置位置
    Gamemap[MAP_L / 2 - 1][MAP_U / 2 - 1] = 9;
    for(int i = MAP_L / 2 - 2; i <= MAP_L / 2 + 2; i++)
    {
        for(int j = MAP_U / 2 - 2; j <= MAP_U / 2 + 2; j++)
        {
            mapFlag[i][j] = true;
        }
    }

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

void Map::genDesert(int i, int j, int number, int Map[][74])
{
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

void Map::generateLandforms()
{
    //    int Map[74][74] = {0};
    //    srand(time(nullptr));
    //    for(int i = 0; i < GENERATELANDFORMS_NUM; i++)
    //    {
    //        int randX = rand() % 72, randY = rand() % 72;
    //        genDesert(randX + 1, randY + 1, 25 + rand() % 5, Map);
    //    }

    //    // 沙地赋值为1，草地赋值为0
    //    for(int i = 0; i < 74; i++)
    //    {
    //        for(int j = 0; j < 74; j++)
    //        {
    //            if(i == 0 || i == 73) Map[i][j] = 1;
    //            else
    //            {
    //                if(j == 0 || j == 73) Map[i][j] = 1;
    //            }
    //            if(Map[i][j] != 0)
    //            {
    //                Map[i][j] = 1;
    //            }
    //        }
    //    }

    //    for (int i = 1; i < 73; i++)
    //    {
    //        for (int j = 1; j < 73; j++)
    //        {
    //            int TmpCal[4][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
    //            if (Map[i][j] != 0)
    //            {
    //                int ZeroCnt = 0;
    //                int FlagK = 0;
    //                int tmpk[4] = {0};
    //                for (int k = 0; k < 4; k++)
    //                {
    //                    // 如果沙地旁有草地，则记录草地相对沙地的位置和草地数量（0 <= cnt <= 4）
    //                    if (Map[i + TmpCal[k][0]][j + TmpCal[k][1]] == 0)
    //                    {
    //                        ZeroCnt++;
    //                        tmpk[k] = 1;
    //                    }
    //                }
    //                Map[i][j] = Map[i][j] * 10 + ZeroCnt;
    //                FlagK = tmpk[0] * 1000 + tmpk[1] * 100 + tmpk[2] * 10 + tmpk[3];

    //                // 判断FlagK的值
    //                switch (ZeroCnt)
    //                {
    //                case 1:
    //                    FlagK = (FlagK == 1000) ? 1 : (FlagK == 100) ? 2 : (FlagK == 10) ? 3 : (FlagK == 1) ? 4 : 0;
    //                    break;
    //                case 2:
    //                    FlagK = (FlagK == 1001) ? 4 : (FlagK == 1010) ? 6 : (FlagK == 1100) ? 1 : (FlagK == 101) ? 5 : (FlagK == 110) ? 2 : (FlagK == 11) ? 3 : 0;
    //                    break;
    //                case 3:
    //                    FlagK = (FlagK == 1011) ? 4 : (FlagK == 1101) ? 1 : (FlagK == 1110) ? 2 : (FlagK == 111) ? 3 : 0;
    //                    break;
    //                default:
    //                    FlagK = 0;
    //                    break;
    //                }
    //                Map[i][j] = Map[i][j] * 10 + FlagK;
    //            }
    //        }
    //    }

    //    for(int i = 1; i < 73; i++)
    //    {
    //        for(int j = 1; j < 73; j++)
    //        {
    //            if(Map[i][j] == 0)
    //            {
    //                this->cell[i - 1][j - 1].Num = 0;
    //            }
    //            else
    //            {
    //                switch(Map[i][j])
    //                {
    //                case 100:
    //                    this->cell[i - 1][j - 1].Num = 1;
    //                    break;
    //                case 111:
    //                    this->cell[i - 1][j - 1].Num = 2;
    //                    break;
    //                case 112:
    //                    this->cell[i - 1][j - 1].Num = 3;
    //                    break;
    //                case 113:
    //                    this->cell[i - 1][j - 1].Num = 4;
    //                    break;
    //                case 114:
    //                    this->cell[i - 1][j - 1].Num = 5;
    //                    break;
    //                case 121:
    //                    this->cell[i - 1][j - 1].Num = 6;
    //                    break;
    //                case 122:
    //                    this->cell[i - 1][j - 1].Num = 7;
    //                    break;
    //                case 123:
    //                    this->cell[i - 1][j - 1].Num = 8;
    //                    break;
    //                case 124:
    //                    this->cell[i - 1][j - 1].Num = 9;
    //                    break;
    //                case 125:
    //                    this->cell[i - 1][j - 1].Num = 10;
    //                    break;
    //                case 126:
    //                    this->cell[i - 1][j - 1].Num = 11;
    //                    break;
    //                case 131:
    //                    this->cell[i - 1][j - 1].Num = 12;
    //                    break;
    //                case 132:
    //                    this->cell[i - 1][j - 1].Num = 13;
    //                    break;
    //                case 133:
    //                    this->cell[i - 1][j - 1].Num = 14;
    //                    break;
    //                case 134:
    //                    this->cell[i - 1][j - 1].Num = 15;
    //                    break;
    //                case 140:
    //                    this->cell[i - 1][j - 1].Num = 16;
    //                    break;
    //                }
    //            }
    //        }
    //    }

    for(int i = 0; i < 72; i++)
    {
        for(int j = 0; j < 72; j++)
        {
            this->cell[i][j].setMapPattern(MAPPATTERN_GRASS);
        }
    }

    return ;
}

void Map::loadfindPathMap()
{

    clearfindPathMap();
    //    for (int i = 0; i < MAP_L; ++i)
    //    {
    //        for (int j = 0; j < MAP_U; ++j)
    //        {
    //            if(cell[i][j].Explored==false)
    //            {
    //                findPathMap[i][j]=1;
    //            }
    //        }
    //    }
    //    for(int i=0;i<MAXPLAYER;i++)
    //    {
    //        if(!player[i]->build.empty())
    //        {
    //            std::list<Building *>::iterator iter=player[i]->build.begin();
    //            while(iter!=player[i]->build.end())
    //            {
    //                if((*iter)->isMiddleFoundation())
    //                {
    //                    findPathMap[(*iter)->getBlockL()][(*iter)->getBlockU()]=1;
    //                    findPathMap[(*iter)->getBlockL()][(*iter)->getBlockU()+1]=1;
    //                    findPathMap[(*iter)->getBlockL()][(*iter)->getBlockU()+2]=1;
    //                    findPathMap[(*iter)->getBlockL()+1][(*iter)->getBlockU()]=1;
    //                    findPathMap[(*iter)->getBlockL()+1][(*iter)->getBlockU()+1]=1;
    //                    findPathMap[(*iter)->getBlockL()+1][(*iter)->getBlockU()+2]=1;
    //                    findPathMap[(*iter)->getBlockL()+2][(*iter)->getBlockU()]=1;
    //                    findPathMap[(*iter)->getBlockL()+2][(*iter)->getBlockU()+1]=1;
    //                    findPathMap[(*iter)->getBlockL()+2][(*iter)->getBlockU()+2]=1;
    //                }
    //                else if((*iter)->isSmallFoundation())
    //                {
    //                    findPathMap[(*iter)->getBlockL()][(*iter)->getBlockU()]=1;
    //                    findPathMap[(*iter)->getBlockL()][(*iter)->getBlockU()+1]=1;
    //                    findPathMap[(*iter)->getBlockL()+1][(*iter)->getBlockU()]=1;
    //                    findPathMap[(*iter)->getBlockL()+1][(*iter)->getBlockU()+1]=1;
    //                }
    //                iter++;
    //            }
    //        }
    //        //        if(!player[i]->human.empty())
    //        //        {
    //        //            std::list<Human *>::iterator iter=player[i]->human.begin();
    //        //            while(iter!=player[i]->human.end())
    //        //            {
    //        //                CollisionObject.push_back((*ite));
    //        //                iter++;
    //        //            }
    //        //        }
    //    }
    //    if(!map->coores.empty())
    //    {
    //        std::list<Coores *>::iterator iter=map->coores.begin();
    //        while(iter!=map->coores.end())
    //        {
    //            if((*iter)->isBlockres())
    //            {
    //                findPathMap[(*iter)->getBlockL()][(*iter)->getBlockU()]=1;
    //                findPathMap[(*iter)->getBlockL()][(*iter)->getBlockU()+1]=1;
    //                findPathMap[(*iter)->getBlockL()+1][(*iter)->getBlockU()]=1;
    //                findPathMap[(*iter)->getBlockL()+1][(*iter)->getBlockU()+1]=1;
    //            }
    //            iter++;
    //        }
    //    }
    //    if(!map->animal.empty())
    //    {
    //        std::list<Animal *>::iterator iter=map->animal.begin();
    //        while(iter!=map->animal.end())
    //        {
    //            Animal *animal=(Animal *)(*iter);
    //            if(animal->isTree())
    //            {
    //                findPathMap[(*iter)->getBlockL()][(*iter)->getBlockU()]=1;
    //            }
    //            iter++;
    //        }
    //    }
}

void Map::clearfindPathMap()
{
    for (int i = 0; i < MAP_L; ++i)
    {
        for (int j = 0; j < MAP_U; ++j)
        {
            findPathMap[i][j] = 0;
        }
    }
}

int Map::addStaticRes(int Num, double DR, double UR)
{
    StaticRes *newstaticres=new StaticRes(Num,DR,UR);
    this->staticres.push_back(newstaticres);
    return 0;
}

int Map::addStaticRes(int Num, int BlockDR, int BlockUR)
{
    StaticRes *newstaticres=new StaticRes(Num,BlockDR,BlockUR);
    this->staticres.push_back(newstaticres);
    return 0;
}

int Map::addAnimal(int Num, double DR, double UR)
{
    Animal *newanimal=new Animal(Num,DR,UR);
    this->animal.push_back(newanimal);
    return 0;
}

int Map::loadResource()
{
    addAnimal(ANIMAL_GAZELLE,36*BLOCKSIDELENGTH,36*BLOCKSIDELENGTH);
    addAnimal(1,0*BLOCKSIDELENGTH + BLOCKSIDELENGTH / 2,72*BLOCKSIDELENGTH + BLOCKSIDELENGTH / 2);
    //    for(int i = 0; i < MAP_U; i++)
    //    {
    //        for(int j = 0; j < MAP_L; j++)
    //        {
    //            if(Gamemap[i][j] == 7) addAnimal(2, tranL(i)+BLOCKSIDELENGTH/2, tranU(j)+BLOCKSIDELENGTH/2); // 大象
    //            else if(Gamemap[i][j] == 6) addAnimal(3, tranL(i)+BLOCKSIDELENGTH/2, tranU(j)+BLOCKSIDELENGTH/2); // 狮子
    //            else if(Gamemap[i][j] == 5) addStaticRes(2, i, j); // 金矿
    //            else if(Gamemap[i][j] == 4) addStaticRes(1, i, j); // 石头
    //            else if(Gamemap[i][j] == 3) addAnimal(1, tranL(i)+BLOCKSIDELENGTH/2, tranU(j)+BLOCKSIDELENGTH/2); // 瞪羚
    //            else if(Gamemap[i][j] == 2) addStaticRes(0, tranL(i)+BLOCKSIDELENGTH/2, tranU(j)+BLOCKSIDELENGTH/2); // 浆果
    //            else if(Gamemap[i][j] == 1) addAnimal(0, i, j); // 树
    //            else if(Gamemap[i][j] == 11) addAnimal(0, i, j);
    //            /*
    //                种类：
    //                0为空地；
    //                1为树木；
    //                2为浆果；
    //                3为瞪羚；
    //                4为石头；
    //                5为金矿；
    //                6为狮子；
    //                7为大象；
    //                9为主营；
    //                10为箭塔废墟；
    //                11为树林。
    //            */
    //            Gamemap[i][j] = 0;  // :)
    //            mapFlag[i][j] = 0;
    //        }
    //    }
    return 0;
}

void Map::GenerateTerrain()
{
    srand(time(NULL));

    // 1.每间隔10格，设置一个高度锚点，高度在2~4之间；
    for(int i = 0; i < 72; i += 10)
    {
        for(int j = 0; j < 72; j += 10)
        {
            if(i == 0 || i == 71 || j == 0 || j == 71)
            {
                this->cell[i][j].setMapHeight(3);
                continue;
            }
            this->cell[i][j].setMapHeight(rand() % 5 + 2);
        }
    }

    // 2. 加权平均算出每个方格里各Block的高度
    for(int i = 0; i < 72; i++)
    {
        for(int j = 0; j < 71; j++)
        {
            if(this->cell[i][j].getMapHeight() == MAPHEIGHT_EMPTY)
            {
                int LUi = (i / 10) * 10;
                int LUj = (j / 10) * 10;
                int RUi = (i / 10) * 10;
                int RUj = (j / 10 + 1) * 10;
                int LDi = (i / 10 + 1) * 10;
                int LDj = (j / 10) * 10;
                int RDi = (i / 10 + 1) * 10;
                int RDj = (j / 10 + 1) * 10;
                if(LDi >= 72) LDi = 71;
                if(RDi >= 72) RDi = 71;
                if(RDj >= 71) RDi = 70;
                double MapH_LU = this->cell[LUi][LUj].getMapHeight();
                double MapH_RU = this->cell[RUi][RUj].getMapHeight();
                double MapH_LD = this->cell[LDi][LDj].getMapHeight();
                double MapH_RD = this->cell[RDi][RDj].getMapHeight();
                // 此时地块与四个角的点的曼哈顿距离 / 在整个大块里（10 * 10小块矩阵）理论上最远的距离（即20）= 高度占左上角的比例
                double LU = 1 - smooth(double(abs(i - LUi) + abs(j - LUj)) / 20);
                double RU = 1 - smooth(double(abs(i - RUi) + abs(j - RUj)) / 20);
                double LD = 1 - smooth(double(abs(i - LDi) + abs(j - LDj)) / 20);
                double RD = 1 - smooth(double(abs(i - RDi) + abs(j - RDj)) / 20);
                this->cell[i][j].setMapHeight(int((LU * MapH_LU + RU * MapH_RU + LD * MapH_LD + RD * MapH_RD) / 2 + 0.5));
            }
        }
    }

    // 3.初步修整地图边缘
    for(int i = 0; i < 72; i++)
    {
        this->cell[i][71].setMapHeight(0);
        this->cell[i][0].setMapHeight(0);
    }
    for(int j = 0; j < 72; j++)
    {
        this->cell[71][j].setMapHeight(0);
        this->cell[0][j].setMapHeight(0);
    }

    // 将可能存在的突出/凹陷部分平滑化
    for(int tmp = 2; tmp < 5; tmp ++)
    {
        int T = 20;
        while(T--)
        {
            for(int i = 1; i < 71; i++)
            {
                for(int j = 1; j < 71; j++)
                {
                    if(this->cell[i][j].getMapHeight() == tmp)
                    {
                        if(this->cell[i - 1][j - 1].getMapHeight() == this->cell[i + 1][j + 1].getMapHeight() && this->cell[i][j].getMapHeight() != this->cell[i - 1][j - 1].getMapHeight())
                            this->cell[i][j].setMapHeight(this->cell[i - 1][j - 1].getMapHeight());
                        if(this->cell[i - 1][j + 1].getMapHeight() == this->cell[i + 1][j - 1].getMapHeight() && this->cell[i][j].getMapHeight() != this->cell[i + 1][j - 1].getMapHeight())
                            this->cell[i][j].setMapHeight(this->cell[i - 1][j + 1].getMapHeight());
                        if(this->cell[i][j - 1].getMapHeight() == this->cell[i][j + 1].getMapHeight() && this->cell[i][j].getMapHeight() != this->cell[i][j + 1].getMapHeight())
                            this->cell[i][j].setMapHeight(this->cell[i][j - 1].getMapHeight());
                        if(this->cell[i - 1][j].getMapHeight() == this->cell[i + 1][j].getMapHeight() && this->cell[i][j].getMapHeight() != this->cell[i + 1][j].getMapHeight())
                            this->cell[i][j].setMapHeight(this->cell[i - 1][j].getMapHeight());
                    }
                }
            }
        }
    }

    //    // 再次修整地图边缘
    //    for(int i = 0; i < 72; i++)
    //    {
    //        for(int j = 0; j < 72; j++)
    //        {
    //            if(this->cell[i][j].getMapHeight() <= 3) this->cell[i][j].setMapHeight(3);
    //            else this->cell[i][j].setMapHeight(4);
    //        }
    //    }

    for(int i = 0; i < 72; i++)
    {
        for(int j = 0; j < 72; j++)
        {
            if(this->cell[i][j].getMapHeight() > 4) this->cell[i][j].setMapHeight(4);
            else if(this->cell[i][j].getMapHeight() < 3) this->cell[i][j].setMapHeight(3);
        }
    }

    // 测试：将地形高度存储进Gamemap[][]然后读取
    for(int i = 0; i < 72; i++)
    {
        for(int j = 0; j < 72; j++)
        {
            Gamemap[i][j] = this->cell[i][j].getMapHeight();
        }
    }

    return ;
}

void Map::GenerateType()
{
    /*
     * 坐标系映射如下：
     * —————→ y
     * |
     * |
     * ↓
     * x
     */
    for(int i = 0; i < MAP_L; i++)
    {
        for(int j = 0; j < MAP_U; j++)
        {
            if(i == 0 || i == 71 || j == 0 || j == 71)
            {
                this->cell[i][j].setMapType(MAPTYPE_FLAT);
                continue;
            }
            // 判断四个角与中间区块的高度差
            int heightDiffA = abs(this->cell[i][j].getMapHeight() - this->cell[i - 1][j - 1].getMapHeight()) + abs(this->cell[i][j].getMapHeight() - this->cell[i + 1][j - 1].getMapHeight()) + abs(this->cell[i][j].getMapHeight() - this->cell[i - 1][j + 1].getMapHeight()) + abs(this->cell[i][j].getMapHeight() - this->cell[i + 1][j + 1].getMapHeight());
            switch(heightDiffA)
            {
            case 1:
                if(this->cell[i + 1][j + 1].getMapHeight() - this->cell[i][j].getMapHeight() == 1)
                {
                    this->cell[i][j].setMapType(MAPTYPE_A3_UPTOR);
                }
                else if(this->cell[i + 1][j - 1].getMapHeight() - this->cell[i][j].getMapHeight() == 1)
                {
                    this->cell[i][j].setMapType(MAPTYPE_A0_UPTOD);
                }
                else if(this->cell[i - 1][j + 1].getMapHeight() - this->cell[i][j].getMapHeight() == 1)
                {
                    this->cell[i][j].setMapType(MAPTYPE_A2_UPTOU);
                }
                else if(this->cell[i - 1][j - 1].getMapHeight() - this->cell[i][j].getMapHeight() == 1)
                {
                    this->cell[i][j].setMapType(MAPTYPE_A1_UPTOL);
                }
                break;
            }

            // 判断四条临边与中间区块的高度差
            int heightDiffL = abs(this->cell[i][j].getMapHeight() - this->cell[i - 1][j].getMapHeight()) + abs(this->cell[i][j].getMapHeight() - this->cell[i + 1][j].getMapHeight()) + abs(this->cell[i][j].getMapHeight() - this->cell[i][j - 1].getMapHeight()) + abs(this->cell[i][j].getMapHeight() - this->cell[i][j + 1].getMapHeight());
            switch(heightDiffL)
            {
            case 2:
                /*
                    * X 0 X
                    * 0 # 1
                    * X 1 X
                    */
                if((this->cell[i + 1][j].getMapHeight() - this->cell[i][j].getMapHeight() == 1) && (this->cell[i][j + 1].getMapHeight() - this->cell[i][j].getMapHeight() == 1))
                {
                    this->cell[i][j].setMapType(MAPTYPE_A1_DOWNTOL);
                }
                /*
                     * X 1 X
                     * 1 # 0
                     * X 0 X
                     */
                else if((this->cell[i - 1][j].getMapHeight() - this->cell[i][j].getMapHeight() == 1) && (this->cell[i][j - 1].getMapHeight() - this->cell[i][j].getMapHeight() == 1))
                {
                    this->cell[i][j].setMapType(MAPTYPE_A3_DOWNTOR);
                }
                /*
                     * X 1 X
                     * 0 # 1
                     * X 0 X
                     */
                else if((this->cell[i - 1][j].getMapHeight() - this->cell[i][j].getMapHeight() == 1) && (this->cell[i][j + 1].getMapHeight() - this->cell[i][j].getMapHeight() == 1))
                {
                    this->cell[i][j].setMapType(MAPTYPE_A0_DOWNTOD);
                }
                /*
                     * X 0 X
                     * 1 # 0
                     * X 1 X
                     */
                else if((this->cell[i + 1][j].getMapHeight() - this->cell[i][j].getMapHeight() == 1) && (this->cell[i][j - 1].getMapHeight() - this->cell[i][j].getMapHeight() == 1))
                {
                    this->cell[i][j].setMapType(MAPTYPE_A0_DOWNTOD);
                }
                break;

            case 1:
                if(this->cell[i + 1][j].getMapHeight() - this->cell[i][j].getMapHeight() == 1)
                {
                    this->cell[i][j].setMapType(MAPTYPE_L3_UPTORD);
                }
                else if(this->cell[i - 1][j].getMapHeight() - this->cell[i][j].getMapHeight() == 1)
                {
                    this->cell[i][j].setMapType(MAPTYPE_L1_UPTOLU);
                }
                else if(this->cell[i][j + 1].getMapHeight() - this->cell[i][j].getMapHeight() == 1)
                {
                    this->cell[i][j].setMapType(MAPTYPE_L2_UPTORU);
                }
                else if(this->cell[i][j - 1].getMapHeight() - this->cell[i][j].getMapHeight() == 1)
                {
                    this->cell[i][j].setMapType(MAPTYPE_L0_UPTOLD);
                }
                break;
            }

            if(this->cell[i][j].getMapType() == 13) this->cell[i][j].setMapType(12);
            //            if(this->cell[i][j].getMapType() == 2) this->cell[i][j].setMapType(3);
            // 经验函数
            if(this->cell[i][j].getMapHeight() != 3)
            {
                this->cell[i][j].setOffsetY(-15);
            }
            if(this->cell[i][j].getMapType() == 2 || this->cell[i][j].getMapType() == 3 || this->cell[i][j].getMapType() == 4 || this->cell[i][j].getMapType() == 5 || this->cell[i][j].getMapType() == 8 || this->cell[i][j].getMapType() == 9)
            {
                this->cell[i][j].setOffsetY(-15);
            }
            if(this->cell[i][j].getMapType() == 13 && this->cell[i][j + 1].getMapType() == 13 && this->cell[i - 1][j].getMapType() == 13)
            {
                this->cell[i][j].setMapType(12);
            }
            //            if(this->cell[i][j].getMapType() == 3 && this->cell[i][j + 1].getMapType() == 3 && this->cell[i - 1][j].getMapType() == 3)
            //            {
            //                this->cell[i][j].setMapType(2);
            //            }
            if(this->cell[i][j].getMapType() == 2 && this->cell[i + 1][j] .getMapType() == 2)
            {
                this->cell[i][j].setMapType(3);
                qDebug() << "cell[" << i << "][" << j << "].getMapType() == " << 3;
            }
            if((this->cell[i][j].getMapType() == 2 && this->cell[i][j - 1] .getMapType() == 2) || (this->cell[i][j].getMapType() == 2 && this->cell[i + 1][j] .getMapType() == 2))
            {
                this->cell[i][j].setMapType(3);
                qDebug() << "cell[" << i << "][" << j << "].getMapType() == " << 3;
            }
            if((this->cell[i][j].getMapType() == 2 && this->cell[i + 1][j].getMapType() == 5 && this->cell[i][j - 1].getMapType() == 4) || (this->cell[i][j].getMapType() == 2 && this->cell[i + 1][j].getMapType() == 3 && this->cell[i][j - 1].getMapType() == 4))
            {
                this->cell[i][j].setMapType(3);
                qDebug() << "cell[" << i << "][" << j << "].getMapType() == " << 3;
            }
        }
    }
}

void Map::CalOffset()
{

    return ;
}

double Map::smooth(double x)
{
    return 3 * pow(x, 2) - 2 * pow(x, 3);
}

void Map::init(int MapJudge)
{
    for(int i=0;i<MAP_L;i++){
        for(int j=0;j<MAP_U;j++){
            this->cell[i][j].Num = 0;
            this->cell[i][j].Explored=true;
            //改地图可见度在这里
            this->cell[i][j].Visible=true;
            this->cell[i][j].setMapHeight(MAPHEIGHT_EMPTY);
        }
    }
    generateLandforms();
    generateCenter();
    generateResources();
    generateResource();
    GenerateTerrain();
    GenerateType();

    for(int i = 0; i < 72; i++)
    {
        for(int j = 0; j < 72; j++)
        {
            if(this->cell[i][j].getMapType() != 0 && this->cell[i][j].getMapType() != 1)
            {
                this->cell[i][j].Num = this->cell[i][j].getMapPattern() * 15 + this->cell[i][j].getMapType();
            }
            else if(this->cell[i][j].getMapType() == 1)
            {
                // 设置平地全部为草地
                this->cell[i][j].Num = MAPPATTERN_GRASS;
            }
            else if(this->cell[i][j].getMapType() == 0)
            {
                qDebug() << "第" << i << "行第" << j <<"列的地块MapType未定义！";
            }
            if(this->cell[i][j].Num >= 29) qDebug() << "cell[" << i << "][" << j << "].Num >= 29";
        }
    }

    if(MapJudge == 0) // 随机产生地图
    {
        std::ofstream outMapFile("tmpMap.txt");
        if (outMapFile.is_open())
        {
            for (int i = 0; i < MAP_L; i++)
            {
                for (int j = 0; j < MAP_U; j++)
                {
                    //                    outMapFile << Gamemap[i][j] << "\n";
                    outMapFile << this->cell[i][j].getMapHeight() << " ";
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
    else if(MapJudge == 1) // 读取上一次随机的地图
    {
        QFile inputGameFile("tmpMap.txt"); // 打开文本文件以读取数据

        if (inputGameFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&inputGameFile);
            int j = 0, i = 0;
            //            for (int T = 0; i < MAP_L * MAP_U; T++)
            //            {
            //                QString line = in.readLine();
            //                Gamemap[i][j] = line.toInt(); // 将每行的整数值存储到数组中
            //                j++;
            //                if(j >= MAP_U)
            //                {
            //                    j = 0;
            //                    i++;
            //                }
            //                if(i >= MAP_L)
            //                {
            //                    break;
            //                }
            //            }
            for(int i = 0; i < 72; i++)
            {
                for(int j = 0; j < 72; j++)
                {
                    this->cell[i][j].setMapHeight(Gamemap[i][j]);
                }
            }
            inputGameFile.close();
            qDebug() << "上一次的随机地图数据已读取到游戏内";
        }
        else
        {
            qDebug() << "无法打开上一次的随机地图文件";
        }
    }
    else if(MapJudge == 2)  // 读取确定的地图
    {
        QFile inputGameFile("gameMap.txt"); // 打开文本文件以读取数据

        if (inputGameFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&inputGameFile);
            int j = 0, i = 0;
            //            for (int T = 0; i < MAP_L * MAP_U; T++)
            //            {
            //                QString line = in.readLine();
            //                Gamemap[i][j] = line.toInt(); // 将每行的整数值存储到数组中
            //                j++;
            //                if(j >= MAP_U)
            //                {
            //                    j = 0;
            //                    i++;
            //                }
            //                if(i >= MAP_L)
            //                {
            //                    break;
            //                }
            //            }
            for(int i = 0; i < 72; i++)
            {
                for(int j = 0; j < 72; j++)
                {
                    this->cell[i][j].setMapHeight(Gamemap[i][j]);
                }
            }
            inputGameFile.close();
            qDebug() << "固定地图数据已读取到游戏内";
        }
        else qDebug() << "无法打开固定地图文件";

    }
}

