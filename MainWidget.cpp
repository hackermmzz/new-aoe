#include "MainWidget.h"
#include "ui_MainWidget.h"

int g_globalNum=1;
std::map<int,Coordinate*> g_Object;
MainWidget::MainWidget(bool MapJudge, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    //初始化资源
    InitImageResMap(RESPATH);
    InitSoundResMap(RESPATH);
    initBlock();

    this->setFixedSize(GAME_WIDTH,GAME_HEIGHT);
    this->setWindowTitle("Age Of Empires");
    this->setWindowIcon(QIcon());
    //设定游戏计时器
    timer=new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);
    timer->start(40);
    //    connect(timer,&QTimer::timeout,this,&MainWidget::setShowTimeFrame);
    //游戏帧数初始化
    gameframe=0;

    //新建map对象
    map=new Map;
    //map初始化
    map->init(MapJudge);
    //内存图开辟空间
    for(int i=0;i<MEMORYROW;i++)
    {
        memorymap[i]=new int[MEMORYCOLUMN];
    }

    //向地图中添加资源
    initmap();


    connect(timer,SIGNAL(timeout()),this,SLOT(FrameUpdate()));
}

MainWidget::~MainWidget()
{
    delete ui;
    deleteBlock();
}

MainWidget::initmap()
{
    if(map->loadResource()==0)
    {
        return 0;
    }

}

void MainWidget::initBlock()
{
    for(int num=0;num<17;num++)
    {
        Block::allocateblock(num);
        loadResource(Block::getBlockname(num),Block::getblock(num));
        Block::allocategrayblock(num);
        loadGrayRes(Block::getblock(num), Block::getgrayblock(num));
        Block::allocateblackblock(num);
        loadBlackRes(Block::getblock(num),Block::getblackblock(num));
    }
}

void MainWidget::deleteBlock()
{
    for(int i=0;i<1;i++)
    {
        Block::deallocateblock(i);
        Block::deallocateblackblock(i);
        Block::deallocategrayblock(i);
    }
}
void MainWidget::FrameUpdate()
{
    ui->Game->update();
    emit mapmove();
    return;

}
