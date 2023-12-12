#include "MainWidget.h"
#include "ui_MainWidget.h"

int g_globalNum=1;
std::map<int,Coordinate*> g_Object;
MainWidget::MainWidget(int MapJudge, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    // 初始化资源
    InitImageResMap(RESPATH);   // 图像资源
    InitSoundResMap(RESPATH);   // 音频资源
    initBlock();

    this->setFixedSize(GAME_WIDTH,GAME_HEIGHT); // 设置窗口大小
    this->setWindowTitle("Age Of Empires");     // 设置标题
    this->setWindowIcon(QIcon());               // 设置图标（暂空）

    // 设定游戏计时器
    timer=new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);
    timer->start(40);
    //    connect(timer,&QTimer::timeout,this,&MainWidget::setShowTimeFrame);
    // 游戏帧数初始化
    gameframe=0;

    // 新建map对象
    map=new Map;
    // map初始化
    map->init(MapJudge);
    // 内存图开辟空间
    for(int i=0;i<MEMORYROW;i++)
    {
        memorymap[i]=new int[MEMORYCOLUMN];
    }

    // 向地图中添加资源
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
