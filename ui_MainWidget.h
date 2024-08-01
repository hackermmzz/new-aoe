/********************************************************************************
** Form generated from reading UI file 'MainWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWIDGET_H
#define UI_MAINWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
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

    void setupUi(QWidget *MainWidget)
    {
        if (MainWidget->objectName().isEmpty())
            MainWidget->setObjectName(QStringLiteral("MainWidget"));
        MainWidget->resize(1920, 1000);
        Game = new GameWidget(MainWidget);
        Game->setObjectName(QStringLiteral("Game"));
        Game->setGeometry(QRect(0, 45, 1440, 751));
        tip = new QLabel(Game);
        tip->setObjectName(QStringLiteral("tip"));
        tip->setGeometry(QRect(10, 690, 1401, 41));
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        font.setWeight(75);
        tip->setFont(font);
        statusLbl = new QLabel(Game);
        statusLbl->setObjectName(QStringLiteral("statusLbl"));
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
        interact3->setObjectName(QStringLiteral("interact3"));
        interact3->setGeometry(QRect(480, 810, 81, 81));
        interact8 = new ActWidget(MainWidget);
        interact8->setObjectName(QStringLiteral("interact8"));
        interact8->setGeometry(QRect(480, 900, 81, 81));
        interact5 = new ActWidget(MainWidget);
        interact5->setObjectName(QStringLiteral("interact5"));
        interact5->setGeometry(QRect(660, 810, 81, 81));
        interact2 = new ActWidget(MainWidget);
        interact2->setObjectName(QStringLiteral("interact2"));
        interact2->setGeometry(QRect(390, 810, 81, 81));
        interact1 = new ActWidget(MainWidget);
        interact1->setObjectName(QStringLiteral("interact1"));
        interact1->setGeometry(QRect(300, 810, 81, 81));
        interact4 = new ActWidget(MainWidget);
        interact4->setObjectName(QStringLiteral("interact4"));
        interact4->setGeometry(QRect(570, 810, 81, 81));
        interact6 = new ActWidget(MainWidget);
        interact6->setObjectName(QStringLiteral("interact6"));
        interact6->setGeometry(QRect(300, 900, 81, 81));
        interact7 = new ActWidget(MainWidget);
        interact7->setObjectName(QStringLiteral("interact7"));
        interact7->setGeometry(QRect(390, 900, 81, 81));
        lcdNumber = new QLCDNumber(MainWidget);
        lcdNumber->setObjectName(QStringLiteral("lcdNumber"));
        lcdNumber->setGeometry(QRect(680, 20, 64, 23));
        resWood = new QLabel(MainWidget);
        resWood->setObjectName(QStringLiteral("resWood"));
        resWood->setGeometry(QRect(85, 10, 81, 21));
        QFont font2;
        font2.setPointSize(9);
        font2.setBold(true);
        font2.setWeight(75);
        resWood->setFont(font2);
        resFood = new QLabel(MainWidget);
        resFood->setObjectName(QStringLiteral("resFood"));
        resFood->setGeometry(QRect(240, 10, 81, 21));
        resFood->setFont(font2);
        resGold = new QLabel(MainWidget);
        resGold->setObjectName(QStringLiteral("resGold"));
        resGold->setGeometry(QRect(395, 10, 81, 21));
        resGold->setFont(font2);
        resStone = new QLabel(MainWidget);
        resStone->setObjectName(QStringLiteral("resStone"));
        resStone->setGeometry(QRect(550, 10, 81, 21));
        resStone->setFont(font2);
        interact9 = new ActWidget(MainWidget);
        interact9->setObjectName(QStringLiteral("interact9"));
        interact9->setGeometry(QRect(570, 900, 81, 81));
        interact10 = new ActWidget(MainWidget);
        interact10->setObjectName(QStringLiteral("interact10"));
        interact10->setGeometry(QRect(660, 900, 81, 81));
        DebugTexter = new QTextBrowser(MainWidget);
        DebugTexter->setObjectName(QStringLiteral("DebugTexter"));
        DebugTexter->setGeometry(QRect(1440, 0, 480, 1000));
        mapView = new ViewWidget(MainWidget);
        mapView->setObjectName(QStringLiteral("mapView"));
        mapView->setGeometry(QRect(920, 810, 513, 177));
        version = new QLabel(MainWidget);
        version->setObjectName(QStringLiteral("version"));
        version->setGeometry(QRect(760, 910, 121, 61));
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
        radioButton_1->setObjectName(QStringLiteral("radioButton_1"));
        radioButton_1->setGeometry(QRect(1120, 0, 61, 30));
        radioButton_2 = new QRadioButton(MainWidget);
        radioButton_2->setObjectName(QStringLiteral("radioButton_2"));
        radioButton_2->setGeometry(QRect(1180, 0, 61, 30));
        radioButton_4 = new QRadioButton(MainWidget);
        radioButton_4->setObjectName(QStringLiteral("radioButton_4"));
        radioButton_4->setGeometry(QRect(1240, 0, 61, 30));
        radioButton_8 = new QRadioButton(MainWidget);
        radioButton_8->setObjectName(QStringLiteral("radioButton_8"));
        radioButton_8->setEnabled(true);
        radioButton_8->setGeometry(QRect(1300, 0, 61, 30));
        radioButton_8->setContextMenuPolicy(Qt::PreventContextMenu);
        option = new QPushButton(MainWidget);
        option->setObjectName(QStringLiteral("option"));
        option->setGeometry(QRect(760, 850, 80, 50));
        stopButton = new QPushButton(MainWidget);
        stopButton->setObjectName(QStringLiteral("stopButton"));
        stopButton->setGeometry(QRect(1370, 0, 71, 31));

        retranslateUi(MainWidget);

        QMetaObject::connectSlotsByName(MainWidget);
    } // setupUi

    void retranslateUi(QWidget *MainWidget)
    {
        MainWidget->setWindowTitle(QApplication::translate("MainWidget", "MainWidget", Q_NULLPTR));
        tip->setText(QString());
        statusLbl->setText(QString());
        resWood->setText(QString());
        resFood->setText(QString());
        resGold->setText(QString());
        resStone->setText(QString());
        version->setText(QString());
        radioButton_1->setText(QApplication::translate("MainWidget", "1\303\227", Q_NULLPTR));
        radioButton_2->setText(QApplication::translate("MainWidget", "2\303\227", Q_NULLPTR));
        radioButton_4->setText(QApplication::translate("MainWidget", "4\303\227", Q_NULLPTR));
        radioButton_8->setText(QApplication::translate("MainWidget", "8\303\227", Q_NULLPTR));
        option->setText(QApplication::translate("MainWidget", "\350\256\276\347\275\256", Q_NULLPTR));
        stopButton->setText(QApplication::translate("MainWidget", "\346\232\202\345\201\234", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWidget: public Ui_MainWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWIDGET_H
