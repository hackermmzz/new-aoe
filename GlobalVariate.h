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

extern int g_globalNum;
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

extern Coordinate *nowobject;
//当前选中对象
//出于gamewidget和core均需要获取当前访问对象

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

struct MouseEvent
{
    int memoryMapX;
    int memoryMapY;
    double DR;
    double UR;
    int mouseEventType;
    //鼠标点击类型 自定义对应关系 左键点击 左键拉框 右键点击等

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

struct relation_Object
{
    bool isExist , isUseAlterGoal=false;
    Coordinate* goalObject;     //目标对象
    int sort;
    int relationAct;
    int nowPhaseNum = 0;
    double DR_goal,UR_goal , DR_alter , UR_alter;

    relation_Object()
    {
        isExist = false;
    }

    relation_Object( Coordinate* goal , int eventClass)
    {
        isExist = true;
        goalObject = goal;
        relationAct = eventClass;
    }

    relation_Object(double DR_goal , double UR_goal , int eventClass )
    {
        isExist = true;
        goalObject = nullptr;
        this->DR_goal = DR_goal;
        this->UR_goal = UR_goal;
        relationAct = eventClass;
    }
};

struct conditionF
{
    int variableArgu;
    bool (*condition)( Coordinate* , relation_Object & , int);

    conditionF()
    {

    }

    conditionF( bool (*func)(Coordinate* , relation_Object & , int))
    {
        condition = func;
        variableArgu = OPERATECON_DEFAULT;
    }

    conditionF( bool (*func)(Coordinate* , relation_Object & , int) , int variableArgu)
    {
        condition = func;
        this->variableArgu = variableArgu;
    }
};

struct detail_EventPhase
{
    /*关于条件
    * 对所有条件：true 代表执行切换阶段操作
    * 所有条件list中条件均为“或”连接
    */
    /*一定需要：phaseAmount<= CoreDetailLinkMaxNum-2*/

    int phaseAmount = 0;    //阶段总数
    int phaseInterrup = CoreDetailLinkMaxNum-1;   //预设阶段 ， list中该位置存储强制中止行动的命令
    int phaseList[CoreDetailLinkMaxNum];      //core中，由此值指示进入函数
    conditionF chageCondition[CoreDetailLinkMaxNum-2];   //切换phase的判断条件
    map<int,int> changeLinkedList;  //关系链，
    list<conditionF>forcedInterrupCondition;//细节控制的强制中止条件

    map< int , list<conditionF> > loopOverCondition;  //loop的中止条件
    /**
    * 第一个键值：    loopEndPhase 为loop中阶段编号最大的阶段，    如：loop:1->2->3->1，loopEndPhase为3
    * 第二个值：      loop的中止条件。在loopEndPhase进行判断，条件链表中有一个为真即中止
    *               loop中止后将进入loop后的一个阶段。   如：loop:1->2->3->1，  中止条件在3进行判断，判断中止loop后，进入阶段4
    */


    detail_EventPhase()
    {
        phaseList[0] = CoreDetail_NormalEnd;
        phaseList[phaseInterrup] = CoreDetail_AbsoluteEnd;
    }

    detail_EventPhase( int phaseAmount , int* theList, conditionF* conditionList , list< conditionF >forcedInterrupCondition )
    {
        /** ********************************************************
         *构造函数
         *传入：    阶段总数 ， 阶段表 ， 各个阶段的切换条件表 ， 强制中止条件
         */
        this->phaseAmount = phaseAmount;
        for(int i = 0; i<phaseAmount; i++)
        {
            phaseList[i] = theList[i];
            chageCondition[i] = conditionList[i];
            changeLinkedList[i] = i+1;
        }
        this->forcedInterrupCondition = forcedInterrupCondition;

        phaseList[phaseAmount] = CoreDetail_NormalEnd;//中止细节操作
        phaseList[phaseInterrup] = CoreDetail_AbsoluteEnd;  //强制中止
    }

    bool setLoop( int loopBeginPhase , int loopEndPhase , list<conditionF>overCondition )
    {
        if( loopBeginPhase<loopEndPhase && loopBeginPhase>-1&& loopEndPhase <phaseAmount )
        {
            changeLinkedList[loopEndPhase] = loopBeginPhase;
            loopOverCondition[loopEndPhase] = overCondition;
            return true;
        }
        else return false;
    }

    bool isLoop( int phase){ return phase<phaseAmount && phase>changeLinkedList[phase]; }
    //判断phase是否是loopEndPhase，是否需要loop中止判断
    //由于map对未在表内的键值自己设初值，可能会造成bug

    void setEnd_Absolute(){ phaseList[phaseAmount] = CoreDetail_AbsoluteEnd; }
};

struct conditionDevelop
{
    int civilization;
    int sort_building;

    int need_Wood;
    int need_Food;
    int need_Stone;
    int need_Gold;


    conditionDevelop(){ }
    conditionDevelop( int civilization , int sort_building,int need_Wood,int need_Food, int need_Stone,int need_Gold )
    {
        this->civilization = civilization;
        this->sort_building = sort_building;
        this->need_Wood = need_Wood;
        this->need_Food = need_Food;
        this->need_Stone = need_Stone;
        this->need_Gold = need_Gold;
    }

    bool executable( int wood,int food,int stone,int gold ) { return wood>=need_Wood && food>=need_Food && stone>=need_Stone && gold>=need_Gold; }
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
