#ifndef GLOBALVARIATE_H
#define GLOBALVARIATE_H

#include <QMutex>
#include <QSoundEffect>

#include "config.h"

using namespace std;
class Coordinate;
extern bool isExamining;

extern bool AIfinished;
extern bool INSfinshed;
extern int g_globalNum;
extern int mapmoveFrequency;
extern std::map<int, Coordinate*> g_Object;

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
//当前选中对象
//出于gamewidget和core均需要获取当前访问对象

struct st_DebugMassage {
    QString color;
    QString content;
    st_DebugMassage() {}
    st_DebugMassage(QString color, QString content)
    {
        this->color = color;
        this->content = content;
    }
};
void call_debugText(QString color, QString content, int playerID);

extern std::queue<st_DebugMassage>debugMassagePackage;
extern bool only_debug_Player0;
extern bool filterRepetitionMessage;
extern std::map<QString, int>debugMessageRecord;

enum ScoreType {
    _WOOD = 0,
    _STONE,
    _GOLD,
    _MEAT,

    _BERRY,
    _GAZELLE,
    _ELEPHANT,
    _FARM,
    _FISH,
    _ISWOOD,
    _ISGOLD,
    _ISSTONE,

    _TECH,
    _BUILDING1,
    _BUILDING2,
    _HUMAN1,
    _HUMAN2,

    _KILL2,
    _KILL10,

    _DESTORY2,
    _DESTORY4,
    _DESTORY5,
    _DESTORY10,
    _FINDENEMYLAND,
    SCORE_TYPE_COUNT
};

struct Score {
private:
    int id;
    int score;
    int scoreTypes[SCORE_TYPE_COUNT] = { 0 };

    void addScore(int points, const QString& message) {
        score += points;
        if (id == 0)
            call_debugText("blue", " 玩家" + message, REPRESENT_BOARDCAST_MESSAGE);
        else
            call_debugText("red", " 敌方" + message, REPRESENT_BOARDCAST_MESSAGE);
    }

public:
    Score(int id) : id(id), score(0) {}
    int getScore() {
        return score;
    }
    void update(int type, int num = 1) {
        if(type==_FINDENEMYLAND){
            addScore(10,"登录地方大陆,分数+10");
            return;
        }
        if (type <= _ISSTONE && scoreTypes[type] == 0 && type > _MEAT) {
            addScore(5, " 采集到新资源，分数+5");
            if (type == _ISGOLD) {
                addScore(10, " 采集到黄金，分数+10");
            }
        }

        if (type > _MEAT && type <= _ISSTONE) {
            scoreTypes[type] = scoreTypes[type] | 1;
            return;
        }

        int before = scoreTypes[type] / 100;
        scoreTypes[type] += num;

        if (type <= _MEAT) {
            int after = scoreTypes[type] / 100;
            int change = after - before;
            while (change > 0) {
                addScore(1, " 单种资源收集满100个，分数+1");
                change--;
            }
        }

        switch (type) {
        case _TECH:
            addScore(2, " 解锁新科技，分数+2");
            break;
        case _HUMAN1:
            addScore(1, " 生产普通单位，分数+1");
            break;
        case _HUMAN2:
            addScore(2, " 生产特殊单位，分数+2");
            break;
        case _BUILDING1:
            addScore(1, " 建造住房或农田，分数+1");
            break;
        case _BUILDING2:
            addScore(2, " 建造一般建筑，分数+2");
            break;
        case _KILL2:
            addScore(2, " 击杀一般敌人，分数+2");
            break;
        case _DESTORY2:
            addScore(2, " 摧毁房屋或农田，分数+2");
            break;
        case _DESTORY4:
            addScore(4, " 摧毁一般建筑，分数+4");
            break;
        case _DESTORY5:
            addScore(5, " 摧毁箭塔，分数+5");
            break;
        case _DESTORY10:
            addScore(10, " 摧毁主营，分数+10");
            break;
        default:
            break;
        }
    }
};
//
struct tagObj{
    int SN;// 序列号
    int BlockDR, BlockUR; //区块坐标
    bool operator <(const tagObj&obj)const{
        return SN<obj.SN;
    }
};
struct tagBuilding:tagObj
{
    int Type; // 建筑类型
    int Blood; // 当前血量
    int MaxBlood; // 最大血量
    int Percent; // 完成百分比
    int Project; // 当前项目
    int ProjectPercent; // 项目完成百分比
    int Cnt; // 剩余资源量（仅农田）
    tagBuilding toEnemy() {
        this->Cnt = -1;
        this->Project = -1;
        this->ProjectPercent = -1;
        return *this;
    }
};

