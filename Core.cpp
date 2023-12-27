#include "Core.h"

Core::Core()
{

}

void Core::gameUpdate(Map* map, Player* player[], int** memorymap, MouseEvent *mouseEvent)
{
    std::list<Animal*>::iterator animaliter=map->animal.begin();
    while(animaliter!=map->animal.end())
    {
        (*animaliter)->nextframe();
        animaliter++;
    }
    std::list<StaticRes*>::iterator SRiter=map->staticres.begin();
    while(SRiter!=map->staticres.end())
    {
        (*SRiter)->nextframe();
        SRiter++;
    }
}

bool Core::isValidPoint(const int (&map)[MAP_L][MAP_U], const Point &p)
{
    int rows = MAP_L;
    int cols = MAP_U;
    return (p.x >= 0 && p.x < rows && p.y >= 0 && p.y < cols);
}

vector<Point> Core::getAdjacentPoints(const int (&map)[MAP_L][MAP_U], const Point &p)
{
    vector<Point> adjacentPoints;

    // 八个相邻正方向的偏移量
    int dx[] = { -1, 1, 0, 0, -1, -1, 1, 1 };
    int dy[] = { 0, 0, -1, 1, -1, 1, -1, 1 };

    for (int i = 0; i < 8; i++) {
        int newX = p.x + dx[i];
        int newY = p.y + dy[i];
        Point newPoint = { newX, newY };
        if (isValidPoint(map, newPoint) && map[newX][newY] != 1) {
            if(abs(dx[i])+abs(dy[i])==2)
            {
                if(map[newX][p.y]!=1&&map[p.x][newY]!=1)
                {
                    adjacentPoints.push_back(newPoint);
                }
            }
            else adjacentPoints.push_back(newPoint);
        }
    }
    return adjacentPoints;
}

stack<Point> Core::findPath(const int (&findPathMap)[MAP_L][MAP_U], Map *map, const Point &start, const Point &destination)
{
    int rows = 72;
    int cols = 72;

    // 记录已访问的点
    vector<vector<bool>> visited(rows, vector<bool>(cols, false));

    // 使用map记录每个点的前驱点，以便回溯路径
    vector<vector<Point>> prev(rows, vector<Point>(cols));

    // 使用queue保存路径
    queue<Point> q;

    // 广度优先搜索
    q.push(start);
    visited[start.x][start.y] = true;

    while (!q.empty()) {
        Point current = q.front();
        q.pop();

        // 找到目标点，回溯路径
        if (current.x == destination.x && current.y == destination.y) {
            stack<Point> pathStack;
            while (!(current.x == start.x && current.y == start.y)) {
                pathStack.push(current);
                current = prev[current.x][current.y];
            }
            return pathStack;
        }

        vector<Point> adjacentPoints = getAdjacentPoints(findPathMap, current);
        for (const Point& next : adjacentPoints) {
            if (!visited[next.x][next.y]) {
                visited[next.x][next.y] = true;
                q.push(next);
                prev[next.x][next.y] = current;
            }
        }
    }

    return findPathAlternative(map->intmap,start,destination);
}

stack<Point> Core::findPathAlternative(const int (&map)[MAP_L][MAP_U], const Point &start, const Point &destination)
{
    int rows = 72;
    int cols = 72;

    // 记录已访问的点
    vector<vector<bool>> visited(rows, vector<bool>(cols, false));

    // 使用map记录每个点的前驱点，以便回溯路径
    vector<vector<Point>> prev(rows, vector<Point>(cols));

    // 使用queue保存路径
    queue<Point> q;

    // 广度优先搜索
    q.push(start);
    visited[start.x][start.y] = true;

    while (!q.empty()) {
        Point current = q.front();
        q.pop();

        // 找到目标点，回溯路径
        if (current.x == destination.x && current.y == destination.y) {
            stack<Point> pathStack;
            while (!(current.x == start.x && current.y == start.y)) {
                pathStack.push(current);
                current = prev[current.x][current.y];
            }
            return pathStack;
        }

        vector<Point> adjacentPoints = getAdjacentPoints(map, current);
        for (const Point& next : adjacentPoints) {
            if (!visited[next.x][next.y]) {
                visited[next.x][next.y] = true;
                q.push(next);
                prev[next.x][next.y] = current;
            }
        }
    }

    return stack<Point>();
}
