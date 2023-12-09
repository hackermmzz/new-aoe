#ifndef COORDINATE_H
#define COORDINATE_H

#include <QPixmap>
#include <QDebug>
#include <config.h>
#include <QImage>
#include <GlobalVariate.h>

class Coordinate
{
public:
    Coordinate();

    virtual void nextframe();

    double getDR()
    {
        return this->DR;
    }
    double getUR()
    {
        return this->UR;
    }
    double getimageX()
    {
        return this->imageX;
    }
    double getimageY()
    {
        return this->imageY;
    }
    double getSideLength()
    {
        return this->SideLength;
    }
    int getimageH()
    {
        return this->imageH;
    }
    std::list<ImageResource>::iterator getNowRes()
    {
        return this->nowres;
    }
    int getglobalNum()
    {
        return this->globalNum;
    }

protected:

    double DR;//当前物体中心所在的坐标位置
    double UR;//在块类中该坐标即为正中心
    //此LU所指游戏中坐标

    int BlockDR;
    int BlockUR;
    //对象所在的区块

    double imageX;//该物体的长宽（即占地面积）
    double imageY;//需要根据占地大小来就算确切的绘制偏移量

    double SideLength;
    //占地大小转换成游戏内坐标 边长

    int imageH;//绘制y坐标
    //该物体在平面中的上下位置

    int globalNum;
    //每个对象所拥有的独一无二的num，用来确定具体对象

    int explored=0;
    //0为未探索 1为探索

    int visible=0;
    //0为不可见 1为可见

    int inWindow=0;
    //在游戏窗口内

    std::list<ImageResource>::iterator nowres;

};

#endif // COORDINATE_H
