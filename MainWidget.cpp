#include "MainWidget.h"
#include "ui_MainWidget.h"

int g_globalNum=1;
int g_frame=0;

std::map<int,Coordinate*> g_Object;
std::queue<instruction> instructions;   ///AI返回的指令队列
bool AIfinished=true;   ///AI线程锁
bool INSfinshed=true;
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

    //时间增加
    connect(showTimer, &QTimer::timeout, sel, &SelectWidget::timeUpdate);

    connect(timer, &QTimer::timeout, sel, &SelectWidget::frameUpdate);
    //    connect((const QObject*)core, SIGNAL(clickOnObject()), sel, SLOT(initActs()));
    // 游戏帧数初始化
    gameframe = 0;    

    // 玩家开辟空间
    for(int i = 0; i < MAXPLAYER; i++){player[i] = new Player(i);}

    // 新建map对象并初始化
    map = new Map;
    //为map添加player指针
    map->setPlayer(player);
    map->init(MapJudge);

    // 内存图开辟空间
    for(int i = 0;i < MEMORYROW; i++)
    {
        memorymap[i] = new int[MEMORYCOLUMN];
    }

    // 向地图中添加资源
    initmap();


    // 添加资源测试
//    player[0]->addBuilding(BUILDING_CENTER, 10, 10);
//    player[0]->addBuilding(BUILDING_CENTER, 33, 33);
    player[0]->addFarmer(25*BLOCKSIDELENGTH,25*BLOCKSIDELENGTH);

//    Building* temp = player[0]->addBuilding(BUILDING_FARM , 20,20,100);
//    qDebug()<<temp;


//    player[0]->addBuilding(BUILDING_STOCK, 40 , 40 ,100);
//    player[0]->addBuilding(BUILDING_GRANARY , 50 , 50 , 100);
//    player[0]->addBuilding(BUILDING_MARKET , 60 ,60 , 100);
//    player[0]->addArmy(AT_BOWMAN , 20*BLOCKSIDELENGTH , 40*BLOCKSIDELENGTH);

//    player[0]->addArmy(AT_SCOUT, 30*BLOCKSIDELENGTH , 40*BLOCKSIDELENGTH);
//    player[0]->addBuilding(BUILDING_FARM , 20 , 20 , 100);

//    map->addAnimal(ANIMAL_TREE , 40 , 50);
//    map->addAnimal(ANIMAL_FOREST , 50*BLOCKSIDELENGTH , 60*BLOCKSIDELENGTH);
//    map->addAnimal(ANIMAL_ELEPHANT , 20*BLOCKSIDELENGTH,20*BLOCKSIDELENGTH);

//    player[0]->addBuilding(BUILDING_CENTER, 33, 33 , 100);
//    player[0]->addBuilding(BUILDING_CENTER, MAP_L / 2 - 1, MAP_U / 2 - 1, 100);
//    player[0]->addFarmer(25*BLOCKSIDELENGTH,25*BLOCKSIDELENGTH);
//    player[0]->addBuilding(BUILDING_STOCK, 40 , 40 ,100);
//    player[0]->addBuilding(BUILDING_GRANARY , 50 , 50 , 100);
//    player[0]->addBuilding(BUILDING_MARKET , 60 ,60 , 100);
//    player[0]->addBuilding(BUILDING_FARM , 20 , 20 , 100);

//    map->addAnimal(ANIMAL_TREE , 40 , 50);
//    map->addAnimal(ANIMAL_FOREST , 50*BLOCKSIDELENGTH , 60*BLOCKSIDELENGTH);
//    map->addAnimal(ANIMAL_ELEPHANT , 20*BLOCKSIDELENGTH,20*BLOCKSIDELENGTH);

//    player[0]->addBuilding(BUILDING_ARMYCAMP , 35 , 30,100);

//    map->addStaticRes(NUM_STATICRES_Bush , 50,65);
//    map->addStaticRes(NUM_STATICRES_Stone , 40,55);
//    map->addStaticRes(NUM_STATICRES_GoldOre , 30,45);


    core = new Core(map,player,memorymap,mouseEvent);
    sel->setCore(core);

    ai=new AI();
    core->sel = sel;
    connect(timer,SIGNAL(timeout()),this,SLOT(FrameUpdate()));

    player[0]->setCiv(CIVILIZATION_TOOLAGE);
    player[0]->changeResource(2000,2000,2000,2000);
    debugText("blue"," 游戏开始");
}

