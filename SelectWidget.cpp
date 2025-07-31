#include "SelectWidget.h"
#include "ui_SelectWidget.h"


SelectWidget::SelectWidget(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::SelectWidget)
{
    ui->setupUi(this);
    MainWidget* c = (MainWidget*)this->parentWidget();
    mainPtr = c;
    QPalette pe;
    pe.setColor(QPalette::WindowText, Qt::white);
    ui->objHp->setPalette(pe);
    ui->objName->setPalette(pe);
    ui->objText->setPalette(pe);//设置白色字体
    ui->objText_ATK->setPalette(pe);
    ui->objText_DEF_melee->setPalette(pe);
    ui->objText_DEF_range->setPalette(pe);
    initActionResourceMap();
}

void SelectWidget::initActionResourceMap() {
    // 定义动作类型与资源映射
    actionResourceMap[ACT_CREATEFARMER] = "Button_Villager";
    actionResourceMap[ACT_STOP] = "Button_Stop";
    actionResourceMap[ACT_UPGRADE_AGE] = "ButtonTech_Center1";
    actionResourceMap[ACT_UPGRADE_FARM] = "ButtonTech_Cow";
    actionResourceMap[ACT_UPGRADE_STONE] = "ButtonTech_Stone";
    actionResourceMap[ACT_UPGRADE_GOLD] = "ButtonTech_GoldOre";
    actionResourceMap[ACT_UPGRADE_WOOD] = "ButtonTech_Lumber";
    actionResourceMap[ACT_UPGRADE_TOWERBUILD] = "ButtonTech_ArrowTower";
    actionResourceMap[ACT_STOCK_UPGRADE_USETOOL] = "ButtonTech_Spear";
    actionResourceMap[ACT_STOCK_UPGRADE_DEFENSE_INFANTRY] = "ButtonTech_Sword";
    actionResourceMap[ACT_STOCK_UPGRADE_DEFENSE_ARCHER] = "ButtonTech_Arrow";
    actionResourceMap[ACT_STOCK_UPGRADE_DEFENSE_RIDER] = "ButtonTech_Horse";
    actionResourceMap[ACT_ARMYCAMP_UPGRADE_CLUBMAN] = "ButtonTech_Axeman";
    actionResourceMap[ACT_ARMYCAMP_CREATE_SLINGER] = "Button_Slinger";
    actionResourceMap[ACT_RANGE_CREATE_BOWMAN] = "Button_Archer";
    actionResourceMap[ACT_STABLE_CREATE_SCOUT] = "Button_Scout";
    actionResourceMap[ACT_DOCK_CREATE_SAILING] = "Button_Sailing";
    actionResourceMap[ACT_DOCK_CREATE_WOOD_BOAT] = "Button_Wood_Boat";
    actionResourceMap[ACT_DOCK_CREATE_SHIP] = "Button_Ship";
    actionResourceMap[ACT_BUILD] = "Button_Build";
    actionResourceMap[ACT_BUILD_ARROWTOWER] = "Button_ArrowTower";
    actionResourceMap[ACT_BUILD_CANCEL] = "Exit";
    actionResourceMap[ACT_BUILD_FARM] = "Button_Farm";
    actionResourceMap[ACT_BUILD_GRANARY] = "Button_Granary";
    actionResourceMap[ACT_BUILD_HOUSE] = "Button_House1";
    actionResourceMap[ACT_BUILD_MARKET] = "Button_Market";
    actionResourceMap[ACT_BUILD_STOCK] = "Button_Stock";
    actionResourceMap[ACT_BUILD_ARMYCAMP] = "Button_ArmyCamp";
    actionResourceMap[ACT_BUILD_RANGE] = "Button_Range";
    actionResourceMap[ACT_BUILD_STABLE] = "Button_Stable";
    actionResourceMap[ACT_BUILD_DOCK] = "Button_Dock";
    actionResourceMap[ACT_SHIP_LAY] = "Button_Lay";
}

SelectWidget::~SelectWidget()
{
    delete ui;
}

//时间相关
QString SelectWidget::getShowTime()
{
    QString minute, second, millSecond;
    //分钟
    if (elapsedSec / 60 < 10) minute = "0" + QString::number(elapsedSec / 60);
    else minute = QString::number(elapsedSec / 60);
    //秒
    if (elapsedSec % 60 < 10) second = "0" + QString::number(elapsedSec % 60);
    else second = QString::number(elapsedSec % 60);
    //毫秒
    if (elapsedFrame < 10) millSecond = "0" + QString::number(elapsedFrame);
    else millSecond = QString::number(elapsedFrame);
    return (minute + ":" + second + ":" + millSecond);
}

