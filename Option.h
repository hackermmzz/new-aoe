#ifndef OPTION_H
#define OPTION_H

#include <QDialog>
#include <QPainter>
namespace Ui {
class Option;
}

class Option : public QDialog
{
    Q_OBJECT

public:
    explicit Option(QWidget *parent = 0);
    ~Option();
    QPushButton *btnHtml, *btntxt, *btnTextClear, *btnFileClear, *btnMusic, *btnSound, *btnPause, *btnSelect, *btnLine, *btnPos, *btnOverlap;
private slots:

    void on_music_clicked();
    void on_sound_clicked();
    void paintEvent(QPaintEvent *event);
//    void on_pause_clicked();

    void on_select_clicked();

    void on_showLine_clicked();

    void on_clickPos_clicked();

    void on_overlap_clicked();

private:
    Ui::Option *ui;
    bool music = false;
    bool sound = false;
    bool select = false;
//    bool pause = false;
    bool line = false;
    bool pos = false;
    bool overlap = false;
    QPixmap pix;


};

#endif // OPTION_H
