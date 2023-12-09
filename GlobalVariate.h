#ifndef GLOBALVARIATE_H
#define GLOBALVARIATE_H

#include <map>
#include <QPixmap>
#include <QString>
#include <list>
#include <QSound>
#include <config.h>
#include <QTextBrowser>
#include <QDebug>

using namespace std;
class Coordinate;

extern int g_selectNum;
extern std::map<int,Coordinate*> g_Object;

extern int MidX;
extern int MidY;
extern int MAP_LSide[2];
extern int MAP_USide[2];
extern int Gamemap[MAP_L][MAP_U]; // 地图二维数组
extern bool mapFlag[MAP_L][MAP_U]; // 地图标识二维数组，0为可放置，1为不可放置
extern int Forest[3][15][15];
extern int Food[5][5][5];
extern int Stone[5][5][5];
extern int g_frame;
extern QTextBrowser* g_DebugText;
extern int ProcessDataWork;

extern map<string, list<QPixmap>> resMap;
extern map<string, QSound*> SoundMap;

extern std::list<Coordinate*> drawlist;

struct tagBuilding
{
    int BlockL,BlockU;
    int Type;
    int Foundation;
    int SN;
    int Blood;
    int MaxBlood;
    int Percent;
    int Project;
    int ProjectPercent;
    int Cnt;
};

struct tagResource
{
    double L,U;
    int BlockL,BlockU;
    int Type;
    int SN;
    int ProductSort;
    int Cnt;
    int Blood;
};

struct tagHuman
{
    double L,U;
    int BlockL,BlockU;
    double L0,U0;
    int NowState;
    int WorkObjectSN;
    int Blood;
    int SN;
    int ResourceSort;
    int Resource;
};

struct tagGame
{
    tagBuilding *building;
    int building_n;
    tagHuman *human;
    int human_n;
    tagResource *resource;
    int resource_n;
    int GameFrame;
    int civilizationStage;
    int Wood;
    int Meat;
    int Stone;
    int Gold;
    int Human_MaxNum;
};

struct tagAction
{
    int A; // 操作种类
    int SN;
    int Action;
    double L0, U0;
    int BlockL, BlockU;
    int BuildingNum;
    int obSN;
};
extern std::map<int, tagAction> g_AiAction;

extern tagGame AIGame;
extern tagGame *p_AIGame;

extern std::string direction[5];

struct pixMemoryMap
{
    vector<char> MemoryMap;
    int width;
    int height;

    // 构造函数
    pixMemoryMap(int w, int h) : width(w), height(h) {
        // 分配内存图空间
        MemoryMap.resize(width *height);
    }

    pixMemoryMap() : width(0), height(0) { }

    pixMemoryMap(const pixMemoryMap &other) : width(other.width), height(other.height)
    {

        MemoryMap = other.MemoryMap;
    }

    pixMemoryMap& operator=(const pixMemoryMap &other)
    {
        width=other.width;
        height=other.height;

        MemoryMap = other.MemoryMap;

        return *this;
    }
    void setMemoryMap(int i, int j) {
        int index = i * height + j;
        MemoryMap[index] = 1;
    }

    char getMemoryMap(int i, int j) {
        int index = i * height + j;
        return MemoryMap[index];
    }

    void fillBlockMemoryMap()
    {
        for(int i=0;i<width/2;i++)
        {
            for(int j=0;j<height/2;j++)
            {
                if(j * width >= -height * (i - width/2))
                {
                    setMemoryMap(i,j);
                }
            }
        }
        for(int i=width/2;i<width;i++)
        {
            for(int j=0;j<height/2;j++)
            {
                if(j * width >= height * (i - width/2))
                {
                    setMemoryMap(i,j);
                }
            }
        }
        for(int i=0;i<width/2;i++)
        {
            for(int j=height/2;j<height;j++)
            {
                if(j * width <= height * (i + width/2))
                {
                    setMemoryMap(i,j);
                }
            }
        }
        for(int i=width/2;i<width;i++)
        {
            for(int j=height/2;j<height;j++)
            {
                if(j * width <= -height * i + 3* width * height/2)
                {
                    setMemoryMap(i,j);
                }
            }
        }
    }
};

struct ImageResource
{
    QPixmap pix;
    pixMemoryMap memorymap;

    ImageResource(QPixmap pix):pix(pix)
    {
        if (pix.isNull()) {
            // 图片未成功加载，执行错误处理操作
            qDebug()<<"fault";
        }
    }

    ImageResource()
    {

    }
};
// 定义二维点的结构体
struct Point {
    int x;
    int y;
};

/*
 * 0是成功
 * -1是SN不存在
 * -2是Action不存在
 * -3是位置超界
 * -4是obSN不存在
 * -5是BuildingNum不存在
 * -6是资源不足
 */

int InitImageResMap(QString path);
int InitSoundResMap(QString path);
QPixmap applyTransparencyEffect(const QPixmap& originalPixmap, qreal opacity);
void loadResource(std::string name,std::list<ImageResource> *targetlist);
void loadGrayRes(std::list<ImageResource>* res, std::list<ImageResource>* grayres);
void loadBlackRes(std::list<ImageResource>* res,std::list<ImageResource>* blackres);

void flipResource(std::list<ImageResource> *currentlist,std::list<ImageResource> *targetlist);
void initMemory(ImageResource *res);
double countdistance(double L,double U,double L0,double U0);
int calculateManhattanDistance(int x1, int y1, int x2, int y2);


#endif // GLOBALVARIATE_H
