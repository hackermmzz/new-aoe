#include "SelectWidget.h"
#include "ui_SelectWidget.h"

SelectWidget::SelectWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SelectWidget)
{
    ui->setupUi(this);
    MainWidget* c=(MainWidget*)this->parentWidget();
//    connect()
}

SelectWidget::~SelectWidget()
{
    delete ui;
}

void SelectWidget::paintEvent(QPaintEvent *event)
{
//    if(obj != g_Object.end()->second && obj != NULL && (objType == 1 || objType == 6 || objType == 7 || !obj->isDie()))
//    {
//        QPainter painter(this);
//        painter.drawRect(0, 0, 290, 170);
//        painter.fillRect(QRect(0, 0, 290, 170), QBrush(QColor(Qt::black)));
//        if(objType == 2 || (objType != 2 && !obj->isDie())){
//            painter.drawRect(130, 90, 120, 20);
//            if(objType != 2)
//            {
//                int StartX = 130, StartY = 110;
//                int percent = obj->getBlood() * 100 / obj->getMaxBlood();
//                if(percent > 100) percent = 100;
//                painter.fillRect(QRect(117 + StartX, StartY, 3, 20), QBrush(QColor(0, 255, 0)));
//                painter.fillRect(QRect(StartX, StartY, 120, 4), QBrush(QColor(0, 242, 11)));
//                painter.fillRect(QRect(StartX, StartY + 4, 120, 12), QBrush(QColor(0, 103, 99)));
//                painter.fillRect(QRect(StartX, StartY + 16, 120, 4), QBrush(QColor(0, 143, 71)));
//                painter.fillRect(QRect(StartX, StartY + 3, 3, 3), QBrush(QColor(0, 103, 99)));
//                painter.fillRect(QRect(StartX, StartY + 14, 3, 3), QBrush(QColor(0, 103, 99)));
//                painter.fillRect(QRect(StartX + 120 * percent / 100, StartY, 120 - 120 * percent / 100, 20), QBrush(Qt::red));
//            }

//        }
//    }
//    else
//    {
//        this->hide();
//    }
}

