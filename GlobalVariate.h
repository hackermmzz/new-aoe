#ifndef GLOBALVARIATE_H
#define GLOBALVARIATE_H

#include <map>
#include <queue>
#include <QPixmap>
#include <QString>
#include <list>
#include <QSound>
#include <config.h>
#include <QTextBrowser>
#include <QDebug>
#include <QMutex>

using namespace std;
class Coordinate;
extern bool AIfinished;
extern bool INSfinshed;
extern int g_globalNum;
extern std::map<int,Coordinate*> g_Object;

extern int MidX;
extern int MidY;
extern int MAP_LSide[2];
extern int MAP_USide[2];
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

struct st_DebugMassage{
    QString color;
    QString content;
    st_DebugMassage(){}
    st_DebugMassage( QString color, QString content)
    {
        this->color = color;
        this->content = content;
    }
};

extern std::queue<st_DebugMassage>debugMassagePackage;


struct tagBuilding
{
    int BlockL,BlockU;
    int Type;
    int SN;
    int Blood;
    int MaxBlood;
    int Percent;
    int Project;
    int ProjectPercent;
    int Cnt;
    int Owner;
    tagBuilding toEnemy(){
        this->Cnt=-1;
        this->Project=-1;
        this->ProjectPercent=-1;
        return *this;
    }
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
    double L0,U0;   // 目的地
    int NowState;
    int WorkObjectSN;
    int Blood;
    int SN;
    int Owner;
    void cast_from(tagHuman taghuman){
        this->L=taghuman.L;
        this->U=taghuman.U;
        this->BlockL=taghuman.BlockL;
        this->BlockU=taghuman.BlockU;
        this->L0=taghuman.L0;
        this->U0=taghuman.U0;
        this->NowState=taghuman.NowState;
        this->WorkObjectSN=taghuman.WorkObjectSN;
        this->Blood=taghuman.Blood;
        this->SN=taghuman.SN;
        this->Owner=taghuman.Owner;
    }
};
struct tagFarmer:public tagHuman
{
    int ResourceSort;
    int Resource;
    tagFarmer toEnemy(){
        Resource=-1;
        L0=-1.0;
        U0=-1.0;
        return *this;
    }
};

struct tagArmy:public tagHuman
{
    int Sort;   //军队种类
    tagArmy toEnemy(){
        L0=-1.0;
        U0=-1.0;
        return *this;
    }
};
struct tagBlock{
    bool explored=false;
    int height=0;
};

struct instruction;
struct tagGame
{
    vector<tagBuilding> buildings;
    int buildings_n;
    vector<tagFarmer> farmers;
    int farmers_n;
    vector<tagArmy> armies;
    int armies_n;
    vector<tagBuilding> enemy_buildings;
    int enemy_buildings_n;
    vector<tagFarmer> enemy_farmers;
    int enemy_farmers_n;
    vector<tagArmy> enemy_armies;
    int enemy_armies_n;
    vector<tagResource> resources;
    int resources_n;
    map<int,instruction> ins_ret;
    tagBlock blocks[MAP_L][MAP_U];
    int GameFrame;
    int civilizationStage;
    int Wood;
    int Meat;
    int Stone;
    int Gold;
    int Human_MaxNum;
    ~tagGame(){
        for (int i = 0; i < MAP_L; ++i) {
            delete[] blocks[i];
        }
        delete[] blocks;

        buildings.clear();
        farmers.clear();
        armies.clear();
        enemy_buildings.clear();
        enemy_farmers.clear();
        enemy_armies.clear();
        resources.clear();
        ins_ret.clear();
    }
};

struct tagMap
{
    bool explore;
    int high;

    //该位置资源信息
    int type;       //资源种类（浆果、树等）
    int ResType;    //采集获得的资源种类（食物、木头等） human_WOOD...
    int fundation;  //该资源占地图大小
    int SN;
    int remain;     //剩余资源量

