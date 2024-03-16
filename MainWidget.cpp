#include "MainWidget.h"
#include "ui_MainWidget.h"

int g_globalNum=1;
std::map<int,Coordinate*> g_Object;
ActWidget *acts[ACT_WINDOW_NUM_FREE];
std::map<int, std::string> actNames = {
    {ACT_CREATEFARMER, ACT_CREATEFARMER_NAME},
    {ACT_UPGRADE_AGE, ACT_UPGRADE_AGE_NAME},
    {ACT_UPGRADE_TOWERBUILD, ACT_UPGRADE_TOWERBUILD_NAME},
    {ACT_UPGRADE_WOOD, ACT_UPGRADE_WOOD_NAME},
    {ACT_UPGRADE_STONE, ACT_UPGRADE_STONE_NAME},
    {ACT_UPGRADE_FARM, ACT_UPGRADE_FARM_NAME},
    {ACT_STOP, ACT_STOP_NAME},
    {ACT_BUILD, ACT_BUILD_NAME},
    {ACT_BUILD_HOUSE, ACT_BUILD_HOUSE_NAME},
    {ACT_BUILD_GRANARY, ACT_BUILD_GRANARY_NAME},
    {ACT_BUILD_STOCK, ACT_BUILD_STOCK_NAME},
    {ACT_BUILD_CANCEL, ACT_BUILD_CANCEL_NAME},
    {ACT_BUILD_FARM, ACT_BUILD_FARM_NAME},
    {ACT_BUILD_MARKET, ACT_BUILD_MARKET_NAME},
    {ACT_BUILD_ARROWTOWER, ACT_BUILD_ARROWTOWER_NAME},
    {ACT_NULL, ACT_NULL_NAME}
};
MainWidget::MainWidget(int MapJudge, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    // 初始化游戏资源
    InitImageResMap(RESPATH);   // 图像资源
    InitSoundResMap(RESPATH);   // 音频资源

    // 初始化游戏元素
    initBlock();
    initBuilding();
    initAnimal();
    initFarmer();

    // 设置当前窗口属性
    this->setFixedSize(GAME_WIDTH,GAME_HEIGHT); // 设置窗口大小
    this->setWindowTitle("Age Of Empires");     // 设置标题
    this->setWindowIcon(QIcon());               // 设置图标（暂空）

    SelectWidget *sel = new SelectWidget(this); // 设置左下角窗口
    sel->move(20, 810);
    sel->show();

    ActWidget *acts_[ACT_WINDOW_NUM_FREE] = {ui->interact1, ui->interact2, ui->interact3, ui->interact4, ui->interact5, ui->interact6, ui->interact7, ui->interact8};
    for(int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
    {
        acts[i] = acts_[i];
        acts[i]->setStatus(0);
        acts[i]->setNum(i);
        acts[i]->hide();
        acts[i]->setAttribute(Qt::WA_Hover, true);
        acts[i]->installEventFilter(this);
    }
    connect(ui->interact1,SIGNAL(actPress(int)),sel,SLOT(widgetAct(int)));
    connect(ui->interact2,SIGNAL(actPress(int)),sel,SLOT(widgetAct(int)));
    connect(ui->interact3,SIGNAL(actPress(int)),sel,SLOT(widgetAct(int)));
    connect(ui->interact4,SIGNAL(actPress(int)),sel,SLOT(widgetAct(int)));
    connect(ui->interact5,SIGNAL(actPress(int)),sel,SLOT(widgetAct(int)));
    connect(ui->interact6,SIGNAL(actPress(int)),sel,SLOT(widgetAct(int)));
    connect(ui->interact7,SIGNAL(actPress(int)),sel,SLOT(widgetAct(int)));
    connect(ui->interact8,SIGNAL(actPress(int)),sel,SLOT(widgetAct(int)));
    connect(ui->Game,SIGNAL(sendView(int,int,int)),sel,SLOT(getBuild(int,int,int)));
    connect(sel,SIGNAL(sendBuildMode(int)),ui->Game,SLOT(setBuildMode(int)));
    // 设定游戏计时器
    timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);
    timer->start(40);
    //    connect(timer,&QTimer::timeout,this,&MainWidget::setShowTimeFrame);
    showTimer=new QTimer(this);
    showTimer->setTimerType(Qt::PreciseTimer);
    showTimer->start(1000);
    connect(showTimer, &QTimer::timeout, sel, &SelectWidget::timeUpdate);
    connect(timer, &QTimer::timeout, sel, &SelectWidget::frameUpdate);
//    connect((const QObject*)core, SIGNAL(clickOnObject()), sel, SLOT(initActs()));
    core->sel = sel;
    // 游戏帧数初始化
    gameframe = 0;

    // 新建map对象并初始化
    map = new Map;
    map->init(MapJudge);

    // 内存图开辟空间
    for(int i = 0;i < MEMORYROW; i++)
    {
        memorymap[i] = new int[MEMORYCOLUMN];
    }

    // 玩家开辟空间
    for(int i = 0; i < MAXPLAYER; i++)
    {
        player[i] = new Player(i);
    }

    // 向地图中添加资源
    initmap();
    player[0]->addBuilding( BUILDING_CENTER ,30,30);
    player[0]->addBuilding(BUILDING_CENTER, 33, 33);
    player[0]->addFarmer(25*BLOCKSIDELENGTH,25*BLOCKSIDELENGTH);

    core = new Core(map,player,memorymap,mouseEvent);
    connect(timer,SIGNAL(timeout()),this,SLOT(FrameUpdate()));
}

