/********************************************************************************
** Form generated from reading UI file 'Option.ui'
**
** Created by: Qt User Interface Compiler version 5.15.17
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OPTION_H
#define UI_OPTION_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_Option
{
public:
    QPushButton *exportHtml;
    QPushButton *exportTxt;
    QPushButton *textClear;
    QPushButton *exportClear;
    QPushButton *music;
    QPushButton *sound;
    QPushButton *select;
    QPushButton *showLine;
    QPushButton *clickPos;
    QPushButton *overlap;

    void setupUi(QDialog *Option)
    {
        if (Option->objectName().isEmpty())
            Option->setObjectName(QString::fromUtf8("Option"));
        Option->resize(600, 500);
        exportHtml = new QPushButton(Option);
        exportHtml->setObjectName(QString::fromUtf8("exportHtml"));
        exportHtml->setGeometry(QRect(20, 60, 271, 81));
        exportTxt = new QPushButton(Option);
        exportTxt->setObjectName(QString::fromUtf8("exportTxt"));
        exportTxt->setGeometry(QRect(300, 60, 271, 81));
        textClear = new QPushButton(Option);
        textClear->setObjectName(QString::fromUtf8("textClear"));
        textClear->setGeometry(QRect(20, 150, 271, 81));
        exportClear = new QPushButton(Option);
        exportClear->setObjectName(QString::fromUtf8("exportClear"));
        exportClear->setGeometry(QRect(300, 150, 271, 81));
        music = new QPushButton(Option);
        music->setObjectName(QString::fromUtf8("music"));
        music->setGeometry(QRect(20, 20, 119, 31));
        sound = new QPushButton(Option);
        sound->setObjectName(QString::fromUtf8("sound"));
        sound->setGeometry(QRect(160, 20, 119, 31));
        select = new QPushButton(Option);
        select->setObjectName(QString::fromUtf8("select"));
        select->setGeometry(QRect(300, 240, 271, 71));
        showLine = new QPushButton(Option);
        showLine->setObjectName(QString::fromUtf8("showLine"));
        showLine->setGeometry(QRect(20, 240, 271, 71));
        clickPos = new QPushButton(Option);
        clickPos->setObjectName(QString::fromUtf8("clickPos"));
        clickPos->setGeometry(QRect(300, 320, 271, 71));
        overlap = new QPushButton(Option);
        overlap->setObjectName(QString::fromUtf8("overlap"));
        overlap->setGeometry(QRect(20, 320, 271, 71));

        retranslateUi(Option);

        QMetaObject::connectSlotsByName(Option);
    } // setupUi

    void retranslateUi(QDialog *Option)
    {
        Option->setWindowTitle(QCoreApplication::translate("Option", "Dialog", nullptr));
        exportHtml->setText(QCoreApplication::translate("Option", "\345\257\274\345\207\272Debug\350\260\203\350\257\225\346\240\217\344\277\241\346\201\257\344\270\272HTML", nullptr));
        exportTxt->setText(QCoreApplication::translate("Option", "\345\257\274\345\207\272Debug\350\260\203\350\257\225\346\240\217\344\277\241\346\201\257\344\270\272TXT", nullptr));
        textClear->setText(QCoreApplication::translate("Option", "\346\270\205\351\231\244\345\275\223\345\211\215\350\260\203\350\257\225\346\240\217\344\277\241\346\201\257", nullptr));
        exportClear->setText(QCoreApplication::translate("Option", "\346\270\205\351\231\244\345\257\274\345\207\272\347\233\256\345\275\225\344\270\213\346\211\200\346\234\211\350\260\203\350\257\225\346\226\207\344\273\266", nullptr));
        music->setText(QCoreApplication::translate("Option", "\345\274\200\345\220\257\351\237\263\344\271\220", nullptr));
        sound->setText(QCoreApplication::translate("Option", "\345\274\200\345\220\257\351\237\263\346\225\210", nullptr));
        select->setText(QCoreApplication::translate("Option", "\345\274\200\345\220\257\346\230\276\347\244\272\351\274\240\346\240\207\347\202\271\345\207\273\345\257\271\350\261\241\347\274\226\345\217\267", nullptr));
        showLine->setText(QCoreApplication::translate("Option", "\346\230\276\347\244\272\346\240\274\345\255\220\350\276\271\346\241\206\347\272\277\346\235\241", nullptr));
        clickPos->setText(QCoreApplication::translate("Option", "\345\274\200\345\220\257\346\230\276\347\244\272\351\274\240\346\240\207\347\202\271\345\207\273\346\240\274\345\255\220\345\235\220\346\240\207", nullptr));
        overlap->setText(QCoreApplication::translate("Option", "\345\274\200\345\220\257\346\230\276\347\244\272\345\273\272\347\255\221\346\224\276\347\275\256\344\275\215\347\275\256\n"
"\345\206\262\347\252\201\347\273\206\350\212\202\344\275\215\347\275\256", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Option: public Ui_Option {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OPTION_H
