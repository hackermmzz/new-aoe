#ifndef GLOBALVARIATE_H
#define GLOBALVARIATE_H

#include <QMutex>
#include <QSoundEffect>
#include "EventFilter.h"
#include "config.h"
#include<QNetworkAccessManager>
#include"networkplugin.h"
#include<QNetworkReply>
#include "RuntimeConfig.h"
#include "library/random/random.hpp"

using namespace std;
class Coordinate;
//
extern Random Rand;
extern QString ResultLogFile;
extern EventFilter *eventFilter;
extern bool tryCaptured;
extern bool is_cheatAction;
extern NetworkPlugin*NetworkManager;
//
extern bool AIfinished;
extern bool INSfinshed;
extern int g_globalNum;
extern int mapmoveFrequency;
extern std::map<int, Coordinate*> g_Object;
extern int** memorymap;
extern int MidX;
extern int MidY;
extern int MAP_LSide[2];
extern int MAP_USide[2];
extern int Forest[3][15][15];
extern int Food[5][5][5];
extern int Stone[5][5][5];
extern int g_frame;
extern QTextBrowser* g_DebugText;
extern map<string, list<QPixmap>> resMap;
extern map<string, QSoundEffect*> SoundMap;
extern std::queue<string> soundQueue;

extern std::list<Coordinate*> drawlist;

extern Coordinate* nowobject;
extern Coordinate* LeftMouseObjCapture;
extern Coordinate*RightMouseObjCaptrue;
extern bool GenerateHumanLock;//
//每一帧传输到输出日志的结构体
struct ResultLogInfo{
    bool win;
    int wood;
    int food;
    int gold;
    int stone;
    int score;
    string msg;
    ResultLogInfo(bool win_,int score_,int wood_,int food_,int gold_,int stone_,string msg_="");
    void LogOut();
    QString ToString();
};
//当前选中对象
//出于gamewidget和core均需要获取当前访问对象

struct st_DebugMassage {
    QString color;
    QString content;
    st_DebugMassage() {}
    st_DebugMassage(QString color, QString content);
};
void call_debugText(QString color, QString content, int playerID);

extern std::queue<st_DebugMassage>debugMassagePackage;
extern std::map<QString, int>debugMessageRecord;

struct Score {
private:
    int id;
    int score;
    int scoreTypes[SCORE_TYPE_COUNT] = { 0 };

    void addScore(int points, const QString& message);

public:
    Score(int id);
    int getScore();
    void update(int type, int num = 1, bool isConversion = false);
};
Score& scoreForPlayerRepresent(int playerRepresent);
/***********************************************结构体，主要要保证相同架构的平台的不同编译器内存对齐问题******************************/

struct tagObj{
    int32_t SN;// 序列号
    int32_t BlockDR, BlockUR; //区块坐标
    bool operator <(const tagObj&obj)const;
};

struct tagBuilding:tagObj
{
    int32_t Type; // 建筑类型
    int32_t Blood; // 当前血量
    int32_t MaxBlood; // 最大血量
    int32_t Percent; // 完成百分比
    int32_t Project; // 当前项目
    int32_t ProjectPercent; // 项目完成百分比
    int32_t Cnt; // 剩余资源量（仅农田）
    tagBuilding toEnemy();
};

struct tagResource:tagObj
{
    double DR, UR; //细节坐标
    int32_t Type; // 资源类型
    int32_t ProductSort; // 产品种类
    int32_t Cnt; // 剩余资源数量
    int32_t Blood; // 当前血量
};

struct tagHuman:tagObj
{
    double DR, UR; //细节坐标
    double DR0, UR0; // 目的地坐标
    int32_t NowState; // 当前状态
    int32_t WorkObjectSN; // 工作对象序列号
    int32_t Blood; // 当前血量
    int32_t attack; // 攻击力
    int32_t rangedDefense; // 远程防御
    int32_t meleeDefense; // 近战防御
    void cast_from(tagHuman taghuman);
};

struct tagFarmer : public tagHuman
{
    int32_t ResourceSort; // 手持资源种类
    int32_t Resource; // 手持资源数量
    int32_t FarmerSort;//农民的类型
    tagFarmer toEnemy();
};

struct tagArmy : public tagHuman
{
    int32_t Sort; // 军队种类
    int32_t status;
    int32_t starttime;
    int32_t finishtime;
    double startpointDR;
    double startpointUR;
    double destinaDR;
    double destinaUR;
    bool ifAttack;
    int32_t timelock;
    int32_t ConvertCooldown; // 祭司剩余转换冷却（毫秒）；0表示可转换，敌方视角为-1
    tagArmy toEnemy();
};


