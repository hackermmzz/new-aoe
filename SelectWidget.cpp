#include "SelectWidget.h"
#include "ui_SelectWidget.h"

SelectWidget::SelectWidget(QWidget* parent) : QWidget(parent),
ui(new Ui::SelectWidget)
{
    ui->setupUi(this);
    MainWidget* c = (MainWidget*)this->parentWidget();
    mainPtr = c;
    QPalette pe;
    pe.setColor(QPalette::WindowText, Qt::white);
    ui->objHp->setPalette(pe);
    ui->objName->setPalette(pe);
    ui->objText->setPalette(pe); // 设置白色字体
    ui->objText_ATK->setPalette(pe);
    ui->objText_DEF_melee->setPalette(pe);
    ui->objText_DEF_range->setPalette(pe);
    initActionResourceMap();
}

void SelectWidget::initActionResourceMap()
{
    // 定义动作类型与资源映射
    actionResourceMap[ACT_CREATEFARMER] = "Button_Villager";
    actionResourceMap[ACT_STOP] = "Button_Stop";
    actionResourceMap[ACT_UPGRADE_AGE] = "ButtonTech_Center1";
    actionResourceMap[ACT_UPGRADE_BRONZEAGE] = "ButtonTech_Center2";
    actionResourceMap[ACT_UPGRADE_FARM] = "ButtonTech_Cow";
    actionResourceMap[ACT_UPGRADE_STONE] = "ButtonTech_Stone";
    actionResourceMap[ACT_UPGRADE_GOLD] = "ButtonTech_GoldOre";
    actionResourceMap[ACT_UPGRADE_WOOD] = "ButtonTech_Lumber";
    actionResourceMap[ACT_UPGRADE_WHEEL] = "ButtonTech_Wheel";
    actionResourceMap[ACT_UPGRADE_CRAFT] = "ButtonTech_Craft";  // 工艺科技按钮图标
    actionResourceMap[ACT_UPGRADE_PLOW] = "ButtonTech_Plow";   // 犁科技按钮图标
    actionResourceMap[ACT_UPGRADE_TOWERBUILD] = "ButtonTech_ArrowTower";
    // 谷仓「箭塔升为二级」科技按钮：资源键 ButtonTech_ArrowTower2（如 ButtonTech_ArrowTower2_001.png）
    actionResourceMap[ACT_UPGRADE_ARROWTOWER] = "ButtonTech_ArrowTower2";
    actionResourceMap[ACT_STOCK_UPGRADE_USETOOL] = "ButtonTech_Spear";
    actionResourceMap[ACT_STOCK_UPGRADE_DEFENSE_INFANTRY] = "ButtonTech_Sword";
    actionResourceMap[ACT_STOCK_UPGRADE_DEFENSE_ARCHER] = "ButtonTech_Arrow";
    actionResourceMap[ACT_STOCK_UPGRADE_DEFENSE_RIDER] = "ButtonTech_Horse";
    actionResourceMap[ACT_STOCK_UPGRADE_MISSILE_DEFENSE_INFANTRY] = "ButtonTech_BronzeShield";
    actionResourceMap[ACT_STOCK_UPGRADE_METALWORKING] = "ButtonTech_Spear2";
    actionResourceMap[ACT_STOCK_UPGRADE_DEFENSE_INFANTRY_SCALE] = "ButtonTech_Sword2";
    actionResourceMap[ACT_STOCK_UPGRADE_DEFENSE_ARCHER_SCALE] = "ButtonTech_Arrow2";
    actionResourceMap[ACT_STOCK_UPGRADE_DEFENSE_RIDER_SCALE] = "ButtonTech_Horse2";
    actionResourceMap[ACT_ARMYCAMP_UPGRADE_CLUBMAN] = "ButtonTech_Axeman";
    actionResourceMap[ACT_ARMYCAMP_CREATE_SLINGER] = "Button_Slinger";
    actionResourceMap[ACT_ARMYCAMP_CREATE_BROADSWORD] = "Button_BroadSwordsman";
    actionResourceMap[ACT_ARMYCAMP_UPGRADE_BROADSWORD] = "ButtonTech_Broadsword";
    actionResourceMap[ACT_ARMYCAMP_RESEARCH_LOGISTICS] = "ButtonTech_Logistics";
    actionResourceMap[ACT_COLLAGE_CREATE_HOPLITE] = "Button_Hoplite";
    actionResourceMap[ACT_RANGE_CREATE_BOWMAN] = "Button_Archer";
    actionResourceMap[ACT_RANGE_CREATE_COMPOSITE_BOWMAN] = "Button_CompositeBowman";
    actionResourceMap[ACT_RANGE_UPGRADE_COMPOSITE_BOW] = "ButtonTech_CompositeBow";
    actionResourceMap[ACT_RANGE_CREATE_CHARIOT_ARCHER] = "Button_ChariotArcher";
    actionResourceMap[ACT_STABLE_CREATE_SCOUT] = "Button_Scout";
    actionResourceMap[ACT_STABLE_CREATE_CAVALRY] = "Button_Cavalry";
    actionResourceMap[ACT_STABLE_CREATE_CHARIOT] = "Button_Chariot";
    actionResourceMap[ACT_DOCK_CREATE_SAILING] = "Button_Sailing";
    actionResourceMap[ACT_DOCK_CREATE_WOOD_BOAT] = "Button_Wood_Boat";
    actionResourceMap[ACT_DOCK_CREATE_SHIP] = "Button_Ship";
    actionResourceMap[ACT_SIEGE_CREATE_STONE_THROWER] = "Button_StoneThrower";
    actionResourceMap[ACT_STONE_THROWER_PINPOINT_STRIKE] = "Button_PinPointStrike";
    actionResourceMap[ACT_STONE_THROWER_CANCEL_PINPOINT_STRIKE] = "Exit";
    actionResourceMap[ACT_BUILD] = "Button_Build";
    // 默认图标；若已完成谷仓箭塔强化且存在 Button_ArrowTower2_Egypt，则由 drawActs 优先选用
    actionResourceMap[ACT_BUILD_ARROWTOWER] = "Button_ArrowTower";
    actionResourceMap[ACT_BUILD_CANCEL] = "Exit";
//    actionResourceMap[ACT_BUILD_FARM] = "Button_Farm";
//    actionResourceMap[ACT_BUILD_GRANARY] = "Button_Granary";
//    actionResourceMap[ACT_BUILD_HOUSE] = "Button_House1";
//    actionResourceMap[ACT_BUILD_MARKET] = "Button_Market";
//    actionResourceMap[ACT_BUILD_STOCK] = "Button_Stock";
//    actionResourceMap[ACT_BUILD_ARMYCAMP] = "Button_ArmyCamp";
//    actionResourceMap[ACT_BUILD_RANGE] = "Button_Range";
//    actionResourceMap[ACT_BUILD_STABLE] = "Button_Stable";
//    actionResourceMap[ACT_BUILD_DOCK] = "Button_Dock";
//    actionResourceMap[ACT_SHIP_LAY] = "Button_Lay";
//    actionResourceMap[ACT_BUILD_COLLAGE] = "Button_Collage_Egypt";
//    actionResourceMap[ACT_BUILD_SIEGE] = "Button_Siege_Egypt";
}

