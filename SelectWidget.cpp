#include "SelectWidget.h"
#include "ui_SelectWidget.h"
#include "MainWidget.h"

SelectWidget::SelectWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SelectWidget)
{
    ui->setupUi(this);
    MainWidget* c=(MainWidget*)this->parentWidget();
    mainPtr = c;
    QPalette pe;
    pe.setColor(QPalette::WindowText,Qt::white);
    ui->objHp->setPalette(pe);
    ui->objName->setPalette(pe);
    ui->objText->setPalette(pe);//设置白色字体
    ui->objText_ATK->setPalette(pe);
    ui->objText_DEF_melee->setPalette(pe);
    ui->objText_DEF_range->setPalette(pe);
}

SelectWidget::~SelectWidget()
{
    delete ui;
}

//时间相关
QString SelectWidget::getShowTime()
{
    QString minute ,second,millSecond;
    //分钟
    if(elapsedSec / 60 < 10) minute = "0" + QString::number(elapsedSec / 60);
    else minute = QString::number(elapsedSec / 60);
    //秒
    if(elapsedSec % 60 < 10) second = "0" + QString::number(elapsedSec % 60);
    else second = QString::number(elapsedSec % 60);
    //毫秒
    if(elapsedFrame < 10) millSecond = "0" + QString::number(elapsedFrame);
    else millSecond = QString::number(elapsedFrame);
    return (minute + ":" + second + ":" + millSecond);
}

