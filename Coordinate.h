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

    virtual bool get_isActionEnd(){ return true; }
    virtual bool get_isActionImageToPhaseFromEnd(int phaseFormEnd){ return true; }

    virtual int getPlayerRepresent(){ return MAXPLAYER; }

    virtual void setAttribute(){ }
    virtual void setNowRes(){ }

    virtual void resetCoreAttribute(){}

    virtual void setAction( int actNum){ this->actNum = actNum; }
    virtual void ActNumToActName(){ actName = actNum; }
    /***************指针强制转化****************/
    //若类有多重继承时，指针强制转化为父类指针,务必用以下函数!
    virtual void printer_ToCoordinate(void** ptr){ *ptr = this; }   //传入ptr为Coordinatel类指针的地址,需要强制转换为（void**）
    virtual void printer_ToMoveObject(void** ptr){ *ptr = NULL; }   //传入ptr为MoveObject类指针的地址,需要强制转换为（void**）
    virtual void printer_ToHuman(void** ptr){ *ptr = NULL; }        //传入ptr为Human类指针的地址,需要强制转换为（void**）
    virtual void printer_ToBloodHaver(void** ptr){ *ptr = NULL; }   //传入ptr为BloodHaver类指针的地址,需要强制转换为（void**）
    virtual void printer_ToResource(void** ptr){ *ptr = NULL; }     //传入ptr为Resource类指针的地址,需要强制转换为（void**）
    virtual void printer_ToBuilding(void** ptr){ *ptr = NULL; }
    virtual void printer_ToMissile(void** ptr){ *ptr = NULL; }
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
    int getBlockDR()
    {
        return this->BlockDR;
    }
    int getBlockUR()
    {
        return this->BlockUR;
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

    double get_BlockSizeLen(){ return BlockSizeLen; }
    double get_SideLen(){return SideLength;}


    void set_interAct(int interSort , int interNum){ interactSort = interSort ; interactNum = interNum; }

/*****************act获取***************/
//需要协商，act位置

    double getActPercent() {return this->actPercent;}
    double getActSpeed(){ return this->actSpeed;}
    void setActName(int actName){this->actName = actName;}
    int getActName(){return this->actName;}
    void setActSpeed(double actSpeed){ this->actSpeed = actSpeed;}
    void setActPercent(double actPercent){this->actPercent = actPercent;}
//    void updatePercent(){this->setActPercent(actPercent + actSpeed);}
    int getActNum(){ return this->actNum;}
    void setActNum(int actNum){this->actNum = actNum;}

    void initAction(){ actName = ACT_NULL; actNum = ACT_NULL; actSpeed = 0; actPercent = 0; }
/*****************act获取***************/

protected:

    int Num;//对象在对应类中的编号
    //比如building类下Num==0为小房子
    //在不同的类有着不同的含义

    //此时此刻交互对象的类别和Num
    int interactSort = -1;
    int interactNum = -1;

    double DR;//当前物体中心所在的坐标位置
    double UR;//在块类中该坐标即为正中心
    //此LU所指游戏中坐标

    int BlockDR;
    int BlockUR;
    //对象所在的区块

    double imageX;//该物体的长宽（即占地面积）
    double imageY;//需要根据占地大小来就算确切的绘制偏移量

    double BlockSizeLen = SIZELEN_SINGEL; //物体占地,块坐标， 如小房子，为2，中型房子为3，动物为1

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
    std::list<ImageResource> *nowlist=NULL;

    void setDetailPointAttrb_FormBlock()
    {
        DR = ( BlockDR + BlockSizeLen/2.0)*BLOCKSIDELENGTH;
        UR = ( BlockUR + BlockSizeLen/2.0)*BLOCKSIDELENGTH;
        setSideLenth();
    }
    void setSideLenth(){ SideLength = BlockSizeLen*BLOCKSIDELENGTH; }

    /*****************act获取***************/
    double actPercent = 0;
    double actSpeed = 0;
    int actName = 0;
    //执行行动时的进度、速率和行动类型
    int actNum=0;
    //动作类型的编号
    /*****************act获取***************/
};

#endif // COORDINATE_H
