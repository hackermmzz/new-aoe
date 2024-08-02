#include "MainWidget.h"
#include "ui_MainWidget.h"

int g_globalNum=1;
int g_frame=0;
int mapmoveFrequency = 1;
extern Score usrScore;
extern Score enemyScore;
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
    {ACT_NULL, ACT_NULL_NAME},
    {ACT_ARMYCAMP_CREATE_CLUBMAN, ACT_ARMYCAMP_CREATE_CLUBMAN_NAME},
    {ACT_ARMYCAMP_CREATE_SLINGER, ACT_ARMYCAMP_CREATE_SLINGER_NAME},
    {ACT_ARMYCAMP_UPGRADE_CLUBMAN, ACT_ARMYCAMP_UPGRADE_CLUBMAN_NAME},
    {ACT_BUILD_ARMYCAMP, ACT_BUILD_ARMYCAMP_NAME},
    {ACT_BUILD_RANGE, ACT_BUILD_RANGE_NAME},
    {ACT_BUILD_STABLE, ACT_BUILD_STABLE_NAME},
    {ACT_RANGE_CREATE_BOWMAN, ACT_RANGE_CREATE_BOWMAN_NAME},
    {ACT_RESEARCH_WALL, ACT_RESEARCH_WALL_NAME},
    {ACT_STABLE_CREATE_SCOUT, ACT_STABLE_CREATE_SCOUT_NAME},
    {ACT_STOCK_UPGRADE_DEFENSE_ARCHER, ACT_STOCK_UPGRADE_DEFENSE_ARCHER_NAME},
    {ACT_STOCK_UPGRADE_DEFENSE_INFANTRY, ACT_STOCK_UPGRADE_DEFENSE_INFANTRY_NAME},
    {ACT_STOCK_UPGRADE_DEFENSE_RIDER, ACT_STOCK_UPGRADE_DEFENSE_RIDER_NAME},
    {ACT_STOCK_UPGRADE_USETOOL, ACT_STOCK_UPGRADE_USETOOL_NAME},

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
    initStaticResource();
    initFarmer();
    initArmy();
    initMissile();

    // 设置当前窗口属性
    this->setFixedSize(GAME_WIDTH,GAME_HEIGHT); // 设置窗口大小
    this->setWindowTitle("Age Of Empires");     // 设置标题
    this->setWindowIcon(QIcon());               // 设置图标（暂空）

    sel = new SelectWidget(this); // 设置左下角窗口
    sel->move(20, 810);
    sel->show();
    ActWidget *acts_[ACT_WINDOW_NUM_FREE] = {ui->interact1, ui->interact2, ui->interact3, ui->interact4, ui->interact5, ui->interact6, ui->interact7, ui->interact8 , ui->interact9 , ui->interact10};
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
    connect(ui->interact9,SIGNAL(actPress(int)),sel,SLOT(widgetAct(int)));
    connect(ui->interact10,SIGNAL(actPress(int)),sel,SLOT(widgetAct(int)));
    connect(ui->Game,SIGNAL(sendView(int,int,int)),sel,SLOT(getBuild(int,int,int)));
    connect(sel,SIGNAL(sendBuildMode(int)),ui->Game,SLOT(setBuildMode(int)));
    // 设定游戏计时器
    timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);
    timer->start(40);
    showTimer=new QTimer(this);
    showTimer->setTimerType(Qt::PreciseTimer);
    showTimer->start(1000);
    pbuttonGroup = new QButtonGroup(this);
    pbuttonGroup->addButton(ui->radioButton_1,0);
    pbuttonGroup->addButton(ui->radioButton_2,1);
    pbuttonGroup->addButton(ui->radioButton_4,2);
    pbuttonGroup->addButton(ui->radioButton_8,3);
    //绑定倍速按钮
    connect(ui->radioButton_1, SIGNAL(clicked()), this, SLOT(onRadioClickSlot()));
    connect(ui->radioButton_2, SIGNAL(clicked()), this, SLOT(onRadioClickSlot()));
    connect(ui->radioButton_4, SIGNAL(clicked()), this, SLOT(onRadioClickSlot()));
    connect(ui->radioButton_8, SIGNAL(clicked()), this, SLOT(onRadioClickSlot()));
    //时间增加
    connect(showTimer, &QTimer::timeout, sel, &SelectWidget::timeUpdate);

    connect(timer, &QTimer::timeout, sel, &SelectWidget::frameUpdate);
    //    connect((const QObject*)core, SIGNAL(clickOnObject()), sel, SLOT(initActs()));

    // 玩家开辟空间
    for(int i = 0; i < MAXPLAYER; i++){player[i] = new Player(i);}
    player[1]->set_AllTechnology();