    //*********************************
    tagMap(){ clear(); }
    void clear()
    {
        explore = false;
        high = -1;
        clear_r();
    }

    void clear_r()
    {
        type = -1;
        ResType = -1;
        fundation = -1;
        SN = -1;
        remain = -1;
    }
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

    Point(){}
    Point( int x , int y ){ this->x = x,this->y = y; }
    Point( const Point& board ){ x = board.x , y = board.y; }

    Point operator +(const Point& ps ) { return Point(x+ps.x , y+ps.y); }
    Point operator -(const Point& ps){ return Point(x - ps.x, y-ps.y); }
    bool operator ==(const Point& ps)const { return ps.x == x && ps.y == y; }
    bool operator < (const Point& ps)const { return x<ps.x&& y<ps.y; }
};

struct conditionDevelop
{
    int civilization;
    int sort_building;  //所属建筑

    double times_second;

    int acttimes = 0;    //表示执行的此数

    bool isCreatObjectAction = false; //行动结束后是否需要创建对象
    int creatObjectSort = -1;   //需要创建对象的类sort
    int creatObjectNum = -1;    //需要创建对象的类中的Num

    //记录前置条件
    list<conditionDevelop*> preCondition;

    //本行动对应的下一项行动
    conditionDevelop* nextDevAction = NULL;

    //需要的物资
    int need_Wood ,need_Food ,need_Stone , need_Gold;

    //**********************************************************
    //构造器 ， 创建conditionDevelop实例相关
    conditionDevelop(){ }
    conditionDevelop( int civilization , int sort_building, double needTimes ,  int need_Wood = 0, int need_Food = 0, int need_Stone = 0 , int need_Gold = 0 )
    {
        this->civilization = civilization;
        this->sort_building = sort_building;
        this->need_Wood = need_Wood;
        this->need_Food = need_Food;
        this->need_Stone = need_Stone;
        this->need_Gold = need_Gold;
        this->times_second = needTimes;
    }

    //添加该行动的前置行动
    void addPreCondition(conditionDevelop* con_need){ preCondition.push_back(con_need);  }

    //添加行动结束后创建对象
    void setCreatObjectAfterAction(int creatSort)
    {
        isCreatObjectAction = true;
        creatObjectSort = creatSort;
    }
    void setCreatObjectAfterAction( int creatSort , int creatNum )
    {
        setCreatObjectAfterAction(creatSort);
        creatObjectNum = creatNum;
    }

    //**********************************************************
    //判断行动是否能进行
    //判断资源是否满足行动需要
    bool executable( int wood,int food,int stone,int gold ) { return wood>=need_Wood && food>=need_Food && stone>=need_Stone && gold>=need_Gold; }
    void get_needResource( int& wood, int& food ,int& stone, int& gold ){ wood = need_Wood, food = need_Food,stone = need_Stone, gold = need_Gold; }

    //判断当前时代、已完成的建筑行动，是否解锁当前行动
    bool isShowable( int nowcivilization )
    {
        if(civilization>nowcivilization) return false;

        for(list<conditionDevelop*>::iterator iter = preCondition.begin(); iter!= preCondition.end(); iter++)
            if(!(*iter)->acttimes) return false;

        return true;
    }

    //**********************************************************
    //行动结束后相关操作
    void finishAct(){ acttimes++; }

    //获取是否需要创建对象
    bool isNeedCreatObject( int& creatSort , int& creatNum )
    {
        creatSort = creatObjectSort;
        creatNum = creatObjectNum;
        return isCreatObjectAction;
    }

    bool isNeedCreatObject(){ return isCreatObjectAction; }

    int getActTimes(){ return acttimes; }
};

struct st_upgradeLab{
    conditionDevelop *headAct = NULL , *nowExecuteNode = NULL , *endNode = NULL;
    int haveFinishedPhaseNum = 0;

    st_upgradeLab(){}
    ~st_upgradeLab()
    {
        while(headAct!=endNode)
        {
            nowExecuteNode = headAct;
            headAct = headAct->nextDevAction;
            delete nowExecuteNode;
        }
        delete endNode;
    }

