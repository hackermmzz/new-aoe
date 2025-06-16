#ifndef COORDINATE_H
#define COORDINATE_H

#include "config.h"
#include "GlobalVariate.h"

class Coordinate
{
public:
    Coordinate(){}

  /**********************虚函数**************************/
    virtual int getSort(){return SORT_COORDINATE;}

    /*******状态与图像显示*******/
    virtual void nextframe(){}
    virtual bool get_isActionEnd(){ return true; }
    virtual void setNowRes(){ }


    /*******player相关*******/
    virtual bool isPlayerControl(){ return false; }
    virtual int getPlayerRepresent(){ return MAXPLAYER; }


    /*******状态与属性设置、获取*******/
    virtual void setAttribute(){ }
    virtual int getVision(){ return vision; }
    virtual void resetCoreAttribute(){}
    //是否需要监视地图
    virtual bool isMonitorObject(Coordinate* judOb){ return false; }
    virtual QString getChineseName(){ return ""; }


    /*******行动相关*******/
    virtual void setAction( int actNum){ this->actNum = actNum; }
    virtual void ActNumToActName(){ actName = actNum; }
    //重置行动record
    virtual void initAction(){ actName = ACT_NULL; actNum = ACT_NULL; actSpeed = 0; }


    /*******音乐与音效*******/
    virtual string getSound_Click(){return "";}


    /***************指针强制转化****************/
    //若类有多重继承时，指针强制转化为父类指针,务必用以下函数!
    //传入ptr为Coordinatel类指针的地址,需要强制转换为（void**）
    virtual void printer_ToCoordinate(void** ptr){ *ptr = this; }

    //传入ptr为MoveObject类指针的地址,需要强制转换为（void**）
    virtual void printer_ToMoveObject(void** ptr){ *ptr = NULL; }

    //传入ptr为Human类指针的地址,需要强制转换为（void**）
    virtual void printer_ToHuman(void** ptr){ *ptr = NULL; }

    //传入ptr为BloodHaver类指针的地址,需要强制转换为（void**）
    virtual void printer_ToBloodHaver(void** ptr){ *ptr = NULL; }

    //传入ptr为Resource类指针的地址,需要强制转换为（void**）
    virtual void printer_ToResource(void** ptr){ *ptr = NULL; }

    virtual void printer_ToBuilding(void** ptr){ *ptr = NULL; }

    virtual void printer_ToBuilding_Resource(void **ptr){ *ptr = NULL; }

    virtual void printer_ToMissile(void** ptr){ *ptr = NULL; }

    virtual void printer_ToAnimal(void** ptr){ *ptr = NULL; }

    virtual void printer_ToStaticRes(void**ptr){ *ptr = NULL; }

    /*************以上指针强制转化****************/
  /********************以上虚函数**************************/


    /*******坐标相关*******/
    //获取坐标
    double getDR(){return this->DR;}
    double getUR(){return this->UR;}
    int getBlockDR(){return this->BlockDR;}
    int getBlockUR(){return this->BlockUR;}

    //块、细节坐标转换
    double transDetail( int blockNum ){ return blockNum*BLOCKSIDELENGTH;  }
    int transBlock( double detailNum ){ return (int)detailNum/BLOCKSIDELENGTH; }

    //获取中心点块坐标
    Point getBlockPosition(){ return Point(transBlock(DR), transBlock(UR)); }

    //获取两点间欧几里得距离
    double getDis_E_Detail(Coordinate* __x){return countdistance(DR ,UR , __x->getDR() , __x->getUR());}
    double getDis_E_Detail(Coordinate& __x){return countdistance(DR ,UR , __x.getDR() , __x.getUR());}

    // 获取地图块高度导致的偏移量
    int getMapHeightOffsetY(){ return MapHeightOffsetY; }
    void setMapHeightOffsetY(int m_MapHeightOffsetY){ MapHeightOffsetY = m_MapHeightOffsetY; }

    double getSideLength(){return this->SideLength;}
    double get_BlockSizeLen(){ return BlockSizeLen; }

    /*******可见性相关*******/
    void setExplored(int explored){ this->explored = explored; }
    void setvisible( int visible  ){ this->visible = visible; }
    int getexplored(){ return explored; }
    int getvisible(){ return (int)(visible||timer_Visible>0); }
    void visibleSomeTimes(){ timer_Visible = 250;}
    vector<Point> getViewLab();