//    player[0]->set_AllTechnology();

    // 新建map对象并初始化
    map = new Map;
    //为map添加player指针
    map->setPlayer(player);
    map->init(MapJudge);
    map->init_Map_Height();
    // 内存图开辟空间
    for(int i = 0;i < MEMORYROW; i++)
    {
        memorymap[i] = new int[MEMORYCOLUMN];
    }
    // 向地图中添加资源
    initmap();
    buildInitialStock();

    core = new Core(map,player,memorymap,mouseEvent);
    sel->setCore(core);
    // AI初始化
    UsrAi=new UsrAI();
    EnemyAi=new EnemyAI();
    connect(this,&MainWidget::startAI,UsrAi,&AI::startProcessing);
    connect(this,&MainWidget::startAI,EnemyAi,&AI::startProcessing);
    connect(UsrAi, &AI::cheatAttack, EnemyAi, &EnemyAI::onWaveAttack);
    UsrAi->start();
    EnemyAi->start();

    core->sel = sel;
    connect(timer,SIGNAL(timeout()),this,SLOT(FrameUpdate()));

    //设置user初始时代
    player[0]->setCiv(CIVILIZATION_TOOLAGE);
    //设置user初始资源
    player[0]->changeResource(200,200,200,200);
//    player[1]->addArmy(AT_SCOUT , 35*BLOCKSIDELENGTH , 35*BLOCKSIDELENGTH);

    // 设置鼠标追踪
    ui->Game->setMouseTracking(true);
    ui->Game->setAttribute(Qt::WA_MouseTracking,true);
    ui->Game->installEventFilter(this);

    // 设置提示颜色
    QPalette pe;
    pe.setColor(QPalette::WindowText,Qt::green);
    ui->tip->setPalette(pe);
    tipLbl = ui->tip;
    // 给小地图传递list
    ui->mapView->setFriendlyFarmerList(&(player[0]->human));
    ui->mapView->setEnemyFarmerList(&(player[1]->human));
    ui->mapView->setFriendlyBuildList(&(player[0]->build));
    ui->mapView->setEnemyBuildList(&(player[1]->build));
    ui->mapView->setAnimalList(&(map->animal));
    ui->mapView->setResList(&(map->staticres));
    debugText("blue"," 游戏开始");
}

// MainWidget析构函数
MainWidget::~MainWidget()
{
    UsrAi->stopProcessing();
    EnemyAi->stopProcessing();
    UsrAi->wait();
    EnemyAi->wait();
    delete UsrAi;
    delete EnemyAi;
    delete ui;
    delete pbuttonGroup;
    deleteBlock();
    deleteAnimal();
    deleteStaticResource();
    deleteFarmer();
    deleteBuilding();
    deleteArmy();
    deleteMissile();
    delete core;
}

void MainWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPixmap pix;
    pix=resMap["Interface"].front();
    painter.drawPixmap(0,0,1440,45,pix);
    pix=resMap["Interface"].back();
    painter.drawPixmap(0,GAME_HEIGHT - 203.5,1440,203.5,pix);

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
        for(int j=0;j<10;j++)
        {
            Building::allocatebuilt(i,j);
            loadResource(Building::getBuiltname(i,j),Building::getBuilt(i,j));
        }
    }

    //市镇中心
    Building::setActNames(BUILDING_CENTER , 0 , ACT_CREATEFARMER);
    Building::setActNames(BUILDING_CENTER , 1 , ACT_UPGRADE_AGE);
    //谷仓
    Building::setActNames(BUILDING_GRANARY , 0 , ACT_RESEARCH_WALL);
    Building::setActNames(BUILDING_GRANARY , 1 , ACT_UPGRADE_TOWERBUILD);
    //仓库
    Building::setActNames(BUILDING_STOCK , 0 , ACT_STOCK_UPGRADE_USETOOL);
    Building::setActNames(BUILDING_STOCK , 1 , ACT_STOCK_UPGRADE_DEFENSE_INFANTRY);
    Building::setActNames(BUILDING_STOCK , 2 , ACT_STOCK_UPGRADE_DEFENSE_ARCHER);
    Building::setActNames(BUILDING_STOCK , 3 , ACT_STOCK_UPGRADE_DEFENSE_RIDER);
    //市场
    Building::setActNames(BUILDING_MARKET , 0 , ACT_UPGRADE_WOOD);
    Building::setActNames(BUILDING_MARKET , 1 , ACT_UPGRADE_STONE);
    Building::setActNames(BUILDING_MARKET , 2 , ACT_UPGRADE_FARM);
    Building::setActNames(BUILDING_MARKET , 3 , ACT_UPGRADE_GOLD);
    //军队
    Building::setActNames(BUILDING_ARMYCAMP , 0 , ACT_ARMYCAMP_CREATE_CLUBMAN);
    Building::setActNames(BUILDING_ARMYCAMP , 5 , ACT_ARMYCAMP_UPGRADE_CLUBMAN);
    Building::setActNames(BUILDING_ARMYCAMP , 3 , ACT_ARMYCAMP_CREATE_SLINGER);
    Building::setActNames(BUILDING_RANGE , 0 , ACT_RANGE_CREATE_BOWMAN);
    Building::setActNames(BUILDING_STABLE , 0 , ACT_STABLE_CREATE_SCOUT);
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
        else if(num == ANIMAL_FOREST)
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
            Animal::allocateDisappear(num,i);
            loadResource(Animal::getAnimalName(num)+"_Stand_"+direction[i],Animal::getStand(num,i));
            loadResource(Animal::getAnimalName(num)+"_Walk_"+direction[i],Animal::getWalk(num,i));
            loadResource(Animal::getAnimalName(num)+"_Attack_"+direction[i],Animal::getAttack(num,i));
            loadResource(Animal::getAnimalName(num)+"_Die_"+direction[i],Animal::getDie(num,i));
            loadResource(Animal::getAnimalName(num)+"_Disappear_"+direction[i],Animal::getDisappear(num,i));
        }
        for(int i=5;i<8;i++)
        {
            Animal::allocateAttack(num,i);
            Animal::allocateWalk(num,i);
            Animal::allocateStand(num,i);
            Animal::allocateDie(num,i);
            Animal::allocateDisappear(num,i);
            flipResource(Animal::getWalk(num,8-i),Animal::getWalk(num,i));
            flipResource(Animal::getStand(num,8-i),Animal::getStand(num,i));
            flipResource(Animal::getAttack(num,8-i),Animal::getAttack(num,i));
            flipResource(Animal::getDie(num,8-i),Animal::getDie(num,i));
            flipResource(Animal::getDisappear(num,8-i),Animal::getDisappear(num,i));
        }
    }
}