    void setHead(conditionDevelop* head) { endNode = nowExecuteNode = headAct= head; }

    void push_back( conditionDevelop* node )
    {
        endNode->nextDevAction = node;
        endNode = node;
    }

    //设置建筑的行动没有下一个行动，但该行动可以重复地执行
    void endNodeAsOver(){ endNode->nextDevAction = endNode; }

    //切换
    void shift()
    {
        if(nowExecuteNode!=NULL)
        {
            haveFinishedPhaseNum++;
            nowExecuteNode = nowExecuteNode->nextDevAction;
        }
    }

    //当前行动是否可在SelectWidget中显示
    bool isShowAble( int nowcivilization )
    {
        if(nowExecuteNode == NULL) return false;
        else return nowExecuteNode->isShowable(nowcivilization);
    }

    //当前行动是否可执行
    bool executable( int nowcivilization,int wood,int food,int stone,int gold ){ return isShowAble(nowcivilization) && nowExecuteNode->executable(wood , food, stone,gold) ; }

    //获取当前行动列表执行过几轮（一个链node算一轮）
    int getPhaseTimes(){return this->haveFinishedPhaseNum;}
    //获取需要的资源
    void get_needResource( int& wood, int& food ,int& stone, int& gold )
    {
        if(nowExecuteNode!=NULL)
            nowExecuteNode->get_needResource(wood,food,stone,gold);
        else wood = 0,food =0 ,stone = 0,gold = 0;
    }
    /**
    *考虑加入错误码，以判断错误类型
    */

    bool isNeedCreatObject(){
        if(nowExecuteNode!=NULL) return nowExecuteNode->isNeedCreatObject();
        else return false;
    }
};

struct st_buildAction
{
    //建筑的建造条件
    conditionDevelop* buildCon = NULL;

    //存储该建筑拥有哪些行动。第一键值为行动标号，第二键值为对应的行动表，行动表为链表，存储了执行条件
    map<int , st_upgradeLab> actCon;

    st_buildAction(){}
    ~st_buildAction()
    {
        if(buildCon!=NULL)
        {
            delete buildCon;
            buildCon = NULL;
        }
    }

    void finishBuild(){ buildCon->finishAct();}
    void finishAction(int actNum)
    {
        actCon[actNum].nowExecuteNode->finishAct();
        actCon[actNum].shift();
    }
};

struct instruction{
    ///用于存储ai发出的指令信息
    /// @param type 指令类型
    /// @param option 对应类型下的操作
    /// type 0:终止对象self的动作
    /// type 1:命令村民self走向指定坐标L0，U0
    /// type 2:将obj对象设定为村民self的工作对象，村民会自动走向对象并工作
    /// type 3:命令村民self在块坐标BlockL,BlockU处建造类型为option的新建筑
    /// type 4:对建筑self发出命令option
    int ret=-1;
    int type;
    int id;
    Coordinate* self;
    Coordinate* obj;
    int option;
    int BL,BU;
    double L,U;
    bool isExist(){
        return type!=-1;
    }
    instruction(){ type=-1; }
    instruction(int type,Coordinate* self,Coordinate* obj);
    instruction(int type,Coordinate* self,int BL,int BU,int option);
    instruction(int type,Coordinate* self,double L,double U);
    instruction(int type,Coordinate* self,int option);
};
struct ins{
    int g_id=0;
    std::queue<instruction> instructions;
    QMutex lock;
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
bool isNear_Manhattan( double dr , double ur , double dr1  , double ur1 , double distance );

int calculateManhattanDistance(int x1, int y1, int x2, int y2);
double calculateManhattanDistance(double x1, double y1, double x2, double y2);

void calMirrorPoint( double& dr , double &ur , double dr_mirror, double ur_mirror , double dis);

double trans_BlockPointToDetailCenter( int p );

void call_debugText(QString color, QString content);


#endif // GLOBALVARIATE_H