SelectWidget::~SelectWidget()
{
    delete ui;
}

// 时间相关
QString SelectWidget::getShowTime()
{
    QString minute, second, millSecond;
    // 分钟
    if (elapsedSec / 60 < 10)
        minute = "0" + QString::number(elapsedSec / 60);
    else
        minute = QString::number(elapsedSec / 60);
    // 秒
    if (elapsedSec % 60 < 10)
        second = "0" + QString::number(elapsedSec % 60);
    else
        second = QString::number(elapsedSec % 60);
    // 毫秒
    if (elapsedFrame < 10)
        millSecond = "0" + QString::number(elapsedFrame);
    else
        millSecond = QString::number(elapsedFrame);
    return (minute + ":" + second + ":" + millSecond);
}

void SelectWidget::paintEvent(QPaintEvent* event)
{
    if (nowobject != NULL)
    {
        QPainter painter(this);
        painter.drawRect(0, 0, 270, 170);
        painter.fillRect(QRect(0, 0, 270, 170), QBrush(QColor(Qt::black)));
        if (true)
        {
            painter.drawRect(130, 90, 120, 20);

            BloodHaver* bloodobject = NULL;
            Animal* animalObject = NULL;
            nowobject->printer_ToBloodHaver((void**)&bloodobject);
            nowobject->printer_ToAnimal((void**)&animalObject);
            if ((bloodobject != NULL && animalObject == NULL) || (animalObject != NULL && !animalObject->get_Gatherable()))
            { // 如果有血条的对象，绘制血条
                int StartX = 130, StartY = 110;
                int percent = 0;

                // 修改percent
                percent = bloodobject->getBloodPercent() * 100;

                if (percent > 100)
                    percent = 100;
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

    // 根据点击的对象初始化行动数组
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
        actionStatus[i] = ACT_STATUS_ENABLED; // 重置窗口状态为可用，真正判断是否可用的代码在refreshActs
    }

    if (type == SORT_BUILDING) // 建筑
    {
        Building* buildOb = (Building*)nowobject;
        bool isActing = buildOb->isFinish() && buildOb->getActSpeed() > Double::Zero();
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
                // 新增判断是否显示
                if (mainPtr->player[0]->get_isBuildActionShowAble(buildOb->getNum(), buildOb->ActNameToActNum(buildOb->getActNames(i))))
                {
                    actions[i] = ((Building*)nowobject)->getActNames(i); // getActNames是Building特有的,用来获取action数组
                    actionStatus[i] = ((Building*)nowobject)->getActStatus(i);
                }
                else
                    actions[i] = ACT_NULL;
            }
        }
    }
    else if (type == SORT_FARMER) // 人类
    {
        Farmer* human = (Farmer*)nowobject;
        if (!human->isShip())
            actions[0] = ACT_BUILD;
        if (human->isShip() && human->get_farmerType() == 1)
        {
            actions[0] = ACT_SHIP_LAY;
        }
        for (int i = 1; i < ACT_WINDOW_NUM_FREE; i++)
        {
            actionStatus[i] = ACT_STATUS_DISABLED;
        }
    }
    else if (type == SORT_ARMY) // 军队单位
    {
        Army* objArmy = (Army*)nowobject;
        // 如果是投石车
        if (objArmy->getNum() == AT_STONE_THROWER)
        {
            // 检查是否正在等待顶点投射
            if (g_mainWidget && g_mainWidget->isWaitingForPinPointStrike() &&
                g_mainWidget->getPinPointStrikeUnit() == nowobject)
            {
                // 如果正在等待，显示取消按钮
                actions[0] = ACT_STONE_THROWER_CANCEL_PINPOINT_STRIKE;
                actionStatus[0] = ACT_STATUS_ENABLED;
            }
            else
            {
                // 否则显示顶点投射按钮
                actions[0] = ACT_STONE_THROWER_PINPOINT_STRIKE;
                actionStatus[0] = ACT_STATUS_ENABLED;
            }
            for (int i = 1; i < ACT_WINDOW_NUM_FREE; i++)
            {
                actions[i] = ACT_NULL;
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
    // 更新行动的逻辑：点击一个对象后调用一次initActs函数根据类型给行动数组赋值,人物直接赋值，建筑通过getActNames赋值
    // refreshActs函数：每帧执行，仅根据行动id执行对应的判断条件决定是否可以执行(对于点击按钮手动操作游戏)，不再像以前重复刷新行动数组的具体行动。以及刷新本窗口的对象信息显示
    // 执行了doActs之后，新的行动数组通过updateActs更新
    // 上面更新的内容都是int数组，然后initActs和updateActs最后再调用一个函数(drawActs)根据数组的值贴上图片,refreshActs只控制图像是否为灰色

    // ai调用行动后具体的条件判断还是在doActs里
    // 同时doActs也包括更新新actions数组
    // updateActs改为每帧刷新判断所有建筑的行动数组
    // updateActs只刷新建筑的行动，人物行动不刷新，人物的行动只受initActs和doActs的影响，是否为灰仍由refreshActs判断
    // drawActs就根据actions数组的内容给ActWidget窗口贴上对应图片

    // 留的问题：
    // updateActs里面setActStatus的一系列判断条件都也要写到(复制，两个地方都要有，updateActs更新建筑的action,actionStatus数组要用于手操按钮游玩时判定是否可以执行行动)doActs的对应行动里面(用于拒绝ai无效命令)
    bool isBuild, isBuildingAct;
    int buildType, buildingActType;
    if (nowobject != NULL)
    {
        if (nowobject->getActSpeed() > Double::Zero())
            ui->objText->setText(QString::number((int)(nowobject->getActPercent())) + "%"); // 如果有进行中的任务则显示进度
        else
            ui->objText->setText("");

        ui->objIcon->setPixmap(QPixmap());
        ui->objIconSmall->setPixmap(QPixmap());
        ui->objHp->setText("");
        ui->objText_ATK->setText("");
        ui->objIconSmall_ATK->setPixmap(QPixmap());
        ui->objText_DEF_melee->setText("");
        ui->objIconSmall_DEF_melee->setPixmap(QPixmap());
        ui->objText_DEF_range->setText("");
        ui->objIconSmall_DEF_range->setPixmap(QPixmap());
    } // 行动的进度

    // 人口当前数量、建筑情况等，直接遍历
    // 计算一些参数
    std::list<Building*>::iterator buildIt = mainPtr->player[0]->build.begin(), buildItTemp = buildIt;
    std::list<Human*>::iterator humanIt = mainPtr->player[0]->human.begin(), humanItTemp = humanIt;

    // 当前人口
    human_num = mainPtr->player[0]->getHumanNum();
    // 当前人口上限
    build_hold_human_num = mainPtr->player[0]->getMaxHumanNum();

    // 建成谷仓
    isGranaryBuilt = mainPtr->player[0]->get_isBuildingHaveBuild(BUILDING_GRANARY);
    // 建成市场
    isMarketBuilt = mainPtr->player[0]->get_isBuildingHaveBuild(BUILDING_MARKET);
    isStockBuilt = mainPtr->player[0]->get_isBuildingHaveBuild(BUILDING_STOCK);

    // 先进行行动状态更新(根据actions决定actionStatus数组)
    for (int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
    {
        isBuild = false, isBuildingAct = false;

        switch (actions[i]) // 根据actions数组的值（行动类别）判断可用性
        {
            // 建造
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
        case ACT_BUILD_COLLAGE:
            isBuild = true;
            buildType = BUILDING_COLLAGE;
            break;
        case ACT_BUILD_SIEGE:
            isBuild = true;
            buildType = BUILDING_SIEGE;
            break;

            // 建筑行动
        case ACT_CREATEFARMER:
            isBuildingAct = true;
            buildType = BUILDING_CENTER;
            buildingActType = BUILDING_CENTER_CREATEFARMER;
            break;
        case ACT_UPGRADE_AGE:
        case ACT_UPGRADE_BRONZEAGE:
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
        case ACT_UPGRADE_CRAFT:
            isBuildingAct = true;
            buildType = BUILDING_MARKET;
            buildingActType = BUILDING_MARKET_WOOD_UPGRADE;  // 使用同一个建筑动作ID
            break;
        case ACT_UPGRADE_PLOW:
            isBuildingAct = true;
            buildType = BUILDING_MARKET;
            buildingActType = BUILDING_MARKET_FARM_UPGRADE;  // 使用同一个建筑动作ID
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
        case ACT_SIEGE_CREATE_STONE_THROWER:
            isBuildingAct = true;
            buildType = BUILDING_SIEGE;
            buildingActType = BUILDING_SIEGE_CREATE_STONE_THROWER;
            break;
        case ACT_COLLAGE_CREATE_HOPLITE:
                   isBuildingAct = true;
                   buildType = BUILDING_COLLAGE;
                   buildingActType = BUILDING_COLLAGE_CREATE_HOPLITE;
                   break;
        case ACT_STABLE_CREATE_CHARIOT:
            isBuildingAct = true;
            buildType = BUILDING_STABLE;
            buildingActType = BUILDING_STABLE_CREATE_CHARIOT;
            break;

        case ACT_RANGE_CREATE_CHARIOT_ARCHER:
            isBuildingAct = true;
            buildType = BUILDING_RANGE;
            buildingActType = BUILDING_RANGE_CREATE_CHARIOT_ARCHER;
            break;

        case ACT_RANGE_CREATE_COMPOSITE_BOWMAN:
            isBuildingAct = true;
            buildType = BUILDING_RANGE;
            buildingActType = BUILDING_RANGE_CREATE_COMPOSITE_BOWMAN;
            break;

        case ACT_RANGE_UPGRADE_COMPOSITE_BOW:
            isBuildingAct = true;
            buildType = BUILDING_RANGE;
            buildingActType = BUILDING_RANGE_UPGRADE_COMPOSITE_BOW;
            break;

        case ACT_ARMYCAMP_CREATE_BROADSWORD:
            isBuildingAct = true;
            buildType = BUILDING_ARMYCAMP;
            buildingActType = BUILDING_ARMYCAMP_CREATE_BROADSWORD;
            break;

        case ACT_ARMYCAMP_UPGRADE_BROADSWORD:
            isBuildingAct = true;
            buildType = BUILDING_ARMYCAMP;
            buildingActType = BUILDING_ARMYCAMP_UPGRADE_BROADSWORD;
            break;

        case ACT_ARMYCAMP_RESEARCH_LOGISTICS:
            isBuildingAct = true;
            buildType = BUILDING_ARMYCAMP;
            buildingActType = BUILDING_ARMYCAMP_RESEARCH_LOGISTICS;
            break;

        case ACT_STOCK_UPGRADE_MISSILE_DEFENSE_INFANTRY:
            isBuildingAct = true;
            buildType = BUILDING_STOCK;
            buildingActType = BUILDING_STOCK_UPGRADE_MISSILE_DEFENSE_INFANTRY;
            break;

        case ACT_STABLE_CREATE_CAVALRY:
            isBuildingAct = true;
            buildType = BUILDING_STABLE;
            buildingActType = BUILDING_STABLE_CREATE_CAVALRY;
            break;

        case ACT_STOCK_UPGRADE_METALWORKING:
            isBuildingAct = true;
            buildType = BUILDING_STOCK;
            buildingActType = BUILDING_STOCK_UPGRADE_USETOOL;  // 使用同一个建筑动作ID
            break;
        case ACT_STOCK_UPGRADE_DEFENSE_INFANTRY_SCALE:
            isBuildingAct = true;
            buildType = BUILDING_STOCK;
            buildingActType = BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY;  // 使用同一个建筑动作ID
            break;
        case ACT_STOCK_UPGRADE_DEFENSE_ARCHER_SCALE:
            isBuildingAct = true;
            buildType = BUILDING_STOCK;
            buildingActType = BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER;  // 使用同一个建筑动作ID
            break;
        case ACT_STOCK_UPGRADE_DEFENSE_RIDER_SCALE:
            isBuildingAct = true;
            buildType = BUILDING_STOCK;
            buildingActType = BUILDING_STOCK_UPGRADE_DEFENSE_RIDER;  // 使用同一个建筑动作ID
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
            if (!mainPtr->player[0]->get_isBuildingShowAble(buildType) || !mainPtr->player[0]->get_isBuildingAble(buildType) || RuntimeConfig_isPlayerBuildingDisabled(buildType))
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
        else
        {
            actionStatus[i] = ACT_STATUS_ENABLED;
        }
    }

    for (int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
    {
        mainPtr->getActs(i)->setStatus(actionStatus[i]); // 应用行动状态,mainPtr->getActs(i)即获取第i个按钮窗口
        mainPtr->getActs(i)->update();                   // 刷新按钮显示状态
    }

    if (secondWidget_Build)
    {
        showBuildActLab();
    }

    // 再进行快捷栏和状态栏显示更新(本窗口的内容)
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
        int type = nowobject->getSort();                             // 获取当前对象类型
        if (type == SORT_BUILDING || type == SORT_Building_Resource) // 建筑
        {
            Building* objBuilding = (Building*)nowobject;
            bool isActing = objBuilding->isFinish() && objBuilding->getActSpeed() > Double::Zero();
            buildType = objBuilding->getNum();                                                    // 获取建筑种类
            ui->objName->setText(QString::fromStdString(objBuilding->getDisplayName(buildType))); // 设置显示名称

            // 根据不同时代设置不同的图标
            // 判断是否为敌方建筑
            int isEnemy = (objBuilding->getPlayerRepresent() != NOWPLAYERREPRESENT) ? 1 : 0;
            string buildingName = Building::getBuiltname(mainPtr->player[0]->getCiv(), isEnemy, buildType);
            string name = "Button_" + buildingName;
            if (resMap.find(name) != resMap.end() && !resMap[name].empty())
            {
                // 如果找到了对应的图片，设置该图片
                ui->objIcon->setPixmap(resMap[name].front().scaled(110, 110));
            }
            else if (resMap.find(buildingName) != resMap.end() && !resMap[buildingName].empty())
            {
                // 没有专用 Button_ 图标时，回退到当前时代、当前阵营的建筑本体图片。
                ui->objIcon->setPixmap(resMap[buildingName].front().scaled(
                    110, 110, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            }
            else
            {
                // 如果没有找到对应的图片，使用默认图片
                ui->objIcon->setPixmap(resMap["Button"].front().scaled(110, 110));
            }

            if (objBuilding->getActSpeed() != Double::Zero())
            {
                ui->objIconSmall->setPixmap(resMap["SmallIcon_Sandglass"].front()); // 如果在行动中，在进度条百分比旁显示一个沙漏小图标
            }
            else
            {
                ui->objIconSmall->setPixmap(QPixmap());
            }

            // 设置hp
            ui->objHp->setText(QString::number(objBuilding->getBlood()) + "/" + QString::number(objBuilding->getMaxBlood()));

            // 同步行动状态至窗口
            if (objBuilding->isFinish())
            {
                // 后续简化代码
                if (isActing)
                {
                    actions[0] = ACT_STOP;
                    actionStatus[0] = ACT_STATUS_ENABLED;
                    for (int i = 1; i < ACT_WINDOW_NUM_FREE; i++)
                        actions[i] = ACT_NULL;
                }
                else
                {
                    for (int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
                    {
                        // 新增判断是否显示
                        if (mainPtr->player[0]->get_isBuildActionShowAble(objBuilding->getNum(), objBuilding->ActNameToActNum(objBuilding->getActNames(i))))
                        {
                            actions[i] = objBuilding->getActNames(i); // getActNames是Building特有的,用来获取action数组
                            actionStatus[i] = objBuilding->getActStatus(i);
                        }
                        else
                            actions[i] = ACT_NULL;
                    }
                }

                // 统一根据内置行动数组设置
                for (int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
                {
                    if (actions[i] != ACT_NULL)
                        mainPtr->getActs(i)->show();
                    else
                        mainPtr->getActs(i)->hide();
                }

                // 详细设置
                if (objBuilding->getNum() == BUILDING_HOME)
                {
                    const int populationHalfSlots = round(human_num * Double(2));
                    const QString populationText = (populationHalfSlots % 2 == 0)
                        ? QString::number(populationHalfSlots / 2)
                        : QString::number(human_num, 'f', 1);
                    ui->objText->setText(populationText + "/" + QString::number(build_hold_human_num));
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
                else if (objBuilding->getNum() == BUILDING_ARROWTOWER)
                {
                    // 箭塔：攻击 / 射程两行；有加成时 "base+bonus"（如图：剑 + "3+1"，靶 + "6+1"）
                    auto pickSmallIconKey = [](const char* a, const char* b, const char* c) -> const char*
                    {
                        for (const char* key : {a, b, c})
                        {
                            if (resMap.find(key) != resMap.end() && !resMap[key].empty())
                                return key;
                        }
                        return nullptr;
                    };

                    const char* atkIconKey = pickSmallIconKey("SmallIcon_Sword", "SmallIcon_Melee", "SmallIcon_Attack");
                    if (atkIconKey != nullptr)
                        ui->objIconSmall_ATK->setPixmap(resMap[atkIconKey].front().scaled(40, 30));
                    {
                        int ab = objBuilding->showATK_Basic();
                        int aa = objBuilding->showATK_Addition();
                        if (aa > 0)
                            ui->objText_ATK->setText(QString::number(ab) + "+" + QString::number(aa));
                        else
                            ui->objText_ATK->setText(QString::number(ab + aa));
                    }

                    const char* rangeIconKey = pickSmallIconKey("SmallIcon_Target", "SmallIcon_Range", "SmallIcon_Defense_Range");
                    if (rangeIconKey != nullptr)
                        ui->objIconSmall_DEF_melee->setPixmap(resMap[rangeIconKey].front().scaled(40, 30));
                    {
                        int rb = objBuilding->showArrowTowerRangeBaseBlocks();
                        int rbonus = objBuilding->showArrowTowerRangeBonusBlocks();
                        if (rbonus > 0)
                            ui->objText_DEF_melee->setText(QString::number(rb) + "+" + QString::number(rbonus));
                        else
                            ui->objText_DEF_melee->setText(QString::number(rb));
                    }

                    ui->objText->setText("");
                    ui->objIconSmall->setPixmap(QPixmap());
                    ui->objText_DEF_range->setText("");
                    ui->objIconSmall_DEF_range->setPixmap(QPixmap());
                }
            }
            else // 如果建筑建造未完成
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
            this->show(); // 必要的update和show
        }
        else if (type == SORT_STATICRES) // 块资源
        {
            StaticRes* objStaticRes = (StaticRes*)(nowobject);
            int num = objStaticRes->getNum();
            ui->objName->setText(QString::fromStdString(objStaticRes->getStaticResDisplayName(num)));
            ui->objHp->setText("");
            // 根据不同时代设置不同的图标
            string name = "Button_" + objStaticRes->getStaticResName(num);
            if (resMap.find(name) != resMap.end() && !resMap[name].empty())
            {
                // 如果找到了对应的图片，设置该图片
                ui->objIcon->setPixmap(resMap[name].front().scaled(110, 110));
            }
            else
            {
                // 如果没有找到对应的图片，使用默认图片
                ui->objIcon->setPixmap(resMap["Button"].front().scaled(110, 110));
            }
            //            ui->objIcon->setPixmap(resMap["Button_"+objStaticRes->getStaticResName(num)].front().scaled(80,80));
            ui->objIconSmall->setPixmap(QPixmap());

            if (objStaticRes->get_ResourceSort() == HUMAN_GRANARYFOOD || objStaticRes->get_ResourceSort() == HUMAN_STOCKFOOD || objStaticRes->get_ResourceSort() == HUMAN_DOCKFOOD)
                ui->objIconSmall->setPixmap(resMap["Icon_Food"].front());
            else if (objStaticRes->get_ResourceSort() == HUMAN_STONE)
                ui->objIconSmall->setPixmap(resMap["Icon_Stone"].front());
            else if (objStaticRes->get_ResourceSort() == HUMAN_GOLD)
                ui->objIconSmall->setPixmap(resMap["Icon_Gold"].front());

            ui->objText->setText(QString::number(objStaticRes->get_Cnt()));
            this->update();
            this->show();
        }
        else if (type == SORT_FARMER) // 村民
        {                             // objIconSmall_ATK objText_ATK用于展示攻击力 objIconSmall objText表示携带资源 objIconSmall_DEF和objText_DEF表示防御（近战和远程分开）
            Farmer* objFarmer = (Farmer*)(nowobject);
            int num = objFarmer->getState(); // 获取工作状态并显示对应名称
            QString name;

            switch (objFarmer->get_farmerType())
            {
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
                if (objFarmer->showATK_Addition() == 0)
                    ui->objText_ATK->setText(QString::number(objFarmer->showATK_Basic()));
                else
                    ui->objText_ATK->setText(QString::number(objFarmer->showATK_Basic()) + "+" + QString::number(objFarmer->showATK_Addition())); // 显示攻击力（基础+额外）
            }

            if (objFarmer->getResourceSort() == HUMAN_WOOD)
                ui->objIconSmall->setPixmap(resMap["Icon_Wood"].front());
            else if (objFarmer->getResourceSort() == HUMAN_GRANARYFOOD || objFarmer->getResourceSort() == HUMAN_STOCKFOOD || objFarmer->getResourceSort() == HUMAN_DOCKFOOD)
                ui->objIconSmall->setPixmap(resMap["Icon_Food"].front());
            else if (objFarmer->getResourceSort() == HUMAN_STONE)
                ui->objIconSmall->setPixmap(resMap["Icon_Stone"].front());
            // 运输船显示运载人口
            else if (objFarmer->get_farmerType() == 1)
                ui->objIconSmall->setPixmap(resMap["SmallIcon_People"].front());
            // 如果当前村民没有资源
            if (objFarmer->getResourceNowHave() == Double::Zero())
            {
                ui->objIconSmall->setPixmap(QPixmap());
                ui->objText->setText("");
            }
            else if (objFarmer->get_farmerType() == FARMERTYPE_WOOD_BOAT)
            {
                ui->objText->setText(QString::number((int)(objFarmer->getResourceNowHave())) + "/5 ");
            }
            else if (objFarmer->get_farmerType() == FARMERTYPE_SAILING)
            {
                ui->objText->setText(QString::number((int)(objFarmer->getResourceNowHave())) + "/15 ");
            }
            else
            {
                ui->objText->setText(QString::number((int)(objFarmer->getResourceNowHave())));
            }

            // 设置血量
            ui->objHp->setText(QString::number(objFarmer->getBlood()) + "/" + QString::number(objFarmer->getMaxBlood()));
            this->update();
            this->show();
        }
        else if (type == SORT_ARMY)
        {
            // objIconSmall_ATK objText_ATK用于展示攻击力 objIconSmall objText表示携带资源或者防御
            Army* objArmy = (Army*)nowobject;
            ui->objName->setText(objArmy->getChineseName());
            std::string buttonName = "Button_" + objArmy->getArmyName(objArmy->getNum(), objArmy->getLevel());
            ui->objIcon->setPixmap(resMap[buttonName].front().scaled(110, 110));
            ui->objIconSmall_ATK->setPixmap(resMap["SmallIcon_Attack"].front().scaled(40, 30)); // 攻击图标
            ui->objIconSmall_DEF_melee->setPixmap(resMap["SmallIcon_Defense_Melee"].front().scaled(40, 30));

            // 判断是否是弓兵类型（远程攻击类型）
            bool isRangedUnit = (objArmy->get_AttackType() == ATTACKTYPE_SHOOT);

            if (objArmy->showATK_Addition() == 0)
                ui->objText_ATK->setText(QString::number(objArmy->showATK_Basic()));
            else
                ui->objText_ATK->setText(QString::number(objArmy->showATK_Basic()) + "+" + QString::number(objArmy->showATK_Addition())); // 显示攻击力（基础+额外）
            if (objArmy->showDEF_Close_Addition() == 0)
                ui->objText_DEF_melee->setText(QString::number(objArmy->showDEF_Close()));
            else
                ui->objText_DEF_melee->setText(QString::number(objArmy->showDEF_Close()) + "+" + QString::number(objArmy->showDEF_Close_Addition())); // 显示近战防御（基础+额外）

            // 第三行：弓兵类型显示射程，其他兵种显示远程防御
            if (isRangedUnit)
            {
                // 弓兵类型：显示射程
                // Army::getDis_attack() 返回的是距离 * BLOCKSIDELENGTH（像素单位），这里转回"格子"显示
                int rangeInBlock = 0;
                if (BLOCKSIDELENGTH > Double::Zero())
                    rangeInBlock = (int)(objArmy->getDis_attack() / BLOCKSIDELENGTH);
                ui->objText_DEF_range->setText(QString::number(rangeInBlock));

                // 射程图标：使用 SmallIcon_Range
                if (resMap.find("SmallIcon_Range") != resMap.end() && !resMap["SmallIcon_Range"].empty())
                    ui->objIconSmall_DEF_range->setPixmap(resMap["SmallIcon_Range"].front().scaled(40, 30));
            }
            else
            {
                // 非弓兵类型：显示远程防御
                if (objArmy->showDEF_Shoot_Addition() == 0)
                    ui->objText_DEF_range->setText(QString::number(objArmy->showDEF_Shoot()));
                else
                    ui->objText_DEF_range->setText(QString::number(objArmy->showDEF_Shoot()) + "+" + QString::number(objArmy->showDEF_Shoot_Addition())); // 显示远程防御（基础+额外）
                ui->objIconSmall_DEF_range->setPixmap(resMap["SmallIcon_Defense_Range"].front().scaled(40, 30));
            }

            // 设置血量
            ui->objHp->setText(QString::number(objArmy->getBlood()) + "/" + QString::number(objArmy->getMaxBlood()));
            this->update();
        }
        else if (type == SORT_ANIMAL) // 动物
        {
            Animal* objAnimal = (Animal*)(nowobject);
            ui->objName->setText(QString::fromStdString(Animal::getAnimalDisplayName(objAnimal->getNum())));

            if (objAnimal->getNum() == 1) // 瞪羚
            {
                ui->objIcon->setPixmap(resMap["Button_Gazelle"].front().scaled(110, 110));
            }
            else if (objAnimal->getNum() == 2) // 大象
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

            // 动物的可采集资源信息
            // 显示动物可采集资源数
            ui->objText->setText(QString::number((int)(objAnimal->get_Cnt())));
            // 显示动物的采集资源类型图片
            switch (objAnimal->get_ResourceSort())
            {
            case HUMAN_WOOD:
                ui->objIconSmall->setPixmap(resMap["Icon_Wood"].front());
                break;
            case HUMAN_STOCKFOOD:
                ui->objIconSmall->setPixmap(resMap["Icon_Food"].front());
                break;
            default:
                break;
            }

            // 死亡后，不显示血量
            if (objAnimal->isDie())
                ui->objHp->setText(""); // 原代码意思是当动物死亡后变为收集的资源时，不显示血条，现在可能有新的设计
            else
                ui->objHp->setText(QString::number(objAnimal->getBlood()) + "/" + QString::number(objAnimal->getMaxBlood()));

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
    if (mainPtr->getActs(num)->getStatus() == ACT_STATUS_DISABLED)
        return;
    int actName = actions[num];
    doActs(actName);
}

int SelectWidget::aiAct(int actName, Coordinate* self)
{
    return doActs(actName, self);
}

void SelectWidget::manageBuildBottom(int position, int actNum, int buildingNum)
{
    if (mainPtr->player[0]->get_isBuildingShowAble(buildingNum) && !RuntimeConfig_isPlayerBuildingDisabled(buildingNum))
    {
        actions[position] = actNum;
        if (mainPtr->player[0]->get_isBuildingAble(buildingNum))
            actionStatus[position] = ACT_STATUS_ENABLED;
        else
            actionStatus[position] = ACT_STATUS_DISABLED;
    }
    else
        actions[position] = ACT_NULL;
}
int SelectWidget::doActs(int actName, Coordinate* nowobject)
{
    if (nowobject == nullptr)
        return ACTION_INVALID_ACTION;
    // Helper function to handle cursor and build mode changes
    auto setCursorAndBuildMode = [&](const QString& resourceKey, int buildMode)
        {
            QApplication::restoreOverrideCursor();
            QCursor cursor(resMap[resourceKey.toStdString()].front());
            QApplication::setOverrideCursor(cursor);
            emit sendBuildMode(buildMode);
        };
    // 根据当前时代(civ)为"建造建筑"动作选择光标资源（使用我方风格）
    auto setCursorAndBuildModeByBuilding = [&](int buildingNum, const QString& fallbackKey)
        {
            int civ = mainPtr->player[0]->getCiv();
            if (civ < 1 || civ > 3) civ = CIVILIZATION_STONEAGE;
            std::string base = Building::getBuiltname(civ, 0, buildingNum);
            QString key = base.empty() ? fallbackKey : QString::fromStdString(base);

            // 资源不存在则回退到 fallbackKey，避免空资源导致崩溃
            auto it = resMap.find(key.toStdString());
            if (it == resMap.end() || it->second.empty())
                key = fallbackKey;

            setCursorAndBuildMode(key, buildingNum);
        };
    auto rejectDisabledBuilding = [&](int buildingNum) -> bool
        {
            if (RuntimeConfig_isPlayerBuildingDisabled(buildingNum)) {
                QString chineseName = QString::fromStdString(Building::getDisplayName(buildingNum));
                call_debugText("red", " 建造" + chineseName + " 建造失败,该建筑已被禁用", 0);
                return true;
            }
            return false;
        };
    // Handle building actions
    switch (actName)
    {
    case ACT_BUILD:
        secondWidget_Build = true;
        showBuildActLab();
        break;
    case ACT_BUILD_HOUSE:
        if (rejectDisabledBuilding(BUILDING_HOME)) break;
        setCursorAndBuildModeByBuilding(BUILDING_HOME, "House1");
        break;
    case ACT_BUILD_GRANARY:
        if (rejectDisabledBuilding(BUILDING_GRANARY)) break;
        setCursorAndBuildModeByBuilding(BUILDING_GRANARY, "Granary");
        break;
    case ACT_BUILD_STOCK:
        if (rejectDisabledBuilding(BUILDING_STOCK)) break;
        setCursorAndBuildModeByBuilding(BUILDING_STOCK, "Stock");
        break;
    case ACT_BUILD_FARM:
        if (rejectDisabledBuilding(BUILDING_FARM)) break;
        setCursorAndBuildModeByBuilding(BUILDING_FARM, "Farm");
        break;
    case ACT_BUILD_MARKET:
        if (rejectDisabledBuilding(BUILDING_MARKET)) break;
        setCursorAndBuildModeByBuilding(BUILDING_MARKET, "Market");
        break;
    case ACT_BUILD_ARROWTOWER:
        if (rejectDisabledBuilding(BUILDING_ARROWTOWER)) break;
        if (mainPtr->player[0]->get_buildActLevel(BUILDING_GRANARY, BUILDING_GRANARY_ARROWTOWE_UPGRADE) >= 1)
        {
            const QString upKey("ArrowTower2_Egypt");
            auto it = resMap.find(upKey.toStdString());
            if (it != resMap.end() && !it->second.empty())
            {
                setCursorAndBuildMode(upKey, BUILDING_ARROWTOWER);
                break;
            }
        }
        setCursorAndBuildModeByBuilding(BUILDING_ARROWTOWER, "ArrowTower");
        break;
    case ACT_BUILD_ARMYCAMP:
        if (rejectDisabledBuilding(BUILDING_ARMYCAMP)) break;
        setCursorAndBuildModeByBuilding(BUILDING_ARMYCAMP, "ArmyCamp");
        break;
    case ACT_BUILD_RANGE:
        if (rejectDisabledBuilding(BUILDING_RANGE)) break;
        setCursorAndBuildModeByBuilding(BUILDING_RANGE, "Range");
        break;
    case ACT_BUILD_STABLE:
        if (rejectDisabledBuilding(BUILDING_STABLE)) break;
        setCursorAndBuildModeByBuilding(BUILDING_STABLE, "Stable");
        break;
    case ACT_BUILD_DOCK:
        if (rejectDisabledBuilding(BUILDING_DOCK)) break;
        setCursorAndBuildModeByBuilding(BUILDING_DOCK, "Dock");
        break;
    case ACT_BUILD_COLLAGE:
        if (rejectDisabledBuilding(BUILDING_COLLAGE)) break;
        setCursorAndBuildModeByBuilding(BUILDING_COLLAGE, "Collage_Egypt");
        break;
    case ACT_BUILD_SIEGE:
        if (rejectDisabledBuilding(BUILDING_SIEGE)) break;
        setCursorAndBuildModeByBuilding(BUILDING_SIEGE, "Siege_Egypt");
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
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_CENTER_CREATEFARMER);
        break;
    case ACT_UPGRADE_AGE:
    case ACT_UPGRADE_BRONZEAGE:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_CENTER_UPGRADE);
        // if (mainPtr->player[0]->get_civiBuild_Times(mainPtr->player[0]->getCiv()) >= 2) 这个条件在内核满足就行，这里不需要给
        break;
    case ACT_UPGRADE_TOWERBUILD:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_GRANARY_ARROWTOWER);
        break;
    case ACT_UPGRADE_ARROWTOWER:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_GRANARY_ARROWTOWE_UPGRADE);
        break;
    case ACT_UPGRADE_WOOD:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_MARKET_WOOD_UPGRADE);
        break;
    case ACT_UPGRADE_STONE:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_MARKET_STONE_UPGRADE);
        break;
    case ACT_UPGRADE_GOLD:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_MARKET_GOLD_UPGRADE);
        break;
    case ACT_UPGRADE_FARM:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_MARKET_FARM_UPGRADE);
        break;
    case ACT_UPGRADE_WHEEL:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_MARKET_WHEEL_UPGRADE);
        break;
    case ACT_UPGRADE_CRAFT:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_MARKET_WOOD_UPGRADE);
        break;
    case ACT_UPGRADE_PLOW:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_MARKET_FARM_UPGRADE);
        break;
    case ACT_STOCK_UPGRADE_USETOOL:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_STOCK_UPGRADE_USETOOL);
        break;
    case ACT_STOCK_UPGRADE_DEFENSE_INFANTRY:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY);
        break;
    case ACT_STOCK_UPGRADE_DEFENSE_ARCHER:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER);
        break;
    case ACT_STOCK_UPGRADE_DEFENSE_RIDER:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_STOCK_UPGRADE_DEFENSE_RIDER);
        break;
    case ACT_STOCK_UPGRADE_METALWORKING:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_STOCK_UPGRADE_USETOOL);
        break;
    case ACT_STOCK_UPGRADE_DEFENSE_INFANTRY_SCALE:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY);
        break;
    case ACT_STOCK_UPGRADE_DEFENSE_ARCHER_SCALE:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER);
        break;
    case ACT_STOCK_UPGRADE_DEFENSE_RIDER_SCALE:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_STOCK_UPGRADE_DEFENSE_RIDER);
        break;
    case ACT_STOCK_UPGRADE_MISSILE_DEFENSE_INFANTRY:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_STOCK_UPGRADE_MISSILE_DEFENSE_INFANTRY);
        break;
    case ACT_ARMYCAMP_CREATE_CLUBMAN:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_ARMYCAMP_CREATE_CLUBMAN);
        break;
    case ACT_ARMYCAMP_UPGRADE_CLUBMAN:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_ARMYCAMP_UPGRADE_CLUBMAN);
        break;
    case ACT_ARMYCAMP_CREATE_BROADSWORD:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_ARMYCAMP_CREATE_BROADSWORD);
        break;
    case ACT_ARMYCAMP_UPGRADE_BROADSWORD:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_ARMYCAMP_UPGRADE_BROADSWORD);
        break;
    case ACT_ARMYCAMP_CREATE_SLINGER:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_ARMYCAMP_CREATE_SLINGER);
        break;
    case ACT_ARMYCAMP_RESEARCH_LOGISTICS:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_ARMYCAMP_RESEARCH_LOGISTICS);
        break;
    case ACT_COLLAGE_CREATE_HOPLITE:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_COLLAGE_CREATE_HOPLITE);
        break;
    case ACT_RANGE_CREATE_BOWMAN:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_RANGE_CREATE_BOWMAN);
        break;
    case ACT_RANGE_CREATE_CHARIOT_ARCHER:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_RANGE_CREATE_CHARIOT_ARCHER);
        break;
    case ACT_RANGE_CREATE_COMPOSITE_BOWMAN:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_RANGE_CREATE_COMPOSITE_BOWMAN);
        break;
    case ACT_RANGE_UPGRADE_COMPOSITE_BOW:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_RANGE_UPGRADE_COMPOSITE_BOW);
        break;
    case ACT_STABLE_CREATE_SCOUT:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_STABLE_CREATE_SCOUT);
        break;
    case ACT_STABLE_CREATE_CAVALRY:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_STABLE_CREATE_CAVALRY);
        break;
    case ACT_STABLE_CREATE_CHARIOT:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_STABLE_CREATE_CHARIOT);
        break;
    case ACT_DOCK_CREATE_SAILING:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_DOCK_CREATE_SAILING);
        break;
    case ACT_DOCK_CREATE_WOOD_BOAT:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_DOCK_CREATE_WOOD_BOAT);
        break;
    case ACT_DOCK_CREATE_SHIP:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_DOCK_CREATE_SHIP);
        break;
    case ACT_SIEGE_CREATE_STONE_THROWER:
        g_mainWidget->getUsrAI()->BuildingAction(nowobject->getglobalNum(), BUILDING_SIEGE_CREATE_STONE_THROWER);
        break;

    case ACT_STONE_THROWER_PINPOINT_STRIKE:
        // 顶点投射：点击按钮后，等待用户点击地图上的目标位置
        // 设置一个状态，表示正在等待地图点击
        if (nowobject != nullptr && nowobject->getSort() == SORT_ARMY)
        {
            Army* objArmy = (Army*)nowobject;
            if (objArmy->getNum() == AT_STONE_THROWER)
            {
                // 设置等待地图点击的状态，坐标将在manageMouseEvent中获取
                // 这里先设置一个标志，实际坐标获取在Core::manageMouseEvent中处理
                g_mainWidget->setWaitingForPinPointStrike(true);
                g_mainWidget->setPinPointStrikeUnit(nowobject);
                // 刷新按钮显示
                initActs();
            }
        }
        break;

    case ACT_STONE_THROWER_CANCEL_PINPOINT_STRIKE:
        // 取消定点投射：重置等待状态并停止正在进行的投射动作
        if (g_mainWidget && g_mainWidget->isWaitingForPinPointStrike())
        {
            Coordinate* strikeUnit = g_mainWidget->getPinPointStrikeUnit();
            // 如果投石车正在执行投射动作，停止它
            if (strikeUnit != nullptr && nowobject == strikeUnit)
            {
                core->suspendRelation(strikeUnit);
            }
            // 重置等待状态
            g_mainWidget->setWaitingForPinPointStrike(false);
            g_mainWidget->setPinPointStrikeUnit(nullptr);
            // 刷新按钮显示
            initActs();
        }
        // 即使不在等待状态，如果当前选中的投石车正在执行动作，也停止它
        else if (nowobject != nullptr && nowobject->getSort() == SORT_ARMY)
        {
            Army* objArmy = (Army*)nowobject;
            if (objArmy->getNum() == AT_STONE_THROWER)
            {
                core->suspendRelation(nowobject);
                // 刷新按钮显示
                initActs();
            }
        }
        break;

    case ACT_STOP:
        if (nowobject != nullptr)
        {
            core->suspendRelation(nowobject);
            ui->objText->setText("");
        }
        else
        {
            return ACTION_INVALID_ACTION;
        }
        break;

    default:
        return ACTION_INVALID_ACTION;
    }

    // Update all actions
    for (int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
    {
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
    //manageBuildBottom(9, ACT_BUILD_DOCK, BUILDING_DOCK);
    manageBuildBottom(9, ACT_BUILD_COLLAGE, BUILDING_COLLAGE);
    manageBuildBottom(10, ACT_BUILD_SIEGE, BUILDING_SIEGE);

    actions[11] = ACT_BUILD_CANCEL;
    actionStatus[11] = ACT_STATUS_ENABLED;
}

void SelectWidget::updateActs()
{
    // 遍历建筑更新活动列表
    std::list<Building*>::iterator buildIt = mainPtr->player[0]->build.begin();
    int wood = mainPtr->player[0]->getWood();
    int food = mainPtr->player[0]->getFood();
    int stone = mainPtr->player[0]->getStone();
    int gold = mainPtr->player[0]->getGold();

    for (; buildIt != mainPtr->player[0]->build.end(); buildIt++)
    {
        if ((*buildIt)->isFinish())
        {
            // 为每个位置设置act_status,是否显示、显示停止在initact中判断
            (*buildIt)->setActStatus(wood, food, stone, gold);
        }
    }
}

void SelectWidget::drawActs()
{
    QString actionKey;
    int size = 70;

    // 遍历 actions 数组，设置图标
    for (int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
    {
        QPixmap pix;

        // 建造类动作：根据当前时代(civ)自动选择按钮图标 Button_ + Builtname[civ][我方][建筑类型]
        auto setBuildActionKeyIfExists = [&](int buildingNum) -> bool
            {
                int civ = mainPtr->player[0]->getCiv();
                if (civ < 1 || civ > 3) civ = CIVILIZATION_STONEAGE;
                std::string base = Building::getBuiltname(civ, 0, buildingNum); // 建造按钮永远显示我方风格
                if (base.empty()) return false;
                std::string key = "Button_" + base;
                auto it = resMap.find(key);
                if (it != resMap.end() && !it->second.empty())
                {
                    actionKey = QString::fromStdString(key);
                    return true;
                }
                return false;
            };

        bool handled = false;
        switch (actions[i])
        {
        case ACT_BUILD_HOUSE:      handled = setBuildActionKeyIfExists(BUILDING_HOME); break;
        case ACT_BUILD_GRANARY:    handled = setBuildActionKeyIfExists(BUILDING_GRANARY); break;
        case ACT_BUILD_STOCK:      handled = setBuildActionKeyIfExists(BUILDING_STOCK); break;
        case ACT_BUILD_MARKET:     handled = setBuildActionKeyIfExists(BUILDING_MARKET); break;
        case ACT_BUILD_ARROWTOWER:
            if (mainPtr->player[0]->get_buildActLevel(BUILDING_GRANARY, BUILDING_GRANARY_ARROWTOWE_UPGRADE) >= 1)
            {
                const std::string upKey = "Button_ArrowTower2_Egypt";
                auto itUp = resMap.find(upKey);
                if (itUp != resMap.end() && !itUp->second.empty())
                {
                    actionKey = QString::fromStdString(upKey);
                    handled = true;
                }
            }
            if (!handled)
                handled = setBuildActionKeyIfExists(BUILDING_ARROWTOWER);
            break;
        case ACT_BUILD_ARMYCAMP:   handled = setBuildActionKeyIfExists(BUILDING_ARMYCAMP); break;
        case ACT_BUILD_RANGE:      handled = setBuildActionKeyIfExists(BUILDING_RANGE); break;
        case ACT_BUILD_STABLE:     handled = setBuildActionKeyIfExists(BUILDING_STABLE); break;
        case ACT_BUILD_DOCK:       handled = setBuildActionKeyIfExists(BUILDING_DOCK); break;
        case ACT_BUILD_FARM:       handled = setBuildActionKeyIfExists(BUILDING_FARM); break;
        case ACT_BUILD_COLLAGE:    handled = setBuildActionKeyIfExists(BUILDING_COLLAGE); break;
        case ACT_BUILD_SIEGE:      handled = setBuildActionKeyIfExists(BUILDING_SIEGE); break;
        default: break;
        }

        if (!handled && actionResourceMap.contains(actions[i]))
        {
            actionKey = actionResourceMap[actions[i]];
        }
        else if (actions[i] == ACT_ARMYCAMP_CREATE_CLUBMAN)
        {
            // 特殊情况：根据等级决定资源
            switch ((int)mainPtr->player[0]->get_buildActLevel(BUILDING_ARMYCAMP, BUILDING_ARMYCAMP_UPGRADE_CLUBMAN))
            {
            case 0:
                actionKey = "Button_Clubman";
                break;
            case 1:
                actionKey = "Button_Axeman";
                break;
            default:
                actionKey = "Button_Clubman";
                break;
            }
        }
        else if (!handled)
        {
            actionKey = "Button"; // 默认值
        }

        std::string actionKeyStd = actionKey.toStdString();

        // 尝试获取资源，如果找不到则使用默认按钮资源
        if (resMap.find(actionKeyStd) != resMap.end())
        {
            pix = resMap[actionKeyStd].front().scaled(size, size);
        }
        else
        {
            pix = resMap["Button"].front().scaled(size, size);
        }

        mainPtr->getActs(i)->setPix(pix);
        // 设置 actions[i] 为 NULL 时隐藏
        if (actions[i] != ACT_NULL)
        {
            mainPtr->getActs(i)->show();
        }
        else
        {
            mainPtr->getActs(i)->hide();
        }
        mainPtr->getActs(i)->update();
    }
}

// 所有建造
void SelectWidget::getBuild(int BlockL, int BlockU, int num)
{
    core->addRelation(nowobject, BlockL, BlockU, CoreEven_CreatBuilding, true, num);
}
