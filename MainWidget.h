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
    void playSound(string soundType);   //音效
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
