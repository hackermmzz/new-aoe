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

    initmap();
    void paintEvent(QPaintEvent *);
    void initBlock();
    void initBuilding();
    void initAnimal();
    void initStaticResource();
    void initFarmer();
    void initArmy();
    void initMissile();

    void deleteBlock();
    void deleteBuilding();
    void deleteAnimal();
    void deleteStaticResource();
    void deleteFarmer();
    void deleteArmy();
    void deleteMissile();

    //****************输出框****************
    void respond_DebugMessage();
    void debugText(const QString& color,const QString& content);
    void clearDebugText();
    void exportDebugTextHtml();
    void exportDebugTextTxt();
    void clearDebugTextFile();

    //*************************************

    void playSound(string soundType);
    void makeSound();

    ActWidget* getActs(int num){return acts[num];}
    void statusUpdate();
    void showPlayerResource(int playerRepresent);



    SelectWidget *sel;
    Core *core;
    UsrAI* UsrAi;
    EnemyAI *EnemyAi;
    bool eventFilter(QObject *watched, QEvent *event);
    Map *map;
    int **memorymap=new int*[MEMORYROW];//动态
    Player* player[MAXPLAYER];
    MouseEvent *mouseEvent=new MouseEvent();
    QLabel *tipLbl =NULL;

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
    Ui::MainWidget *ui;
    QTimer *timer;
    int gameframe = 0; // 游戏帧数初始化
    QButtonGroup *pbuttonGroup = NULL;

    void gameDataUpdate();
    void paintUpdate();

    void buildInitialStock();

    //判断胜利
    void judgeVictory();
    bool isLoss();
    bool isWin();
    void ScoreSave(string gameResult);

    bool pause = false;
    Option *option = NULL;
    ActWidget *acts[ACT_WINDOW_NUM_FREE];
    AboutDialog* aboutDialog = NULL;

    //****************Music*********************
    QSoundEffect* bgm = NULL;

    //****************Music*********************
};

#endif // MAINWIDGET_H
