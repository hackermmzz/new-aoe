/********************************************************************************
** Form generated from reading UI file 'AboutDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABOUTDIALOG_H
#define UI_ABOUTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AboutDialog
{
public:
    QHBoxLayout *horizontalLayout;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QGridLayout *gridLayout;
    QLabel *nameLabel;
    QLabel *versionLabel;
    QLabel *teacherLabel;
    QLabel *memberLabel;
    QLabel *member1;
    QLabel *member2;

    void setupUi(QDialog *AboutDialog)
    {
        if (AboutDialog->objectName().isEmpty())
            AboutDialog->setObjectName(QStringLiteral("AboutDialog"));
        AboutDialog->resize(600, 500);
        horizontalLayout = new QHBoxLayout(AboutDialog);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        scrollArea = new QScrollArea(AboutDialog);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 580, 480));
        gridLayout = new QGridLayout(scrollAreaWidgetContents);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        nameLabel = new QLabel(scrollAreaWidgetContents);
        nameLabel->setObjectName(QStringLiteral("nameLabel"));
        QFont font;
        font.setFamily(QStringLiteral("Agency FB"));
        font.setBold(true);
        font.setWeight(75);
        nameLabel->setFont(font);

        gridLayout->addWidget(nameLabel, 0, 0, 1, 1);

        versionLabel = new QLabel(scrollAreaWidgetContents);
        versionLabel->setObjectName(QStringLiteral("versionLabel"));
        versionLabel->setFont(font);

        gridLayout->addWidget(versionLabel, 0, 1, 1, 1);

        teacherLabel = new QLabel(scrollAreaWidgetContents);
        teacherLabel->setObjectName(QStringLiteral("teacherLabel"));
        teacherLabel->setFont(font);

        gridLayout->addWidget(teacherLabel, 1, 0, 1, 1);

        memberLabel = new QLabel(scrollAreaWidgetContents);
        memberLabel->setObjectName(QStringLiteral("memberLabel"));
        memberLabel->setFont(font);

        gridLayout->addWidget(memberLabel, 2, 0, 1, 1);

        member1 = new QLabel(scrollAreaWidgetContents);
        member1->setObjectName(QStringLiteral("member1"));
        member1->setFont(font);

        gridLayout->addWidget(member1, 3, 0, 1, 2);

        member2 = new QLabel(scrollAreaWidgetContents);
        member2->setObjectName(QStringLiteral("member2"));
        member2->setFont(font);

        gridLayout->addWidget(member2, 4, 0, 1, 2);

        scrollArea->setWidget(scrollAreaWidgetContents);

        horizontalLayout->addWidget(scrollArea);


        retranslateUi(AboutDialog);

        QMetaObject::connectSlotsByName(AboutDialog);
    } // setupUi

    void retranslateUi(QDialog *AboutDialog)
    {
        AboutDialog->setWindowTitle(QApplication::translate("AboutDialog", "Dialog", Q_NULLPTR));
        nameLabel->setText(QApplication::translate("AboutDialog", "\351\241\271\347\233\256\345\220\215\347\247\260\357\274\232", Q_NULLPTR));
        versionLabel->setText(QApplication::translate("AboutDialog", "\345\275\223\345\211\215\347\211\210\346\234\254\357\274\232", Q_NULLPTR));
        teacherLabel->setText(QApplication::translate("AboutDialog", "\346\214\207\345\257\274\350\200\201\345\270\210\357\274\232  \345\250\204\351\234\207", Q_NULLPTR));
        memberLabel->setText(QApplication::translate("AboutDialog", "\345\274\200\345\217\221\344\272\272\345\221\230\357\274\232", Q_NULLPTR));
        member1->setText(QApplication::translate("AboutDialog", "v0.99-v1.03gbeta\357\274\232  \350\265\265\345\256\207\351\243\236  \345\272\204\346\230\216\345\223\262  \345\274\240\345\256\207\345\270\206", Q_NULLPTR));
        member2->setText(QApplication::translate("AboutDialog", "v1.10-v2.02e\357\274\232         \346\262\210\345\215\223\350\257\272  \350\203\241\344\270\200\346\245\240  \350\214\203\345\256\207\346\201\222", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class AboutDialog: public Ui_AboutDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUTDIALOG_H