void SelectWidget::initActs()
{
    //根据点击的对象初始化行动数组
    int currentSelectNum;
    int type = currentSelectNum / 10000 % 10;
    for(int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
    {
        actions[i] = ACT_NULL;
        actionStatus[i] = ACT_STATUS_ENABLED;//重置窗口状态为可用，真正判断是否可用的代码在refreshActs
    }
    if(g_Object.find(currentSelectNum) == g_Object.end())
    {
        return;
    }
    if(type == 1)//建筑
    {
        int *buildActions = ((Building *)g_Object[currentSelectNum])->getActions();
        for(int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
        {
            actions[i] = buildActions[i];
        }
    }
    else if(type == 5)//人类
    {
        actions[0] = ACT_BUILD;
    }
    else
    {
        for(int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
        {
            actions[i] = ACT_NULL;
            actionStatus[i] = ACT_STATUS_ENABLED;
        }
    }
    if(g_Object[currentSelectNum]->getActSpeed() > 0)
    {
        actions[0] = ACT_STOP;
        for(int i = 1; i < ACT_WINDOW_NUM_FREE; i++)
        {
            actions[0] = ACT_NULL;
        }
    }
    refreshActs();
}

void SelectWidget::refreshActs()
{
//更新行动的逻辑：点击一个对象后根据类型给行动数组赋值(initActs函数)，对象不改变时循环仅根据行动id执行对应的判断条件决定是否可以执行，不再重复刷新行动数组的具体行动(refreshActs)
//执行了doAct之后，新的行动数组通过updateActs更新
//上面更新的内容都是int数组，然后initActs和updateActs最后再调用一个函数(drawActs)根据数组的值贴上图片,refreshActs只控制图像是否为灰色

    int currentSelectNum;
    std::map<int,Coordinate*> g_Object;
    int elapsedSec;
    if(this->getObj() != NULL && g_Object.find(currentSelectNum) != g_Object.end() && this->getObj() == g_Object[currentSelectNum])
    {
        if(this->getObj()->getActSpeed() > 0) ui->objText->setText(QString::number((int)(this->getObj()->getActPercent())) + "%");//如果有进行中的任务则显示进度
        else ui->objText->setText("");
    }//行动的进度

    //计算一些参数
    std::list<Building *>::iterator buildIt = player[0]->build.begin(), buildItTemp = buildIt;
    std::list<Human *>::iterator humanIt = player[0]->human.begin(), humanItTemp = humanIt;
    bool isGranaryBuilt = false, isMarketBuilt = false, isStockBuilt = false;
    int human_num = 0;
    for(humanItTemp = player[0]->human.begin(); humanItTemp != player[0]->human.end(); humanItTemp++)
    {
        human_num++;
    }
    for(; buildIt != player[0]->build.end(); buildIt++)
    {
        if((*buildIt)->getNum() == BUILDING_GRANARY && (*buildIt)->isFinish())//建成谷仓
        {
            isGranaryBuilt = true;
        }
        if((*buildIt)->getNum() == BUILDING_MARKET && (*buildIt)->isFinish())//建成市场
        {
            isMarketBuilt = true;
        }
        if((*buildIt)->getNum() == BUILDING_STOCK && (*buildIt)->isFinish())
        {
            isStockBuilt = true;
        }
    }

    //先进行行动状态更新(根据actions决定actionStatus数组)
    for(int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
    {
        switch(actions[i])//根据actions数组的值（行动类别）判断可用性
        {
            case ACT_BUILD_ARROWTOWER:
                if(player[0]->getStone() < BUILD_ARROWTOWER_STONE || !player[0]->getArrowTowerUnlocked())
                {
                    actionStatus[i] = ACT_STATUS_DISABLED;
                }
                break;
            case ACT_BUILD_FARM:
                if(player[0]->getWood() < BUILD_FARM_WOOD || !isMarketBuilt)
                {
                    actionStatus[i] = ACT_STATUS_DISABLED;
                }
                break;
            case ACT_BUILD_GRANARY:
                if(player[0]->getWood() < BUILD_GRANARY_WOOD)
                {
                    actionStatus[i] = ACT_STATUS_DISABLED;
                }
                break;
            case ACT_BUILD_HOUSE:
                if(player[0]->getWood() < BUILD_HOUSE_WOOD)
                {
                    actionStatus[i] = ACT_STATUS_DISABLED;
                }
                break;
            case ACT_BUILD_MARKET:
                if(player[0]->getWood() < BUILD_MARKET_WOOD)
                {
                    actionStatus[i] = ACT_STATUS_DISABLED;
                }
                break;
            case ACT_BUILD_STOCK:
                if(player[0]->getWood() < BUILD_STOCK_WOOD)
                {
                    actionStatus[i] = ACT_STATUS_DISABLED;
                }
                break;
            case ACT_CREATEFARMER:
                if(player[0]->getFood() < BUILDING_CENTER_CREATEFARMER_FOOD || human_num >= player[0]->getMaxHumanNum())
                {
                    actionStatus[i] = ACT_STATUS_DISABLED;
                }
                break;
            case ACT_UPGRADE_AGE:
                if(player[0]->getFood() < BUILDING_CENTER_UPGRADE_FOOD || !isGranaryBuilt || !isStockBuilt)
                {
                    actionStatus[i] = ACT_STATUS_DISABLED;
                }
                break;
            case ACT_UPGRADE_FARM:
                if(player[0]->getWood() < BUILDING_MARKET_FARM_UPGRADE_WOOD || player[0]->getFood() < BUILDING_MARKET_FARM_UPGRADE_FOOD)
                {
                    actionStatus[i] = ACT_STATUS_DISABLED;
                }
                break;
            case ACT_UPGRADE_STONE:
                if(player[0]->getFood() < BUILDING_MARKET_STONE_UPGRADE_FOOD || player[0]->getStone() < BUILDING_MARKET_STONE_UPGRADE_STONE)
                {
                    actionStatus[i] = ACT_STATUS_DISABLED;
                }
                break;
            case ACT_UPGRADE_WOOD:
                if(player[0]->getFood() < BUILDING_MARKET_WOOD_UPGRADE_FOOD || player[0]->getWood() < BUILDING_MARKET_WOOD_UPGRADE_WOOD)
                {
                    actionStatus[i] = ACT_STATUS_DISABLED;
                }
                break;
            case ACT_NULL:
                actionStatus[i] = ACT_STATUS_DISABLED;
                break;
        }
    }
    for(int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
    {
        acts[i]->setStatus(actionStatus[i]);//应用行动状态
    }

    //再进行快捷栏和状态栏显示更新(本窗口的内容)
    if(currentSelectNum <= 0)
    {
        ui->objHp->setText("");
        ui->objIcon->setPixmap(QPixmap());
        ui->objName->setText("");
        ui->objText->setText("");
        ui->objIconSmall->setPixmap(QPixmap());
        this->hide();
        this->update();
    }
    else
    {
        int type = currentSelectNum / 10000 % 10;
        if(type == 1)//建筑
        {
            if(g_Object.find(currentSelectNum)!=g_Object.end())
            {
                Building *objBuilding = (Building *)(g_Object[currentSelectNum]);
                int num = objBuilding->getNum();
                ui->objName->setText(QString::fromStdString(objBuilding->getDisplayName(num)));
                if(player[0]->getCiv() == CIVILIZATION_STONEAGE) ui->objIcon->setPixmap(resMap["Button_"+objBuilding->Builtname[player[0]->getCiv()][num]].front().scaled(110,110));
                else if(player[0]->getCiv() == CIVILIZATION_TOOLAGE) ui->objIcon->setPixmap(resMap["Button_"+objBuilding->Builtname[player[0]->getCiv()][num]].front().scaled(110,110));

                if(objBuilding->getActSpeed() != 0)
                {
                    ui->objIconSmall->setPixmap(resMap["SmallIcon_Sandglass"].front());
                }
                else
                {
                    ui->objIconSmall->setPixmap(QPixmap());
                }
                ui->objHp->setText(QString::number(objBuilding->getBlood()) + "/" + QString::number(objBuilding->getMaxBlood()));
                //同步行动状态至窗口
                if(objBuilding->isFinish())
                {
                    for(int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
                    {
                        acts[i]->setActName(objBuilding->getActNames(i));
                        acts[i]->setStatus(objBuilding->getActStatus(i));
                        if(objBuilding->getActNames(i) != ACT_NULL)
                        {
                            if(objBuilding->getActNames(i) == ACT_STOP) acts[i]->setPix(resMap["Button_Stop"].front().scaled(80,80));
                            else if(objBuilding->getActNames(i) == ACT_CREATEFARMER) acts[i]->setPix(resMap["Button_Village"].front().scaled(80,80));
                            else if(objBuilding->getActNames(i) == ACT_UPGRADE_AGE) acts[i]->setPix(resMap["ButtonTech_Center1"].front().scaled(80,80));
                            else if(objBuilding->getActNames(i) == ACT_UPGRADE_FARM) acts[i]->setPix(resMap["ButtonTech_Cow"].front().scaled(80,80));
                            else if(objBuilding->getActNames(i) == ACT_UPGRADE_STONE) acts[i]->setPix(resMap["ButtonTech_Stone"].front().scaled(80,80));
                            else if(objBuilding->getActNames(i) == ACT_UPGRADE_WOOD) acts[i]->setPix(resMap["ButtonTech_Lumber"].front().scaled(80,80));
                            else if(objBuilding->getActNames(i) == ACT_UPGRADE_TOWERBUILD) acts[i]->setPix(resMap["ButtonTech_ArrowTower"].front().scaled(80,80));
                            acts[i]->show();
                        }
                        else acts[i]->hide();
                    }
                    if(objBuilding->getNum() == BUILDING_HOME)
                    {
                        ui->objText->setText(QString::number(human_num) + "/" + QString::number(build_hold_human_num));
                        ui->objIconSmall->setPixmap(resMap["SmallIcon_People"].front());
                    }
                    else if(objBuilding->getNum() == BUILDING_FARM)
                    {
                        if(objBuilding->getCnt() > 0)
                        {
                            ui->objText->setText(QString::number((int)(objBuilding->getCnt())));
                            ui->objIconSmall->setPixmap(resMap["Icon_Food"].front());
                        }
                        else
                        {
                            receiveselectNum(0);
                        }
                    }
                }
                else
                {
                    ui->objText->setText(QString::number((int)(objBuilding->getPercent())) + "%");
                    ui->objIconSmall->setPixmap(resMap["SmallIcon_Sandglass"].front());
                }
                this->update();
                this->show();
            }
            else
            {
                receiveselectNum(0);
            }
        }
        else if(type == 2)//块资源
        {
            if(g_Object.find(currentSelectNum)!=g_Object.end())
            {
                Coores *objCoores = (Coores *)(g_Object[currentSelectNum]);
                int num = objCoores->getNum();
                this->setObj(g_Object[currentSelectNum]);
                this->setObjType(type);
                ui->objName->setText(QString::fromStdString(objCoores->getCooresDisplayName(num)));
                ui->objHp->setText("");
                ui->objIcon->setPixmap(resMap["Button_"+objCoores->getCooresName(num)].front().scaled(80,80));
                ui->objIconSmall->setPixmap(QPixmap());
                if(objCoores->getNum() == 0) ui->objIconSmall->setPixmap(resMap["Icon_Food"].front());
                else if(objCoores->getNum() == 1) ui->objIconSmall->setPixmap(resMap["Icon_Stone"].front());
                ui->objText->setText(QString::number((int)(objCoores->getCnt())));
                this->update();
                this->show();
            }
            else
            {
                receiveselectNum(0);
            }
        }
        else if(type == 5)//村民
        {
            if(g_Object.find(currentSelectNum)!=g_Object.end())
            {
                Farmer *objFarmer = (Farmer *)(g_Object.find(currentSelectNum)->second);
                if(!objFarmer->isDie())
                {
                    int num = objFarmer->getstate();
                    this->setObj((Coordinate *)objFarmer);
                    this->setObjType(type);
                    QString name = QString::fromStdString(objFarmer->getDisplayName(num));
                    ui->objName->setText(name);
                    ui->objIcon->setPixmap(resMap["Button_Village"].front().scaled(110,110));
                    if(objFarmer->getresourceSort() == 1) ui->objIconSmall->setPixmap(resMap["Icon_Wood"].front());
                    else if(objFarmer->getresourceSort() == 2||objFarmer->getresourceSort() == 5) ui->objIconSmall->setPixmap(resMap["Icon_Food"].front());
                    else if(objFarmer->getresourceSort() == 3) ui->objIconSmall->setPixmap(resMap["Icon_Stone"].front());
                    if(objFarmer->getresource() == 0)
                    {
                        ui->objIconSmall->setPixmap(QPixmap());
                        ui->objText->setText("");
                    }
                    else
                    {
                        ui->objText->setText(QString::number((int)(objFarmer->getresource())));
                    }
                    ui->objHp->setText(QString::number(objFarmer->getBlood()) + "/" + QString::number(objFarmer->getMaxBlood()));
                    this->update();
                    this->show();
                }
                else
                {
                    receiveselectNum(0);
                }
            }
        }
        else if(type == 6)//树
        {
            if(g_Object.find(currentSelectNum)!=g_Object.end())
            {
                Animal *objAnimal = (Animal *)(g_Object[currentSelectNum]);
                if(objAnimal->getCnt() <= 0) receiveselectNum(0);
                else
                {
                    this->setObj((Coordinate *)objAnimal);
                    this->setObjType(type);
                    ui->objName->setText("树");
                    ui->objIcon->setPixmap(resMap["Button_Tree"].front().scaled(110,110));
                    ui->objIconSmall->setPixmap(resMap["SmallIcon_Wood"].front());
                    ui->objText->setText(QString::number((int)(objAnimal->getCnt())));
                    if(objAnimal->isDie()) ui->objHp->setText("");
                    else ui->objHp->setText(QString::number(objAnimal->getBlood()) + "/" + QString::number(objAnimal->getMaxBlood()));
                    this->update();
                    this->show();
                }
            }
            else
            {
                receiveselectNum(0);
            }
        }
        else if(type == 7)//动物
        {
            if(g_Object.find(currentSelectNum)!=g_Object.end())
            {
                Animal *objAnimal = (Animal *)(g_Object[currentSelectNum]);
                if(objAnimal->getCnt() <= 0) receiveselectNum(0);
                else
                {
                    this->setObj((Coordinate *)objAnimal);
                    this->setObjType(type);
                    if(objAnimal->getNum() == 1)//瞪羚
                    {
                        ui->objName->setText("瞪羚");
                        ui->objIcon->setPixmap(resMap["Button_Gazelle"].front().scaled(110,110));

                    }
                    else if(objAnimal->getNum() == 2)//大象
                    {
                        ui->objName->setText("大象");
                        ui->objIcon->setPixmap(resMap["Button_Elephant"].front().scaled(110,110));
                    }
                    else if(objAnimal->getNum() == 3)
                    {
                        ui->objName->setText("狮子");
                        ui->objIcon->setPixmap(resMap["Button_Lion"].front().scaled(110,110));
                    }
                    ui->objIconSmall->setPixmap(resMap["Icon_Food"].front());
                    ui->objText->setText(QString::number((int)(objAnimal->getCnt())));
                    if(objAnimal->isDie()) ui->objHp->setText("");
                    else ui->objHp->setText(QString::number(objAnimal->getBlood()) + "/" + QString::number(objAnimal->getMaxBlood()));
                    this->update();
                    this->show();
                }
            }
            else
            {
                receiveselectNum(0);
            }
        }
    }
    for(int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
    {
        acts[i]->update();
    }
}

void SelectWidget::widgetAct(int num)
{
    if(acts[num]->getStatus() == ACT_STATUS_DISABLED) return;
    int actName = acts[num]->getActName();
    doActs(actName);
}

void SelectWidget::aiAct(int acter, int actName)
{
    doActs(actName);
}

int SelectWidget::doActs(int actName)
{
    if(this->getObj() == NULL) return ACTION_INVALID_ACTION;
    if(actName == ACT_BUILD){
        acts[0]->setPix(resMap["Button_House1"].front().scaled(80,80));
        acts[0]->show();
        acts[0]->setActName(ACT_BUILD_HOUSE);
        acts[1]->setPix(resMap["Button_Granary"].front().scaled(80,80));
        acts[1]->show();
        acts[1]->setActName(ACT_BUILD_GRANARY);
        acts[2]->setPix(resMap["Button_Stock"].front().scaled(80,80));
        acts[2]->show();
        acts[2]->setActName(ACT_BUILD_STOCK);
        acts[3]->setPix(resMap["Exit"].front().scaled(80,80));
        acts[3]->show();
        acts[3]->setActName(ACT_BUILD_CANCEL);
        if(player[0]->getWood() < BUILD_HOUSE_WOOD)
        {
            acts[0]->setStatus(ACT_STATUS_DISABLED);
        }
        if(player[0]->getWood() < BUILD_GRANARY_WOOD)
        {
            acts[1]->setStatus(ACT_STATUS_DISABLED);
            acts[2]->setStatus(ACT_STATUS_DISABLED);
        }
        if(player[0]->getCiv() == CIVILIZATION_TOOLAGE)
        {
            acts[3]->setPix(resMap["Button_Farm"].front().scaled(80,80));
            acts[3]->show();
            acts[3]->setActName(ACT_BUILD_FARM);
            acts[4]->setPix(resMap["Button_Market"].front().scaled(80,80));
            acts[4]->show();
            acts[4]->setActName(ACT_BUILD_MARKET);
            acts[5]->setPix(resMap["Button_ArrowTower"].front().scaled(80,80));
            acts[5]->show();
            acts[5]->setActName(ACT_BUILD_ARROWTOWER);
            acts[6]->setPix(resMap["Exit"].front().scaled(80,80));
            acts[6]->show();
            acts[6]->setActName(ACT_BUILD_CANCEL);
            std::list<Building *>::iterator buildIt = player[0]->build.begin();
            bool isGranaryBuilt = false, isMarketBuilt = false;
            for(; buildIt != player[0]->build.end(); buildIt++)
            {
                if((*buildIt)->getNum() == BUILDING_GRANARY && (*buildIt)->isFinish())//建成谷仓
                {
                    isGranaryBuilt = true;
                }
                if((*buildIt)->getNum() == BUILDING_MARKET && (*buildIt)->isFinish())//建成市场
                {
                    isMarketBuilt = true;
                }
            }
            if(!isGranaryBuilt || player[0]->getWood() < BUILD_MARKET_WOOD)
            {
                acts[4]->setStatus(ACT_STATUS_DISABLED);
            }
            if(!isMarketBuilt || player[0]->getWood() < BUILD_FARM_WOOD)
            {
                acts[3]->setStatus(ACT_STATUS_DISABLED);
            }
            if(!player[0]->getArrowTowerUnlocked() || player[0]->getStone() < BUILD_ARROWTOWER_STONE)
            {
                acts[5]->setStatus(ACT_STATUS_DISABLED);
            }
        }

    }
    else if(actName == ACT_BUILD_HOUSE){
        ui->Game->buildMode = BUILDING_HOME + 1;
        QCursor my(resMap["House1"].front());
        QApplication::setOverrideCursor(my);
    }
    else if(actName == ACT_BUILD_GRANARY){
        ui->Game->buildMode = BUILDING_GRANARY + 1;
        QCursor my(resMap["Granary"].front());
        QApplication::setOverrideCursor(my);
    }
    else if(actName == ACT_BUILD_STOCK){
        QCursor my(resMap["Stock"].front());
        QApplication::setOverrideCursor(my);
        ui->Game->buildMode = BUILDING_STOCK + 1;
    }
    else if(actName == ACT_BUILD_FARM){
        QCursor my(resMap["Farm"].front());
        QApplication::setOverrideCursor(my);
        ui->Game->buildMode = BUILDING_FARM + 1;
    }
    else if(actName == ACT_BUILD_MARKET){
        QCursor my(resMap["Market"].front());
        QApplication::setOverrideCursor(my);
        ui->Game->buildMode = BUILDING_MARKET + 1;
    }
    else if(actName == ACT_BUILD_ARROWTOWER){
        QCursor my(resMap["ArrowTower"].front());
        QApplication::setOverrideCursor(my);
        ui->Game->buildMode = BUILDING_ARROWTOWER + 1;
    }
    else if(actName == ACT_BUILD_CANCEL)
    {
        QApplication::restoreOverrideCursor();
        ui->Game->buildMode = 0;
    }
    else if(actName == ACT_CREATEFARMER){
        if(player[0]->getFood() < BUILDING_CENTER_CREATEFARMER_FOOD) return ACTION_INVALID_RESOURCE;
        player[0]->setFood(player[0]->getFood() - BUILDING_CENTER_CREATEFARMER_FOOD);
        this->getObj()->setActPercent(0);
        this->getObj()->setActSpeed(0.25);
        this->getObj()->setActNum(BUILDING_CENTER_CREATEFARMER);
        this->getObj()->setActName(ACT_CREATEFARMER);
    }
    else if(actName == ACT_UPGRADE_AGE)
    {
        std::list<Building *>::iterator buildIt = player[0]->build.begin();
        bool isGranaryBuilt = false, isStockBuilt = false;
        for(; buildIt != player[0]->build.end(); buildIt++)
        {
            if((*buildIt)->getNum() == BUILDING_GRANARY && (*buildIt)->isFinish()) isGranaryBuilt = true;
            else if((*buildIt)->getNum() == BUILDING_STOCK && (*buildIt)->isFinish()) isStockBuilt = true;
        }
        if(!isGranaryBuilt || !isStockBuilt) return ACTION_INVALID_ACTION;

        if(player[0]->getFood() < BUILDING_CENTER_UPGRADE_FOOD) return ACTION_INVALID_RESOURCE;
        player[0]->setFood(player[0]->getFood() - BUILDING_CENTER_UPGRADE_FOOD);
        this->getObj()->setActPercent(0);
        this->getObj()->setActSpeed(0.05);
        this->getObj()->setActNum(BUILDING_CENTER_UPGRADE);
        this->getObj()->setActName(ACT_UPGRADE_AGE);
    }
    else if(actName == ACT_UPGRADE_TOWERBUILD)
    {
        if(player[0]->getFood() < BUILDING_GRANARY_ARROWTOWER_FOOD) return ACTION_INVALID_RESOURCE;
        player[0]->setFood(player[0]->getFood() - BUILDING_GRANARY_ARROWTOWER_FOOD);
        this->getObj()->setActPercent(0);
        this->getObj()->setActSpeed(0.25);
        this->getObj()->setActNum(BUILDING_GRANARY_ARROWTOWER);
        this->getObj()->setActName(ACT_UPGRADE_TOWERBUILD);
    }
    else if(actName == ACT_UPGRADE_WOOD)
    {
        if(player[0]->getFood() < BUILDING_MARKET_WOOD_UPGRADE_FOOD || player[0]->getWood() < BUILDING_MARKET_WOOD_UPGRADE_WOOD) return ACTION_INVALID_RESOURCE;
        player[0]->setFood(player[0]->getFood() - BUILDING_MARKET_WOOD_UPGRADE_FOOD);
        player[0]->setWood(player[0]->getWood() - BUILDING_MARKET_WOOD_UPGRADE_WOOD);
        this->getObj()->setActPercent(0);
        this->getObj()->setActSpeed(0.25);
        this->getObj()->setActNum(BUILDING_MARKET_WOOD_UPGRADE);
        this->getObj()->setActName(ACT_UPGRADE_WOOD);
    }
    else if(actName == ACT_UPGRADE_STONE)
    {
        if(player[0]->getFood() < BUILDING_MARKET_STONE_UPGRADE_FOOD || player[0]->getStone() < BUILDING_MARKET_STONE_UPGRADE_STONE) return ACTION_INVALID_RESOURCE;
        player[0]->setFood(player[0]->getFood() - BUILDING_MARKET_STONE_UPGRADE_FOOD);
        player[0]->setStone(player[0]->getStone() - BUILDING_MARKET_STONE_UPGRADE_STONE);
        this->getObj()->setActPercent(0);
        this->getObj()->setActSpeed(0.25);
        this->getObj()->setActNum(BUILDING_MARKET_STONE_UPGRADE);
        this->getObj()->setActName(ACT_UPGRADE_STONE);
    }
    else if(actName == ACT_UPGRADE_FARM)
    {
        if(player[0]->getFood() < BUILDING_MARKET_FARM_UPGRADE_FOOD || player[0]->getWood() < BUILDING_MARKET_FARM_UPGRADE_WOOD) return ACTION_INVALID_RESOURCE;
        player[0]->setFood(player[0]->getFood() - BUILDING_MARKET_FARM_UPGRADE_FOOD);
        player[0]->setWood(player[0]->getWood() - BUILDING_MARKET_FARM_UPGRADE_WOOD);
        this->getObj()->setActPercent(0);
        this->getObj()->setActSpeed(0.25);
        this->getObj()->setActNum(BUILDING_MARKET_FARM_UPGRADE);
        this->getObj()->setActName(ACT_UPGRADE_FARM);
    }
    else if(actName == ACT_STOP)
    {
        this->getObj()->setActNum(0);
        if(this->getObj()->getActName() == ACT_CREATEFARMER) player[0]->setFood(player[0]->getFood() + BUILDING_CENTER_CREATEFARMER_FOOD);
        else if(this->getObj()->getActName() == ACT_UPGRADE_AGE) player[0]->setFood(player[0]->getFood() + BUILDING_CENTER_UPGRADE_FOOD);
        else if(this->getObj()->getActName() == ACT_UPGRADE_TOWERBUILD) player[0]->setFood(player[0]->getFood() + BUILDING_GRANARY_ARROWTOWER_FOOD);
        else if(this->getObj()->getActName() == ACT_UPGRADE_WOOD)
        {
            player[0]->setFood(player[0]->getFood() + BUILDING_MARKET_WOOD_UPGRADE_FOOD);
            player[0]->setWood(player[0]->getWood() + BUILDING_MARKET_WOOD_UPGRADE_WOOD);
        }
        else if(this->getObj()->getActName() == ACT_UPGRADE_STONE)
        {
            player[0]->setFood(player[0]->getFood() + BUILDING_MARKET_STONE_UPGRADE_FOOD);
            player[0]->setStone(player[0]->getStone() + BUILDING_MARKET_STONE_UPGRADE_STONE);
        }
        else if(this->getObj()->getActName() == ACT_UPGRADE_FARM)
        {
            player[0]->setFood(player[0]->getFood() + BUILDING_MARKET_FARM_UPGRADE_FOOD);
            player[0]->setWood(player[0]->getWood() + BUILDING_MARKET_FARM_UPGRADE_WOOD);
        }
        if(this->getObj()->getActName() == ACT_CREATEFARMER || this->getObj()->getActName() == ACT_UPGRADE_AGE)
        {
            ui->objText->setText("");
            this->getObj()->setActSpeed(0);
            this->getObj()->setActPercent(0);
            this->getObj()->setActName(ACT_NULL);
        }
        else if(this->getObj()->getActName() == ACT_UPGRADE_TOWERBUILD)
        {
            ui->objText->setText("");
            this->getObj()->setActSpeed(0);
            this->getObj()->setActPercent(0);
            this->getObj()->setActName(ACT_NULL);
        }
        else if(this->getObj()->getActName() == ACT_UPGRADE_WOOD || this->getObj()->getActName() == ACT_UPGRADE_STONE || this->getObj()->getActName() == ACT_UPGRADE_FARM)
        {
            ui->objText->setText("");
            this->getObj()->setActSpeed(0);
            this->getObj()->setActPercent(0);
            this->getObj()->setActName(ACT_NULL);
        }
        else
        {
            return ACTION_INVALID_ACTION;
        }
    }
    for(int i = 0; i < 10; i++)
    {
        acts[i]->update();
    }
    return ACTION_SUCCESS;
}

void SelectWidget::updateActs()
{

}

void SelectWidget::drawActs()
{

}
