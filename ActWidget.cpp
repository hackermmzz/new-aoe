#include "ActWidget.h"

ActWidget::ActWidget(QWidget *parent) : QWidget(parent)
{

}

void ActWidget::setActName(int actName)
{
    this->actName = actName;
}

void ActWidget::setPix(QPixmap pix)
{
    this->pix = pix;
}

void ActWidget::setNum(int num)
{
    this->num = num;
}

void ActWidget::setPhase(int phase)
{
    this->phase = phase;
}

void ActWidget::setStatus(int status)
{
    this->status = status;
}

int ActWidget::getActName()
{
    return actName;
}

QPixmap ActWidget::getPix()
{
    return pix;
}

int ActWidget::getNum()
{
    return num;
}

int ActWidget::getPhase()
{
    return phase;
}

int ActWidget::getStatus()
{
    return status;
}

void ActWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    if(status == 0)//普通状态
    {
        painter.drawPixmap(0,0,80,80,pix);
        if(this->num < ACT_WINDOW_NUM_FREE)
        {
            painter.fillRect(QRect(0, 0, 4, 80), QBrush(Qt::white));
            painter.fillRect(QRect(4, 0, 76, 4), QBrush(Qt::gray));
            painter.fillRect(QRect(2, 0, 2, 2), QBrush(Qt::gray));
        }
        this->show();
    }
    else if(status == 1)//凹效果
    {
        if(this->num < ACT_WINDOW_NUM_FREE)
        {
            painter.drawPixmap(4,4,76,76,pix);
            painter.fillRect(QRect(0, 0, 4, 80), QBrush(Qt::white));
            painter.fillRect(QRect(4, 0, 76, 4), QBrush(Qt::gray));
            painter.fillRect(QRect(2, 0, 2, 2), QBrush(Qt::gray));
        }
        else
        {
            painter.drawPixmap(0,0,120,120,pix);
        }
        this->show();
    }
    else if(status == 2)//灰色图标
    {
        if(!pix.isNull())
        {
            QImage img = pix.toImage().scaled(80, 80);
            for(int i = 0; i < 80; i++)
            {
                for(int j = 0; j < 80; j++)
                {
                    QColor clr = img.pixelColor(i, j);
                    int gray = clr.red() * 0.3 + clr.green() * 0.59 + clr.blue() * 0.11;
                    img.setPixelColor(i, j, QColor(gray, gray, gray));
                }
            }
            pix = QPixmap::fromImage(img);
            painter.drawPixmap(0,0,80,80,pix);
        }
    }
}


void ActWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(this->isVisible())
    {
        emit actPress(num);
        this->update();
    }
}
void ActWidget::mousePressEvent(QMouseEvent *event){
    this->update();
    qDebug() << this->num;
}

