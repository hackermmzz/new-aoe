#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "config.h"
#include "SelectWidget.h"
#include "Option.h"
#include "Core.h"
#include "UsrAI.h"
#include "enemyai.h"
#include "ActWidget.h"
#include "AboutDialog.h"
#include "GlobalVariate.h"
#include "soudplaythread.h"
#include "Editor.h"
#include "ui_Editor.h"
#include"AreaSelected.h"

// 前向声明，避免循环包含
struct RectAreaData;
struct CircleAreaData;
struct LineAreaData;
namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(int MapJudge, QWidget *parent = 0);
    ~MainWidget();

    Player** player;
    Map *map;
    MouseEvent *mouseEvent=new MouseEvent();
    int **memorymap=new int*[MEMORYROW];    //动态

    //获取实体信息框的按钮
    ActWidget* getActs(int num){return acts[num];}
    
    // 单位清理函数
    void cleanupUnitReferences(Coordinate* unit);  // 清理单位的所有引用
    
    // 获取核心对象
    Core* getCore() const { return core; }
    AI* getUsrAI(){return UsrAi;}
    // 敌人状态相关函数 - 需要被外部调用，所以放在public中
    void setEnemyStatus(Coordinate* unit, const string& status);  // 设置敌人状态
    string getEnemyStatus(Coordinate* unit);  // 获取敌人状态

    // 投石车定点投射相关函数
    void setWaitingForPinPointStrike(bool waiting) { waitingForPinPointStrike = waiting; }
    void setPinPointStrikeUnit(Coordinate* unit) { pinPointStrikeUnit = unit; }
    bool isWaitingForPinPointStrike() const { return waitingForPinPointStrike; }
    Coordinate* getPinPointStrikeUnit() const { return pinPointStrikeUnit; }

    // 顶点投射相关变量
    bool waitingForPinPointStrike = false;  // 是否正在等待地图点击以执行顶点投射
    Coordinate* pinPointStrikeUnit = nullptr;  // 等待执行顶点投射的单位

    // 编辑器相关内容
    Editor* editor;
    AreaSelected*rectArea;//矩形区域生成监听器
    AreaSelected*circleArea;//圆形区域生成监听器
    AreaSelected*lineArea;//曲线路径生成
    int currentSelected;  // 用于记录当前的选择的内容
    
    // 单位选择和区域管理相关变量
    vector<Coordinate*> selectedUnits;  // 当前选中的单位
    
    // 敌人状态相关变量
    std::map<Coordinate*, string> enemyStatusMap;  // 存储敌人单位的状态(attack/defend)
    
    // 高亮区域管理
    vector<RectAreaData> highlightedRectAreas;  // 需要高亮显示的矩形区域
    vector<CircleAreaData> highlightedCircleAreas;  // 需要高亮显示的圆形区域
    vector<LineAreaData> highlightedLineAreas;  // 需要高亮显示的线形区域
    // 枚举编辑器中的功能键
    enum EditorElement{
        DELETEOBJECT,
        FLAT,
        OCEAN,
        HIGHTERLAND,
        LOWERLAND,
        PLAYERDOWNTOWN,
        PLAYERTRANSPORTSHIP,
        PLAYERFISHERY,
        PLAYERWARSHIP,
        PLAYERDOCK,
        PLAYERHOME,
        PLAYERFISHINGBOAT,
        PLAYERREPOSITORY,
        PLAYERARROWTOWER,
        PLAYERBARRACKS,
        PLAYERFARMER,
        PLAYERCLUBMAN,
        PLAYERAXEMAN,
        PLAYERSCOUT,
        PLAYERBOWMAN,
        AIWARSHIP,
        AIARROWTOWER,
        AISIEGE,
        AICLUBMAN,
        AIAXEMAN,
        AISCOUT,
        AIBOWMAN,
        AIPRIEST,
        AIHOPLITE,
        AICOMPARCHER,
        AIBROADSWORDSMAN,
        AICHARIOT,
        AICHARIOTARCHER,
        AISTONETHROWER,
        GAZELLE,
        LION,
        ELEPHANT,
        TREE,
        STONM,
        GOLDORE,
        RECT_AREA,
        CIRCLE_AREA,
        LINE_AREA,
        PATROL_RECT_AREA,
        PATROL_CIRCLE_AREA,
        PATROL_LINE_AREA,
        ENEMY_STATUS_ATTACK,
        ENEMY_STATUS_DEFEND,
        NORMAL_MOUSE,
    };


public slots:
    void cheat_Player0Resource();
private slots:
    void FrameUpdate();
    void onRadioClickSlot();
    void on_stopButton_clicked();
    void responseMusicChange();
    void on_option_2_clicked();
