#ifndef CONFIG_H
#define CONFIG_H

#include <QWidget>
#include <QPainter>
#include <stack>
#include <qtimer.h>
#include <QTextBrowser>
#include <QElapsedTimer>
#include <stack>
#include <queue>
#include <QButtonGroup>
#include <QMessageBox>
#include <QThread>
#include <Windows.h>
#include <time.h>
#include <map>
#include <QPixmap>
#include <QString>
#include <list>
//#include <QSound>
#include <QDebug>
#include <QDirIterator>
#include <QDir>
#include <QFileInfo>
#include <QtWidgets>
#include <QMouseEvent>
#include <QImage>
#include <QObject>
#include <QKeyEvent>
//#include <QMediaPlayer>
#include <fstream>
#include <sstream>
#include <vector>

/********** 游戏配置数据 **********/
#define GAME_WIDTH 1920                 //总窗口宽度
#define GAME_HEIGHT 1000              //总窗口高度
#define GAME_VERSION "v1.0"
#define GAME_TITLE "Age of Empires"     //总窗口名称
#define GAME_LOSE_SEC 1500
#define GOLD 10                         //金块资源数量
#define STONE 10                        //石头资源数量
#define gen5 sqrt(5)
#define MAXPLAYER 8
#define MEMORYROW 400                  //列 行长度
#define MEMORYCOLUMN 200               //行 列长度
#define GAMEWIDGET_WIDTH 1440
#define GAMEWIDGET_HEIGHT 751
#define BLOCKSIDELENGTH (16*gen5)
#define MAP_L 72
#define MAP_U 72
#define GAMEWIDGET_MIDBLOCKL 22
#define GAMEWIDGET_MIDBLOCKU 0
#define HUMAN_SPEED sqrt(5)
#define ANIMAL_SPEED sqrt(7)
#define RESPATH ":/new/prefix1/res"
#define ForestMAX 2
#define TREEMAX 40
#define BLOCKPIXEL_X (65 - showLine)
#define BLOCKPIXEL_Y (33 - showLine)
#define GENERATELANDFORMS_NUM 4
#define KEY_UP 87
#define KEY_LEFT 65
#define KEY_DOWN 83
#define KEY_RIGHT 68

#endif // CONFIG_H