struct instruction {
    ///用于存储ai发出的指令信息
    /// @param type 指令类型
    /// @param option 对应类型下的操作
    /// type 0:终止对象self的动作
    /// type 1:命令村民self走向指定坐标L0，U0
    /// type 2:将obj对象设定为村民self的工作对象，村民会自动走向对象并工作
    /// type 3:命令村民self在块坐标BlockL,BlockU处建造类型为option的新建筑
    /// type 4:对建筑self发出命令option
    int32_t ret = -1;
    int32_t type;
    int32_t id;
    Coordinate* self;
    Coordinate* obj;
    int32_t option;
    int32_t BlockDR, BlockUR;
    int32_t SN = -1, obSN = -1;
    Double DR, UR;
    bool isExist();
    instruction() { type = -1; }
    instruction(int type, int SN, int obSN, bool twoCoredinate);
    instruction(int type, int SN, int BlockDR, int BlockUR, int option);
    instruction(int type, int SN, Double DR, Double UR);
    instruction(int type, int SN, int option);
};

struct ins {
    int32_t g_id = 0;
    std::queue<instruction> instructions;
    QMutex lock;
};

struct tagTerrain {
    int32_t height;
    int32_t type;
};
// 定义二维点的结构体
struct Point {
    int32_t x;
    int32_t y;
    Point();
    Point(int x, int y);
    Point(const Point& board);
    Point operator +(const Point& ps);
    Point operator -(const Point& ps);
    bool operator ==(const Point& ps)const;
    bool operator < (const Point& ps)const;
};

struct tagInfo
{
    using TerrainData=const vector<vector<tagTerrain>>;
    vector<tagBuilding> buildings; // 我方建筑列表
    vector<tagFarmer> farmers; // 我方农民列表
    vector<tagArmy> armies; // 我方军队列表
    vector<tagBuilding> enemy_buildings; // 敌方建筑列表
    vector<tagFarmer> enemy_farmers; // 敌方农民列表
    vector<tagArmy> enemy_armies; // 敌方军队列表
    vector<tagResource> resources; // 资源列表
    map<int, int> ins_ret; // 指令返回值，map<id, ret>
    TerrainData*theMap;// 高程图
    vector<Point>exploredUpdate;//多探索的区域
    int32_t GameFrame; // 游戏帧数
    int32_t civilizationStage; // 文明阶段
    int32_t Wood; // 木材数量
    int32_t Meat; // 肉类数量
    int32_t Stone; // 石头数量
    int32_t Gold; // 黄金数量
    int32_t Human_MaxNum; // 最大人口数量
    // Assignment operator
    tagInfo& operator=(const tagInfo& other);

    void clear();
};

/***********************************************结构体，主要要保证相同架构的平台的不同编译器内存对齐问题******************************/

struct tagGame
{
private:
    tagInfo* Info;
    QMutex Locker;
public:
    template<class T>
    void WLHHunYao(vector<T>& v) {
        vector<T> res = v;
        static auto randint = [](int a, int b)->int {return Rand.nextRaw() % (b - a + 1) + a;};
        for (int i = 0;i < v.size();++i) {
            int idx = randint(0, res.size() - 1);
            swap(res.back(), res[idx]);
            v[i] = res.back();
            res.pop_back();
        }
    }
    void update(tagInfo* newinfo);
    bool tryLock();
    void release();
    void insertInsRet(int id, instruction ins);
    tagInfo getInfo();
    void clearInsRet();
};

struct MouseEvent
{
private:
    int memoryMapX;
    int memoryMapY;
    Double DR;
    Double UR;
    int mouseEventType;
public:
    //鼠标点击类型 自定义对应关系 左键点击 左键拉框 右键点击等
    MouseEvent();
    int GetMouseEventType();
    void SetMouseEventType(int tp);
    bool HaveEvent();
    int GetMemoryMapX();
    int GetMemoryMapY();
    void SetMemoeyMapX(int v);
    void SetMemoryMapY(int v);
    Double GetDR();
    Double GetUR();
    void SetDR(Double v);
    void SetUR(Double v);
    void Reset();
};


extern std::string direction[5];

struct pixMemoryMap
{
    vector<char> MemoryMap;
    int width;
    int height;

    // 构造函数
    pixMemoryMap(int w, int h);

    pixMemoryMap();

    pixMemoryMap(const pixMemoryMap& other);

