/********************************************************************************
** Form generated from reading UI file 'SelectWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.17
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SELECTWIDGET_H
#define UI_SELECTWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SelectWidget
{
public:
    QLabel *objName;
    QLabel *objIcon;
    QLabel *objIconSmall_ATK;
    QLabel *objText_ATK;
    QLabel *objHp;
    QLabel *objIconSmall;
    QLabel *objText;
    QLabel *objText_DEF_melee;
    QLabel *objText_DEF_range;
    QLabel *objIconSmall_DEF_melee;
    QLabel *objIconSmall_DEF_range;

    void setupUi(QWidget *SelectWidget)
    {
        if (SelectWidget->objectName().isEmpty())
            SelectWidget->setObjectName(QString::fromUtf8("SelectWidget"));
        SelectWidget->resize(280, 170);
        objName = new QLabel(SelectWidget);
        objName->setObjectName(QString::fromUtf8("objName"));
        objName->setGeometry(QRect(10, 10, 131, 31));
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        objName->setFont(font);
        objIcon = new QLabel(SelectWidget);
        objIcon->setObjectName(QString::fromUtf8("objIcon"));
        objIcon->setGeometry(QRect(10, 50, 111, 111));
        objIconSmall_ATK = new QLabel(SelectWidget);
        objIconSmall_ATK->setObjectName(QString::fromUtf8("objIconSmall_ATK"));
        objIconSmall_ATK->setEnabled(true);
        objIconSmall_ATK->setGeometry(QRect(130, 10, 41, 31));
        objText_ATK = new QLabel(SelectWidget);
        objText_ATK->setObjectName(QString::fromUtf8("objText_ATK"));
        objText_ATK->setGeometry(QRect(180, 10, 101, 31));
        QFont font1;
        font1.setPointSize(13);
        font1.setBold(true);
        objText_ATK->setFont(font1);
        objHp = new QLabel(SelectWidget);
        objHp->setObjectName(QString::fromUtf8("objHp"));
        objHp->setGeometry(QRect(140, 130, 121, 31));
        objHp->setFont(font1);
        objIconSmall = new QLabel(SelectWidget);
        objIconSmall->setObjectName(QString::fromUtf8("objIconSmall"));
        objIconSmall->setGeometry(QRect(130, 50, 40, 31));
        objText = new QLabel(SelectWidget);
        objText->setObjectName(QString::fromUtf8("objText"));
        objText->setEnabled(true);
        objText->setGeometry(QRect(180, 50, 101, 31));
        objText->setFont(font1);
        objText_DEF_melee = new QLabel(SelectWidget);
        objText_DEF_melee->setObjectName(QString::fromUtf8("objText_DEF_melee"));
        objText_DEF_melee->setGeometry(QRect(180, 40, 101, 31));
        objText_DEF_melee->setFont(font1);
        objText_DEF_range = new QLabel(SelectWidget);
        objText_DEF_range->setObjectName(QString::fromUtf8("objText_DEF_range"));
        objText_DEF_range->setGeometry(QRect(180, 70, 101, 31));
        objText_DEF_range->setFont(font1);
        objIconSmall_DEF_melee = new QLabel(SelectWidget);
        objIconSmall_DEF_melee->setObjectName(QString::fromUtf8("objIconSmall_DEF_melee"));
        objIconSmall_DEF_melee->setEnabled(true);
        objIconSmall_DEF_melee->setGeometry(QRect(130, 40, 41, 31));
        objIconSmall_DEF_range = new QLabel(SelectWidget);
        objIconSmall_DEF_range->setObjectName(QString::fromUtf8("objIconSmall_DEF_range"));
        objIconSmall_DEF_range->setEnabled(true);
        objIconSmall_DEF_range->setGeometry(QRect(130, 70, 41, 31));

        retranslateUi(SelectWidget);

        QMetaObject::connectSlotsByName(SelectWidget);
    } // setupUi

    void retranslateUi(QWidget *SelectWidget)
    {
        SelectWidget->setWindowTitle(QCoreApplication::translate("SelectWidget", "Form", nullptr));
        objName->setText(QString());
        objIcon->setText(QString());
        objIconSmall_ATK->setText(QString());
        objText_ATK->setText(QString());
        objHp->setText(QString());
        objIconSmall->setText(QString());
        objText->setText(QString());
        objText_DEF_melee->setText(QString());
        objText_DEF_range->setText(QString());
        objIconSmall_DEF_melee->setText(QString());
        objIconSmall_DEF_range->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class SelectWidget: public Ui_SelectWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SELECTWIDGET_H
