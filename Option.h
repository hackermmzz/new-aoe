#ifndef OPTION_H
#define OPTION_H

#include <QDialog>
#include <QPainter>
#include <config.h>
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

    bool getMusic(){ return music; }
    bool getSound(){ return sound; }
    bool getSelect(){ return select; }
    bool getLine(){ return line; }
    bool getPos(){ return pos; }
    bool getOverlap(){ return overlap; }


private:
    Ui::Option *ui;
    bool music = OPTION_MUSIC;
    bool sound = OPTION_SOUND;
    bool select = OPTION_SELECT;
    bool line = OPTION_LINE;
    bool pos = OPTION_POS;
    bool overlap = OPTION_OVERLAP;
    QPixmap pix;

    void showText_Sound();
    void showText_Music();

signals:
    void changeMusic();

private slots:

    void on_music_clicked();
    void on_sound_clicked();
    void paintEvent(QPaintEvent *event);

    void on_select_clicked();

    void on_showLine_clicked();

    void on_clickPos_clicked();

    void on_overlap_clicked();
};

#endif // OPTION_H