    pixMemoryMap& operator=(const pixMemoryMap& other);
    void setMemoryMap(int i, int j);

    char getMemoryMap(int i, int j);

    void fillBlockMemoryMap();
};

struct ImageResource
{
    QPixmap pix;
    pixMemoryMap memorymap;

    ImageResource(QPixmap pix);

    ImageResource();
};


struct conditionDevelop
{
    int civilization;
    int sort_building;  //所属建筑

    Double times_second;

    int acttimes = 0;    //表示执行的此数

    bool isCreatObjectAction = false; //行动结束后是否需要创建对象
    int creatObjectSort = -1;   //需要创建对象的类sort
    int creatObjectNum = -1;    //需要创建对象的类中的Num

    //记录前置条件
    list<conditionDevelop*> preCondition;

    //本行动对应的下一项行动
    conditionDevelop* nextDevAction = NULL;

    //需要的物资
    int need_Wood, need_Food, need_Stone, need_Gold;

    //**********************************************************
    //构造器 ， 创建conditionDevelop实例相关
    conditionDevelop();
    conditionDevelop(int civilization, int sort_building, Double needTimes, int need_Wood = 0, int need_Food = 0, int need_Stone = 0, int need_Gold = 0);

    //添加该行动的前置行动
    void addPreCondition(conditionDevelop* con_need);

    //添加行动结束后创建对象
    void setCreatObjectAfterAction(int creatSort);
    void setCreatObjectAfterAction(int creatSort, int creatNum);

    //**********************************************************
    //判断行动是否能进行
    //判断资源是否满足行动需要
    bool executable(int wood, int food, int stone, int gold);
    void get_needResource(int& wood, int& food, int& stone, int& gold);

    //判断当前时代、已完成的建筑行动，是否解锁当前行动
    bool isShowable(int nowcivilization);

    //**********************************************************
    //行动结束后相关操作
    void finishAct();

    //获取是否需要创建对象
    bool isNeedCreatObject(int& creatSort, int& creatNum);

    bool isNeedCreatObject();

    int getActTimes();
};

struct st_upgradeLab {
    conditionDevelop* headAct = NULL, * nowExecuteNode = NULL, * endNode = NULL;
    int haveFinishedPhaseNum = 0;
    bool nowExecuting = false;

    st_upgradeLab();
    ~st_upgradeLab();

    void setHead(conditionDevelop* head);

    void push_back(conditionDevelop* node);

    //设置建筑的行动没有下一个行动，但该行动可以重复地执行
    void endNodeAsOver();

    //切换
    void shift();

    //当前行动是否可在SelectWidget中显示
    bool isShowAble(int nowcivilization);

    //当前行动是否可执行
    bool executable(int nowcivilization, int wood, int food, int stone, int gold);

    void beginExecute();
    void overExecute();

    //获取当前行动列表执行过几轮（一个链node算一轮）
    int getPhaseTimes();
    //获取需要的资源
    void get_needResource(int& wood, int& food, int& stone, int& gold);
    /**
    *考虑加入错误码，以判断错误类型
    */

    bool isNeedCreatObject();
};

struct st_buildAction
{
    //建筑的建造条件
    conditionDevelop* buildCon = NULL;

    //存储该建筑拥有哪些行动。第一键值为行动标号，第二键值为对应的行动表，行动表为链表，存储了执行条件
    map<int, st_upgradeLab> actCon;

    st_buildAction();
    ~st_buildAction();

    void finishBuild();
    void finishAction(int actNum);
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
void loadResource(std::string name, std::list<ImageResource>* targetlist);
void loadGrayRes(std::list<ImageResource>* res, std::list<ImageResource>* grayres);
void loadBlackRes(std::list<ImageResource>* res, std::list<ImageResource>* blackres);

void flipResource(std::list<ImageResource>* currentlist, std::list<ImageResource>* targetlist);
void initMemory(ImageResource* res);
Double countdistance(Double L, Double U, Double L0, Double U0);
bool isNear_Manhattan(Double dr, Double ur, Double dr1, Double ur1, Double distance);

int calculateManhattanDistance(int x1, int y1, int x2, int y2);
Double calculateManhattanDistance(Double x1, Double y1, Double x2, Double y2);

void calMirrorPoint(Double& dr, Double& ur, Double dr_mirror, Double ur_mirror, Double dis);

Double trans_BlockPointToDetailCenter(int p);
QString JsonMap(const QMap<QString, QVariant>&data);

int sgn(Double __x);
void ParseArguments(const QApplication&app);
void ReadConfig();

#endif // GLOBALVARIATE_H
