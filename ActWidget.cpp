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
    int size=70;//按钮边长
    if(status == 0)//普通状态
    {
        painter.drawPixmap(0,0,size,size,pix);
        if(this->num < ACT_WINDOW_NUM_FREE)
        {
            painter.fillRect(QRect(0, 0, 4, size), QBrush(Qt::white));
            painter.fillRect(QRect(4, 0, size-4, 4), QBrush(Qt::gray));
            painter.fillRect(QRect(2, 0, 2, 2), QBrush(Qt::gray));
        }
        this->show();
    }
    else if(status == 1)//凹效果
    {
        if(this->num < ACT_WINDOW_NUM_FREE)
        {
            painter.drawPixmap(4,4,size-4,size-4,pix);
            painter.fillRect(QRect(0, 0, 4, size), QBrush(Qt::white));
            painter.fillRect(QRect(4, 0, size-4, 4), QBrush(Qt::gray));
            painter.fillRect(QRect(2, 0, 2, 2), QBrush(Qt::gray));
        }
        else
        {
            painter.drawPixmap(0,0,size+40,size+40,pix);
        }
        this->show();
    }
    else if(status == 2)//灰色图标
    {
        if(!pix.isNull())
        {
            QImage img = pix.toImage().scaled(size, size);
            for(int i = 0; i < size; i++)
            {
                for(int j = 0; j < size; j++)
                {
                    QColor clr = img.pixelColor(i, j);
                    int gray = clr.red() * 0.3 + clr.green() * 0.59 + clr.blue() * 0.11;
                    img.setPixelColor(i, j, QColor(gray, gray, gray));
                }
            }
            pix = QPixmap::fromImage(img);
            painter.drawPixmap(0,0,size,size,pix);
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
}

