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
}

Option::~Option()
{
    delete ui;
}

void Option::on_music_clicked()
{
    if(music)
    {
        music = false;
        ui->music->setText("开启音乐");
    }
    else
    {
        music = true;
        ui->music->setText("关闭音乐");
    }
}

void Option::on_sound_clicked()
{
    if(sound)
    {
        sound = false;
        ui->sound->setText("开启音效");
    }
    else
    {
        sound = true;
        ui->sound->setText("关闭音效");
    }
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

void Option::on_select_clicked()
{
    if(select)
    {
        select = false;
        ui->select->setText("开启显示鼠标点击对象编号");
    }
    else
    {
        select = true;
        ui->select->setText("关闭显示鼠标点击对象编号");
    }
}

void Option::on_showLine_clicked()
{
    if(line)
    {
        line = false;
        ui->showLine->setText("显示格子边框线条");
    }
    else
    {
        line = true;
        ui->showLine->setText("隐藏格子边框线条");
    }
}

void Option::on_clickPos_clicked()
{
    if(pos)
    {
        pos = false;
        ui->clickPos->setText("开启显示鼠标点击格子坐标");
    }
    else
    {
        pos = true;
        ui->clickPos->setText("关闭显示鼠标点击格子坐标");
    }
}

void Option::on_overlap_clicked()
{
    if(overlap)
    {
        overlap = false;
        ui->overlap->setText("开启显示建筑放置位置\n冲突细节位置");
    }
    else
    {
        overlap = true;
        ui->overlap->setText("关闭显示建筑放置位置\n冲突细节位置");
    }
}