void SelectWidget::paintEvent(QPaintEvent* event)
{
    if (nowobject != NULL)
    {
        QPainter painter(this);
        painter.drawRect(0, 0, 270, 170);
        painter.fillRect(QRect(0, 0, 270, 170), QBrush(QColor(Qt::black)));
        if (true) {
            painter.drawRect(130, 90, 120, 20);

            BloodHaver* bloodobject = NULL;
            Animal* animalObject = NULL;
            nowobject->printer_ToBloodHaver((void**)&bloodobject);
            nowobject->printer_ToAnimal((void**)&animalObject);
            if ((bloodobject != NULL && animalObject == NULL) || (animalObject != NULL && !animalObject->get_Gatherable()))
            {//如果有血条的对象，绘制血条
                int StartX = 130, StartY = 110;
                int percent = 0;

                //修改percent
                percent = bloodobject->getBloodPercent() * 100;

                if (percent > 100) percent = 100;
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

void SelectWidget::frameUpdate()
{
    refreshActs();
    updateActs();
    drawActs();
}

void SelectWidget::initActs()
{
    secondWidget_Build = false;

    //根据点击的对象初始化行动数组
    if (nowobject == NULL)
    {
        for (int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
        {
            actions[i] = ACT_NULL;
            actionStatus[i] = ACT_STATUS_DISABLED;
        }
        return;
    }
    int type = nowobject->getSort();

    for (int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
    {
        actions[i] = ACT_NULL;
        actionStatus[i] = ACT_STATUS_ENABLED;//重置窗口状态为可用，真正判断是否可用的代码在refreshActs
    }

    if (type == SORT_BUILDING)//建筑
    {
        Building* buildOb = (Building*)nowobject;
        bool isActing = buildOb->isFinish() && buildOb->getActSpeed() > 0;
        //        int nowActPosition = -1;

        if (isActing)
        {
            actions[0] = ACT_STOP;
            actionStatus[0] = ACT_STATUS_ENABLED;
        }
        else
        {
            for (int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
            {
                //新增判断是否显示
                if (mainPtr->player[0]->get_isBuildActionShowAble(buildOb->getNum(), buildOb->ActNameToActNum(buildOb->getActNames(i))))
                {
                    actions[i] = ((Building*)nowobject)->getActNames(i);//getActNames是Building特有的,用来获取action数组
                    actionStatus[i] = ((Building*)nowobject)->getActStatus(i);
                }
                else actions[i] = ACT_NULL;
            }
        }

    }
    else if (type == SORT_FARMER)//人类
    {
        Farmer* human = (Farmer*)nowobject;
        if (!human->isShip())
            actions[0] = ACT_BUILD;
        if (human->isShip() && human->get_farmerType() == 1) {
            actions[0] = ACT_SHIP_LAY;
        }
        for (int i = 1; i < ACT_WINDOW_NUM_FREE; i++)
        {
            actionStatus[i] = ACT_STATUS_DISABLED;
        }
    }
    else
    {
        for (int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
        {
            actions[i] = ACT_NULL;
            actionStatus[i] = ACT_STATUS_DISABLED;
        }
    }
    //    if(nowobject->getActSpeed() > 0)//如果行动中则只出现停止按钮
    //    {
    //        actions[0] = ACT_STOP;
    //        for(int i = 1; i < ACT_WINDOW_NUM_FREE; i++)
    //        {
    //            actions[i] = ACT_NULL;
    //            actionStatus[i] = ACT_STATUS_DISABLED;
    //        }
    //    }
}

void SelectWidget::refreshActs()
{
    //更新行动的逻辑：点击一个对象后调用一次initActs函数根据类型给行动数组赋值,人物直接赋值，建筑通过getActNames赋值
    //refreshActs函数：每帧执行，仅根据行动id执行对应的判断条件决定是否可以执行(对于点击按钮手动操作游戏)，不再像以前重复刷新行动数组的具体行动。以及刷新本窗口的对象信息显示
    //执行了doActs之后，新的行动数组通过updateActs更新
    //上面更新的内容都是int数组，然后initActs和updateActs最后再调用一个函数(drawActs)根据数组的值贴上图片,refreshActs只控制图像是否为灰色

    //ai调用行动后具体的条件判断还是在doActs里
    //同时doActs也包括更新新actions数组
    //updateActs改为每帧刷新判断所有建筑的行动数组
    //updateActs只刷新建筑的行动，人物行动不刷新，人物的行动只受initActs和doActs的影响，是否为灰仍由refreshActs判断
    //drawActs就根据actions数组的内容给ActWidget窗口贴上对应图片

    //留的问题：
    //updateActs里面setActStatus的一系列判断条件都也要写到(复制，两个地方都要有，updateActs更新建筑的action,actionStatus数组要用于手操按钮游玩时判定是否可以执行行动)doActs的对应行动里面(用于拒绝ai无效命令)
    bool isBuild, isBuildingAct;
    int buildType, buildingActType;
    if (nowobject != NULL)
    {
        if (nowobject->getActSpeed() > 0) ui->objText->setText(QString::number((int)(nowobject->getActPercent())) + "%");//如果有进行中的任务则显示进度
        else ui->objText->setText("");

        ui->objIcon->setPixmap(QPixmap());
        ui->objIconSmall->setPixmap(QPixmap());
        ui->objHp->setText("");
        ui->objText_ATK->setText("");
        ui->objIconSmall_ATK->setPixmap(QPixmap());
        ui->objText_DEF_melee->setText("");
        ui->objIconSmall_DEF_melee->setPixmap(QPixmap());
        ui->objText_DEF_range->setText("");
        ui->objIconSmall_DEF_range->setPixmap(QPixmap());
    }//行动的进度

    //人口当前数量、建筑情况等，直接遍历
    //计算一些参数
    std::list<Building*>::iterator buildIt = mainPtr->player[0]->build.begin(), buildItTemp = buildIt;
    std::list<Human*>::iterator humanIt = mainPtr->player[0]->human.begin(), humanItTemp = humanIt;

    //当前人口
    human_num = mainPtr->player[0]->getHumanNum();
    //当前人口上限
    build_hold_human_num = mainPtr->player[0]->getMaxHumanNum();

    //建成谷仓
    isGranaryBuilt = mainPtr->player[0]->get_isBuildingHaveBuild(BUILDING_GRANARY);
    //建成市场
    isMarketBuilt = mainPtr->player[0]->get_isBuildingHaveBuild(BUILDING_MARKET);
    isStockBuilt = mainPtr->player[0]->get_isBuildingHaveBuild(BUILDING_STOCK);


    //先进行行动状态更新(根据actions决定actionStatus数组)
    for (int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
    {
        isBuild = false, isBuildingAct = false;

        switch (actions[i])//根据actions数组的值（行动类别）判断可用性
        {
            //建造
        case ACT_BUILD_ARROWTOWER:
            isBuild = true;
            buildType = BUILDING_ARROWTOWER;
            break;
        case ACT_BUILD_FARM:
            isBuild = true;
            buildType = BUILDING_FARM;
            break;
        case ACT_BUILD_GRANARY:
            isBuild = true;
            buildType = BUILDING_GRANARY;
            break;
        case ACT_BUILD_HOUSE:
            isBuild = true;
            buildType = BUILDING_HOME;
            break;
        case ACT_BUILD_MARKET:
            isBuild = true;
            buildType = BUILDING_MARKET;
            break;
        case ACT_BUILD_STOCK:
            isBuild = true;
            buildType = BUILDING_STOCK;
            break;
        case ACT_BUILD_ARMYCAMP:
            isBuild = true;
            buildType = BUILDING_ARMYCAMP;
            break;
        case ACT_BUILD_RANGE:
            isBuild = true;
            buildType = BUILDING_RANGE;
            break;
        case ACT_BUILD_STABLE:
            isBuild = true;
            buildType = BUILDING_STABLE;
            break;
        case ACT_BUILD_DOCK:
            isBuild = true;
            buildType = BUILDING_DOCK;
            break;

            //建筑行动
        case ACT_CREATEFARMER:
            isBuildingAct = true;
            buildType = BUILDING_CENTER;
            buildingActType = BUILDING_CENTER_CREATEFARMER;
            break;
        case ACT_UPGRADE_AGE:
            isBuildingAct = true;
            buildType = BUILDING_CENTER;
            buildingActType = BUILDING_CENTER_UPGRADE;
            break;
        case ACT_UPGRADE_FARM:
            isBuildingAct = true;
            buildType = BUILDING_MARKET;
            buildingActType = BUILDING_MARKET_FARM_UPGRADE;
            break;
        case ACT_UPGRADE_STONE:
            isBuildingAct = true;
            buildType = BUILDING_MARKET;
            buildingActType = BUILDING_MARKET_STONE_UPGRADE;
            break;
        case ACT_UPGRADE_GOLD:
            isBuildingAct = true;
            buildType = BUILDING_MARKET;
            buildingActType = BUILDING_MARKET_GOLD_UPGRADE;
            break;
        case ACT_UPGRADE_WOOD:
            isBuildingAct = true;
            buildType = BUILDING_MARKET;
            buildingActType = BUILDING_MARKET_WOOD_UPGRADE;
            break;
        case ACT_DOCK_CREATE_SAILING:
            isBuildingAct = true;
            buildType = BUILDING_DOCK;
            buildingActType = BUILDING_DOCK_CREATE_SAILING;
            break;
        case ACT_DOCK_CREATE_WOOD_BOAT:
            isBuildingAct = true;
            buildType = BUILDING_DOCK;
            buildingActType = BUILDING_DOCK_CREATE_WOOD_BOAT;
            break;
        case ACT_DOCK_CREATE_SHIP:
            isBuildingAct = true;
            buildType = BUILDING_DOCK;
            buildingActType = BUILDING_DOCK_CREATE_SHIP;
            break;

        case ACT_NULL:
            actionStatus[i] = ACT_STATUS_DISABLED;
            break;
        case ACT_BUILD_CANCEL:
            actionStatus[i] = ACT_STATUS_ENABLED;
            break;
        }

        if (isBuild)
        {
            if (!mainPtr->player[0]->get_isBuildingShowAble(buildType) || !mainPtr->player[0]->get_isBuildingAble(buildType))
                actionStatus[i] = ACT_STATUS_DISABLED;
            else
                actionStatus[i] = ACT_STATUS_ENABLED;
        }
        else if (isBuildingAct)
        {
            if (!mainPtr->player[0]->get_isBuildActionAble(buildType, buildingActType))
                actionStatus[i] = ACT_STATUS_DISABLED;
            else
                actionStatus[i] = ACT_STATUS_ENABLED;
        }
        else {
            actionStatus[i] = ACT_STATUS_ENABLED;
        }
    }


    for (int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
    {
        mainPtr->getActs(i)->setStatus(actionStatus[i]);//应用行动状态,mainPtr->getActs(i)即获取第i个按钮窗口
        mainPtr->getActs(i)->update();//刷新按钮显示状态
    }

    if (secondWidget_Build)
    {
        showBuildActLab();
    }

    //再进行快捷栏和状态栏显示更新(本窗口的内容)
    if (nowobject == NULL)
    {
        ui->objHp->setText("");
        ui->objIcon->setPixmap(QPixmap());
        ui->objName->setText("");
        ui->objText->setText("");
        ui->objText_ATK->setText("");
        ui->objText_DEF_melee->setText("");
        ui->objText_DEF_range->setText("");
        ui->objIconSmall->setPixmap(QPixmap());
        ui->objIconSmall_ATK->setPixmap(QPixmap());
        ui->objIconSmall_DEF_melee->setPixmap(QPixmap());
        ui->objIconSmall_DEF_range->setPixmap(QPixmap());
        for (int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
        {
            actionStatus[i] = ACT_STATUS_DISABLED;
            actions[i] = ACT_NULL;
        }
        this->hide();
        this->update();
    }
    else
    {
        int type = nowobject->getSort();//获取当前对象类型
        if (type == SORT_BUILDING || type == SORT_Building_Resource)//建筑
        {
            Building* objBuilding = (Building*)nowobject;
            bool isActing = objBuilding->isFinish() && objBuilding->getActSpeed() > 0;
            buildType = objBuilding->getNum();//获取建筑种类
            ui->objName->setText(QString::fromStdString(objBuilding->getDisplayName(buildType)));//设置显示名称

            //根据不同时代设置不同的图标
            string name = "Button_" + objBuilding->getBuiltname(mainPtr->player[0]->getCiv(), buildType);
            if (resMap.find(name) != resMap.end() && !resMap[name].empty()) {
                // 如果找到了对应的图片，设置该图片
                ui->objIcon->setPixmap(resMap[name].front().scaled(110, 110));
            }
            else {
                // 如果没有找到对应的图片，使用默认图片
                ui->objIcon->setPixmap(resMap["Button"].front().scaled(110, 110));
            }


            if (objBuilding->getActSpeed() != 0)
            {
                ui->objIconSmall->setPixmap(resMap["SmallIcon_Sandglass"].front());//如果在行动中，在进度条百分比旁显示一个沙漏小图标
            }
            else
            {
                ui->objIconSmall->setPixmap(QPixmap());
            }

            //设置hp
            ui->objHp->setText(QString::number(objBuilding->getBlood()) + "/" + QString::number(objBuilding->getMaxBlood()));

            //同步行动状态至窗口
            if (objBuilding->isFinish())
            {
                //后续简化代码
                if (isActing)
                {
                    actions[0] = ACT_STOP;
                    actionStatus[0] = ACT_STATUS_ENABLED;
                    for (int i = 1;i < ACT_WINDOW_NUM_FREE;i++) actions[i] = ACT_NULL;
                }
                else
                {
                    for (int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
                    {
                        //新增判断是否显示
                        if (mainPtr->player[0]->get_isBuildActionShowAble(objBuilding->getNum(), objBuilding->ActNameToActNum(objBuilding->getActNames(i))))
                        {
                            actions[i] = objBuilding->getActNames(i);//getActNames是Building特有的,用来获取action数组
                            actionStatus[i] = objBuilding->getActStatus(i);
                        }
                        else actions[i] = ACT_NULL;
                    }
                }

                //统一根据内置行动数组设置
                for (int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
                {
                    if (actions[i] != ACT_NULL) mainPtr->getActs(i)->show();
                    else mainPtr->getActs(i)->hide();
                }

                //详细设置
                if (objBuilding->getNum() == BUILDING_HOME)
                {
                    ui->objText->setText(QString::number(human_num) + "/" + QString::number(build_hold_human_num));
                    ui->objIconSmall->setPixmap(resMap["SmallIcon_People"].front());
                }
                else if (objBuilding->getNum() == BUILDING_FARM)
                {
                    Building_Resource* farm = (Building_Resource*)objBuilding;

                    if (farm->get_Cnt() > 0)
                    {
                        ui->objText->setText(QString::number((int)(farm->get_Cnt())));
                        ui->objIconSmall->setPixmap(resMap["Icon_Food"].front());
                    }
                }

            }
            else//如果建筑建造未完成
            {
                ui->objText->setText(QString::number((int)(objBuilding->getPercent())) + "%");
                ui->objIconSmall->setPixmap(resMap["SmallIcon_Sandglass"].front());
                for (int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
                {
                    actions[i] = ACT_NULL;
                    actionStatus[i] = ACT_STATUS_DISABLED;
                }
            }
            this->update();
            this->show();//必要的update和show
        }
        else if (type == SORT_STATICRES)//块资源
        {
            StaticRes* objStaticRes = (StaticRes*)(nowobject);
            int num = objStaticRes->getNum();
            ui->objName->setText(QString::fromStdString(objStaticRes->getStaticResDisplayName(num)));
            ui->objHp->setText("");
            //根据不同时代设置不同的图标
            string name = "Button_" + objStaticRes->getStaticResName(num);
            if (resMap.find(name) != resMap.end() && !resMap[name].empty()) {
                // 如果找到了对应的图片，设置该图片
                ui->objIcon->setPixmap(resMap[name].front().scaled(110, 110));
            }
            else {
                // 如果没有找到对应的图片，使用默认图片
                ui->objIcon->setPixmap(resMap["Button"].front().scaled(110, 110));
            }
            //            ui->objIcon->setPixmap(resMap["Button_"+objStaticRes->getStaticResName(num)].front().scaled(80,80));
            ui->objIconSmall->setPixmap(QPixmap());

            if (objStaticRes->get_ResourceSort() == HUMAN_GRANARYFOOD || objStaticRes->get_ResourceSort() == HUMAN_STOCKFOOD || objStaticRes->get_ResourceSort() == HUMAN_DOCKFOOD) ui->objIconSmall->setPixmap(resMap["Icon_Food"].front());
            else if (objStaticRes->get_ResourceSort() == HUMAN_STONE) ui->objIconSmall->setPixmap(resMap["Icon_Stone"].front());
            else if (objStaticRes->get_ResourceSort() == HUMAN_GOLD) ui->objIconSmall->setPixmap(resMap["Icon_Gold"].front());

            ui->objText->setText(QString::number(objStaticRes->get_Cnt()));
            this->update();
            this->show();
        }
        else if (type == SORT_FARMER)//村民
        { //objIconSmall_ATK objText_ATK用于展示攻击力 objIconSmall objText表示携带资源 objIconSmall_DEF和objText_DEF表示防御（近战和远程分开）
            Farmer* objFarmer = (Farmer*)(nowobject);
            int num = objFarmer->getState();//获取工作状态并显示对应名称
            QString name;

            switch (objFarmer->get_farmerType()) {
            case 0:
                ui->objIcon->setPixmap(resMap["Button_Village"].front().scaled(110, 110));
                name = QString::fromStdString(objFarmer->getDisplayName(num));
                break;
            case 1:
                ui->objIcon->setPixmap(resMap["Button_Wood_Boat"].front().scaled(110, 110));
                name = "运输船";
                break;
            case 2:
                ui->objIcon->setPixmap(resMap["Button_Sailing"].front().scaled(110, 110));
                name = "渔船";
                break;
            default:
                break;
            }
            ui->objName->setText(name);

            if (objFarmer->getState() == 0 || objFarmer->getState() == 4)
            {
                ui->objIconSmall_ATK->setPixmap(resMap["SmallIcon_Attack"].front().scaled(40, 30));
                if (objFarmer->showATK_Addition() == 0) ui->objText_ATK->setText(QString::number(objFarmer->showATK_Basic()));
                else ui->objText_ATK->setText(QString::number(objFarmer->showATK_Basic()) + "+" + QString::number(objFarmer->showATK_Addition())); // 显示攻击力（基础+额外）

            }

            if (objFarmer->getResourceSort() == HUMAN_WOOD) ui->objIconSmall->setPixmap(resMap["Icon_Wood"].front());
            else if (objFarmer->getResourceSort() == HUMAN_GRANARYFOOD || objFarmer->getResourceSort() == HUMAN_STOCKFOOD || objFarmer->getResourceSort() == HUMAN_DOCKFOOD)
                ui->objIconSmall->setPixmap(resMap["Icon_Food"].front());
            else if (objFarmer->getResourceSort() == HUMAN_STONE) ui->objIconSmall->setPixmap(resMap["Icon_Stone"].front());
            //运输船显示运载人口
            else if (objFarmer->get_farmerType() == 1) ui->objIconSmall->setPixmap(resMap["SmallIcon_People"].front());
            //如果当前村民没有资源
            if (objFarmer->getResourceNowHave() == 0)
            {
                ui->objIconSmall->setPixmap(QPixmap());
                ui->objText->setText("");
            }
            else if (objFarmer->get_farmerType() == FARMERTYPE_WOOD_BOAT) {
                ui->objText->setText(QString::number((int)(objFarmer->getResourceNowHave())) + "/5 ");
            }
            else if(objFarmer->get_farmerType() == FARMERTYPE_SAILING) {
                ui->objText->setText(QString::number((int)(objFarmer->getResourceNowHave())) + "/15 ");
            }
            else {
                ui->objText->setText(QString::number((int)(objFarmer->getResourceNowHave())));
            }

            //设置血量
            ui->objHp->setText(QString::number(objFarmer->getBlood()) + "/" + QString::number(objFarmer->getMaxBlood()));
            this->update();
            this->show();

        }
        else if (type == SORT_ARMY)
        {
            //objIconSmall_ATK objText_ATK用于展示攻击力 objIconSmall objText表示携带资源或者防御
            Army* objArmy = (Army*)nowobject;
            ui->objName->setText(objArmy->getChineseName());
            std::string buttonName = "Button_" + objArmy->getArmyName(objArmy->getNum(), objArmy->getLevel());
            ui->objIcon->setPixmap(resMap[buttonName].front().scaled(110, 110));
            ui->objIconSmall_ATK->setPixmap(resMap["SmallIcon_Attack"].front().scaled(40, 30)); //攻击图标
            ui->objIconSmall_DEF_melee->setPixmap(resMap["SmallIcon_Defense_Melee"].front().scaled(40, 30));
            ui->objIconSmall_DEF_range->setPixmap(resMap["SmallIcon_Defense_Range"].front().scaled(40, 30));
            if (objArmy->showATK_Addition() == 0) ui->objText_ATK->setText(QString::number(objArmy->showATK_Basic()));
            else ui->objText_ATK->setText(QString::number(objArmy->showATK_Basic()) + "+" + QString::number(objArmy->showATK_Addition()));// 显示攻击力（基础+额外）
            if (objArmy->showDEF_Close_Addition() == 0) ui->objText_DEF_melee->setText(QString::number(objArmy->showDEF_Close()));
            else ui->objText_DEF_melee->setText(QString::number(objArmy->showDEF_Close()) + "+" + QString::number(objArmy->showDEF_Close_Addition())); // 显示近战防御（基础+额外）
            if (objArmy->showDEF_Shoot_Addition() == 0) ui->objText_DEF_range->setText(QString::number(objArmy->showDEF_Shoot()));
            else ui->objText_DEF_range->setText(QString::number(objArmy->showDEF_Shoot()) + "+" + QString::number(objArmy->showDEF_Shoot_Addition())); // 显示远程防御（基础+额外）
            //设置血量
            ui->objHp->setText(QString::number(objArmy->getBlood()) + "/" + QString::number(objArmy->getMaxBlood()));
            this->update();
            this->show();
        }
        else if (type == SORT_ANIMAL)//动物
        {
            Animal* objAnimal = (Animal*)(nowobject);
            ui->objName->setText(QString::fromStdString(Animal::getAnimalDisplayName(objAnimal->getNum())));

            if (objAnimal->getNum() == 1)//瞪羚
            {
                ui->objIcon->setPixmap(resMap["Button_Gazelle"].front().scaled(110, 110));
            }
            else if (objAnimal->getNum() == 2)//大象
            {
                ui->objIcon->setPixmap(resMap["Button_Elephant"].front().scaled(110, 110));
            }
            else if (objAnimal->getNum() == 3)
            {
                ui->objIcon->setPixmap(resMap["Button_Lion"].front().scaled(110, 110));
            }
            else if (objAnimal->getNum() == ANIMAL_TREE || objAnimal->getNum() == ANIMAL_FOREST)
            {
                ui->objIcon->setPixmap(resMap["Button_Tree"].front().scaled(110, 110));
            }

            //动物的可采集资源信息
            //显示动物可采集资源数
            ui->objText->setText(QString::number((int)(objAnimal->get_Cnt())));
            //显示动物的采集资源类型图片
            switch (objAnimal->get_ResourceSort()) {
            case HUMAN_WOOD:
                ui->objIconSmall->setPixmap(resMap["Icon_Wood"].front());
                break;
            case HUMAN_STOCKFOOD:
                ui->objIconSmall->setPixmap(resMap["Icon_Food"].front());
                break;
            default:
                break;
            }

            //死亡后，不显示血量
            if (objAnimal->isDie()) ui->objHp->setText(""); //原代码意思是当动物死亡后变为收集的资源时，不显示血条，现在可能有新的设计
            else ui->objHp->setText(QString::number(objAnimal->getBlood()) + "/" + QString::number(objAnimal->getMaxBlood()));

            this->update();
            this->show();
        }

    }
    for (int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
    {
        mainPtr->getActs(i)->update();
    }
}

void SelectWidget::widgetAct(int num)
{
    if (mainPtr->getActs(num)->getStatus() == ACT_STATUS_DISABLED) return;
    int actName = actions[num];
    doActs(actName);
}

int SelectWidget::aiAct(int actName, Coordinate* self)
{
    return doActs(actName, self);
}

void SelectWidget::manageBuildBottom(int position, int actNum, int buildingNum)
{
    if (mainPtr->player[0]->get_isBuildingShowAble(buildingNum))
    {
        actions[position] = actNum;
        if (mainPtr->player[0]->get_isBuildingAble(buildingNum)) actionStatus[position] = ACT_STATUS_ENABLED;
        else actionStatus[position] = ACT_STATUS_DISABLED;
    }
    else actions[position] = ACT_NULL;
}
int SelectWidget::doActs(int actName, Coordinate* nowobject)
{
    if (nowobject == nullptr) return ACTION_INVALID_ACTION;
    // Helper function to handle cursor and build mode changes
    auto setCursorAndBuildMode = [&](const QString& resourceKey, int buildMode) {
        QApplication::restoreOverrideCursor();
        QCursor cursor(resMap[resourceKey.toStdString()].front());
        QApplication::setOverrideCursor(cursor);
        emit sendBuildMode(buildMode);
        };
    // Handle building actions
    switch (actName) {
    case ACT_BUILD:
        secondWidget_Build = true;
        showBuildActLab();
        break;
    case ACT_BUILD_HOUSE:
        setCursorAndBuildMode("House1", BUILDING_HOME);
        break;
    case ACT_BUILD_GRANARY:
        setCursorAndBuildMode("Granary", BUILDING_GRANARY);
        break;
    case ACT_BUILD_STOCK:
        setCursorAndBuildMode("Stock", BUILDING_STOCK);
        break;
    case ACT_BUILD_FARM:
        setCursorAndBuildMode("Farm", BUILDING_FARM);
        break;
    case ACT_BUILD_MARKET:
        setCursorAndBuildMode("Market", BUILDING_MARKET);
        break;
    case ACT_BUILD_ARROWTOWER:
        setCursorAndBuildMode("ArrowTower", BUILDING_ARROWTOWER);
        break;
    case ACT_BUILD_ARMYCAMP:
        setCursorAndBuildMode("ArmyCamp", BUILDING_ARMYCAMP);
        break;
    case ACT_BUILD_RANGE:
        setCursorAndBuildMode("Range", BUILDING_RANGE);
        break;
    case ACT_BUILD_STABLE:
        setCursorAndBuildMode("Stable", BUILDING_STABLE);
        break;
    case ACT_BUILD_DOCK:
        setCursorAndBuildMode("Dock", BUILDING_DOCK);
        break;
    case ACT_BUILD_CANCEL:
        QApplication::restoreOverrideCursor();
        emit sendBuildMode(-1);
        initActs();
        break;
        // Handle other actions
    case ACT_SHIP_LAY:
        core->addRelation(nowobject, nowobject, CoreEven_UnLoad);
        break;
    case ACT_CREATEFARMER:
        core->addRelation(nowobject, CoreEven_BuildingAct, BUILDING_CENTER_CREATEFARMER);
        break;
    case ACT_UPGRADE_AGE:
        if (mainPtr->player[0]->get_civiBuild_Times(mainPtr->player[0]->getCiv()) >= 2)
            core->addRelation(nowobject, CoreEven_BuildingAct, BUILDING_CENTER_UPGRADE);
        break;
    case ACT_UPGRADE_TOWERBUILD:
        core->addRelation(nowobject, CoreEven_BuildingAct, BUILDING_GRANARY_ARROWTOWER);
        break;
    case ACT_UPGRADE_WOOD:
        core->addRelation(nowobject, CoreEven_BuildingAct, BUILDING_MARKET_WOOD_UPGRADE);
        break;
    case ACT_UPGRADE_STONE:
        core->addRelation(nowobject, CoreEven_BuildingAct, BUILDING_MARKET_STONE_UPGRADE);
        break;
    case ACT_UPGRADE_GOLD:
        core->addRelation(nowobject, CoreEven_BuildingAct, BUILDING_MARKET_GOLD_UPGRADE);
        break;
    case ACT_UPGRADE_FARM:
        core->addRelation(nowobject, CoreEven_BuildingAct, BUILDING_MARKET_FARM_UPGRADE);
        break;
    case ACT_STOCK_UPGRADE_USETOOL:
        core->addRelation(nowobject, CoreEven_BuildingAct, BUILDING_STOCK_UPGRADE_USETOOL);
        break;
    case ACT_STOCK_UPGRADE_DEFENSE_INFANTRY:
        core->addRelation(nowobject, CoreEven_BuildingAct, BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY);
        break;
    case ACT_STOCK_UPGRADE_DEFENSE_ARCHER:
        core->addRelation(nowobject, CoreEven_BuildingAct, BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER);
        break;
    case ACT_STOCK_UPGRADE_DEFENSE_RIDER:
        core->addRelation(nowobject, CoreEven_BuildingAct, BUILDING_STOCK_UPGRADE_DEFENSE_RIDER);
        break;
    case ACT_ARMYCAMP_CREATE_CLUBMAN:
        core->addRelation(nowobject, CoreEven_BuildingAct, BUILDING_ARMYCAMP_CREATE_CLUBMAN);
        break;
    case ACT_ARMYCAMP_UPGRADE_CLUBMAN:
        core->addRelation(nowobject, CoreEven_BuildingAct, BUILDING_ARMYCAMP_UPGRADE_CLUBMAN);
        break;
    case ACT_ARMYCAMP_CREATE_SLINGER:
        core->addRelation(nowobject, CoreEven_BuildingAct, BUILDING_ARMYCAMP_CREATE_SLINGER);
        break;
    case ACT_RANGE_CREATE_BOWMAN:
        core->addRelation(nowobject, CoreEven_BuildingAct, BUILDING_RANGE_CREATE_BOWMAN);
        break;
    case ACT_STABLE_CREATE_SCOUT:
        core->addRelation(nowobject, CoreEven_BuildingAct, BUILDING_STABLE_CREATE_SCOUT);
        break;
    case ACT_DOCK_CREATE_SAILING:
        core->addRelation(nowobject, CoreEven_BuildingAct, BUILDING_DOCK_CREATE_SAILING);
        break;
    case ACT_DOCK_CREATE_WOOD_BOAT:
        core->addRelation(nowobject, CoreEven_BuildingAct, BUILDING_DOCK_CREATE_WOOD_BOAT);
        break;
    case ACT_DOCK_CREATE_SHIP:
        core->addRelation(nowobject, CoreEven_BuildingAct, BUILDING_DOCK_CREATE_SHIP);
        break;
    case ACT_STOP:
        if (nowobject != nullptr) {
            core->suspendRelation(nowobject);
            ui->objText->setText("");
        }
        else {
            return ACTION_INVALID_ACTION;
        }
        break;

    default:
        return ACTION_INVALID_ACTION;
    }

    // Update all actions
    for (int i = 0; i < ACT_WINDOW_NUM_FREE; i++) {
        mainPtr->getActs(i)->update();
    }

    return ACTION_SUCCESS;
}


void SelectWidget::showBuildActLab()
{
    manageBuildBottom(0, ACT_BUILD_HOUSE, BUILDING_HOME);
    manageBuildBottom(1, ACT_BUILD_GRANARY, BUILDING_GRANARY);
    manageBuildBottom(2, ACT_BUILD_STOCK, BUILDING_STOCK);
    manageBuildBottom(3, ACT_BUILD_MARKET, BUILDING_MARKET);
    manageBuildBottom(4, ACT_BUILD_ARROWTOWER, BUILDING_ARROWTOWER);
    manageBuildBottom(5, ACT_BUILD_ARMYCAMP, BUILDING_ARMYCAMP);
    manageBuildBottom(6, ACT_BUILD_RANGE, BUILDING_RANGE);
    manageBuildBottom(7, ACT_BUILD_STABLE, BUILDING_STABLE);
    manageBuildBottom(8, ACT_BUILD_FARM, BUILDING_FARM);
    manageBuildBottom(9, ACT_BUILD_DOCK, BUILDING_DOCK);

    actions[11] = ACT_BUILD_CANCEL;
    actionStatus[11] = ACT_STATUS_ENABLED;
}

void SelectWidget::updateActs()
{
    //遍历建筑更新活动列表
    std::list<Building*>::iterator buildIt = mainPtr->player[0]->build.begin();
    int wood = mainPtr->player[0]->getWood();
    int food = mainPtr->player[0]->getFood();
    int stone = mainPtr->player[0]->getStone();
    int gold = mainPtr->player[0]->getGold();

    for (; buildIt != mainPtr->player[0]->build.end(); buildIt++)
    {
        if ((*buildIt)->isFinish())
        {
            //为每个位置设置act_status,是否显示、显示停止在initact中判断
            (*buildIt)->setActStatus(wood, food, stone, gold);
        }
    }
}

void SelectWidget::drawActs()
{
    QString actionKey;
    int size = 70;

    // 遍历 actions 数组，设置图标
    for (int i = 0; i < ACT_WINDOW_NUM_FREE; i++) {
        QPixmap pix;

        if (actionResourceMap.contains(actions[i])) {
            actionKey = actionResourceMap[actions[i]];
        }
        else if (actions[i] == ACT_ARMYCAMP_CREATE_CLUBMAN) {
            // 特殊情况：根据等级决定资源
            switch ((int)mainPtr->player[0]->get_buildActLevel(BUILDING_ARMYCAMP, BUILDING_ARMYCAMP_UPGRADE_CLUBMAN)) {
            case 0: actionKey = "Button_Clubman"; break;
            case 1: actionKey = "Button_Axeman"; break;
            default: actionKey = "Button_Clubman"; break;
            }
        }
        else {
            actionKey = "Button"; // 默认值
        }

        std::string actionKeyStd = actionKey.toStdString();

        // 尝试获取资源，如果找不到则使用默认按钮资源
        if (resMap.find(actionKeyStd) != resMap.end()) {
            pix = resMap[actionKeyStd].front().scaled(size, size);
        }
        else {
            pix = resMap["Button"].front().scaled(size, size);
            qWarning() << "Resource for action" << actions[i] << "not found. Using default Button.";
        }


        mainPtr->getActs(i)->setPix(pix);

        // 设置 actions[i] 为 NULL 时隐藏
        if (actions[i] != ACT_NULL) {
            mainPtr->getActs(i)->show();
        }
        else {
            mainPtr->getActs(i)->hide();
        }

        mainPtr->getActs(i)->update();
    }
}


//所有建造
void SelectWidget::getBuild(int BlockL, int BlockU, int num)
{
    core->addRelation(nowobject, BlockL, BlockU, CoreEven_CreatBuilding, true, num);
}
