#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include "config.h"
#include <QDialog>

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = 0);
    ~AboutDialog();

    void paintEvent(QPaintEvent *event);
private:
    Ui::AboutDialog *ui;
    QPixmap pix;
};

#endif // ABOUTDIALOG_H