// MainWidget析构函数
MainWidget::~MainWidget()
{
    ai->exit(0);
    delete ai;
    delete ui;
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

void MainWidget::initArmy()
{
    //加载素材
    //"Archer","Axeman","Clubman","Scout"

    // Stand Walk Die
    for(int statei=0;statei<4;statei++)
    {
        for(int level = 0 ; level<2;level++)
        {
            for(int i=0;i<=4;i++)
            {
                Army::allocateWalk(statei,level,i);
                Army::allocateStand(statei,level,i);
                Army::allocateDie(statei,level,i);
                Army::allocateDisappear(statei,level,i);
                Army::allocateAttack(statei,level,i);
                loadResource(Army::getArmyName(statei,level)+"_Attack_"+direction[i],Army::getAttack(statei,level,i));
                loadResource(Army::getArmyName(statei,level)+"_Work_"+direction[i],Army::getDisappear(statei,level,i));
                loadResource(Army::getArmyName(statei,level)+"_Stand_"+direction[i],Army::getStand(statei,level,i));
                loadResource(Army::getArmyName(statei,level)+"_Walk_"+direction[i],Army::getWalk(statei,level,i));
                loadResource(Army::getArmyName(statei,level)+"_Die_"+direction[i],Army::getDie(statei,level,i));
            }
            for(int i=5;i<8;i++)
            {
                Army::allocateWalk(statei,level,i);
                Army::allocateStand(statei,level,i);
                Army::allocateDie(statei,level,i);
                Army::allocateDisappear(statei,level,i);
                Army::allocateAttack(statei,level,i);
                flipResource(Army::getAttack(statei,level,8-i),Army::getAttack(statei,level,i));
                flipResource(Army::getDisappear(statei,level,8-i),Army::getDisappear(statei,level,i));
                flipResource(Army::getWalk(statei,level,8-i),Army::getWalk(statei,level,i));
                flipResource(Army::getStand(statei,level,8-i),Army::getStand(statei,level,i));
                flipResource(Army::getDie(statei,level,8-i),Army::getDie(statei,level,i));
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
    for(int statei = 0; statei < 4; statei++)
    {
        for(int level = 0 ;level<2;level++)
        {
            for(int i = 0; i < 8; i++)
            {
                Army::deallocateWalk(statei,level, i);
                Army::deallocateStand(statei,level, i);
                Army::deallocateDie(statei,level, i);
                Army::deallocateAttack(statei,level, i);
                Army::deallocateDisappear(statei,level, i);
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

void MainWidget::showPlayerResource(int playerRepresent)
{
    int wood,food,stone,gold;
    core->getPlayerNowResource(playerRepresent,wood,food,stone,gold);
    ui->resWood->setText(QString::number(wood));
    ui->resFood->setText(QString::number(food));
    ui->resStone->setText(QString::number(stone));
    ui->resGold->setText(QString::number(gold));
}

void MainWidget::statusUpdate()
{
    showPlayerResource(0);
}

// 游戏帧更新
void MainWidget::FrameUpdate()
{
    //打印debug栏
    respond_DebugMessage();

    gameframe++;
    g_frame=gameframe;

    ui->lcdNumber->display(gameframe);
    ui->Game->update();
    core->gameUpdate();
    statusUpdate();
    if(AIfinished){
        core->infoShare();
        ai->start();///AI 线程开始
    }
    emit mapmove();
    return;
}


//***********************************************************************
//输出提示框
void MainWidget::respond_DebugMessage()
{
    while(!debugMassagePackage.empty())
    {
        debugText(debugMassagePackage.front().color,debugMassagePackage.front().content);
        debugMassagePackage.pop();
    }
}

void MainWidget::debugText(const QString& color, const QString& content)
{
    if (color == "blue")
    {
        ui->DebugTexter->insertHtml(COLOR_BLUE(sel->getShowTime() + content));
    }
    else if (color == "red")
    {
        ui->DebugTexter->insertHtml(COLOR_RED(sel->getShowTime() + content));
    }
    else if (color == "green")
    {
        ui->DebugTexter->insertHtml(COLOR_GREEN(sel->getShowTime() + content));
    }
    ui->DebugTexter->insertPlainText("\n");
    QScrollBar *bar = ui->DebugTexter->verticalScrollBar();
    bar->setValue(bar->maximum());
}

void MainWidget::clearDebugText()
{
    ui->DebugTexter->clear();
}

