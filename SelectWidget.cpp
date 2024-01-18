#include "SelectWidget.h"
#include "ui_SelectWidget.h"

SelectWidget::SelectWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SelectWidget)
{
    ui->setupUi(this);
    MainWidget* c=(MainWidget*)this->parentWidget();
//    connect()
    mainPtr = c;
}

SelectWidget::~SelectWidget()
{
    delete ui;
}

void SelectWidget::paintEvent(QPaintEvent *event)
{
    if(obj != NULL && (objType == 1 || objType == 6 || objType == 7))
    {
        QPainter painter(this);
        painter.drawRect(0, 0, 290, 170);
        painter.fillRect(QRect(0, 0, 290, 170), QBrush(QColor(Qt::black)));
        if(objType == 2){
            painter.drawRect(130, 90, 120, 20);
            if(objType != 2)
            {
                int StartX = 130, StartY = 110;
                int percent;
//                if(objType == 5) percent = obj->getBlood() * 100 / obj->getMaxBlood();
                if(percent > 100) percent = 100;
                painter.fillRect(QRect(117 + StartX, StartY, 3, 20), QBrush(QColor(0, 255, 0)));
                painter.fillRect(QRect(StartX, StartY, 120, 4), QBrush(QColor(0, 242, 11)));
                painter.fillRect(QRect(StartX, StartY + 4, 120, 12), QBrush(QColor(0, 103, 99)));
                painter.fillRect(QRect(StartX, StartY + 16, 120, 4), QBrush(QColor(0, 143, 71)));
                painter.fillRect(QRect(StartX, StartY + 3, 3, 3), QBrush(QColor(0, 103, 99)));
                painter.fillRect(QRect(StartX, StartY + 14, 3, 3), QBrush(QColor(0, 103, 99)));
                painter.fillRect(QRect(StartX + 120 * percent / 100, StartY, 120 - 120 * percent / 100, 20), QBrush(Qt::red));
            }

        }
    }
    else
    {
        this->hide();
    }
}

