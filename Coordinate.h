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

  /**********************虚函数**************************/
    virtual void nextframe();
    virtual int getSort();



    /***************指针强制转化****************/
    //若类有多重继承时，指针强制转化为父类指针,务必用以下函数!
    virtual void printer_ToCoordinate(void** ptr){ *ptr = this; }   //传入ptr为Coordinatel类指针的地址,需要强制转换为（void**）
    virtual void printer_ToMoveObject(void** ptr){ *ptr = NULL; }   //传入ptr为MoveObject类指针的地址,需要强制转换为（void**）
    virtual void printer_ToHuman(void** ptr){ *ptr = NULL; }        //传入ptr为Human类指针的地址,需要强制转换为（void**）
    virtual void printer_ToBloodHaver(void** ptr){ *ptr = NULL; }   //传入ptr为BloodHaver类指针的地址,需要强制转换为（void**）
    virtual void printer_ToResource(void** ptr){ *ptr = NULL; }     //传入ptr为Resource类指针的地址,需要强制转换为（void**）
    /*************以上指针强制转化****************/
  /********************以上虚函数**************************/
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
    int getNum()
    {
        return this->Num;
    }

    double get_BlockDR_contrl(){ return BlockDR+BlockSizeLen/2.0; }
    double get_BlockUR_contrl(){ return BlockUR+BlockSizeLen/2.0;}
    double get_BlockSizeLen(){ return BlockSizeLen; }

protected:

    int Num;//对象在对应类中的编号
    //比如building类下Num==0为小房子
    //在不同的类有着不同的含义

    double DR;//当前物体中心所在的坐标位置
    double UR;//在块类中该坐标即为正中心
    //此LU所指游戏中坐标

    int BlockDR;
    int BlockUR;
    //对象所在的区块

    double imageX;//该物体的长宽（即占地面积）
    double imageY;//需要根据占地大小来就算确切的绘制偏移量

    double BlockSizeLen = SIZELEN_SMALL; //物体占地,块坐标， 如小房子，为1，中型房子为2，动物为1

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
