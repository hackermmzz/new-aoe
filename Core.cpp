#include "Core.h"

bool condition_AllFalse( Coordinate* object1, Coordinate* object2 , int operate){return false;}
bool condition_UniObjectDie( Coordinate* , Coordinate* , int );
bool condition_UniObjectUnderAttack( Coordinate* , Coordinate* , int );
bool condition_UniObject_FullBackpack( Coordinate* , Coordinate* , int );
bool condition_ObjectNearby( Coordinate* , Coordinate* , int );

Core::Core()
{
    int *phaseList;
    conditionF* conditionList;
    list<conditionF>forcedInterrupCondition;
    list<conditionF>overCondition;

    //添加静态表
    //行动：只移动
    phaseList = new int(CoreDetail_Move);
    conditionList = new ( conditionF )( conditionF(condition_ObjectNearby , OPERATECON_NEAR_ABSOLUTE) );
    forcedInterrupCondition.push_back(conditionF(condition_UniObjectDie , OPERATECON_OBJECT1));
    forcedInterrupCondition.push_back(conditionF(condition_UniObjectUnderAttack , OPERATECON_OBJECT1));

    relation_Event_Staic[CoreEven_JustMoveTo] = detail_EventPhase( 1 ,  phaseList, conditionList , forcedInterrupCondition );
    delete phaseList;
    delete conditionList;
    forcedInterrupCondition.clear();

    //行动：采集

}

void Core::gameUpdate(Map* map, Player* player[], int** memorymap, MouseEvent *mouseEvent)
{


    std::list<Human *>::iterator humaniter=player[0]->human.begin();
    while(humaniter!=player[0]->human.end())
    {
        Farmer *farmer=(Farmer *)(*humaniter);
        if(farmer->needTranState())
        {
            farmer->setNowState(farmer->getPreState());
            farmer->setPreStateIsIdle();
            farmer->setNowRes();
        }
        humaniter++;
    }

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
    std::list<Human *>::iterator hiter=player[0]->human.begin();
    while(hiter!=player[0]->human.end())
    {
        (*hiter)->nextframe();
        (*hiter)->updateLU();
        hiter++;
    }

    if(mouseEvent->mouseEventType!=NULL_MOUSEEVENT)
    {
        if(mouseEvent->mouseEventType==LEFT_PRESS)
        {
            nowobject=g_Object[memorymap[mouseEvent->memoryMapX][mouseEvent->memoryMapY]];

            mouseEvent->mouseEventType=NULL_MOUSEEVENT;
        }
        if(mouseEvent->mouseEventType==RIGHT_PRESS&&nowobject!=NULL)
        {
            if(nowobject->getSort()==SORT_FARMER)
            {
                Farmer *farmer=(Farmer *)nowobject;
                if(!farmer->isWalking())
                    farmer->setPreWalk();
                farmer->setdestination(mouseEvent->DR,mouseEvent->UR);
                Point start,destination;
                start.x=tranBlockDR(farmer->getDR());
                start.y=tranBlockUR(farmer->getUR());
                destination.x=tranBlockDR(farmer->getDR0());
                destination.y=tranBlockUR(farmer->getUR0());
                map->loadfindPathMap();
                farmer->setPath(findPath(map->findPathMap,map,start,destination));

                mouseEvent->mouseEventType=NULL_MOUSEEVENT;
            }
        }
    }

    manageRelationList();
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

//****************************************************************************************
//
//对人，命令不必手动中止，直接变更；对建筑，命令变更将被无视，必须手动中止当前命令后再下达新命令。
bool Core::addRelation( Coordinate & object1, Coordinate & object2, int eventType)
{
    if(! relate_AllObject[&object1].isExist )
    {
        relate_AllObject[&object1] = relation_Object(&object2 , eventType);
        return true;
    }

    return false;
}

void Core::suspendRelation(Coordinate & object)
{
    if(relate_AllObject[&object].isExist) relate_AllObject.erase(&object);
}

void Core::manageRelationList()
{
    Coordinate* object1;
    Coordinate* object2;
    relation_Object thisRelation;
    detail_EventPhase thisDetailEven;
    int nowPhaseNum;
    bool needForcedInter;
    list<conditionF> forcedInterrupCondition , overCondition;

    for( map<Coordinate* , relation_Object>::iterator iter = relate_AllObject.begin() ; iter!=relate_AllObject.end() ;)
    {
        object1 = iter->first;
        thisRelation = iter->second;
        if( thisRelation.isExist )
        {
            object2 = thisRelation.goalObject;
            nowPhaseNum = thisRelation.nowPhaseNum;
            thisDetailEven = relation_Event_Staic[nowPhaseNum];
            needForcedInter = false;

            forcedInterrupCondition = thisDetailEven.forcedInterrupCondition;
            for(list<conditionF>::iterator iter_list = forcedInterrupCondition.begin();iter_list!=forcedInterrupCondition.end();iter_list++)
            {
                if( iter_list->condition( object1,object2,iter_list->variableArgu ) )
                {
                    needForcedInter = true;
                    break;
                }
            }
            if( needForcedInter )
            {
                iter = relate_AllObject.erase(iter);
                continue;
            }

            if( thisDetailEven.isLoop(nowPhaseNum) )
            {
                overCondition = thisDetailEven.loopOverCondition[nowPhaseNum];
                for(list<conditionF>::iterator iter_list = overCondition.begin(); iter_list!= overCondition.end();iter_list++)
                {
                    if( iter_list->condition( object1 , object2 , iter_list->variableArgu ))
                    {
                        nowPhaseNum ++;
                        iter->second.nowPhaseNum = nowPhaseNum;
                        break;
                    }
                }
            }

            if( thisDetailEven.chageCondition[nowPhaseNum].condition(object1, object2 , thisDetailEven.chageCondition[nowPhaseNum].variableArgu))
            {
                nowPhaseNum = thisDetailEven.changeLinkedList[nowPhaseNum];
                iter->second.nowPhaseNum = nowPhaseNum;
            }

            switch (thisDetailEven.phaseList[nowPhaseNum])
            {
            case CoreDetail_Move:

                break;
            case CoreDetail_Attack:

                break;
            case CoreDetail_Gather:
                break;
            case CoreDetail_NormalEnd:
                break;
            default:
                break;
            }
            iter++;
        }
    }
}

//****************************************************************************************
//通用的关系函数
//
bool condition_UniObjectDie( Coordinate* object1, Coordinate* object2 , int opreate)
{
    return false;

}


bool condition_UniObjectUnderAttack( Coordinate* object1, Coordinate* object2, int operate )
{
    return false;
}

bool condition_UniObject_FullBackpack( Coordinate* object1 , Coordinate* object2, int operate = OPERATECON_OBJECT1)
{
    if(operate == OPERATECON_OBJECT1)
    {
        return false;


    }
}

bool condition_ObjectNearby( Coordinate* object1, Coordinate* object2, int operate = OPERATECON_NEAR_ABSOLUTE)
{
    if(operate == OPERATECON_NEAR_ABSOLUTE)
    {
        return false;
    }


}