signals:
    void mapmove();
    void startAI();
    void stopAI();
private:
    Core *core;
    UsrAI* UsrAi;
    EnemyAI *EnemyAi;
    SoudPlayThread*soundPlayThread;

//*************游戏更新*************
    bool pause = false;
    int gameframe = 0;
    void gameDataUpdate();
    void paintUpdate();
    bool eventFilter(QObject *watched, QEvent *event);
    //绘制窗口界面的图片
    void paintEvent(QPaintEvent *);
    //更新UI信息
    void statusUpdate();
    //更新资源信息
    void showPlayerResource(int playerRepresent);
//*********************************

//***********UI组件**************
    SelectWidget *sel;
    Option *option = NULL;
    ActWidget *acts[ACT_WINDOW_NUM_FREE];
    AboutDialog* aboutDialog = NULL;
    QLabel *tipLbl =NULL;
    Ui::MainWidget *ui;
    QTimer *timer;
    QButtonGroup *pbuttonGroup = NULL;
//*******************************

//****************编辑器*****************
    void SaveCurrentState();
    void ExportCurrentState(const char*file);
    void updateEditor();
    void HigherLand(int blockL,int blockU,int height);
    void LowerLand(int blockL,int blockU,int height);
    void MakeOcean(int blockL,int blockU);
    void DeleteOcean(int blockL,int blockU);
    void MakeGrassland(int blockL,int blockU);
    void MakeTree(double DR,double UR);
    void MakeStaticRes(int blockL,int blockU,int type);
    void MakeAnimal(double DR,double UR,int type);
    void MakeBuilding(int blockL,int blockU,int type);
    void MakeHuman(double DR,double UR,int type);
    void clearArea(int blockL, int blockU, int radius = 1);  // 清空指定区域资源
    
    // 单位选择和区域管理相关函数
    void selectUnit(Coordinate* unit, bool addToSelection = false);  // 选择单位
    void clearSelection();  // 清空选择
    void updateAreaButtons();  // 更新区域按钮状态
    Coordinate* getUnitAtPosition(double DR, double UR);  // 获取指定位置的单位
    void highlightUnitAreas(Coordinate* unit);  // 高亮显示单位的区域
    void highlightSelectedUnitAreas(Coordinate* unit);  // 添加单位区域到高亮列表（不清空现有的）
    void clearHighlightedAreas();  // 清空高亮区域
    void drawHighlightedAreas();  // 绘制高亮区域
    void checkAndDisplayPatrolArea(Coordinate* unit);  // 检查并显示单位的巡逻区域信息
    
    // handleEnemyStatusSelection函数保留在private中，因为它是内部使用的
    void handleEnemyStatusSelection(double DR, double UR);  // 处理敌人状态选择

//**************************************

//****************输出框****************
    void respond_DebugMessage();
    void debugText(const QString& color,const QString& content);
    void clearDebugText();
    void exportDebugTextTreeBlock();
    void exportDebugTextTxt();
    void clearDebugTextFile();
//*****************************************

//***************游戏结算*******************
    void judgeVictory();
    bool isLoss();
    bool isWin();
    void ScoreSave(string gameResult);
    void HandleGameOver();
//*****************************************

//****************Music*********************
    QSoundEffect* bgm = NULL;
    void playSound();   //音效
    void playSound(string s);
    void makeSound();
//*****************************************

//***************InitHelperFunction**********
    void initVar();
    void initEditor();
    void initGameElements();
    void initGameResources();
    void initWindowProperties();
    void initOptions();
    void initInfoPane();
    void initGameTimer();
    void initPlayers();
    void initMap(int MapJudge);
    void initAI();
    void setupCore();
    void setupMouseTracking();
    void setupTipLabel();
    void initBGM();
    void initViewMap();
    void initBlock();
    void initBuilding();
    void initAnimal();
    void initStaticResource();
    void initFarmer();
    void initArmy();
    void initMissile();
    void buildInitialStock();

//*****************************************

//***************DeleteFunction************
    void deleteBlock();
    void deleteBuilding();
    void deleteAnimal();
    void deleteStaticResource();
    void deleteFarmer();
    void deleteArmy();
    void deleteMissile();
//*****************************************
};

// 全局区域对象声明，供EnemyAI访问
extern class RectArea* g_rectArea;
extern class CircleArea* g_circleArea;
extern class LineArea* g_lineArea;

// 全局MainWidget实例指针，供单位清理使用
extern class MainWidget* g_mainWidget;

// 全局单位清理回调函数指针
extern void(*g_cleanupUnitCallback)(Coordinate*);

#endif // MAINWIDGET_H
