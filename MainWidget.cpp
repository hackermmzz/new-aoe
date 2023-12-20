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
    initBuilding();
    initAnimal();

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
    //玩家开辟空间
    for(int i=0;i<MAXPLAYER;i++)
        player[i]=new Player();

    // 向地图中添加资源
    initmap();

    connect(timer,SIGNAL(timeout()),this,SLOT(FrameUpdate()));

    player[0]->addBuilding(0,0,0);

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
void MainWidget::initBuilding()
{
    for (int i = 0; i < 4; i++)
    {
        Building::allocatebuild(i);
        loadResource(Building::getBuildingname(i),Building::getBuild(i));
    }
    for (int i = 1; i < 3; i++)
    {
        for(int j=0;j<7;j++)
        {
            Building::allocatebuilt(i,j);
            loadResource(Building::getBuiltname(i,j),Building::getBuilt(i,j));
        }
    }
}

void MainWidget::initAnimal()
{
    for(int num=0;num<5;num++)
    {
        if(num==ANIMAL_TREE)
        {
            Animal::allocateStand(num,0);
            Animal::allocateDie(num,0);
            loadResource(Animal::getAnimalName(num),Animal::getStand(num,0));
            loadResource(Animal::getAnimalcarcassname(num),Animal::getDie(num,0));
            continue;
        }
        if(num==ANIMAL_GAZELLE||num==ANIMAL_LION)
        {
            for(int i=0;i<=4;i++)
            {
                Animal::allocateRun(num,i);
                loadResource(Animal::getAnimalName(num)+"_Run_"+direction[i],Animal::getRun(num,i));
            }
            for(int i=5;i<8;i++)
            {
                Animal::allocateRun(num,i);
                flipResource(Animal::getRun(num,8-i),Animal::getRun(num,i));
            }
        }
        for(int i=0;i<=4;i++)
        {
            Animal::allocateAttack(num,i);
            Animal::allocateWalk(num,i);
            Animal::allocateStand(num,i);
            Animal::allocateDie(num,i);
            loadResource(Animal::getAnimalName(num)+"_Stand_"+direction[i],Animal::getStand(num,i));
            loadResource(Animal::getAnimalName(num)+"_Walk_"+direction[i],Animal::getWalk(num,i));
            loadResource(Animal::getAnimalName(num)+"_Attack_"+direction[i],Animal::getAttack(num,i));
            loadResource(Animal::getAnimalName(num)+"_Die_"+direction[i],Animal::getDie(num,i));
        }
        for(int i=5;i<8;i++)
        {
            Animal::allocateAttack(num,i);
            Animal::allocateWalk(num,i);
            Animal::allocateStand(num,i);
            Animal::allocateDie(num,i);
            flipResource(Animal::getWalk(num,8-i),Animal::getWalk(num,i));
            flipResource(Animal::getStand(num,8-i),Animal::getStand(num,i));
            flipResource(Animal::getAttack(num,8-i),Animal::getAttack(num,i));
            flipResource(Animal::getDie(num,8-i),Animal::getDie(num,i));
        }
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
void MainWidget::deleteBuilding()
{
    for (int i = 0; i < 4; i++)
    {
        Building::deallocatebuild(i);
    }
    for (int i = 1; i < 3; i++)
    {
        for(int j=0;j<7;j++)
        {
            Building::deallocatebuilt(i,j);
        }
    }
}
void MainWidget::deleteAnimal()
{
    for (int num = 0; num < 3; num++)
    {
        if (num == 0)
        {
            Animal::deallocateStand(num, 0);
            Animal::deallocateDie(num, 0);
            continue;
        }
        if (num == 1|| num == 3)
        {
            for (int i = 0; i <= 4; i++)
            {
                Animal::deallocateRun(num, i);
            }
            for (int i = 5; i < 8; i++)
            {
                Animal::deallocateRun(num, i);
            }
        }
        for (int i = 0; i <= 4; i++)
        {
            Animal::deallocateAttack(num, i);
            Animal::deallocateWalk(num, i);
            Animal::deallocateStand(num, i);
            Animal::deallocateDie(num, i);
        }
        for (int i = 5; i < 8; i++)
        {
            Animal::deallocateAttack(num, i);
            Animal::deallocateWalk(num, i);
            Animal::deallocateStand(num, i);
            Animal::deallocateDie(num, i);
        }
    }
}


void MainWidget::FrameUpdate()
{
    ui->Game->update();
    core->gameUpdate(map,player,memorymap);
    emit mapmove();
    return;

}
