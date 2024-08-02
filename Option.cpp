#include "Option.h"
#include "ui_Option.h"

Option::Option(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Option)
{
    ui->setupUi(this);
    setFixedSize(600,500);
    pix=QPixmap(":/new/prefix1/res/About_Background_001.png");
    setWindowTitle("设置");
    btnHtml = ui->exportHtml;
    btntxt = ui->exportTxt;
    btnTextClear = ui->textClear;
    btnFileClear = ui->exportClear;
    btnMusic = ui->music;
    btnSound = ui->sound;
    btnSelect = ui->select;
    btnLine = ui->showLine;
    btnPos = ui->clickPos;
    btnOverlap = ui->overlap;
//    btnPause = ui->pause;

    showText_Sound();
    showText_Music();

}

Option::~Option()
{
    delete ui;
}

void Option::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(0,0,600,500,pix);
}

//void Option::on_pause_clicked()
//{
//    if(pause)
//    {
//        pause = false;
//        ui->pause->setText("暂停游戏");
//    }
//    else
//    {
//        pause = true;
//        ui->pause->setText("继续游戏");
//    }
//}

void Option::showText_Sound()
{
    if(sound)
        ui->sound->setText("关闭音效");
    else
        ui->sound->setText("开启音效");
}

void Option::showText_Music()
{
    if(music)
        ui->music->setText("关闭音乐");
    else
        ui->music->setText("开启音乐");
}


//***************option槽函数*******************
void Option::on_select_clicked()
{
    select = !select;

    if(select)
        ui->select->setText("关闭显示鼠标点击对象编号");
    else
        ui->select->setText("开启显示鼠标点击对象编号");

}

void Option::on_showLine_clicked()
{
    line = !line;
    if(line)
        ui->showLine->setText("隐藏格子边框线条");
    else
        ui->showLine->setText("显示格子边框线条");
}

void Option::on_clickPos_clicked()
{
    pos = !pos;
    if(pos)
        ui->clickPos->setText("关闭显示鼠标点击格子坐标");
    else
        ui->clickPos->setText("开启显示鼠标点击格子坐标");
}

void Option::on_overlap_clicked()
{
    overlap = !overlap;
    if(overlap)
        ui->overlap->setText("关闭显示建筑放置位置\n冲突细节位置");
    else
        ui->overlap->setText("开启显示建筑放置位置\n冲突细节位置");
}

void Option::on_music_clicked()
{
    music = !music;

    showText_Music();

    emit changeMusic();
}

void Option::on_sound_clicked()
{
    sound = !sound;

    showText_Sound();
}