// MainWidget析构函数
MainWidget::~MainWidget()
{
    delete ui;
    deleteBlock();
    deleteAnimal();
    deleteFarmer();
    deleteBuilding();
    delete core;
}

// 初始化地图
MainWidget::initmap()
{
    if(map->loadResource()==0)
    {
        return 0;
    }

}

// 初始化区块
void MainWidget::initBlock()
{
    for(int num=0;num<BLOCK_COUNT;num++)
    {
        Block::allocateblock(num);
        loadResource(Block::getBlockname(num),Block::getblock(num));
        Block::allocategrayblock(num);
        loadGrayRes(Block::getblock(num), Block::getgrayblock(num));
        Block::allocateblackblock(num);
        loadBlackRes(Block::getblock(num),Block::getblackblock(num));
    }
}

// 初始化建筑
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

// 初始化动物
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

// 初始化农民状态
void MainWidget::initFarmer()
{
    //加载素材
    //"Villager","Lumber","Gatherer","Miner","Hunter","Farmer","Worker"

    for(int statei=0;statei<7;statei++)
    {
        for(int i=0;i<=4;i++)
        {
            Farmer::allocateWalk(statei,i);
            Farmer::allocateStand(statei,i);
            Farmer::allocateDie(statei,i);
            loadResource(Farmer::getFarmerName(statei)+"_Stand_"+direction[i],Farmer::getStand(statei,i));
            loadResource(Farmer::getFarmerName(statei)+"_Walk_"+direction[i],Farmer::getWalk(statei,i));
            loadResource(Farmer::getFarmerName(statei)+"_Die_"+direction[i],Farmer::getDie(statei,i));
        }
        for(int i=5;i<8;i++)
        {
            Farmer::allocateWalk(statei,i);
            Farmer::allocateStand(statei,i);
            Farmer::allocateDie(statei,i);
            flipResource(Farmer::getWalk(statei,8-i),Farmer::getWalk(statei,i));
            flipResource(Farmer::getStand(statei,8-i),Farmer::getStand(statei,i));
            flipResource(Farmer::getDie(statei,8-i),Farmer::getDie(statei,i));
        }
    }
    // Work
    for(int statei=0;statei<7;statei++)
    {
        if(statei==0)
        {
            continue;
        }
        for(int i=0;i<=4;i++)
        {
            Farmer::allocateWork(statei,i);
            loadResource(Farmer::getFarmerName(statei)+"_Work_"+direction[i],Farmer::getWork(statei,i));
        }
        for(int i=5;i<8;i++)
        {
            Farmer::allocateWork(statei,i);
            flipResource(Farmer::getWork(statei,8-i),Farmer::getWork(statei,i));
        }
    }
    // Attack
    for(int statei=0;statei<7;statei++)
    {
        if(statei==2||statei==3||statei==5||statei==6)
        {
            continue;
        }
        for(int i=0;i<=4;i++)
        {
            Farmer::allocateAttack(statei,i);
            loadResource(Farmer::getFarmerName(statei)+"_Attack_"+direction[i],Farmer::getAttack(statei,i));
        }
        for(int i=5;i<8;i++)
        {
            Farmer::allocateAttack(statei,i);
            flipResource(Farmer::getAttack(statei,8-i),Farmer::getAttack(statei,i));
        }
    }
    // Carry 考虑如何对接
    for(int statei=0;statei<=4;statei++)
    {
        if(statei==0)
        {
            continue;
        }
        for(int i=0;i<=4;i++)
        {
            Farmer::allocateCarry(statei,i);
            loadResource(Farmer::getFarmerCarry(statei)+"_"+direction[i],Farmer::getCarry(statei,i));
        }
        for(int i=5;i<8;i++)
        {
            Farmer::allocateCarry(statei,i);
            flipResource(Farmer::getCarry(statei,8-i),Farmer::getCarry(statei,i));
        }
    }

}