void SelectWidget::initActs()
{
    //根据点击的对象初始化行动数组
    int type = nowobject->getSort();
    for(int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
    {
        actions[i] = ACT_NULL;
        actionStatus[i] = ACT_STATUS_ENABLED;//重置窗口状态为可用，真正判断是否可用的代码在refreshmainPtr->acts
    }

    if(type == 1)//建筑
    {
        int *buildActions = ((Building *)nowobject)->getActions();
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
//    if(nowobject->getmainPtr->actspeed() > 0)
//    {
//        actions[0] = ACT_STOP;
//        for(int i = 1; i < ACT_WINDOW_NUM_FREE; i++)
//        {
//            actions[0] = ACT_NULL;
//        }
//    }
    refreshActs();
}

void SelectWidget::refreshActs()
{
//更新行动的逻辑：点击一个对象后根据类型给行动数组赋值(initmainPtr->acts函数)，对象不改变时循环仅根据行动id执行对应的判断条件决定是否可以执行，不再重复刷新行动数组的具体行动(refreshmainPtr->acts)
//执行了doAct之后，新的行动数组通过updatemainPtr->acts更新
//上面更新的内容都是int数组，然后initmainPtr->acts和updatemainPtr->acts最后再调用一个函数(drawmainPtr->acts)根据数组的值贴上图片,refreshmainPtr->acts只控制图像是否为灰色

    int currentSelectNum;
    std::map<int,Coordinate*> g_Object;
    int elapsedSec;
    if(this->getObj() != NULL && this->getObj() == nowobject)
    {
//        if(this->getObj()->getmainPtr->actspeed() > 0) ui->objText->setText(QString::number((int)(this->getObj()->getActPercent())) + "%");//如果有进行中的任务则显示进度
//        else ui->objText->setText("");
    }//行动的进度

    //计算一些参数
    std::list<Building *>::iterator buildIt = mainPtr->player[0]->build.begin(), buildItTemp = buildIt;
    std::list<Human *>::iterator humanIt = mainPtr->player[0]->human.begin(), humanItTemp = humanIt;
    bool isGranaryBuilt = false, isMarketBuilt = false, isStockBuilt = false;
    int human_num = 0;
    for(humanItTemp = mainPtr->player[0]->human.begin(); humanItTemp != mainPtr->player[0]->human.end(); humanItTemp++)
    {
        human_num++;
    }
    for(; buildIt != mainPtr->player[0]->build.end(); buildIt++)
    {
//        if((*buildIt)->getNum() == BUILDING_GRANARY && (*buildIt)->isFinish())//建成谷仓
//        {
//            isGranaryBuilt = true;
//        }
//        if((*buildIt)->getNum() == BUILDING_MARKET && (*buildIt)->isFinish())//建成市场
//        {
//            isMarketBuilt = true;
//        }
//        if((*buildIt)->getNum() == BUILDING_STOCK && (*buildIt)->isFinish())
//        {
//            isStockBuilt = true;
//        }
    }

    //先进行行动状态更新(根据actions决定actionStatus数组)
    for(int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
    {
        switch(actions[i])//根据actions数组的值（行动类别）判断可用性
        {
            case ACT_BUILD_ARROWTOWER:
                if(mainPtr->player[0]->getStone() < BUILD_ARROWTOWER_STONE || !mainPtr->player[0]->getArrowTowerUnlocked())
                {
                    actionStatus[i] = ACT_STATUS_DISABLED;
                }
                break;
            case ACT_BUILD_FARM:
                if(mainPtr->player[0]->getWood() < BUILD_FARM_WOOD || !isMarketBuilt)
                {
                    actionStatus[i] = ACT_STATUS_DISABLED;
                }
                break;
            case ACT_BUILD_GRANARY:
                if(mainPtr->player[0]->getWood() < BUILD_GRANARY_WOOD)
                {
                    actionStatus[i] = ACT_STATUS_DISABLED;
                }
                break;
            case ACT_BUILD_HOUSE:
                if(mainPtr->player[0]->getWood() < BUILD_HOUSE_WOOD)
                {
                    actionStatus[i] = ACT_STATUS_DISABLED;
                }
                break;
            case ACT_BUILD_MARKET:
                if(mainPtr->player[0]->getWood() < BUILD_MARKET_WOOD)
                {
                    actionStatus[i] = ACT_STATUS_DISABLED;
                }
                break;
            case ACT_BUILD_STOCK:
                if(mainPtr->player[0]->getWood() < BUILD_STOCK_WOOD)
                {
                    actionStatus[i] = ACT_STATUS_DISABLED;
                }
                break;
            case ACT_CREATEFARMER:
                if(mainPtr->player[0]->getFood() < BUILDING_CENTER_CREATEFARMER_FOOD || human_num >= mainPtr->player[0]->getMaxHumanNum())
                {
                    actionStatus[i] = ACT_STATUS_DISABLED;
                }
                break;
            case ACT_UPGRADE_AGE:
                if(mainPtr->player[0]->getFood() < BUILDING_CENTER_UPGRADE_FOOD || !isGranaryBuilt || !isStockBuilt)
                {
                    actionStatus[i] = ACT_STATUS_DISABLED;
                }
                break;
            case ACT_UPGRADE_FARM:
                if(mainPtr->player[0]->getWood() < BUILDING_MARKET_FARM_UPGRADE_WOOD || mainPtr->player[0]->getFood() < BUILDING_MARKET_FARM_UPGRADE_FOOD)
                {
                    actionStatus[i] = ACT_STATUS_DISABLED;
                }
                break;
            case ACT_UPGRADE_STONE:
                if(mainPtr->player[0]->getFood() < BUILDING_MARKET_STONE_UPGRADE_FOOD || mainPtr->player[0]->getStone() < BUILDING_MARKET_STONE_UPGRADE_STONE)
                {
                    actionStatus[i] = ACT_STATUS_DISABLED;
                }
                break;
            case ACT_UPGRADE_WOOD:
                if(mainPtr->player[0]->getFood() < BUILDING_MARKET_WOOD_UPGRADE_FOOD || mainPtr->player[0]->getWood() < BUILDING_MARKET_WOOD_UPGRADE_WOOD)
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
        mainPtr->getActs(i)->setStatus(actionStatus[i]);//应用行动状态


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
            Building *objBuilding = (Building *)(nowobject);
            int num = objBuilding->getNum();
//                ui->objName->setText(QString::fromStdString(objBuilding->getDisplayName(num)));
//            if(mainPtr->player[0]->getCiv() == CIVILIZATION_STONEAGE) ui->objIcon->setPixmap(resMap["Button_"+objBuilding->Builtname[mainPtr->player[0]->getCiv()][num]].front().scaled(110,110));
//            else if(mainPtr->player[0]->getCiv() == CIVILIZATION_TOOLAGE) ui->objIcon->setPixmap(resMap["Button_"+objBuilding->Builtname[mainPtr->player[0]->getCiv()][num]].front().scaled(110,110));

//                if(objBuilding->getActspeed() != 0)
//                {
//                    ui->objIconSmall->setPixmap(resMap["SmallIcon_Sandglass"].front());
//                }
//                else
//                {
//                    ui->objIconSmall->setPixmap(QPixmap());
//                }
//                ui->objHp->setText(QString::number(objBuilding->getBlood()) + "/" + QString::number(objBuilding->getMaxBlood()));
            //同步行动状态至窗口
//                if(objBuilding->isFinish())
//                {
//                    for(int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
//                    {
//                        mainPtr->getActs(i)->setActName(objBuilding->getActNames(i));
//                        mainPtr->getActs(i)->setStatus(objBuilding->getActstatus(i));
//                        if(objBuilding->getActNames(i) != ACT_NULL)
//                        {
//                            if(objBuilding->getActNames(i) == ACT_STOP) mainPtr->getActs(i)->setPix(resMap["Button_Stop"].front().scaled(80,80));
//                            else if(objBuilding->getActNames(i) == ACT_CREATEFARMER) mainPtr->getActs(i)->setPix(resMap["Button_Village"].front().scaled(80,80));
//                            else if(objBuilding->getActNames(i) == ACT_UPGRADE_AGE) mainPtr->getActs(i)->setPix(resMap["ButtonTech_Center1"].front().scaled(80,80));
//                            else if(objBuilding->getActNames(i) == ACT_UPGRADE_FARM) mainPtr->getActs(i)->setPix(resMap["ButtonTech_Cow"].front().scaled(80,80));
//                            else if(objBuilding->getActNames(i) == ACT_UPGRADE_STONE) mainPtr->getActs(i)->setPix(resMap["ButtonTech_Stone"].front().scaled(80,80));
//                            else if(objBuilding->getActNames(i) == ACT_UPGRADE_WOOD) mainPtr->getActs(i)->setPix(resMap["ButtonTech_Lumber"].front().scaled(80,80));
//                            else if(objBuilding->getActNames(i) == ACT_UPGRADE_TOWERBUILD) mainPtr->getActs(i)->setPix(resMap["ButtonTech_ArrowTower"].front().scaled(80,80));
//                            mainPtr->getActs(i)->show();
//                        }
//                        else mainPtr->getActs(i)->hide();
//                    }
//                    if(objBuilding->getNum() == BUILDING_HOME)
//                    {
//                        ui->objText->setText(QString::number(human_num) + "/" + QString::number(build_hold_human_num));
//                        ui->objIconSmall->setPixmap(resMap["SmallIcon_People"].front());
//                    }
//                    else if(objBuilding->getNum() == BUILDING_FARM)
//                    {
//                        if(objBuilding->getCnt() > 0)
//                        {
//                            ui->objText->setText(QString::number((int)(objBuilding->getCnt())));
//                            ui->objIconSmall->setPixmap(resMap["Icon_Food"].front());
//                        }
//                        else
//                        {
////                            receiveselectNum(0);
//                        }
//                    }
//                }
//                else
//                {
//                    ui->objText->setText(QString::number((int)(objBuilding->getPercent())) + "%");
//                    ui->objIconSmall->setPixmap(resMap["SmallIcon_Sandglass"].front());
//                }
            this->update();
            this->show();
        }
        else if(type == 2)//块资源
        {
            StaticRes *objStaticRes = (StaticRes *)(nowobject);
//                int num = objStaticRes->getNum();
            this->setObj(nowobject);
            this->setObjType(type);
//                ui->objName->setText(QString::fromStdString(objStaticRes->getStaticResDisplayName(num)));
            ui->objHp->setText("");
//            ui->objIcon->setPixmap(resMap["Button_"+objStaticRes->getStaticResName(num)].front().scaled(80,80));
            ui->objIconSmall->setPixmap(QPixmap());
            if(objStaticRes->getNum() == 0) ui->objIconSmall->setPixmap(resMap["Icon_Food"].front());
            else if(objStaticRes->getNum() == 1) ui->objIconSmall->setPixmap(resMap["Icon_Stone"].front());
//            ui->objText->setText(QString::number((int)(objStaticRes->getCnt())));
            this->update();
            this->show();
        }
        else if(type == 5)//村民
        {
            Farmer *objFarmer = (Farmer *)(nowobject);
//            int num = objFarmer->getstate();
            this->setObj((Coordinate *)objFarmer);
            this->setObjType(type);
//            QString name = QString::fromStdString(objFarmer->getDisplayName(num));
//            ui->objName->setText(name);
            ui->objIcon->setPixmap(resMap["Button_Village"].front().scaled(110,110));
//            if(objFarmer->getresourceSort() == 1) ui->objIconSmall->setPixmap(resMap["Icon_Wood"].front());
//            else if(objFarmer->getresourceSort() == 2||objFarmer->getresourceSort() == 5) ui->objIconSmall->setPixmap(resMap["Icon_Food"].front());
//            else if(objFarmer->getresourceSort() == 3) ui->objIconSmall->setPixmap(resMap["Icon_Stone"].front());
//            if(objFarmer->getresource() == 0)
//            {
//                ui->objIconSmall->setPixmap(QPixmap());
//                ui->objText->setText("");
//            }
//            else
//            {
//                ui->objText->setText(QString::number((int)(objFarmer->getresource())));
//            }
//            ui->objHp->setText(QString::number(objFarmer->getBlood()) + "/" + QString::number(objFarmer->getMaxBlood()));
            this->update();
            this->show();

        }
        else if(type == 6)//树
        {
            Animal *objAnimal = (Animal *)(nowobject);
//            if(objAnimal->getCnt() <= 0) receiveselectNum(0);
//            else
//            {
//                this->setObj((Coordinate *)objAnimal);
//                this->setObjType(type);
//                ui->objName->setText("树");
//                ui->objIcon->setPixmap(resMap["Button_Tree"].front().scaled(110,110));
//                ui->objIconSmall->setPixmap(resMap["SmallIcon_Wood"].front());
//                ui->objText->setText(QString::number((int)(objAnimal->getCnt())));
//                if(objAnimal->isDie()) ui->objHp->setText("");
//                else ui->objHp->setText(QString::number(objAnimal->getBlood()) + "/" + QString::number(objAnimal->getMaxBlood()));
//                this->update();
//                this->show();
//            }

        }
        else if(type == 7)//动物
        {

            Animal *objAnimal = (Animal *)(nowobject);
//            if(objAnimal->getCnt() <= 0) receiveselectNum(0);
//            else
//            {
//                this->setObj((Coordinate *)objAnimal);
//                this->setObjType(type);
//                if(objAnimal->getNum() == 1)//瞪羚
//                {
//                    ui->objName->setText("瞪羚");
//                    ui->objIcon->setPixmap(resMap["Button_Gazelle"].front().scaled(110,110));

//                }
//                else if(objAnimal->getNum() == 2)//大象
//                {
//                    ui->objName->setText("大象");
//                    ui->objIcon->setPixmap(resMap["Button_Elephant"].front().scaled(110,110));
//                }
//                else if(objAnimal->getNum() == 3)
//                {
//                    ui->objName->setText("狮子");
//                    ui->objIcon->setPixmap(resMap["Button_Lion"].front().scaled(110,110));
//                }
//                ui->objIconSmall->setPixmap(resMap["Icon_Food"].front());
////                ui->objText->setText(QString::number((int)(objAnimal->getCnt())));
////                if(objAnimal->isDie()) ui->objHp->setText("");
////                else ui->objHp->setText(QString::number(objAnimal->getBlood()) + "/" + QString::number(objAnimal->getMaxBlood()));
//                this->update();
//                this->show();

//            }
        }
    }
    for(int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
    {
        mainPtr->getActs(i)->update();
    }
}

void SelectWidget::widgetAct(int num)
{
    if(mainPtr->getActs(num)->getStatus() == ACT_STATUS_DISABLED) return;
    int actName = mainPtr->getActs(num)->getActName();
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
        mainPtr->getActs(0)->setPix(resMap["Button_House1"].front().scaled(80,80));
        mainPtr->getActs(0)->show();
        mainPtr->getActs(0)->setActName(ACT_BUILD_HOUSE);
        mainPtr->getActs(1)->setPix(resMap["Button_Granary"].front().scaled(80,80));
        mainPtr->getActs(1)->show();
        mainPtr->getActs(1)->setActName(ACT_BUILD_GRANARY);
        mainPtr->getActs(2)->setPix(resMap["Button_Stock"].front().scaled(80,80));
        mainPtr->getActs(2)->show();
        mainPtr->getActs(2)->setActName(ACT_BUILD_STOCK);
        mainPtr->getActs(3)->setPix(resMap["Exit"].front().scaled(80,80));
        mainPtr->getActs(3)->show();
        mainPtr->getActs(3)->setActName(ACT_BUILD_CANCEL);
        if(mainPtr->player[0]->getWood() < BUILD_HOUSE_WOOD)
        {
            mainPtr->getActs(0)->setStatus(ACT_STATUS_DISABLED);
        }
        if(mainPtr->player[0]->getWood() < BUILD_GRANARY_WOOD)
        {
            mainPtr->getActs(1)->setStatus(ACT_STATUS_DISABLED);
            mainPtr->getActs(2)->setStatus(ACT_STATUS_DISABLED);
        }
        if(mainPtr->player[0]->getCiv() == CIVILIZATION_TOOLAGE)
        {
            mainPtr->getActs(3)->setPix(resMap["Button_Farm"].front().scaled(80,80));
            mainPtr->getActs(3)->show();
            mainPtr->getActs(3)->setActName(ACT_BUILD_FARM);
            mainPtr->getActs(4)->setPix(resMap["Button_Market"].front().scaled(80,80));
            mainPtr->getActs(4)->show();
            mainPtr->getActs(4)->setActName(ACT_BUILD_MARKET);
            mainPtr->getActs(5)->setPix(resMap["Button_ArrowTower"].front().scaled(80,80));
            mainPtr->getActs(5)->show();
            mainPtr->getActs(5)->setActName(ACT_BUILD_ARROWTOWER);
            mainPtr->getActs(6)->setPix(resMap["Exit"].front().scaled(80,80));
            mainPtr->getActs(6)->show();
            mainPtr->getActs(6)->setActName(ACT_BUILD_CANCEL);
            std::list<Building *>::iterator buildIt = mainPtr->player[0]->build.begin();
            bool isGranaryBuilt = false, isMarketBuilt = false;
            for(; buildIt != mainPtr->player[0]->build.end(); buildIt++)
            {
//                if((*buildIt)->getNum() == BUILDING_GRANARY && (*buildIt)->isFinish())//建成谷仓
//                {
//                    isGranaryBuilt = true;
//                }
//                if((*buildIt)->getNum() == BUILDING_MARKET && (*buildIt)->isFinish())//建成市场
//                {
//                    isMarketBuilt = true;
//                }
            }
            if(!isGranaryBuilt || mainPtr->player[0]->getWood() < BUILD_MARKET_WOOD)
            {
                mainPtr->getActs(4)->setStatus(ACT_STATUS_DISABLED);
            }
            if(!isMarketBuilt || mainPtr->player[0]->getWood() < BUILD_FARM_WOOD)
            {
                mainPtr->getActs(3)->setStatus(ACT_STATUS_DISABLED);
            }
            if(!mainPtr->player[0]->getArrowTowerUnlocked() || mainPtr->player[0]->getStone() < BUILD_ARROWTOWER_STONE)
            {
                mainPtr->getActs(5)->setStatus(ACT_STATUS_DISABLED);
            }
        }

    }
    else if(actName == ACT_BUILD_HOUSE){
//        ui->Game->buildMode = BUILDING_HOME + 1;
        QCursor my(resMap["House1"].front());
        QApplication::setOverrideCursor(my);
    }
    else if(actName == ACT_BUILD_GRANARY){
//        ui->Game->buildMode = BUILDING_GRANARY + 1;
        QCursor my(resMap["Granary"].front());
        QApplication::setOverrideCursor(my);
    }
    else if(actName == ACT_BUILD_STOCK){
        QCursor my(resMap["Stock"].front());
        QApplication::setOverrideCursor(my);
//        ui->Game->buildMode = BUILDING_STOCK + 1;
    }
    else if(actName == ACT_BUILD_FARM){
        QCursor my(resMap["Farm"].front());
        QApplication::setOverrideCursor(my);
//        ui->Game->buildMode = BUILDING_FARM + 1;
    }
    else if(actName == ACT_BUILD_MARKET){
        QCursor my(resMap["Market"].front());
        QApplication::setOverrideCursor(my);
//        ui->Game->buildMode = BUILDING_MARKET + 1;
    }
    else if(actName == ACT_BUILD_ARROWTOWER){
        QCursor my(resMap["ArrowTower"].front());
        QApplication::setOverrideCursor(my);
//        ui->Game->buildMode = BUILDING_ARROWTOWER + 1;
    }
    else if(actName == ACT_BUILD_CANCEL)
    {
        QApplication::restoreOverrideCursor();
//        ui->Game->buildMode = 0;
    }
    else if(actName == ACT_CREATEFARMER){
//        if(mainPtr->player[0]->getFood() < BUILDING_CENTER_CREATEFARMER_FOOD) return ACTION_INVALID_RESOURCE;
//        mainPtr->player[0]->setFood(mainPtr->player[0]->getFood() - BUILDING_CENTER_CREATEFARMER_FOOD);
//        this->getObj()->setActPercent(0);
//        this->getObj()->setActspeed(0.25);
//        this->getObj()->setActNum(BUILDING_CENTER_CREATEFARMER);
//        this->getObj()->setActName(ACT_CREATEFARMER);
    }
    else if(actName == ACT_UPGRADE_AGE)
    {
//        std::list<Building *>::iterator buildIt = mainPtr->player[0]->build.begin();
//        bool isGranaryBuilt = false, isStockBuilt = false;
//        for(; buildIt != mainPtr->player[0]->build.end(); buildIt++)
//        {
//            if((*buildIt)->getNum() == BUILDING_GRANARY && (*buildIt)->isFinish()) isGranaryBuilt = true;
//            else if((*buildIt)->getNum() == BUILDING_STOCK && (*buildIt)->isFinish()) isStockBuilt = true;
//        }
//        if(!isGranaryBuilt || !isStockBuilt) return ACTION_INVALID_ACTION;

//        if(mainPtr->player[0]->getFood() < BUILDING_CENTER_UPGRADE_FOOD) return ACTION_INVALID_RESOURCE;
//        mainPtr->player[0]->setFood(mainPtr->player[0]->getFood() - BUILDING_CENTER_UPGRADE_FOOD);
//        this->getObj()->setActPercent(0);
//        this->getObj()->setmainPtr->actspeed(0.05);
//        this->getObj()->setActNum(BUILDING_CENTER_UPGRADE);
//        this->getObj()->setActName(ACT_UPGRADE_AGE);
    }
    else if(actName == ACT_UPGRADE_TOWERBUILD)
    {
//        if(mainPtr->player[0]->getFood() < BUILDING_GRANARY_ARROWTOWER_FOOD) return ACTION_INVALID_RESOURCE;
//        mainPtr->player[0]->setFood(mainPtr->player[0]->getFood() - BUILDING_GRANARY_ARROWTOWER_FOOD);
//        this->getObj()->setActPercent(0);
//        this->getObj()->setmainPtr->actspeed(0.25);
//        this->getObj()->setActNum(BUILDING_GRANARY_ARROWTOWER);
//        this->getObj()->setActName(ACT_UPGRADE_TOWERBUILD);
    }
    else if(actName == ACT_UPGRADE_WOOD)
    {
//        if(mainPtr->player[0]->getFood() < BUILDING_MARKET_WOOD_UPGRADE_FOOD || mainPtr->player[0]->getWood() < BUILDING_MARKET_WOOD_UPGRADE_WOOD) return ACTION_INVALID_RESOURCE;
//        mainPtr->player[0]->setFood(mainPtr->player[0]->getFood() - BUILDING_MARKET_WOOD_UPGRADE_FOOD);
//        mainPtr->player[0]->setWood(mainPtr->player[0]->getWood() - BUILDING_MARKET_WOOD_UPGRADE_WOOD);
//        this->getObj()->setActPercent(0);
//        this->getObj()->setmainPtr->actspeed(0.25);
//        this->getObj()->setActNum(BUILDING_MARKET_WOOD_UPGRADE);
//        this->getObj()->setActName(ACT_UPGRADE_WOOD);
    }
    else if(actName == ACT_UPGRADE_STONE)
    {
//        if(mainPtr->player[0]->getFood() < BUILDING_MARKET_STONE_UPGRADE_FOOD || mainPtr->player[0]->getStone() < BUILDING_MARKET_STONE_UPGRADE_STONE) return ACTION_INVALID_RESOURCE;
//        mainPtr->player[0]->setFood(mainPtr->player[0]->getFood() - BUILDING_MARKET_STONE_UPGRADE_FOOD);
//        mainPtr->player[0]->setStone(mainPtr->player[0]->getStone() - BUILDING_MARKET_STONE_UPGRADE_STONE);
//        this->getObj()->setActPercent(0);
//        this->getObj()->setmainPtr->actspeed(0.25);
//        this->getObj()->setActNum(BUILDING_MARKET_STONE_UPGRADE);
//        this->getObj()->setActName(ACT_UPGRADE_STONE);
    }
    else if(actName == ACT_UPGRADE_FARM)
    {
//        if(mainPtr->player[0]->getFood() < BUILDING_MARKET_FARM_UPGRADE_FOOD || mainPtr->player[0]->getWood() < BUILDING_MARKET_FARM_UPGRADE_WOOD) return ACTION_INVALID_RESOURCE;
//        mainPtr->player[0]->setFood(mainPtr->player[0]->getFood() - BUILDING_MARKET_FARM_UPGRADE_FOOD);
//        mainPtr->player[0]->setWood(mainPtr->player[0]->getWood() - BUILDING_MARKET_FARM_UPGRADE_WOOD);
//        this->getObj()->setActPercent(0);
//        this->getObj()->setmainPtr->actspeed(0.25);
//        this->getObj()->setActNum(BUILDING_MARKET_FARM_UPGRADE);
//        this->getObj()->setActName(ACT_UPGRADE_FARM);
    }
    else if(actName == ACT_STOP)
    {
//        this->getObj()->setActNum(0);
//        if(this->getObj()->getActName() == ACT_CREATEFARMER) mainPtr->player[0]->setFood(mainPtr->player[0]->getFood() + BUILDING_CENTER_CREATEFARMER_FOOD);
//        else if(this->getObj()->getActName() == ACT_UPGRADE_AGE) mainPtr->player[0]->setFood(mainPtr->player[0]->getFood() + BUILDING_CENTER_UPGRADE_FOOD);
//        else if(this->getObj()->getActName() == ACT_UPGRADE_TOWERBUILD) mainPtr->player[0]->setFood(mainPtr->player[0]->getFood() + BUILDING_GRANARY_ARROWTOWER_FOOD);
//        else if(this->getObj()->getActName() == ACT_UPGRADE_WOOD)
//        {
//            mainPtr->player[0]->setFood(mainPtr->player[0]->getFood() + BUILDING_MARKET_WOOD_UPGRADE_FOOD);
//            mainPtr->player[0]->setWood(mainPtr->player[0]->getWood() + BUILDING_MARKET_WOOD_UPGRADE_WOOD);
//        }
//        else if(this->getObj()->getActName() == ACT_UPGRADE_STONE)
//        {
//            mainPtr->player[0]->setFood(mainPtr->player[0]->getFood() + BUILDING_MARKET_STONE_UPGRADE_FOOD);
//            mainPtr->player[0]->setStone(mainPtr->player[0]->getStone() + BUILDING_MARKET_STONE_UPGRADE_STONE);
//        }
//        else if(this->getObj()->getActName() == ACT_UPGRADE_FARM)
//        {
//            mainPtr->player[0]->setFood(mainPtr->player[0]->getFood() + BUILDING_MARKET_FARM_UPGRADE_FOOD);
//            mainPtr->player[0]->setWood(mainPtr->player[0]->getWood() + BUILDING_MARKET_FARM_UPGRADE_WOOD);
//        }
//        if(this->getObj()->getActName() == ACT_CREATEFARMER || this->getObj()->getActName() == ACT_UPGRADE_AGE)
//        {
//            ui->objText->setText("");
//            this->getObj()->setmainPtr->actspeed(0);
//            this->getObj()->setActPercent(0);
//            this->getObj()->setActName(ACT_NULL);
//        }
//        else if(this->getObj()->getActName() == ACT_UPGRADE_TOWERBUILD)
//        {
//            ui->objText->setText("");
//            this->getObj()->setmainPtr->actspeed(0);
//            this->getObj()->setActPercent(0);
//            this->getObj()->setActName(ACT_NULL);
//        }
//        else if(this->getObj()->getActName() == ACT_UPGRADE_WOOD || this->getObj()->getActName() == ACT_UPGRADE_STONE || this->getObj()->getActName() == ACT_UPGRADE_FARM)
//        {
//            ui->objText->setText("");
//            this->getObj()->setmainPtr->actspeed(0);
//            this->getObj()->setActPercent(0);
//            this->getObj()->setActName(ACT_NULL);
//        }
//        else
//        {
//            return ACTION_INVALID_ACTION;
//        }
    }
    for(int i = 0; i < 10; i++)
    {
        mainPtr->getActs(i)->update();
    }
    return ACTION_SUCCESS;
}

void SelectWidget::updateActs()
{

}

void SelectWidget::drawActs()
{

}