struct tagResource:tagObj
{
    double DR, UR; //细节坐标
    int Type; // 资源类型
    int ProductSort; // 产品种类
    int Cnt; // 剩余资源数量
    int Blood; // 当前血量
};

struct tagHuman:tagObj
{
    double DR, UR; //细节坐标
    double DR0, UR0; // 目的地坐标
    int NowState; // 当前状态
    int WorkObjectSN; // 工作对象序列号
    int Blood; // 当前血量
    int attack; // 攻击力
    int rangedDefense; // 远程防御
    int meleeDefense; // 近战防御
    void cast_from(tagHuman taghuman) {
        this->DR = taghuman.DR;
        this->UR = taghuman.UR;
        this->BlockDR = taghuman.BlockDR;
        this->BlockUR = taghuman.BlockUR;
        this->DR0 = taghuman.DR0;
        this->UR0 = taghuman.UR0;
        this->NowState = taghuman.NowState;
        this->WorkObjectSN = taghuman.WorkObjectSN;
        this->Blood = taghuman.Blood;
        this->SN = taghuman.SN;
    }
};

struct tagFarmer : public tagHuman
{
    int ResourceSort; // 手持资源种类
    int Resource; // 手持资源数量
    int FarmerSort;//农民的类型
    tagFarmer toEnemy() {
        Resource = -1;
        DR0 = -1.0;
        UR0 = -1.0;
        return *this;
    }
};

struct tagArmy : public tagHuman
{
    int Sort; // 军队种类
    int status;
    int starttime;
    int finishtime;
    double startpointDR;
    double startpointUR;
    double destinaDR;
    double destinaUR;
    bool ifAttack;
    int timelock;
    tagArmy toEnemy() {
        DR0 = -1.0;
        UR0 = -1.0;
        return *this;
    }
};
//struct tagBlock{
//    bool explored=false;
//    int height=0;
//};

struct instruction {
    ///用于存储ai发出的指令信息
    /// @param type 指令类型
    /// @param option 对应类型下的操作
    /// type 0:终止对象self的动作
    /// type 1:命令村民self走向指定坐标L0，U0
    /// type 2:将obj对象设定为村民self的工作对象，村民会自动走向对象并工作
    /// type 3:命令村民self在块坐标BlockL,BlockU处建造类型为option的新建筑
    /// type 4:对建筑self发出命令option
    int ret = -1;
    int type;
    int id;
    Coordinate* self;
    Coordinate* obj;
    int option;
    int BlockDR, BlockUR;
    int SN = -1, obSN = -1;
    double DR, UR;
    bool isExist() {
        return type != -1;
    }
    instruction() { type = -1; }
    instruction(int type, int SN, int obSN, bool twoCoredinate);
    instruction(int type, int SN, int BlockDR, int BlockUR, int option);
    instruction(int type, int SN, double DR, double UR);
    instruction(int type, int SN, int option);
};

struct ins {
    int g_id = 0;
    std::queue<instruction> instructions;
    QMutex lock;
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
    tagMap() { clear(); }
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
struct tagTerrain {
    int height;
    int type;
};
// 定义二维点的结构体
struct Point {
    int x;
    int y;

