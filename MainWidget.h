#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "config.h"
#include "SelectWidget.h"
#include "Option.h"
#include "Core.h"
#include "UsrAI.h"
#include "EnemyAI.h"
#include "ActWidget.h"
#include "AboutDialog.h"
#include "GlobalVariate.h"
#include "soudplaythread.h"
#include "Editor.h"
#include "ui_Editor.h"

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(int MapJudge, QWidget *parent = 0);
    ~MainWidget();

    Player* player[MAXPLAYER];
    Map *map;
    MouseEvent *mouseEvent=new MouseEvent();
    int **memorymap=new int*[MEMORYROW];    //动态

    //获取实体信息框的按钮
    ActWidget* getActs(int num){return acts[num];}

    // 编辑器相关内容
    bool openEditor = false;
    Editor* editor;
    int currentSelected;  // 用于记录当前的选择的内容
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
        AICLUBMAN,
        AIAXEMAN,
        AISCOUT,
        AIBOWMAN,
        GAZELLE,
        LION,
        ELEPHANT,
        TREE,
        STONM,
        GOLDORE
    };

    bool leftMousePress;

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
    void MakeTree(double DR,double UR);
    void MakeStaticRes(int blockL,int blockU,int type);
    void MakeAnimal(double DR,double UR,int type);
    void MakeBuilding(int blockL,int blockU,int type);
    void MakeHuman(double DR,double UR,int type);
    void clearArea(int blockL, int blockU, int radius = 1);  // 清空指定区域资源

//**************************************

//****************输出框****************
    void respond_DebugMessage();
    void debugText(const QString& color,const QString& content);
    void clearDebugText();
    void exportDebugTextHtml();
    void exportDebugTextTxt();
    void clearDebugTextFile();
//*****************************************

//***************游戏结算*******************
    void judgeVictory();
    bool isLoss();
    bool isWin();
    void ScoreSave(string gameResult);
//*****************************************

//****************Music*********************
    QSoundEffect* bgm = NULL;
    void playSound();   //音效
    void playSound(string s);
    void makeSound();
//*****************************************

//***************InitHelperFunction**********
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


#endif // MAINWIDGET_H
