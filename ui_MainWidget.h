/********************************************************************************
** Form generated from reading UI file 'MainWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.17
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWIDGET_H
#define UI_MAINWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QWidget>
#include <ViewWidget.h>
#include "ActWidget.h"
#include "GameWidget.h"

QT_BEGIN_NAMESPACE

class Ui_MainWidget
{
public:
    GameWidget *Game;
    QLabel *tip;
    QLabel *statusLbl;
    ActWidget *interact3;
    ActWidget *interact8;
    ActWidget *interact5;
    ActWidget *interact2;
    ActWidget *interact1;
    ActWidget *interact4;
    ActWidget *interact6;
    ActWidget *interact7;
    QLCDNumber *lcdNumber;
    QLabel *resWood;
    QLabel *resFood;
    QLabel *resGold;
    QLabel *resStone;
    ActWidget *interact9;
    ActWidget *interact10;
    QTextBrowser *DebugTexter;
    ViewWidget *mapView;
    QLabel *version;
    QRadioButton *radioButton_1;
    QRadioButton *radioButton_2;
    QRadioButton *radioButton_4;
    QRadioButton *radioButton_8;
    QPushButton *option;
    QPushButton *stopButton;
    QLabel *usr;
    QLabel *enemy;
    QLabel *score0;
    QLabel *score1;
    QPushButton *option_2;

    void setupUi(QWidget *MainWidget)
    {
        if (MainWidget->objectName().isEmpty())
            MainWidget->setObjectName(QString::fromUtf8("MainWidget"));
        MainWidget->resize(1920, 1000);
        Game = new GameWidget(MainWidget);
        Game->setObjectName(QString::fromUtf8("Game"));
        Game->setGeometry(QRect(0, 45, 1440, 751));
        tip = new QLabel(Game);
        tip->setObjectName(QString::fromUtf8("tip"));
        tip->setGeometry(QRect(10, 690, 1401, 41));
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        font.setWeight(75);
        tip->setFont(font);
        statusLbl = new QLabel(Game);
        statusLbl->setObjectName(QString::fromUtf8("statusLbl"));
        statusLbl->setEnabled(true);
        statusLbl->setGeometry(QRect(1160, 660, 281, 101));
        statusLbl->setMaximumSize(QSize(16777213, 16777215));
        QPalette palette;
        QBrush brush(QColor(0, 85, 255, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        QBrush brush1(QColor(120, 120, 120, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        statusLbl->setPalette(palette);
        QFont font1;
        font1.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font1.setPointSize(14);
        statusLbl->setFont(font1);
        statusLbl->setTextFormat(Qt::AutoText);
        statusLbl->setScaledContents(false);
        statusLbl->setAlignment(Qt::AlignCenter);
        statusLbl->setMargin(0);
        interact3 = new ActWidget(MainWidget);
        interact3->setObjectName(QString::fromUtf8("interact3"));
        interact3->setGeometry(QRect(480, 810, 81, 81));
        interact8 = new ActWidget(MainWidget);
        interact8->setObjectName(QString::fromUtf8("interact8"));
        interact8->setGeometry(QRect(480, 900, 81, 81));
        interact5 = new ActWidget(MainWidget);
        interact5->setObjectName(QString::fromUtf8("interact5"));
        interact5->setGeometry(QRect(660, 810, 81, 81));
        interact2 = new ActWidget(MainWidget);
        interact2->setObjectName(QString::fromUtf8("interact2"));
        interact2->setGeometry(QRect(390, 810, 81, 81));
        interact1 = new ActWidget(MainWidget);
        interact1->setObjectName(QString::fromUtf8("interact1"));
        interact1->setGeometry(QRect(300, 810, 81, 81));
        interact4 = new ActWidget(MainWidget);
        interact4->setObjectName(QString::fromUtf8("interact4"));
        interact4->setGeometry(QRect(570, 810, 81, 81));
        interact6 = new ActWidget(MainWidget);
        interact6->setObjectName(QString::fromUtf8("interact6"));
        interact6->setGeometry(QRect(300, 900, 81, 81));
        interact7 = new ActWidget(MainWidget);
        interact7->setObjectName(QString::fromUtf8("interact7"));
        interact7->setGeometry(QRect(390, 900, 81, 81));
        lcdNumber = new QLCDNumber(MainWidget);
        lcdNumber->setObjectName(QString::fromUtf8("lcdNumber"));
        lcdNumber->setGeometry(QRect(680, 3, 64, 31));
        resWood = new QLabel(MainWidget);
        resWood->setObjectName(QString::fromUtf8("resWood"));
        resWood->setGeometry(QRect(85, 10, 81, 21));
        QFont font2;
        font2.setPointSize(9);
        font2.setBold(true);
        font2.setWeight(75);
        resWood->setFont(font2);
        resFood = new QLabel(MainWidget);
        resFood->setObjectName(QString::fromUtf8("resFood"));
        resFood->setGeometry(QRect(240, 10, 81, 21));
        resFood->setFont(font2);
        resGold = new QLabel(MainWidget);
        resGold->setObjectName(QString::fromUtf8("resGold"));
        resGold->setGeometry(QRect(395, 10, 81, 21));
        resGold->setFont(font2);
        resStone = new QLabel(MainWidget);
        resStone->setObjectName(QString::fromUtf8("resStone"));
        resStone->setGeometry(QRect(550, 10, 81, 21));
        resStone->setFont(font2);
        interact9 = new ActWidget(MainWidget);
        interact9->setObjectName(QString::fromUtf8("interact9"));
        interact9->setGeometry(QRect(570, 900, 81, 81));
        interact10 = new ActWidget(MainWidget);
        interact10->setObjectName(QString::fromUtf8("interact10"));
        interact10->setGeometry(QRect(660, 900, 81, 81));
        DebugTexter = new QTextBrowser(MainWidget);
        DebugTexter->setObjectName(QString::fromUtf8("DebugTexter"));
        DebugTexter->setGeometry(QRect(1440, 0, 480, 1000));
        mapView = new ViewWidget(MainWidget);
        mapView->setObjectName(QString::fromUtf8("mapView"));
        mapView->setGeometry(QRect(920, 810, 513, 177));
        version = new QLabel(MainWidget);
        version->setObjectName(QString::fromUtf8("version"));
        version->setGeometry(QRect(1780, 940, 121, 61));
        QPalette palette1;
        QBrush brush2(QColor(255, 0, 0, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::WindowText, brush2);
        palette1.setBrush(QPalette::Inactive, QPalette::WindowText, brush2);
        palette1.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        version->setPalette(palette1);
        QFont font3;
        font3.setPointSize(15);
        font3.setBold(true);
        font3.setWeight(75);
        version->setFont(font3);
        radioButton_1 = new QRadioButton(MainWidget);
        radioButton_1->setObjectName(QString::fromUtf8("radioButton_1"));
        radioButton_1->setGeometry(QRect(1120, 0, 61, 30));
        radioButton_2 = new QRadioButton(MainWidget);
        radioButton_2->setObjectName(QString::fromUtf8("radioButton_2"));
        radioButton_2->setGeometry(QRect(1180, 0, 61, 30));
        radioButton_4 = new QRadioButton(MainWidget);
        radioButton_4->setObjectName(QString::fromUtf8("radioButton_4"));
        radioButton_4->setGeometry(QRect(1240, 0, 61, 30));
        radioButton_8 = new QRadioButton(MainWidget);
        radioButton_8->setObjectName(QString::fromUtf8("radioButton_8"));
        radioButton_8->setEnabled(true);
        radioButton_8->setGeometry(QRect(1300, 0, 61, 30));
        radioButton_8->setContextMenuPolicy(Qt::PreventContextMenu);
        option = new QPushButton(MainWidget);
        option->setObjectName(QString::fromUtf8("option"));
        option->setGeometry(QRect(770, 840, 110, 60));
        QFont font4;
        font4.setFamily(QString::fromUtf8("\345\215\216\346\226\207\350\241\214\346\245\267"));
        option->setFont(font4);
        stopButton = new QPushButton(MainWidget);
        stopButton->setObjectName(QString::fromUtf8("stopButton"));
        stopButton->setGeometry(QRect(1370, 0, 71, 31));
        usr = new QLabel(MainWidget);
        usr->setObjectName(QString::fromUtf8("usr"));
        usr->setGeometry(QRect(790, 5, 101, 31));
        usr->setFont(font2);
        usr->setTextFormat(Qt::AutoText);
        enemy = new QLabel(MainWidget);
        enemy->setObjectName(QString::fromUtf8("enemy"));
        enemy->setGeometry(QRect(950, 5, 101, 31));
        enemy->setFont(font2);
        score0 = new QLabel(MainWidget);
        score0->setObjectName(QString::fromUtf8("score0"));
        score0->setGeometry(QRect(890, 5, 61, 31));
        score0->setFont(font2);
        score1 = new QLabel(MainWidget);
        score1->setObjectName(QString::fromUtf8("score1"));
        score1->setGeometry(QRect(1050, 5, 61, 31));
        score1->setFont(font2);
        option_2 = new QPushButton(MainWidget);
        option_2->setObjectName(QString::fromUtf8("option_2"));
        option_2->setGeometry(QRect(770, 920, 110, 60));
        option_2->setFont(font4);

        retranslateUi(MainWidget);

        QMetaObject::connectSlotsByName(MainWidget);
    } // setupUi

    void retranslateUi(QWidget *MainWidget)
    {
        MainWidget->setWindowTitle(QCoreApplication::translate("MainWidget", "MainWidget", nullptr));
        tip->setText(QString());
        statusLbl->setText(QString());
        resWood->setText(QString());
        resFood->setText(QString());
        resGold->setText(QString());
        resStone->setText(QString());
        version->setText(QString());
        radioButton_1->setText(QCoreApplication::translate("MainWidget", "1\303\227", nullptr));
        radioButton_2->setText(QCoreApplication::translate("MainWidget", "2\303\227", nullptr));
        radioButton_4->setText(QCoreApplication::translate("MainWidget", "4\303\227", nullptr));
        radioButton_8->setText(QCoreApplication::translate("MainWidget", "8\303\227", nullptr));
        option->setText(QCoreApplication::translate("MainWidget", "\350\256\276\347\275\256", nullptr));
        stopButton->setText(QCoreApplication::translate("MainWidget", "\346\232\202\345\201\234", nullptr));
        usr->setText(QCoreApplication::translate("MainWidget", "<html><head/><body><p><span style=\" font-size:12pt;\">\347\216\251\345\256\266\345\276\227\345\210\206</span></p></body></html>", nullptr));
        enemy->setText(QCoreApplication::translate("MainWidget", "<html><head/><body><p><span style=\" font-size:12pt;\">\346\225\214\346\226\271\345\276\227\345\210\206</span></p></body></html>", nullptr));
        score0->setText(QCoreApplication::translate("MainWidget", "<html><head/><body><p><span style=\" font-size:12pt; color:#00007f;\">0</span></p></body></html>", nullptr));
        score1->setText(QCoreApplication::translate("MainWidget", "<html><head/><body><p><span style=\" font-size:12pt; color:#aa0000;\">0</span></p></body></html>", nullptr));
        option_2->setText(QCoreApplication::translate("MainWidget", "\345\205\263\344\272\216\346\210\221\344\273\254", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWidget: public Ui_MainWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWIDGET_H