    Point() {}
    Point(int x, int y) { this->x = x, this->y = y; }
    Point(const Point& board) { x = board.x, y = board.y; }

    Point operator +(const Point& ps) { return Point(x + ps.x, y + ps.y); }
    Point operator -(const Point& ps) { return Point(x - ps.x, y - ps.y); }
    bool operator ==(const Point& ps)const { return ps.x == x && ps.y == y; }
    bool operator < (const Point& ps)const { return x < ps.x && y < ps.y; }
};

struct tagInfo
{
    using TerrainData=const tagTerrain[MAP_L][MAP_U];
    vector<tagBuilding> buildings; // 我方建筑列表
    vector<tagFarmer> farmers; // 我方农民列表
    vector<tagArmy> armies; // 我方军队列表
    vector<tagBuilding> enemy_buildings; // 敌方建筑列表
    vector<tagFarmer> enemy_farmers; // 敌方农民列表
    vector<tagArmy> enemy_armies; // 敌方军队列表
    vector<tagResource> resources; // 资源列表
    map<int, int> ins_ret; // 指令返回值，map<id, ret>
    TerrainData*theMap;// 高程图
    //tagTerrain theMap[MAP_L][MAP_U]; //弃用这种形式
    vector<Point>exploredUpdate;//多探索的区域
    int GameFrame; // 游戏帧数
    int civilizationStage; // 文明阶段
    int Wood; // 木材数量
    int Meat; // 肉类数量
    int Stone; // 石头数量
    int Gold; // 黄金数量
    int Human_MaxNum; // 最大人口数量

    // Assignment operator
    tagInfo& operator=(const tagInfo& other) {
        if (this != &other) { // Check for self-assignment
            buildings = other.buildings;
            farmers = other.farmers;
            armies = other.armies;
            enemy_buildings = other.enemy_buildings;
            enemy_farmers = other.enemy_farmers;
            enemy_armies = other.enemy_armies;
            resources = other.resources;
            ins_ret = other.ins_ret;

            // Deep copy theMap array
            theMap=other.theMap;
            /*
            for (int i = 0; i < MAP_L; ++i) {
                for (int j = 0; j < MAP_U; ++j) {
                    theMap[i][j] = other.theMap[i][j];
                }
            }
            */
            GameFrame = other.GameFrame;
            civilizationStage = other.civilizationStage;
            Wood = other.Wood;
            Meat = other.Meat;
            Stone = other.Stone;
            Gold = other.Gold;
            Human_MaxNum = other.Human_MaxNum;
        }
        return *this;
    }