    /*******碰撞相关*******/
    double getCrashLength(){ return crashLength; }


    /*******image资源相关信息*******/
    double getimageX(){return this->imageX;}
    double getimageY(){return this->imageY;}
    int getimageH(){return this->imageH;}
    std::list<ImageResource>::iterator getNowRes(){return this->nowres;}


    /*******创建对象相关信息*******/
    int getglobalNum(){return this->globalNum;}
    int getNum(){return this->Num;}
    bool isIncorrect_Num(){ return incorrectNum; }  //判断当前Num在该类下是否正确


    /*****************act相关***************/
    double getActPercent() {return this->actPercent;}
    double getActSpeed(){ return this->actSpeed;}
    int getActName(){return this->actName;}
    int getActNum(){ return this->actNum;}

    int ActNameToActNum(int actName);

    void initActionPersent(){ actPercent = 0; }

    //设置当前交互对象
    void set_interAct(int interSort , int interNum , bool interRepresent = false , bool interBui_builtUp=false);

    void resetINterAct();


    /*****************在窗口内***************/
    void setInWidget(){ this->inWindow = 1; }
    void setNotInWidget(){ this->inWindow = 0; }
    bool isInWidget(){return this->inWindow == 1; }
    void setDRUR( double DR, double UR ){ this->DR = DR; this->UR = UR; }
    void setBlockDRUR( int BlockDR, int BlockUR ){ this->BlockDR = BlockDR; this->BlockUR = BlockUR; }
public:
    int Num;//对象在对应类中的编号
    //比如building类下Num==0为小房子
    //在不同的类有着不同的含义
    bool incorrectNum = false;  //标识Num的值是否在该Sort下合法

    //此时此刻交互对象的类别和Num
    int interactSort = -1;
    int interactNum = -1;
    bool interactBui_builtUp = false;
    bool interact_sameRepresent = false;

    double DR;//当前物体中心所在的坐标位置
    double UR;//在块类中该坐标即为正中心
    //此DR，UR所指游戏中坐标
    int BlockDR = INT_MAX;
    int BlockUR = INT_MAX;
    //对象所在的区块

    int vision = 0; //视野

    double imageX;//该物体的长宽（即占地面积）
    double imageY;//需要根据占地大小来就算确切的绘制偏移量

    double BlockSizeLen = SIZELEN_SINGEL; //物体占地,块坐标， 如小房子，为2，中型房子为3，动物为1
    double SideLength; //占地大小转换成游戏内坐标 边长

    double crashLength = 0; //碰撞箱大小

    int imageH;//绘制y坐标
    //该物体在平面中的上下位置

    int globalNum;
    //每个对象所拥有的独一无二的num，用来确定具体对象

    int explored=0;
    //0为未探索 1为探索
    int visible=0;
    //0为不可见 1为可见

    int timer_Visible = 0;  //暂时可见时间

    int inWindow=0;
    //在游戏窗口内

    int MapHeightOffsetY = 0;    // 地图块高度导致的Y轴方向偏移量

    std::list<ImageResource>::iterator nowres;
    std::list<ImageResource> *nowlist=NULL;
    int nowres_step = 0;
    int nowres_changeRecord = 0;

    static vector<Point> viewLab[5][12];

    /*****************act获取***************/
    double actPercent = 0;
    double actSpeed = 0;
    int actName = ACT_NULL;
    //执行行动时的进度、速率和行动类型
    int actNum=ACT_NULL;
    //动作类型的编号
    /*****************act获取***************/


    /*******可见性相关*******/
    void time_BeVisible(){ timer_Visible--;}
    static void setViewLab( int blockSize , int visionLen );
    static void addViewLab( vector<Point>& blockLab , int lx , int mx , int y , int y_mirr );


    /*******image资源相关信息*******/
    void initNowresTimer(){ nowres_changeRecord = 0; }
    bool isNowresShift();
    void updateImageXYByNowRes();


    /*******坐标相关*******/
    void setDetailPointAttrb_FormBlock();
    void setSideLenth(){ SideLength = BlockSizeLen*BLOCKSIDELENGTH; }
    void updateBlockByDetail(){ BlockDR = transBlock(DR); BlockUR = transBlock(UR); }

};

#endif // COORDINATE_H