// 删除区块资源
void MainWidget::deleteBlock()
{
    for(int i=0;i<1;i++)
    {
        Block::deallocateblock(i);
        Block::deallocateblackblock(i);
        Block::deallocategrayblock(i);
    }
}

// 删除建筑资源
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

// 删除动物资源
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

// 删除农民资源
void MainWidget::deleteFarmer()
{
    // 清理素材资源
    for(int statei = 0; statei < 7; statei++)
    {
        for(int i = 0; i <= 4; i++)
        {
            Farmer::deallocateWalk(statei, i);
            Farmer::deallocateStand(statei, i);
            Farmer::deallocateDie(statei, i);
        }
        for(int i = 5; i < 8; i++)
        {
            Farmer::deallocateWalk(statei, i);
            Farmer::deallocateStand(statei, i);
            Farmer::deallocateDie(statei, i);
        }
    }

    // 清理 Work 资源
    for(int statei = 0; statei < 7; statei++)
    {
        if(statei == 0)
        {
            continue;
        }

        for(int i = 0; i <= 4; i++)
        {
            Farmer::deallocateWork(statei, i);
        }
        for(int i = 5; i < 8; i++)
        {
            Farmer::deallocateWork(statei, i);
        }
    }

    // 清理 Attack 资源
    for(int statei = 0; statei < 7; statei++)
    {
        if(statei == 2 || statei == 3 || statei == 5 || statei == 6)
        {
            continue;
        }

        for(int i = 0; i <= 4; i++)
        {
            Farmer::deallocateAttack(statei, i);
        }
        for(int i = 5; i < 8; i++)
        {
            Farmer::deallocateAttack(statei, i);
        }
    }

    // 清理 Carry 资源
    for(int statei = 0; statei <= 4; statei++)
    {
        if(statei == 0)
        {
            continue;
        }

        for(int i = 0; i <= 4; i++)
        {
            Farmer::deallocateCarry(statei, i);
        }
        for(int i = 5; i < 8; i++)
        {
            Farmer::deallocateCarry(statei, i);
        }
    }

}

bool MainWidget::eventFilter(QObject *watched, QEvent *event)
{
    for(int i = 0; i < 10; i++)
    {
        if(watched == acts[i] && !acts[i]->isHidden()) {
            if(event->type() == QEvent::HoverEnter) {
                ui->tipLbl->setText(QString::fromStdString(actNames[acts[i]->getActName()]));
                if(acts[i]->getStatus() != 2)
                {
                    acts[i]->setStatus(0);
                    acts[i]->update();
                }
            }
            else if(event->type() == QEvent::MouseButtonRelease && acts[i]->getStatus() != 2){
                acts[i]->setStatus(0);
            }
            else if(event->type() == QEvent::MouseButtonPress && acts[i]->getStatus() != 2){
                acts[i]->setStatus(1);
            }
            else if(event->type() == QEvent::HoverLeave && acts[i]->getStatus() != 2)
            {
                ui->tipLbl->setText("");
            }
        }
    }
    return QWidget::eventFilter(watched,event);
}

// 游戏帧更新
void MainWidget::FrameUpdate()
{
    gameframe++;
    ui->lcdNumber->display(gameframe);
    ui->Game->update();
    core->gameUpdate();
    emit mapmove();
    return;
}