    void clear() {
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



struct tagGame
{
private:
    tagInfo* Info;
    QMutex Locker;
public:
    template<class T>
    void WLHHunYao(vector<T>& v) {
        vector<T> res = v;
        static auto randint = [](int a, int b)->int {return rand() % (b - a + 1) + a;};
        for (int i = 0;i < v.size();++i) {
            int idx = randint(0, res.size() - 1);
            swap(res.back(), res[idx]);
            v[i] = res.back();
            res.pop_back();
        }
    }
    void update(tagInfo* newinfo) {
        //控制ins_ret的大小小于100，若大于100，则优先删除旧值
        QMutexLocker locker(&Locker);
        if (this->Info != NULL) {
            while (Info->ins_ret.size() > 100) {
                Info->ins_ret.erase(Info->ins_ret.begin());
            }
        }
        if (this->Info != NULL)
            newinfo->ins_ret = this->Info->ins_ret;
        Info = newinfo;
        //对内部打乱
        static const bool openHunYao = 1;
        // for (auto& building : Info->buildings) {
        //     qDebug() << "before" << building.SN;
        // }
        if (openHunYao) {
            WLHHunYao(Info->buildings);
            WLHHunYao(Info->farmers);
            WLHHunYao(Info->armies);
            WLHHunYao(Info->enemy_buildings);
            WLHHunYao(Info->enemy_farmers);
            WLHHunYao(Info->enemy_armies);
            WLHHunYao(Info->resources);
        }
        // for (auto& building : Info->buildings) {
        //     qDebug() << "after" << building.SN;
        // }
    }
    void insertInsRet(int id, instruction ins) {
        QMutexLocker locker(&Locker);
        this->Info->ins_ret.insert(make_pair(id, ins.ret));
    }
    tagInfo getInfo() {
        QMutexLocker locker(&Locker);
        return *Info;
    }
    void clearInsRet() {
        QMutexLocker locker(&Locker);
        Info->ins_ret.clear();
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
        MemoryMap.resize(width * height);
    }

    pixMemoryMap() : width(0), height(0) {}

    pixMemoryMap(const pixMemoryMap& other) : width(other.width), height(other.height)
    {

        MemoryMap = other.MemoryMap;
    }

    pixMemoryMap& operator=(const pixMemoryMap& other)
    {
        width = other.width;
        height = other.height;

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
        for (int i = 0;i < width / 2;i++)
        {
            for (int j = 0;j < height / 2;j++)
            {
                if (j * width >= height * (width / 2 - i))
                {
                    setMemoryMap(i, j);
                }
            }
        }
        for (int i = width / 2;i < width;i++)
        {
            for (int j = 0;j < height / 2;j++)
            {
                if (j * width >= height * (i - width / 2))
                {
                    setMemoryMap(i, j);
                }
            }
        }
        for (int i = 0;i < width / 2;i++)
        {
            for (int j = height / 2;j < height;j++)
            {
                if (j * width <= height * (i + width / 2))
                {
                    setMemoryMap(i, j);
                }
            }
        }
        for (int i = width / 2;i < width;i++)
        {
            for (int j = height / 2;j < height;j++)
            {
                if (j * width <= -height * i + 3 * width * height / 2)
                {
                    setMemoryMap(i, j);
                }
            }
        }
    }
};

struct ImageResource
{
    QPixmap pix;
    pixMemoryMap memorymap;

    ImageResource(QPixmap pix) :pix(pix)
    {
        if (pix.isNull()) {
            // 图片未成功加载，执行错误处理操作
            qDebug() << "fault";
        }
    }

    ImageResource()
    {

    }
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
    int need_Wood, need_Food, need_Stone, need_Gold;

    //**********************************************************
    //构造器 ， 创建conditionDevelop实例相关
    conditionDevelop() {}
    conditionDevelop(int civilization, int sort_building, double needTimes, int need_Wood = 0, int need_Food = 0, int need_Stone = 0, int need_Gold = 0)
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
    void addPreCondition(conditionDevelop* con_need) { preCondition.push_back(con_need); }

    //添加行动结束后创建对象
    void setCreatObjectAfterAction(int creatSort)
    {
        isCreatObjectAction = true;
        creatObjectSort = creatSort;
    }
    void setCreatObjectAfterAction(int creatSort, int creatNum)
    {
        setCreatObjectAfterAction(creatSort);
        creatObjectNum = creatNum;
    }

    //**********************************************************
    //判断行动是否能进行
    //判断资源是否满足行动需要
    bool executable(int wood, int food, int stone, int gold) { return wood >= need_Wood && food >= need_Food && stone >= need_Stone && gold >= need_Gold; }
    void get_needResource(int& wood, int& food, int& stone, int& gold) { wood = need_Wood, food = need_Food, stone = need_Stone, gold = need_Gold; }

    //判断当前时代、已完成的建筑行动，是否解锁当前行动
    bool isShowable(int nowcivilization)
    {
        if (civilization > nowcivilization) return false;

        for (list<conditionDevelop*>::iterator iter = preCondition.begin(); iter != preCondition.end(); iter++)
            if (!(*iter)->acttimes) return false;

        return true;
    }

    //**********************************************************
    //行动结束后相关操作
    void finishAct() { acttimes++; }

    //获取是否需要创建对象
    bool isNeedCreatObject(int& creatSort, int& creatNum)
    {
        creatSort = creatObjectSort;
        creatNum = creatObjectNum;
        return isCreatObjectAction;
    }

    bool isNeedCreatObject() { return isCreatObjectAction; }

    int getActTimes() { return acttimes; }
};

struct st_upgradeLab {
    conditionDevelop* headAct = NULL, * nowExecuteNode = NULL, * endNode = NULL;
    int haveFinishedPhaseNum = 0;
    bool nowExecuting = false;

    st_upgradeLab() {}
    ~st_upgradeLab()
    {
        while (headAct != endNode)
        {
            nowExecuteNode = headAct;
            headAct = headAct->nextDevAction;
            delete nowExecuteNode;
        }
        delete endNode;
    }

    void setHead(conditionDevelop* head) { endNode = nowExecuteNode = headAct = head; }

    void push_back(conditionDevelop* node)
    {
        endNode->nextDevAction = node;
        endNode = node;
    }

    //设置建筑的行动没有下一个行动，但该行动可以重复地执行
    void endNodeAsOver() { endNode->nextDevAction = endNode; }

    //切换
    void shift()
    {
        if (nowExecuteNode != NULL)
        {
            overExecute();
            haveFinishedPhaseNum++;
            nowExecuteNode = nowExecuteNode->nextDevAction;
        }
    }

    //当前行动是否可在SelectWidget中显示
    bool isShowAble(int nowcivilization)
    {
        if (nowExecuteNode == NULL) return false;
        else return nowExecuteNode->isShowable(nowcivilization) && (!nowExecuting || nowExecuteNode == nowExecuteNode->nextDevAction);
    }

    //当前行动是否可执行
    bool executable(int nowcivilization, int wood, int food, int stone, int gold) { return isShowAble(nowcivilization) && nowExecuteNode->executable(wood, food, stone, gold); }

    void beginExecute() { this->nowExecuting = true; }
    void overExecute() { this->nowExecuting = false; }

    //获取当前行动列表执行过几轮（一个链node算一轮）
    int getPhaseTimes() { return this->haveFinishedPhaseNum; }
    //获取需要的资源
    void get_needResource(int& wood, int& food, int& stone, int& gold)
    {
        if (nowExecuteNode != NULL)
            nowExecuteNode->get_needResource(wood, food, stone, gold);
        else wood = 0, food = 0, stone = 0, gold = 0;
    }
    /**
    *考虑加入错误码，以判断错误类型
    */

    bool isNeedCreatObject() {
        if (nowExecuteNode != NULL) return nowExecuteNode->isNeedCreatObject();
        else return false;
    }
};

struct st_buildAction
{
    //建筑的建造条件
    conditionDevelop* buildCon = NULL;

    //存储该建筑拥有哪些行动。第一键值为行动标号，第二键值为对应的行动表，行动表为链表，存储了执行条件
    map<int, st_upgradeLab> actCon;

    st_buildAction() {}
    ~st_buildAction()
    {
        if (buildCon != NULL)
        {
            delete buildCon;
            buildCon = NULL;
        }
    }

    void finishBuild() { buildCon->finishAct(); }
    void finishAction(int actNum)
    {
        actCon[actNum].nowExecuteNode->finishAct();
        actCon[actNum].shift();
    }
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
double countdistance(double L, double U, double L0, double U0);
bool isNear_Manhattan(double dr, double ur, double dr1, double ur1, double distance);

int calculateManhattanDistance(int x1, int y1, int x2, int y2);
double calculateManhattanDistance(double x1, double y1, double x2, double y2);

void calMirrorPoint(double& dr, double& ur, double dr_mirror, double ur_mirror, double dis);

double trans_BlockPointToDetailCenter(int p);



int sgn(double __x);


#endif // GLOBALVARIATE_H