void SelectWidget::paintEvent(QPaintEvent *event)
{
    if(nowobject != NULL)
    {
        QPainter painter(this);
        painter.drawRect(0, 0, 270, 170);
        painter.fillRect(QRect(0, 0, 270, 170), QBrush(QColor(Qt::black)));
        if(true){
            painter.drawRect(130, 90, 120, 20);

            BloodHaver* bloodobject = NULL;
            Animal* animalObject = NULL;
            nowobject->printer_ToBloodHaver((void**)&bloodobject);
            nowobject->printer_ToAnimal((void**)&animalObject);
            if((bloodobject!=NULL && animalObject == NULL) || (animalObject!=NULL && !animalObject->get_Gatherable()))
            {//如果有血条的对象，绘制血条
                int StartX = 130, StartY = 110;
                int percent = 0;

                //修改percent
                percent = bloodobject->getBloodPercent()*100;

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

void SelectWidget::frameUpdate()
{
    refreshActs();
    updateActs();
    drawActs();
}

void SelectWidget::initActs()
{
    secondWidget_Build =false;

    //根据点击的对象初始化行动数组
    if(nowobject == NULL)
    {
        for(int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
        {
            actions[i] = ACT_NULL;
            actionStatus[i] = ACT_STATUS_DISABLED;
        }
        return;
    }
    int type = nowobject->getSort();

    for(int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
    {
        actions[i] = ACT_NULL;
        actionStatus[i] = ACT_STATUS_ENABLED;//重置窗口状态为可用，真正判断是否可用的代码在refreshActs
    }

    if(type == SORT_BUILDING)//建筑
    {
        Building* buildOb = (Building *)nowobject;
        bool isActing = buildOb->isFinish() && buildOb->getActSpeed()>0;
//        int nowActPosition = -1;

        if(isActing)
        {
            actions[0] = ACT_STOP;
            actionStatus[0] = ACT_STATUS_ENABLED;
        }
        else
        {
            for(int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
            {
                //新增判断是否显示
                if( mainPtr->player[0]->get_isBuildActionShowAble(buildOb->getNum(), buildOb->ActNameToActNum(buildOb->getActNames(i)) ) )
                {
                    actions[i] = ((Building *)nowobject)->getActNames(i);//getActNames是Building特有的,用来获取action数组
                    actionStatus[i] = ((Building *)nowobject)->getActStatus(i);
                }
                else actions[i] = ACT_NULL;
            }
        }

    }
    else if(type == SORT_FARMER)//人类
    {
        actions[0] = ACT_BUILD;
        for(int i = 1; i < ACT_WINDOW_NUM_FREE; i++)
        {
            actionStatus[i] = ACT_STATUS_DISABLED;
        }
    }
    else
    {
        for(int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
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
    bool isBuild ,isBuildingAct;
    int buildType , buildingActType;
    if(nowobject != NULL)
    {
        if(nowobject->getActSpeed() > 0) ui->objText->setText(QString::number((int)(nowobject->getActPercent())) + "%");//如果有进行中的任务则显示进度
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
    std::list<Building *>::iterator buildIt = mainPtr->player[0]->build.begin(), buildItTemp = buildIt;
    std::list<Human *>::iterator humanIt = mainPtr->player[0]->human.begin(), humanItTemp = humanIt;

    //当前人口
    int human_num = mainPtr->player[0]->getHumanNum();
    //当前人口上限
    int build_hold_human_num =  mainPtr->player[0]->getMaxHumanNum();


    //先进行行动状态更新(根据actions决定actionStatus数组)
    for(int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
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
            case ACT_UPGRADE_WOOD:
                isBuildingAct = true;
                buildType = BUILDING_MARKET;
                buildingActType = BUILDING_MARKET_WOOD_UPGRADE;
                break;

            case ACT_NULL:
                actionStatus[i] = ACT_STATUS_DISABLED;
                break;
            case ACT_BUILD_CANCEL:
                actionStatus[i] = ACT_STATUS_ENABLED;
                break;
        }

        if(isBuild)
        {
            if(!mainPtr->player[0]->get_isBuildingShowAble(buildType) || !mainPtr->player[0]->get_isBuildingAble(buildType))
                actionStatus[i] = ACT_STATUS_DISABLED;
            else
                actionStatus[i] = ACT_STATUS_ENABLED;
        }

        if(isBuildingAct)
        {
            if(!mainPtr->player[0]->get_isBuildActionAble(buildType , buildingActType))
                actionStatus[i] = ACT_STATUS_DISABLED;
            else
                actionStatus[i] = ACT_STATUS_ENABLED;
        }
    }


    for(int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
    {
        mainPtr->getActs(i)->setStatus(actionStatus[i]);//应用行动状态,mainPtr->getActs(i)即获取第i个按钮窗口
        mainPtr->getActs(i)->update();//刷新按钮显示状态
    }

    if(secondWidget_Build)
    {
        showBuildActLab();
    }

    //再进行快捷栏和状态栏显示更新(本窗口的内容)
    if(nowobject == NULL)
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
        for(int i = 0 ; i<ACT_WINDOW_NUM_FREE; i++)
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
        if(type == SORT_BUILDING|| type == SORT_Building_Resource)//建筑
        {
            Building *objBuilding = (Building *)nowobject;
            bool isActing = objBuilding->isFinish() && objBuilding->getActSpeed()>0;
            buildType = objBuilding->getNum();//获取建筑种类
            ui->objName->setText(QString::fromStdString(objBuilding->getDisplayName(buildType)));//设置显示名称

            //根据不同时代设置不同的图标
            string name="Button_"+objBuilding->getBuiltname(mainPtr->player[0]->getCiv(), buildType);
            ui->objIcon->setPixmap(resMap[name].front().scaled(110,110));

            if(objBuilding->getActSpeed() != 0)
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
            if(objBuilding->isFinish())
            {
                //后续简化代码
                if(isActing)
                {
                    actions[0] = ACT_STOP;
                    actionStatus[0] = ACT_STATUS_ENABLED;
                    for(int i = 1;i<ACT_WINDOW_NUM_FREE;i++) actions[i] = ACT_NULL;
                }
                else
                {
                    for(int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
                    {
                        //新增判断是否显示
                        if( mainPtr->player[0]->get_isBuildActionShowAble(objBuilding->getNum(), objBuilding->ActNameToActNum(objBuilding->getActNames(i)) ) )
                        {
                            actions[i] = objBuilding->getActNames(i);//getActNames是Building特有的,用来获取action数组
                            actionStatus[i] = objBuilding->getActStatus(i);
                        }
                        else actions[i] = ACT_NULL;
                    }
                }

                //统一根据内置行动数组设置
                for(int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
                {
                    if(actions[i] != ACT_NULL) mainPtr->getActs(i)->show();
                    else mainPtr->getActs(i)->hide();
                }

                //详细设置
                if(objBuilding->getNum() == BUILDING_HOME)
                {
                    ui->objText->setText(QString::number(human_num) + "/" + QString::number(build_hold_human_num));
                    ui->objIconSmall->setPixmap(resMap["SmallIcon_People"].front());
                }
                else if(objBuilding->getNum() == BUILDING_FARM)
                {
                    Building_Resource* farm = (Building_Resource*) objBuilding;

                    if(farm->get_Cnt()>0)
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
                for(int i = 0 ; i<ACT_WINDOW_NUM_FREE; i++)
                {
                    actions[i] = ACT_NULL;
                    actionStatus[i] = ACT_STATUS_DISABLED;
                }
            }
            this->update();
            this->show();//必要的update和show
        }
        else if(type == SORT_STATICRES)//块资源
        {
            StaticRes *objStaticRes = (StaticRes *)(nowobject);
            int num = objStaticRes->getNum();
            ui->objName->setText(QString::fromStdString(objStaticRes->getStaticResDisplayName(num)));
            ui->objHp->setText("");
            ui->objIcon->setPixmap(resMap["Button_"+objStaticRes->getStaticResName(num)].front().scaled(80,80));
            ui->objIconSmall->setPixmap(QPixmap());

            if(objStaticRes->get_ResourceSort() == HUMAN_GRANARYFOOD || objStaticRes->get_ResourceSort() == HUMAN_STOCKFOOD) ui->objIconSmall->setPixmap(resMap["Icon_Food"].front());
            else if(objStaticRes->get_ResourceSort() == HUMAN_STONE) ui->objIconSmall->setPixmap(resMap["Icon_Stone"].front());
//            else if(objStaticRes->get_ResourceSort() == HUMAN_GOLD) ui->objIconSmall->setPixmap(resMap["Icon_Gold"].front());

            ui->objText->setText(QString::number(objStaticRes->get_Cnt()));
            this->update();
            this->show();
        }
        else if(type == SORT_FARMER)//村民
        { //objIconSmall_ATK objText_ATK用于展示攻击力 objIconSmall objText表示携带资源 objIconSmall_DEF和objText_DEF表示防御（近战和远程分开）
            Farmer *objFarmer = (Farmer *)(nowobject);
            int num = objFarmer->getState();//获取工作状态并显示对应名称
            QString name = QString::fromStdString(objFarmer->getDisplayName(num));
            ui->objName->setText(name);
            ui->objIcon->setPixmap(resMap["Button_Village"].front().scaled(110,110));

            if(objFarmer->getState() == 0 || objFarmer->getState() == 4)
            {
                ui->objIconSmall_ATK->setPixmap(resMap["SmallIcon_Attack"].front().scaled(40, 30));
                if(objFarmer->showATK_Addition() == 0) ui->objText_ATK->setText(QString::number(objFarmer->showATK_Basic()));
                else ui->objText_ATK->setText(QString::number(objFarmer->showATK_Basic()) + "+" + QString::number(objFarmer->showATK_Addition())); // 显示攻击力（基础+额外）

            }

            if(objFarmer->getResourceSort() == HUMAN_WOOD) ui->objIconSmall->setPixmap(resMap["Icon_Wood"].front());
            else if(objFarmer->getResourceSort() == HUMAN_GRANARYFOOD || objFarmer->getResourceSort() == HUMAN_STOCKFOOD)
                ui->objIconSmall->setPixmap(resMap["Icon_Food"].front());
            else if(objFarmer->getResourceSort() == HUMAN_STONE) ui->objIconSmall->setPixmap(resMap["Icon_Stone"].front());

            //如果当前村民没有资源
            if(objFarmer->getResourceNowHave() == 0)
            {
                ui->objIconSmall->setPixmap(QPixmap());
                ui->objText->setText("");
            }
            else
            {
                ui->objText->setText(QString::number((int)(objFarmer->getResourceNowHave())));
            }
            //设置血量
            ui->objHp->setText(QString::number(objFarmer->getBlood()) + "/" + QString::number(objFarmer->getMaxBlood()));
            this->update();
            this->show();

        }
        else if(type == SORT_ARMY)
        {
            //objIconSmall_ATK objText_ATK用于展示攻击力 objIconSmall objText表示携带资源或者防御
            Army* objArmy = (Army*)nowobject;
            ui->objName->setText(objArmy->getChineseName());
            std::string buttonName = "Button_" + objArmy->getArmyName(objArmy->getNum(),objArmy->getLevel());
            ui->objIcon->setPixmap(resMap[buttonName].front().scaled(110,110));
            ui->objIconSmall_ATK->setPixmap(resMap["SmallIcon_Attack"].front().scaled(40, 30)); //攻击图标
            ui->objIconSmall_DEF_melee->setPixmap(resMap["SmallIcon_Defense_Melee"].front().scaled(40, 30));
            ui->objIconSmall_DEF_range->setPixmap(resMap["SmallIcon_Defense_Range"].front().scaled(40, 30));
            if(objArmy->showATK_Addition() == 0) ui->objText_ATK->setText(QString::number(objArmy->showATK_Basic()));
            else ui->objText_ATK->setText(QString::number(objArmy->showATK_Basic())+ "+" +QString::number(objArmy->showATK_Addition()));// 显示攻击力（基础+额外）
            if(objArmy->showDEF_Close_Addition() == 0) ui->objText_DEF_melee->setText(QString::number(objArmy->showDEF_Close()));
            else ui->objText_DEF_melee->setText(QString::number(objArmy->showDEF_Close()) + "+" + QString::number(objArmy->showDEF_Close_Addition())); // 显示近战防御（基础+额外）
            if(objArmy->showDEF_Shoot_Addition() == 0) ui->objText_DEF_range->setText(QString::number(objArmy->showDEF_Shoot()));
            else ui->objText_DEF_range->setText(QString::number(objArmy->showDEF_Shoot()) + "+" + QString::number(objArmy->showDEF_Shoot_Addition())); // 显示远程防御（基础+额外）
            //设置血量
            ui->objHp->setText(QString::number(objArmy->getBlood()) + "/" +QString::number(objArmy->getMaxBlood()));
            this->update();
            this->show();
        }
//        else if(type == SORT_TREEFOREST)//树
//        {
//            Animal *objAnimal = (Animal *)(nowobject);
//            if(objAnimal->get_Cnt() <= 0) receiveselectNum(0);
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
//        }
        else if(type == SORT_ANIMAL)//动物
        {
            Animal *objAnimal = (Animal *)(nowobject);
            ui->objName->setText(QString::fromStdString(Animal::getAnimalDisplayName(objAnimal->getNum())) );

            if(objAnimal->getNum() == 1)//瞪羚
            {
                ui->objIcon->setPixmap(resMap["Button_Gazelle"].front().scaled(110,110));
            }
            else if(objAnimal->getNum() == 2)//大象
            {
                ui->objIcon->setPixmap(resMap["Button_Elephant"].front().scaled(110,110));
            }
            else if(objAnimal->getNum() == 3)
            {
                ui->objIcon->setPixmap(resMap["Button_Lion"].front().scaled(110,110));
            }
            else if(objAnimal->getNum() == ANIMAL_TREE|| objAnimal->getNum() == ANIMAL_FOREST)
            {
                ui->objIcon->setPixmap(resMap["Button_Tree"].front().scaled(110,110));
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
            if(objAnimal->isDie()) ui->objHp->setText(""); //原代码意思是当动物死亡后变为收集的资源时，不显示血条，现在可能有新的设计
            else ui->objHp->setText(QString::number(objAnimal->getBlood()) + "/" + QString::number(objAnimal->getMaxBlood()));

            this->update();
            this->show();
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
    int actName = actions[num];
    doActs(actName);
}

int SelectWidget::aiAct(int actName,Coordinate* self)
{
    return doActs(actName,self);
}

void SelectWidget::manageBuildBottom( int position, int actNum , int buildingNum )
{
    if(mainPtr->player[0]->get_isBuildingShowAble(buildingNum))
    {
        actions[position] = actNum;
        if(mainPtr->player[0]->get_isBuildingAble(buildingNum)) actionStatus[position] = ACT_STATUS_ENABLED;
        else actionStatus[position] = ACT_STATUS_DISABLED;
    }
    else actions[position] = ACT_NULL;
}

int SelectWidget::doActs(int actName,Coordinate* nowobject)
{
    //修改，判断是否可行动逻辑
    if(nowobject == NULL) return ACTION_INVALID_ACTION;

    //建筑的按钮位置
    if(actName == ACT_BUILD)
    {
        secondWidget_Build = true;
        showBuildActLab();
    }
    else if(actName == ACT_BUILD_HOUSE){
        QApplication::restoreOverrideCursor();
        emit sendBuildMode(BUILDING_HOME);
        QCursor my(resMap["House1"].front());
        QApplication::setOverrideCursor(my);
    }
    else if(actName == ACT_BUILD_GRANARY){
        QApplication::restoreOverrideCursor();
        emit sendBuildMode(BUILDING_GRANARY);
        QCursor my(resMap["Granary"].front());
        QApplication::setOverrideCursor(my);
    }
    else if(actName == ACT_BUILD_STOCK){
        QApplication::restoreOverrideCursor();
        QCursor my(resMap["Stock"].front());
        QApplication::setOverrideCursor(my);
        emit sendBuildMode(BUILDING_STOCK);
    }
    else if(actName == ACT_BUILD_FARM){
        QApplication::restoreOverrideCursor();
        QCursor my(resMap["Farm"].front());
        QApplication::setOverrideCursor(my);
        emit sendBuildMode(BUILDING_FARM);
    }
    else if(actName == ACT_BUILD_MARKET){
        QApplication::restoreOverrideCursor();
        QCursor my(resMap["Market"].front());
        QApplication::setOverrideCursor(my);
        emit sendBuildMode(BUILDING_MARKET);
    }
    else if(actName == ACT_BUILD_ARROWTOWER){
        QApplication::restoreOverrideCursor();
        QCursor my(resMap["ArrowTower"].front());
        QApplication::setOverrideCursor(my);
        emit sendBuildMode(BUILDING_ARROWTOWER);
    }
    else if(actName == ACT_BUILD_ARMYCAMP){
        QApplication::restoreOverrideCursor();
        QCursor my(resMap["ArmyCamp"].front());
        QApplication::setOverrideCursor(my);
        emit sendBuildMode(BUILDING_ARMYCAMP);
    }
    else if(actName == ACT_BUILD_RANGE){
        QApplication::restoreOverrideCursor();
        QCursor my(resMap["Range"].front());
        QApplication::setOverrideCursor(my);
        emit sendBuildMode(BUILDING_RANGE);
    }
    else if(actName == ACT_BUILD_STABLE){
        QApplication::restoreOverrideCursor();
        QCursor my(resMap["Stable"].front());
        QApplication::setOverrideCursor(my);
        emit sendBuildMode(BUILDING_STABLE);
    }
    else if(actName == ACT_BUILD_CANCEL)
    {
        QApplication::restoreOverrideCursor();
        emit sendBuildMode(-1);
        initActs();
    }
    else if(actName == ACT_CREATEFARMER){
        core->addRelation(nowobject , CoreEven_BuildingAct,BUILDING_CENTER_CREATEFARMER);
    }
    else if(actName == ACT_UPGRADE_AGE){
        if(mainPtr->player[0]->get_civiBuild_Times(mainPtr->player[0]->getCiv())>=2)
            core->addRelation(nowobject, CoreEven_BuildingAct , BUILDING_CENTER_UPGRADE);
    }
    else if(actName == ACT_UPGRADE_TOWERBUILD){
        core->addRelation(nowobject , CoreEven_BuildingAct , BUILDING_GRANARY_ARROWTOWER);
    }
    else if(actName == ACT_UPGRADE_WOOD){
        core->addRelation(nowobject , CoreEven_BuildingAct , BUILDING_MARKET_WOOD_UPGRADE);
    }
    else if(actName == ACT_UPGRADE_STONE){
        core->addRelation(nowobject , CoreEven_BuildingAct , BUILDING_MARKET_STONE_UPGRADE);
    }
    else if(actName == ACT_UPGRADE_FARM){
        core->addRelation(nowobject,CoreEven_BuildingAct , BUILDING_MARKET_FARM_UPGRADE);
    }
    else if(actName == ACT_STOCK_UPGRADE_USETOOL) core->addRelation(nowobject,CoreEven_BuildingAct , BUILDING_STOCK_UPGRADE_USETOOL);
    else if( actName == ACT_STOCK_UPGRADE_DEFENSE_INFANTRY ) core->addRelation(nowobject , CoreEven_BuildingAct , BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY);
    else if( actName == ACT_STOCK_UPGRADE_DEFENSE_ARCHER ) core->addRelation(nowobject , CoreEven_BuildingAct , BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER);
    else if(actName == ACT_STOCK_UPGRADE_DEFENSE_RIDER) core->addRelation(nowobject , CoreEven_BuildingAct , BUILDING_STOCK_UPGRADE_DEFENSE_RIDER);
    else if(actName == ACT_ARMYCAMP_CREATE_CLUBMAN) core->addRelation(nowobject , CoreEven_BuildingAct , BUILDING_ARMYCAMP_CREATE_CLUBMAN);
    else if(actName == ACT_ARMYCAMP_UPGRADE_CLUBMAN) core->addRelation(nowobject,CoreEven_BuildingAct , BUILDING_ARMYCAMP_UPGRADE_CLUBMAN);
    else if(actName == ACT_ARMYCAMP_CREATE_SLINGER) core->addRelation(nowobject , CoreEven_BuildingAct , BUILDING_ARMYCAMP_CREATE_SLINGER);
    else if(actName == ACT_RANGE_CREATE_BOWMAN) core->addRelation(nowobject,CoreEven_BuildingAct , BUILDING_RANGE_CREATE_BOWMAN);
    else if(actName == ACT_STABLE_CREATE_SCOUT) core->addRelation(nowobject,CoreEven_BuildingAct , BUILDING_STABLE_CREATE_SCOUT);
    else if(actName == ACT_STOP)
    {
        if(nowobject!=NULL)
        {
             core->suspendRelation(nowobject);
             ui->objText->setText("");
        }
        else return ACTION_INVALID_ACTION;
    }
    for(int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
    {
        mainPtr->getActs(i)->update();
    }
    return ACTION_SUCCESS;
}

void SelectWidget::showBuildActLab()
{
    manageBuildBottom( 0 , ACT_BUILD_HOUSE , BUILDING_HOME );
    manageBuildBottom( 1 , ACT_BUILD_GRANARY , BUILDING_GRANARY );
    manageBuildBottom( 2 , ACT_BUILD_STOCK , BUILDING_STOCK);
    manageBuildBottom( 3 , ACT_BUILD_MARKET , BUILDING_MARKET);
    manageBuildBottom( 4 , ACT_BUILD_ARROWTOWER , BUILDING_ARROWTOWER );
    manageBuildBottom( 5 , ACT_BUILD_ARMYCAMP , BUILDING_ARMYCAMP );
    manageBuildBottom( 6 , ACT_BUILD_RANGE , BUILDING_RANGE );
    manageBuildBottom( 7 , ACT_BUILD_STABLE , BUILDING_STABLE );
    manageBuildBottom( 8 , ACT_BUILD_FARM , BUILDING_FARM );

    actions[9] = ACT_BUILD_CANCEL;
    actionStatus[9] = ACT_STATUS_ENABLED;
}

void SelectWidget::updateActs()
{
  //遍历建筑更新活动列表
    std::list<Building *>::iterator buildIt = mainPtr->player[0]->build.begin();
    int wood = mainPtr->player[0]->getWood();
    int food = mainPtr->player[0]->getFood();
    int stone = mainPtr->player[0]->getStone();
    int gold = mainPtr->player[0]->getGold();

     for(; buildIt != mainPtr->player[0]->build.end(); buildIt++)
     {
         if((*buildIt)->isFinish())
         {
             //为每个位置设置act_status,是否显示、显示停止在initact中判断
            (*buildIt)->setActStatus(wood,food,stone,gold);
         }
     }
}

void SelectWidget::drawActs()
{
    for(int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
    {
        QPixmap pix;
        if(actions[i] == ACT_CREATEFARMER) pix = resMap["Button_Villager"].front().scaled(80, 80);
        else if(actions[i] == ACT_STOP) pix = resMap["Button_Stop"].front().scaled(80, 80);
        /******升级技术*****/
        else if(actions[i] == ACT_UPGRADE_AGE) pix = resMap["ButtonTech_Center1"].front().scaled(80,80);
        else if(actions[i] == ACT_UPGRADE_FARM) pix = resMap["ButtonTech_Cow"].front().scaled(80,80);
        else if(actions[i] == ACT_UPGRADE_STONE) pix = resMap["ButtonTech_Stone"].front().scaled(80,80);
        else if(actions[i] == ACT_UPGRADE_WOOD) pix = resMap["ButtonTech_Lumber"].front().scaled(80,80);
        else if(actions[i] == ACT_UPGRADE_TOWERBUILD) pix = resMap["ButtonTech_ArrowTower"].front().scaled(80,80);
        //仓库
        else if(actions[i] == ACT_STOCK_UPGRADE_USETOOL) pix = resMap["ButtonTech_Spear"].front().scaled(80,80);
        else if(actions[i] == ACT_STOCK_UPGRADE_DEFENSE_INFANTRY) pix = resMap["ButtonTech_Sword"].front().scaled(80,80);
        else if(actions[i] == ACT_STOCK_UPGRADE_DEFENSE_ARCHER) pix = resMap["ButtonTech_Arrow"].front().scaled(80,80);
        else if(actions[i] == ACT_STOCK_UPGRADE_DEFENSE_RIDER) pix = resMap["ButtonTech_Horse"].front().scaled(80,80);
        //兵营
        else if(actions[i] == ACT_ARMYCAMP_UPGRADE_CLUBMAN) pix = resMap["ButtonTech_Axeman"].front().scaled(80,80);

        /******军队*****/
        else if(actions[i] == ACT_ARMYCAMP_CREATE_CLUBMAN)
        {
            //部分兵种有等级——判断clubman等级，根据等级贴图
            switch ( (int)mainPtr->player[0]->get_buildActLevel(BUILDING_ARMYCAMP , BUILDING_ARMYCAMP_UPGRADE_CLUBMAN) ) {
            case 0: //初始
                pix = resMap["Button_Clubman"].front().scaled(80,80);
                break;
            case 1: //1级
                pix = resMap["Button_Axeman"].front().scaled(80,80);
                break;
            }
        }
        else if(actions[i] == ACT_ARMYCAMP_CREATE_SLINGER) pix = resMap["Button_Slinger"].front().scaled(80,80);
        else if(actions[i] == ACT_RANGE_CREATE_BOWMAN) pix = resMap["Button_Archer"].front().scaled(80,80);
        else if(actions[i] == ACT_STABLE_CREATE_SCOUT) pix = resMap["Button_Scout"].front().scaled(80,80);

        /******建造*****/
        else if(actions[i] == ACT_BUILD) pix = resMap["Button_Build"].front().scaled(80,80);
        else if(actions[i] == ACT_BUILD_ARROWTOWER) pix = resMap["Button_ArrowTower"].front().scaled(80,80);
        else if(actions[i] == ACT_BUILD_CANCEL) pix = resMap["Exit"].front().scaled(80,80);
        else if(actions[i] == ACT_BUILD_FARM) pix = resMap["Button_Farm"].front().scaled(80,80);
        else if(actions[i] == ACT_BUILD_GRANARY) pix = resMap["Button_Granary"].front().scaled(80,80);
        else if(actions[i] == ACT_BUILD_HOUSE) pix = resMap["Button_House1"].front().scaled(80,80);
        else if(actions[i] == ACT_BUILD_MARKET) pix = resMap["Button_Market"].front().scaled(80,80);
        else if(actions[i] == ACT_BUILD_STOCK) pix = resMap["Button_Stock"].front().scaled(80,80);
        else if(actions[i] == ACT_BUILD_ARMYCAMP) pix = resMap["Button_ArmyCamp"].front().scaled(80,80);
        else if(actions[i] == ACT_BUILD_RANGE) pix = resMap["Button_Range"].front().scaled(80,80);
        else if(actions[i] == ACT_BUILD_STABLE) pix = resMap["Button_Stable"].front().scaled(80,80);
        mainPtr->getActs(i)->setPix(pix);

        //设置actions[i]为NULL,等于隐藏
        if(actions[i] != ACT_NULL) mainPtr->getActs(i)->show();
        else mainPtr->getActs(i)->hide();
        mainPtr->getActs(i)->update();
    }
}

//所有建造
void SelectWidget::getBuild(int BlockL, int BlockU, int num)
{
    core->addRelation(nowobject,BlockL,BlockU,CoreEven_CreatBuilding,true,num);
}