// 初始化不可移动的资源
void MainWidget::initStaticResource()
{
    for(int num = 0; num<3; num++)
    {
        StaticRes::allocateStaticResource(num);
        loadResource(StaticRes::getStaticResName(num) , StaticRes::getStaticResource(num));
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
            Farmer::allocateDisappear(statei,i);
            loadResource(Farmer::getFarmerName(statei)+"_Stand_"+direction[i],Farmer::getStand(statei,i));
            loadResource(Farmer::getFarmerName(statei)+"_Walk_"+direction[i],Farmer::getWalk(statei,i));
            loadResource(Farmer::getFarmerName(statei)+"_Die_"+direction[i],Farmer::getDie(statei,i));
            loadResource(Farmer::getFarmerName(statei)+"_Disappear_"+direction[i],Farmer::getDisappear(statei,i));
        }
        for(int i=5;i<8;i++)
        {
            Farmer::allocateWalk(statei,i);
            Farmer::allocateStand(statei,i);
            Farmer::allocateDie(statei,i);
            Farmer::allocateDisappear(statei,i);
            flipResource(Farmer::getWalk(statei,8-i),Farmer::getWalk(statei,i));
            flipResource(Farmer::getStand(statei,8-i),Farmer::getStand(statei,i));
            flipResource(Farmer::getDie(statei,8-i),Farmer::getDie(statei,i));
            flipResource(Farmer::getDisappear(statei,8-i),Farmer::getDisappear(statei,i));
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

void MainWidget::initArmy()
{
    //加载素材
    //"Archer","Axeman","Clubman","Scout"

    // Stand Walk Die
    for(int statei=0;statei<7;statei++)
    {
        for(int level = 0 ; level<2;level++)
        {
            for(int i=0;i<=4;i++)
            {
                Army::allocateWalk(0,statei,level,i);
                Army::allocateStand(0,statei,level,i);
                Army::allocateDie(0,statei,level,i);
                Army::allocateDisappear(0,statei,level,i);
                Army::allocateAttack(0,statei,level,i);
                loadResource(Army::getArmyName(statei,level)+"_Attack_"+direction[i],Army::getAttack(0,statei,level,i));
                loadResource(Army::getArmyName(statei,level)+"_Disappear_"+direction[i],Army::getDisappear(0,statei,level,i));
                loadResource(Army::getArmyName(statei,level)+"_Stand_"+direction[i],Army::getStand(0,statei,level,i));
                loadResource(Army::getArmyName(statei,level)+"_Walk_"+direction[i],Army::getWalk(0,statei,level,i));
                loadResource(Army::getArmyName(statei,level)+"_Die_"+direction[i],Army::getDie(0,statei,level,i));

                Army::allocateWalk(1,statei,level,i);
                Army::allocateStand(1,statei,level,i);
                Army::allocateDie(1,statei,level,i);
                Army::allocateDisappear(1,statei,level,i);
                Army::allocateAttack(1,statei,level,i);
                loadResource("Enemy_"+Army::getArmyName(statei,level)+"_Attack_"+direction[i],Army::getAttack(1,statei,level,i));
                loadResource("Enemy_"+Army::getArmyName(statei,level)+"_Disappear_"+direction[i],Army::getDisappear(1,statei,level,i));
                loadResource("Enemy_"+Army::getArmyName(statei,level)+"_Stand_"+direction[i],Army::getStand(1,statei,level,i));
                loadResource("Enemy_"+Army::getArmyName(statei,level)+"_Walk_"+direction[i],Army::getWalk(1,statei,level,i));
                loadResource("Enemy_"+Army::getArmyName(statei,level)+"_Die_"+direction[i],Army::getDie(1,statei,level,i));
            }
            for(int i=5;i<8;i++)
            {
                Army::allocateWalk(0,statei,level,i);
                Army::allocateStand(0,statei,level,i);
                Army::allocateDie(0,statei,level,i);
                Army::allocateDisappear(0,statei,level,i);
                Army::allocateAttack(0,statei,level,i);
                flipResource(Army::getAttack(0,statei,level,8-i),Army::getAttack(0,statei,level,i));
                flipResource(Army::getDisappear(0,statei,level,8-i),Army::getDisappear(0,statei,level,i));
                flipResource(Army::getWalk(0,statei,level,8-i),Army::getWalk(0,statei,level,i));
                flipResource(Army::getStand(0,statei,level,8-i),Army::getStand(0,statei,level,i));
                flipResource(Army::getDie(0,statei,level,8-i),Army::getDie(0,statei,level,i));

                Army::allocateWalk(1,statei,level,i);
                Army::allocateStand(1,statei,level,i);
                Army::allocateDie(1,statei,level,i);
                Army::allocateDisappear(1,statei,level,i);
                Army::allocateAttack(1,statei,level,i);
                flipResource(Army::getAttack(1,statei,level,8-i),Army::getAttack(1,statei,level,i));
                flipResource(Army::getDisappear(1,statei,level,8-i),Army::getDisappear(1,statei,level,i));
                flipResource(Army::getWalk(1,statei,level,8-i),Army::getWalk(1,statei,level,i));
                flipResource(Army::getStand(1,statei,level,8-i),Army::getStand(1,statei,level,i));
                flipResource(Army::getDie(1,statei,level,8-i),Army::getDie(1,statei,level,i));
            }
        }
    }

}

void MainWidget::initMissile()
{
    //加载飞行物素材

    for(int statei = 0; statei<NUMBER_MISSILE; statei++)
    {
        Missile::allocatemissile(statei);
        loadResource(Missile::getMissilename(statei),Missile::getmissile(statei));
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
        for(int j=0;j<10;j++)
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
            Animal::deallocateDisappear(num,i);
        }
        for (int i = 5; i < 8; i++)
        {
            Animal::deallocateAttack(num, i);
            Animal::deallocateWalk(num, i);
            Animal::deallocateStand(num, i);
            Animal::deallocateDie(num, i);
            Animal::deallocateDisappear(num,i);
        }
    }
}

// 删除不可移动的资源
void MainWidget::deleteStaticResource()
{
    for(int num = 0; num<3; num++)
        StaticRes::deallocateStaticResource(num);
}

// 删除农民资源
void MainWidget::deleteFarmer()
{
    // 清理素材资源
    for(int statei = 0; statei < 7; statei++)
    {
        for(int i = 0; i < 8; i++)
        {
            Farmer::deallocateWalk(statei, i);
            Farmer::deallocateStand(statei, i);
            Farmer::deallocateDie(statei, i);
            Farmer::deallocateDisappear(statei, i);
        }
    }

    // 清理 Work 资源
    for(int statei = 0; statei < 7; statei++)
    {
        if(statei == 0)
        {
            continue;
        }

        for(int i = 0; i < 8; i++)
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

        for(int i = 0; i < 8; i++)
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

        for(int i = 0; i < 8; i++)
        {
            Farmer::deallocateCarry(statei, i);
        }
    }

}

void MainWidget::deleteArmy()
{
    // 清理素材资源
    for(int statei = 0; statei < 7; statei++)
    {
        for(int level = 0 ;level<2;level++)
        {
            for(int i = 0; i < 8; i++)
            {
                for(int re = 0 ; re <NOWPLAYER; re++)
                {
                    Army::deallocateWalk(re, statei,level, i);
                    Army::deallocateStand(re, statei,level, i);
                    Army::deallocateDie(re , statei,level, i);
                    Army::deallocateAttack(re , statei,level, i);
                    Army::deallocateDisappear(re , statei,level, i);
                }
            }
        }
    }
}

// 删除飞行物资源
void MainWidget::deleteMissile()
{
    for(int statei = 0; statei < NUMBER_MISSILE; statei++ ) Missile::deallocatemissile(statei);
}

bool MainWidget::eventFilter(QObject *watched, QEvent *event)
{
    for(int i = 0; i < 10; i++)
    {
        if(watched == acts[i] && !acts[i]->isHidden()) {
            if(event->type() == QEvent::HoverEnter) {
                ui->tip->setText(QString::fromStdString(actNames[sel->actions[i]]));
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
                ui->tip->setText("");
            }
        }
    }
    return QWidget::eventFilter(watched,event);
}

void MainWidget::showPlayerResource(int playerRepresent)
{
    int wood,food,stone,gold;
    core->getPlayerNowResource(playerRepresent,wood,food,stone,gold);
    ui->resWood->setText(QString::number(wood));
    ui->resFood->setText(QString::number(food));
    ui->resStone->setText(QString::number(gold));
    ui->resGold->setText(QString::number(stone));
}

void MainWidget::statusUpdate()
{
    showPlayerResource(0);

    QFont currentFont = ui->score0->font();
    ui->score0->setTextFormat(Qt::RichText); // 确保使用富文本格式
    ui->score0->setText("<html><head/><body><p><span style=\" font-size:12pt; color:#00007f;\">"
                        +QString::number(usrScore.getScore())+"</span></p></body></html>");

    ui->score1->setTextFormat(Qt::RichText); // 确保使用富文本格式
    ui->score1->setText("<html><head/><body><p><span style=\" font-size:12pt; color:#aa0000;\">"
                        +QString::number(enemyScore.getScore())+"</span></p></body></html>");

    ui->mapView->screenL = ui->Game->getBlockDR();
    ui->mapView->screenU = ui->Game->getBlockUR();
    ui->statusLbl->setText(sel->getShowTime() + QString::fromStdString("\n"));
    ui->version->setText(QString::fromStdString(GAME_VERSION));
}

void MainWidget::gameDataUpdate()
{
    core->gameUpdate();
    core->infoShare();
    emit startAI();
}
void MainWidget::paintUpdate()
{
    statusUpdate();
    ui->Game->update();
    ui->mapView->update();
    ui->tip->update();
    ui->statusLbl->update();
    emit mapmove();
}

bool MainWidget::isLoss()
{
    return sel->getSecend()>=GAME_LOSE_SEC || player[0]->get_centerNum()<1;
}
bool MainWidget::isWin()
{
    bool enemyClear = true;
    std::list<Human *>::iterator enemyiter = player[1]->human.begin(), enemyitere = player[1]->human.end();
    while(enemyiter != enemyitere)
    {
        if(!(*enemyiter)->isDie())
        {
            enemyClear = false;
            break;
        }
        enemyiter++;
    }
    return enemyClear;
}

void MainWidget::judgeVictory()
{
    if(isLoss())
    {
        //停止当前动作
        timer->stop();
        showTimer->stop();
//        playSound("Lose");
        debugText("blue"," 游戏失败，未达成目标。最终得分为:" + QString::number(player[0]->getScore()));
        //            ui->DebugTextBrowser->insertHtml(COLOR_BLUE(getShowTime() + " 游戏失败，未达成目标。最终得分为:" + QString::number(player[0]->getScore())));
        //            ui->DebugTextBrowser->insertPlainText("\n");
        //弹出胜利提示
        if(QMessageBox::information(this, QStringLiteral("游戏失败"), "很遗憾你没能成功保护部落。", QMessageBox::Ok))
        {
//            setLose();
        }
    }

    if(isWin())
    {
        //停止当前动作
        timer->stop();
        showTimer->stop();

//        playSound("Win");
        debugText("blue"," 游戏胜利");
        //                    ui->DebugTextBrowser->insertHtml(COLOR_BLUE(getShowTime() + " 游戏胜利"));
        //                    ui->DebugTextBrowser->insertPlainText("\n");
        //弹出胜利提示
        if(QMessageBox::information(this, QStringLiteral("游戏胜利"), "恭喜你取得了游戏的胜利，成功抵御了敌人的侵略！", QMessageBox::Ok))
        {
//            setWinning();
        }
    }
    else return;
}


//**************槽函数***************
// 游戏帧更新
void MainWidget::FrameUpdate()
{
    judgeVictory();

    //打印debug栏
    respond_DebugMessage();

    gameframe++;
    g_frame=gameframe;
    ui->lcdNumber->display(gameframe);

    if(mapmoveFrequency==1||mapmoveFrequency==2){
        paintUpdate();
    }
    else if(mapmoveFrequency==4){
        if(gameframe%2==0) paintUpdate();
    }
    else if(mapmoveFrequency==8){
        if(gameframe%4==0) paintUpdate();
    }
    else{
        qDebug()<<"速度设置错误";
    }
    gameDataUpdate();
    return;
}
void MainWidget::onRadioClickSlot()
{
    switch(pbuttonGroup->checkedId())
    {
    case 0:
        timer->setInterval(40);
        showTimer->setInterval(1000);
        mapmoveFrequency=1;
        break;
    case 1:
        timer->setInterval(20);
        showTimer->setInterval(500);
        mapmoveFrequency=2;
        break;
    case 2:
        timer->setInterval(10);
        showTimer->setInterval(250);
        mapmoveFrequency=4;
        break;
    case 3:
        timer->setInterval(5);
        showTimer->setInterval(125);
        mapmoveFrequency=8;
        nowobject=NULL;
        break;
    }
}

void MainWidget::cheat_Player0Resource()
{
    player[0]->changeResource(500,500,500,500);
}


//***********************************************************************
//输出提示框
void MainWidget::respond_DebugMessage()
{
    std::map<QString,int>::iterator iter = debugMessageRecord.begin(), itere = debugMessageRecord.end();

    while(!debugMassagePackage.empty())
    {
        debugText(debugMassagePackage.front().color,debugMassagePackage.front().content);
        debugMassagePackage.pop();
    }

    while(iter!= itere)
    {
        if(gameframe - iter->second > 200) iter = debugMessageRecord.erase(iter);
        else iter++;
    }
}

void MainWidget::debugText(const QString& color, const QString& content)
{
    if (color == "blue")
        ui->DebugTexter->insertHtml(COLOR_BLUE(sel->getShowTime() + content));
    else if (color == "red")
        ui->DebugTexter->insertHtml(COLOR_RED(sel->getShowTime() + content));
    else if (color == "green")
        ui->DebugTexter->insertHtml(COLOR_GREEN(sel->getShowTime() + content));
    else if(color == "black")
        ui->DebugTexter->insertHtml(COLOR_BLACK(sel->getShowTime() + content));

    ui->DebugTexter->insertPlainText("\n");
    QScrollBar *bar = ui->DebugTexter->verticalScrollBar();
    bar->setValue(bar->maximum());
}

void MainWidget::clearDebugText()
{
    ui->DebugTexter->clear();
}

//***********************************************************************
//设置初始资源
void MainWidget::buildInitialStock()
{
    int minBDR = MAP_L/2 - 9, minBUR = MAP_U/2 - 9;
    int maxBDR = MAP_L/2 + 7, maxBUR = MAP_U/2 + 7;
    int lenth = 0,step;
    Point StockPoint,judPoint;
    vector<Point> findLab;
    bool tasking = true;
    int labSize,treeNum,maxtreeNum = 0;
    int lx,ly,mx,my;

    map->loadBarrierMap();
    map->reset_Map_Object_Resource();
    map->reset_resMap_AI();
    while(tasking)
    {
        lenth = maxBDR - minBDR;
        for(int y = minBUR; y<=maxBUR; y++)
        {
            if(y==minBUR || y==maxBUR) step = 1;
            else step = lenth;

            for(int x = minBDR; x<=maxBDR;x+=lenth)
                if(!map->isBarrier(x,y,3) && map->isFlat(x,y,3)) findLab.push_back(Point(x,y));
        }

        labSize = findLab.size();

        if(labSize>0)
        {
            for(int i = 0 ; i<labSize; i++)
            {
                judPoint = findLab[i];
                lx = max(judPoint.x - 5 , 0);
                ly = max(judPoint.y - 5 , 0);
                mx = min(judPoint.x + 8, MAP_L);
                my = min(judPoint.y + 8, MAP_U);
                treeNum = 0;

                for(int x = lx ; x <mx; x++)
                    for(int y = ly; y<my; y++)
                        if(map->resMap_EnemyAI[x][y].type == RESOURCE_TREE) treeNum++;

                if(treeNum > 10)
                {
                    tasking = false;
                    if(treeNum > maxtreeNum)
                    {
                        maxtreeNum = treeNum;
                        StockPoint = judPoint;
                    }
                }
            }
        }
        findLab.clear();
        minBDR = max(minBDR-1, 0);
        minBUR = max(minBUR-1, 0);
        maxBDR = min(maxBDR+1, MAP_L);
        maxBUR = min(maxBUR+1, MAP_U);
    }

    player[0]->finishBuild(player[0]->addBuilding(BUILDING_STOCK , StockPoint.x, StockPoint.y , 100));
    return;
}
