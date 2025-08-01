#include "MainWidget.h"
#include "ui_MainWidget.h"
#include "ui_Editor.h"
#include <iostream>
#include <QString>
#include <algorithm>
#include<rectarea.h>
#include<circlearea.h>
#include<LineArea.h>
int g_globalNum = rand() % 11;
int g_frame = 0;
int mapmoveFrequency = INITIAL_FREQUENCY;
extern Score usrScore;
extern Score enemyScore;
std::map<int, Coordinate*> g_Object;
ActWidget* acts[ACT_WINDOW_NUM_FREE];
std::map<int, std::string> actNames = {
    {ACT_CREATEFARMER, ACT_CREATEFARMER_NAME},
    {ACT_UPGRADE_AGE, ACT_UPGRADE_AGE_NAME},
    {ACT_UPGRADE_TOWERBUILD, ACT_UPGRADE_TOWERBUILD_NAME},
    {ACT_UPGRADE_WOOD, ACT_UPGRADE_WOOD_NAME},
    {ACT_UPGRADE_STONE, ACT_UPGRADE_STONE_NAME},
    {ACT_UPGRADE_GOLD, ACT_UPGRADE_GOLD_NAME},
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
    {ACT_BUILD_DOCK, ACT_BUILD_DOCK_NAME},
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
    {ACT_DOCK_CREATE_SAILING, ACT_DOCK_CREATE_SAILING_NAME},
    {ACT_DOCK_CREATE_WOOD_BOAT, ACT_DOCK_CREATE_WOOD_BOAT_NAME},
    {ACT_DOCK_CREATE_SHIP, ACT_DOCK_CREATE_SHIP_NAME},
    {ACT_SHIP_LAY, ACT_SHIP_LAY_NAME},
};


MainWidget::MainWidget(int MapJudge, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    qInfo() << "主程序启动参数：" << MapJudge << " 开始初始化...";
    ui->setupUi(this);
    //初始化一些编辑器配置
    initEditor();
    // 初始化游戏资源
    initGameResources();
    // 初始化游戏元素
    initGameElements();
    // 初始化当前窗口属性
    initWindowProperties();
    // 初始化窗口选项属性
    initOptions();
    // 初始化游戏实体属性框（左下角）
    initInfoPane();
    // 初始化计时器
    initGameTimer();
    // 初始化玩家
    initPlayers();
    // 初始化地图
    initMap(MapJudge);
    // 设置内核
    setupCore();
    // 初始化AI
    initAI();
    // 设置鼠标追踪
    setupMouseTracking();
    // 设置信息栏文本颜色
    setupTipLabel();
    // 设置小地图
    initViewMap();
    // 设置背景音乐
    initBGM();
    //开辟音乐播放线程
    soundPlayThread = (new SoudPlayThread);
    soundPlayThread->start();
    //
    debugText("blue", " 游戏开始");
    qInfo() << "初始化结束，游戏开始！";

    // 创建编辑器
    editor = new Editor(this);

    // 显示编辑器
    editor->show();
    if (!EditorMode) editor->hide();

    // 导出地图
    connect(editor->ui->export_map, QPushButton::clicked, this, [=]() {
        this->ExportCurrentState((string("map.")+MAPFILE_SUFFIX).c_str());
        call_debugText("green", " 导出地图", 0);
        });
    connect(editor->ui->delete_object, QPushButton::clicked, this, [=]() {
        call_debugText("green", " 删除资源/建筑", 0);
        this->currentSelected = DELETEOBJECT;
        });
    // 连接 QComboBox 的 currentIndexChanged 信号
    connect(editor->ui->land_type, QOverload<const QString&>::of(&QComboBox::currentIndexChanged), this, [=](const QString& text) {
        // 获取当前选中的选项索引
        QString selectedText = text;
        if (text == "草地") this->currentSelected = FLAT;
        else if (text == "海洋") this->currentSelected = OCEAN;
        if (text != "地皮类型") call_debugText("green", " " + text, 0);
        });
    connect(editor->ui->land_height, QOverload<const QString&>::of(&QComboBox::currentIndexChanged), this, [=](const QString& text) {
        QString selectedText = text;
        if (text == "提升高度") this->currentSelected = HIGHTERLAND;
        else if (text == "降低高度") this->currentSelected = LOWERLAND;
        if (text != "地皮高度") call_debugText("green", " " + text, 0);
        });
    connect(editor->ui->player_building_and_source, QOverload<const QString&>::of(&QComboBox::currentIndexChanged), this, [=](const QString& text) {
        QString selectedText = text;
        if (text == "玩家市中心") this->currentSelected = PLAYERDOWNTOWN;
        else if (text == "玩家运输船") this->currentSelected = PLAYERTRANSPORTSHIP;
        else if (text == "玩家渔船") this->currentSelected = PLAYERFISHINGBOAT;
        else if (text == "玩家船坞") this->currentSelected = PLAYERDOCK;
        else if (text == "玩家战船") this->currentSelected = PLAYERWARSHIP;
        else if (text == "玩家仓库") this->currentSelected = PLAYERREPOSITORY;
        else if (text == "玩家兵营") this->currentSelected = PLAYERBARRACKS;
        else if (text == "玩家箭塔") this->currentSelected = PLAYERARROWTOWER;
        else if (text == "玩家渔场") this->currentSelected = PLAYERFISHERY;
        if (text != "玩家资源/建筑") call_debugText("green", " " + text, 0);
        });
    connect(editor->ui->player_human, QOverload<const QString&>::of(&QComboBox::currentIndexChanged), this, [=](const QString& text) {
        QString selectedText = text;
        if (text == "玩家农民") this->currentSelected = PLAYERFARMER;
        else if (text == "玩家棍棒兵") this->currentSelected = PLAYERCLUBMAN;
        else if (text == "玩家斧头兵") this->currentSelected = PLAYERAXEMAN;
        else if (text == "玩家侦察兵") this->currentSelected = PLAYERSCOUT;
        else if (text == "玩家弓箭手") this->currentSelected = PLAYERBOWMAN;
        if (text != "玩家人物") call_debugText("green", " " + text, 0);
        });
    connect(editor->ui->ai_building_and_resource, QOverload<const QString&>::of(&QComboBox::currentIndexChanged), this, [=](const QString& text) {
        QString selectedText = text;
        if (text == "地方战船") this->currentSelected = AIWARSHIP;
        else if (text == "敌方箭塔") this->currentSelected = AIARROWTOWER;
        if (text != "敌方资源/建筑") call_debugText("green", " " + text, 0);
        });
    connect(editor->ui->ai_human, QOverload<const QString&>::of(&QComboBox::currentIndexChanged), this, [=](const QString& text) {
        QString selectedText = text;
        if (text == "敌方棍棒兵") this->currentSelected = AICLUBMAN;
        else if (text == "地方斧头兵") this->currentSelected = AIAXEMAN;
        else if (text == "地方侦察兵") this->currentSelected = AISCOUT;
        else if (text == "地方弓箭手") this->currentSelected = AIBOWMAN;
        if (text != "敌方人物") call_debugText("green", " " + text, 0);
        });
    connect(editor->ui->animal, QOverload<const QString&>::of(&QComboBox::currentIndexChanged), this, [=](const QString& text) {
        QString selectedText = text;
        if (text == "瞪羚") this->currentSelected = GAZELLE;
        else if (text == "狮子") this->currentSelected = LION;
        else if (text == "大象") this->currentSelected = ELEPHANT;
        if (text != "动物") call_debugText("green", " " + text, 0);
        });
    connect(editor->ui->resource, QOverload<const QString&>::of(&QComboBox::currentIndexChanged), this, [=](const QString& text) {
        QString selectedText = text;
        if (text == "树木") this->currentSelected = TREE;
        else if (text == "石头") this->currentSelected = STONM;
        else if (text == "金矿") this->currentSelected = GOLDORE;
        if (text != "公立资源") call_debugText("green", " " + text, 0);
        });
    connect(editor->ui->areaLimit, QOverload<const QString&>::of(&QComboBox::currentIndexChanged), this, [=](const QString& text) {
        QString selectedText = text;
        if (text == "矩形区域") this->currentSelected = RECT_AREA;
        else if (text == "圆形区域") this->currentSelected = CIRCLE_AREA;
        else if(text=="曲线区域")this->currentSelected=LINE_AREA;
        if (text != "敌方区域限制") call_debugText("green", " " + text, 0);
        });

}

//******************导出地图*******************
void MainWidget::ExportCurrentState(const char* fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qDebug() << "can't open error!";
        return;

    }
    QTextStream stream(&file);
    //获取对象的区域限制
    auto GetAreaLimit=[&](Coordinate*obj)->pair<string,void*>{
          auto&lineR=((LineArea*)(lineArea))->relation;
          auto&circleR=((CircleArea*)(circleArea))->relation;
          auto&rectR=((RectArea*)(rectArea))->relation;
          //
          if(lineR.count(obj)){
            auto&d=lineR[obj];
            return pair<string,void*>{LineArea::Name(),&d};
          }else if(circleR.count(obj)){
              auto&d=circleR[obj];
              return pair<string,void*>{CircleArea::Name(),&d};
          }else if(rectR.count(obj)){
              auto&d=rectR[obj];
              return pair<string,void*>{RectArea::Name(),&d};
          }
          return pair<string,void*>{"",0};
    };
    //
    auto JsonAreaLimit=[&](string type,void*data)->QJsonObject{
        QJsonObject json;
        //
        if(data==0)return json;
        json.insert("Type",QString(type.c_str()));
        //
        if(type==LineArea::Name()){
            LineAreaData&d=*(LineAreaData*)data;
            QJsonArray array;
            for(auto&p:d.data){
                QJsonArray arr;
                arr.append(p[0]);
                arr.append(p[1]);
                array.append(arr);
            }
            json.insert("Point",array);

        }else if(type==CircleArea::Name()){
            CircleAreaData&d=*(CircleAreaData*)data;
            json.insert("DR",d.dr);
            json.insert("UR",d.ur);
            json.insert("R",d.rad);
        }else if(type==RectArea::Name()){
            RectAreaData&d=*(RectAreaData*)data;
            json.insert("DR",d.dr);
            json.insert("UR",d.ur);
            json.insert("W",d.w);
            json.insert("H",d.h);
        }
        return json;
    };
    auto GetAreaJson=[&](Coordinate*obj)->pair<bool,QJsonObject>{
        auto&&ret0=GetAreaLimit(obj);
        auto&&ret1=JsonAreaLimit(ret0.first,ret0.second);
        if(ret0.first=="")return pair<bool,QJsonObject>{0,ret1};
        return pair<bool,QJsonObject>{1,ret1};
    };
    //
    //////////////保存cell图
    QJsonObject root;
    for (int i = 0, idx = 0;i < MAP_L;++i)
        for (int j = 0;j < MAP_U;++j) {
            Block& cell = map->cell[i][j];
            QJsonObject obj;
            obj.insert("BlockDR", i);
            obj.insert("BlockUR", j);
            obj.insert("Num", cell.Num);
            obj.insert("Visible", cell.Visible);
            obj.insert("Explored", cell.Explored);
            obj.insert("Type", cell.getMapType());
            obj.insert("Pattern", cell.getMapPattern());
            obj.insert("Height", cell.getMapHeight());
            obj.insert("OffsetX", cell.getOffsetX());
            obj.insert("OffsetY", cell.getOffsetY());
            obj.insert("Resource", cell.getMapResource());
            root.insert("Cell_" + QString::number(idx++), obj);
        }
    //////////////保存building
    int building_idx = 0;
    for (Building* build : player[0]->build) {
        QJsonObject obj;
        obj.insert("BlockDR", build->getBlockDR());
        obj.insert("BlockUR", build->getBlockUR());
        obj.insert("Num", build->getNum());
        obj.insert("Own", "WLH");
        root.insert("Building_" + QString::number(building_idx++), obj);
    }
    for (Building* build : player[1]->build) {
        QJsonObject obj;
        obj.insert("BlockDR", build->getBlockDR());
        obj.insert("BlockUR", build->getBlockUR());
        obj.insert("Num", build->getNum());
        obj.insert("Own", "LZ");
        root.insert("Building_" + QString::number(building_idx++), obj);
    }
    ////////////////保存人物
    int Human_idx = 0;
    for (Human* human : player[0]->human) {
        QJsonObject obj;
        obj.insert("DR", human->getDR());
        obj.insert("UR", human->getUR());
        obj.insert("Num", human->getNum());
        obj.insert("Sort", human->getSort() == SORT_FARMER ? "Farmer" : "Army");
        obj.insert("FarmerType", human->getSort() == SORT_FARMER ? ((Farmer*)human)->get_farmerType() : -1);
        obj.insert("Own", "WLH");
        //获取区域限制
        auto&&ret=GetAreaJson(human);
        if(ret.first){
            obj.insert("AreaLimit",ret.second);
        }
        //
        root.insert("Human_" + QString::number(Human_idx++), obj);

    }
    for (Human* human : player[1]->human) {
        QJsonObject obj;
        obj.insert("DR", human->getDR());
        obj.insert("UR", human->getUR());
        obj.insert("Num", human->getNum());
        obj.insert("Sort", human->getSort() == SORT_FARMER ? "Farmer" : "Army");
        obj.insert("Own", "LZ");
        //获取区域限制
        auto&&ret=GetAreaJson(human);
        if(ret.first){
            obj.insert("AreaLimit",ret.second);
        }
        //
        root.insert("Human_" + QString::number(Human_idx++), obj);

    }
    /////////////////保存静态资源
    int res_idx = 0;
    for (StaticRes* res : map->staticres) {
        QJsonObject obj;
        obj.insert("BlockDR", res->getBlockDR());
        obj.insert("BlockUR", res->getBlockUR());
        obj.insert("Num", res->getNum());
        root.insert("StaticRes_" + QString::number(res_idx++), obj);
    }
    //////////////////保存动物
    int animal_idx = 0;
    for (Animal* animal : map->animal) {
        QJsonObject obj;
        obj.insert("DR", animal->getDR());
        obj.insert("UR", animal->getUR());
        obj.insert("Num", animal->getNum());
        root.insert("Animal_" + QString::number(animal_idx++), obj);
    }
    /////////////////////////////
    QJsonDocument doc(root);
    stream << doc.toJson();
    file.close();
}


void MainWidget::updateEditor()
{
    //
    extern EventFilter *eventFilter;
    //
    using PD=array<int,2>;
    //
    static int preHeight = -1;
    static int needSave = 1;
    //
    double DR = ui->Game->TranGlobalPosToDR(eventFilter->MouseX(),eventFilter->MouseY());
    double UR = ui->Game->TranGlobalPosToUR(eventFilter->MouseX(),eventFilter->MouseY());
    int L = DR / BLOCKSIDELENGTH, U = UR / BLOCKSIDELENGTH;
    if (L < 0 || L >= MAP_L || U < 0 || U >= MAP_U)return;
    // 如果左边一直被摁住
    if (eventFilter->LeftMousePress()) {
        if (needSave) {
            needSave = 0;
            // SaveCurrentState();
        }
        switch (currentSelected) {
        case HIGHTERLAND:
            if (preHeight == -1)preHeight = map->cell[L][U].getMapHeight() + 1;
            HigherLand(L, U, preHeight);
            break;
        case OCEAN:
            MakeOcean(L, U);
            break;
        case LOWERLAND:
            if (preHeight == -1)preHeight = map->cell[L][U].getMapHeight() - 1;
            if (preHeight >= 0)LowerLand(L, U, preHeight);
            break;
        case DELETEOBJECT:
            clearArea(L, U);
            break;
        case FLAT:
            DeleteOcean(L, U);
            break;
        default:
            needSave = 1;
            delete ui->Game->RollBackState();
            break;
        }
    }
    else {
        needSave = 1;
        preHeight = -1;
    }
    // 单击生成
    if (eventFilter->LeftMouseClicked()) {
        // SaveCurrentState();
        //
        switch (currentSelected) {
        case TREE:
            MakeTree(mouseEvent->GetDR(), mouseEvent->GetUR());
            break;
        case GOLDORE:
            MakeStaticRes(L, U, GOLDORE);
            break;
        case STONM:
            MakeStaticRes(L, U, STONM);
            break;
        case ELEPHANT:
            MakeAnimal(mouseEvent->GetDR(), mouseEvent->GetUR(), ELEPHANT);
            break;
        case LION:
            MakeAnimal(mouseEvent->GetDR(), mouseEvent->GetUR(), LION);
            break;
        case GAZELLE:
            MakeAnimal(mouseEvent->GetDR(), mouseEvent->GetUR(), GAZELLE);
            break;
        case PLAYERDOWNTOWN:
            MakeBuilding(L, U, PLAYERDOWNTOWN);
            break;
        case AIARROWTOWER:
            MakeBuilding(L, U, AIARROWTOWER);
            break;
        case PLAYERFARMER:
            MakeHuman(mouseEvent->GetDR(), mouseEvent->GetUR(), PLAYERFARMER);
            break;
        case AICLUBMAN:
            MakeHuman(mouseEvent->GetDR(), mouseEvent->GetUR(), AICLUBMAN);
            break;
        case AIBOWMAN:
            MakeHuman(mouseEvent->GetDR(), mouseEvent->GetUR(), AIBOWMAN);
            break;
        case AISCOUT:
            MakeHuman(mouseEvent->GetDR(), mouseEvent->GetUR(), AISCOUT);
            break;
        case PLAYERDOCK:
            MakeBuilding(L, U, PLAYERDOCK);
            break;
        case AIWARSHIP:
            MakeHuman(mouseEvent->GetDR(), mouseEvent->GetUR(), AIWARSHIP);
            break;
        case PLAYERFISHINGBOAT:
            MakeHuman(mouseEvent->GetDR(), mouseEvent->GetUR(), PLAYERFISHINGBOAT);
            break;
        case PLAYERTRANSPORTSHIP:
            MakeHuman(mouseEvent->GetDR(), mouseEvent->GetUR(), PLAYERTRANSPORTSHIP);
            break;
        case PLAYERFISHERY:
            MakeStaticRes(L, U, PLAYERFISHERY);
            break;
        default:
            delete ui->Game->RollBackState();
            break;
        }
    }
    //更新区域绘制
    rectArea->SetFilter(currentSelected!=RECT_AREA);
    rectArea->Draw();
    circleArea->SetFilter(currentSelected!=CIRCLE_AREA);
    circleArea->Draw();
    lineArea->SetFilter(currentSelected!=LINE_AREA);
    lineArea->Draw();
}

void MainWidget::clearArea(int blockL, int blockU, int radius) {
    // 保存当前状态以支持撤销
    // SaveCurrentState();

    // 清空静态资源（金矿、石堆）
    auto& staticResList = map->staticres;
    auto itStatic = staticResList.begin();
    while (itStatic != staticResList.end()) {
        int x = (*itStatic)->getBlockDR();
        int y = (*itStatic)->getBlockUR();
        if (abs(x - blockL) <= radius && abs(y - blockU) <= radius) {
            delete* itStatic;
            itStatic = staticResList.erase(itStatic);
        }
        else {
            ++itStatic;
        }
    }

    // 清空动物（树、瞪羚等）
    auto& animalList = map->animal;
    auto itAnimal = animalList.begin();
    while (itAnimal != animalList.end()) {
        int x = (*itAnimal)->getBlockDR();
        int y = (*itAnimal)->getBlockUR();
        if (abs(x - blockL) <= radius && abs(y - blockU) <= radius) {
            delete* itAnimal;
            itAnimal = animalList.erase(itAnimal);
        }
        else {
            ++itAnimal;
        }
    }

    // 清空玩家建筑 - 修复版本
    for (int i = 0; i < MAXPLAYER; ++i) {
        auto& buildList = player[i]->build;
        auto itBuild = buildList.begin();
        while (itBuild != buildList.end()) {
            int x = (*itBuild)->getBlockDR();
            int y = (*itBuild)->getBlockUR();
            if (abs(x - blockL) <= radius && abs(y - blockU) <= radius) {
                // 先从map_Object中移除建筑，避免悬空指针
                Coordinate* buildingToDelete = *itBuild;
                for(int mapX = buildingToDelete->getBlockDR(); 
                    mapX < buildingToDelete->getBlockDR() + buildingToDelete->get_BlockSizeLen(); 
                    mapX++) {
                    for(int mapY = buildingToDelete->getBlockUR(); 
                        mapY < buildingToDelete->getBlockUR() + buildingToDelete->get_BlockSizeLen(); 
                        mapY++) {
                        if(mapX >= 0 && mapX < MAP_L && mapY >= 0 && mapY < MAP_U) {
                            auto& objects = map->map_Object[mapX][mapY];
                            objects.erase(remove(objects.begin(), objects.end(), buildingToDelete), objects.end());
                        }
                    }
                }
                
                delete* itBuild;
                itBuild = buildList.erase(itBuild);
            }
            else {
                ++itBuild;
            }
        }
    }

    // 更新障碍物地图和资源地图
    map->loadBarrierMap(true);
    map->reset_resMap_AI();
    ui->Game->update();  // 触发界面重绘
}


// void MainWidget::SaveCurrentState()
// {
//     GameState* state = new GameState;
//     ////////////////////////
//     //保存地形
//     for (int i = 0;i < MAP_L;++i)for (int j = 0;j < MAP_U;++j)state->cell[i][j] = map->cell[i][j];
//     for (int i = 0;i < GENERATE_L;++i)for (int j = 0;j < GENERATE_U;++j)state->m_heightMap[i][j] = map->m_heightMap[i][j];
//     //保存我方人物信息
//     {
//         Player* p = player[0];
//         state->myBuilding = p->build;
//         state->myHuman = p->human;
//     }
//     //保存敌方信息
//     {
//         Player* p = player[1];
//         state->myBuilding = p->build;
//         state->myHuman = p->human;
//     }
//     state->myHuman = player[0]->human;
//     state->myBuilding = player[0]->build;
//     state->enemyHuman = player[1]->human;
//     state->enemyBuilding = player[1]->build;
//     state->animal = map->animal;
//     state->resource = map->staticres;
//     //
//     ////////////////////////
//     ui->Game->SaveCurrentState(state);
// }


void MainWidget::HigherLand(int blockL, int blockU, int height)
{
    static const int width = 3;
    static const int half = width / 2;
    /////////////////////////////////////先去检查能不能去拔高(即周围不能有比他高，或者比他矮低于1的方块)
    for (int i = -half;i <= half;++i) {
        for (int j = -half;j <= half;++j) {
            int ll = blockL + i, uu = blockU + j;
            if (ll >= 0 && uu >= 0 && ll < MAP_L && uu < MAP_U) {
                for (int i = -2;i <= 2;++i) {
                    for (int j = -2;j <= 2;++j) {
                        int l = ll + i, u = uu + j;
                        if (l >= 0 && u >= 0 && l < MAP_L && u < MAP_U) {
                            int h1 = map->cell[l][u].getMapHeight();
                            if (height < h1 || height - h1>1)return;
                        }
                    }
                }
            }
        }
    }
    /////////////////////////////////////
    for (int i = -half;i <= half;++i) {
        for (int j = -half;j <= half;++j) {
            int ll = blockL + i, uu = blockU + j;
            if (ll >= 0 && uu >= 0 && ll < MAP_L && uu < MAP_U) {
                map->m_heightMap[ll + 4][uu + 4] = height;
                map->cell[ll][uu].setMapHeight(height);
                map->cell[ll][uu].reset();
            }
        }
    }
    for (int i = 0;i < MAP_L;++i) {
        for (int j = 0;j < MAP_U;++j) {
            map->cell[i][j].resetOffset();
        }
    }
    map->GenerateType();
    map->CalOffset();
    map->InitFaultHandle();
    ///////////////////////////////////////////////
}

void MainWidget::LowerLand(int blockL, int blockU, int height)
{
    static const int width = 3;
    static const int half = width / 2;
    /////////////////////////////////////先去检查能不能去压低
    for (int i = -half;i <= half;++i) {
        for (int j = -half;j <= half;++j) {
            int ll = blockL + i, uu = blockU + j;
            if (ll >= 0 && uu >= 0 && ll < MAP_L && uu < MAP_U) {
                for (int i = -2;i <= 2;++i) {
                    for (int j = -2;j <= 2;++j) {
                        int l = ll + i, u = uu + j;
                        if (l >= 0 && u >= 0 && l < MAP_L && u < MAP_U) {
                            int h1 = map->cell[l][u].getMapHeight();
                            if (height > h1 || h1 - height > 1)return;
                        }
                    }
                }
            }
        }
    }
    /////////////////////////////////////
    for (int i = -half;i <= half;++i) {
        for (int j = -half;j <= half;++j) {
            int ll = blockL + i, uu = blockU + j;
            if (ll >= 0 && uu >= 0 && ll < MAP_L && uu < MAP_U) {
                map->m_heightMap[ll + 4][uu + 4] = height;
                map->cell[ll][uu].setMapHeight(height);
                map->cell[ll][uu].reset();
            }
        }
    }
    for (int i = 0;i < MAP_L;++i) {
        for (int j = 0;j < MAP_U;++j) {
            map->cell[i][j].resetOffset();
        }
    }
    ///////////////////////////////////////////////
    map->GenerateType();
    map->CalOffset();
    map->InitFaultHandle();
    ///////////////////////////////////////////////
}

void MainWidget::MakeOcean(int blockL, int blockU)
{
    static const int width = 3;
    static const int half = width / 2;
    for (int i = -half;i <= half;++i) {
        for (int j = -half;j <= half;++j) {
            int ll = blockL + i, uu = blockU + j;
            if (ll >= 0 && uu >= 0 && ll < MAP_L && uu < MAP_U) {
                map->m_heightMap[ll + 4][uu + 4] = MAPHEIGHT_OCEAN;
                map->cell[ll][uu].setMapHeight(MAPHEIGHT_OCEAN);
                map->cell[ll][uu].setOffsetX(0);
                map->cell[ll][uu].setOffsetY(0);
            }
        }
    }
    map->GenerateType();
    for (int i = -half;i <= half;++i) {
        for (int j = -half;j <= half;++j) {
            int ll = blockL + i, uu = blockU + j;
            if (ll >= 0 && uu >= 0 && ll < MAP_L && uu < MAP_U) {
                map->CalCellOffset(ll, uu);
            }
        }
    }
    map->InitFaultHandle();
}


void MainWidget::DeleteOcean(int blockL, int blockU)
{
    static const int width = 3;
    static const int half = width / 2;
    for (int i = -half;i <= half;++i) {
        for (int j = -half;j <= half;++j) {
            int ll = blockL + i, uu = blockU + j;
            if (ll >= 0 && uu >= 0 && ll < MAP_L && uu < MAP_U) {
                map->m_heightMap[ll + 4][uu + 4] = MAPHEIGHT_FLAT;
                map->cell[ll][uu].setMapHeight(MAPHEIGHT_FLAT);
                map->cell[ll][uu].setOffsetX(0);
                map->cell[ll][uu].setOffsetY(0);
                map->ResetMapType(ll, uu);
            }
        }
    }

    map->GenerateType();
    for (int i = -half;i <= half;++i) {
        for (int j = -half;j <= half;++j) {
            int ll = blockL + i, uu = blockU + j;
            if (ll >= 0 && uu >= 0 && ll < MAP_L && uu < MAP_U) {
                map->CalCellOffset(ll, uu);
            }
        }
    }
    map->InitFaultHandle();
}

void MainWidget::MakeTree(double DR, double UR)
{
    static const float minus = 1.0;
    std::list<Animal*>& list = map->animal;
    //去判断该位置是不是已经种了树了
    int L = DR / BLOCKSIDELENGTH, U = UR / BLOCKSIDELENGTH;
    for (Animal* res : list) {
        if (res->getBlockDR() == L && res->getBlockUR() == U) return;
    }
    map->addAnimal(0, DR, UR); // 树
}

void MainWidget::MakeStaticRes(int blockL, int blockU, int type)
{
    //////////////////////////////去重
    for (StaticRes* res : map->staticres) {
        if (res->getBlockDR() == blockL && res->getBlockUR() == blockU) return;
    }
    //////////////////////////////
    if (type == GOLDORE)
    {
        map->addStaticRes(2, blockL, blockU);
    }
    else if (type == STONM)
    {
        map->addStaticRes(1, blockL, blockU);
    }
    else if (type == PLAYERFISHERY)
    {
        map->addStaticRes(3, blockL, blockU);
    }
}

void MainWidget::MakeAnimal(double DR, double UR, int type)
{

    int finalType = -1;
    if (type == ELEPHANT)finalType = 2;
    else if (type == LION)finalType = 3;
    else if (type == GAZELLE)finalType = 1;
    /////////////////////////////////
    if (finalType != -1)
        map->addAnimal(finalType, DR, UR);
}

void MainWidget::MakeBuilding(int blockL, int blockU, int type)
{
    for (Building* build : player[0]->build) {
        if (build->getBlockDR() == blockL && build->getBlockUR() == blockU)return;
    }
    for (Building* build : player[1]->build) {
        if (build->getBlockDR() == blockL && build->getBlockUR() == blockU)return;
    }
    //////////////////判断类型
    if (type == PLAYERDOWNTOWN) {
        player[0]->addBuilding(BUILDING_CENTER, blockL, blockU, 100);
    }
    else if (type == AIARROWTOWER) {
        player[1]->addBuilding(BUILDING_ARROWTOWER, blockL, blockU, 100);
    }
    else if (type == PLAYERDOCK) {
        player[0]->addBuilding(BUILDING_DOCK, blockL, blockU, 100);
    }
}

void MainWidget::MakeHuman(double DR, double UR, int type)
{
    if (type == PLAYERFARMER) {
        player[0]->addFarmer(DR, UR);
    }
    else if (type == AISCOUT) {
        player[1]->addArmy(AT_SCOUT, DR, UR);
    }
    else if (type == AICLUBMAN) {
        player[1]->addArmy(AT_CLUBMAN, DR, UR);
    }
    else if (type == AIBOWMAN) {
        player[1]->addArmy(AT_BOWMAN, DR, UR);
    }
    else if (type == AIWARSHIP) {
        player[1]->addArmy(AT_SHIP, DR, UR);
    }
    else if (type == PLAYERFISHINGBOAT) {
        player[0]->addShip(FARMERTYPE_WOOD_BOAT, DR, UR);
    }
    else if (type == PLAYERTRANSPORTSHIP) {
        player[0]->addShip(FARMERTYPE_SAILING, DR, UR);
    }
}



//***************InitHelperFunctionBegin**************
void MainWidget::initGameResources() {
    qDebug() << "游戏资源初始化...";
    InitImageResMap(RESPATH);   // 图像资源
    InitSoundResMap(RESPATH);   // 音频资源
}

void MainWidget::initGameElements() {
    qDebug() << "游戏元素初始化...";
    initBlock();
    initBuilding();
    initAnimal();
    initStaticResource();
    initFarmer();
    initArmy();
    initMissile();
}

void MainWidget::initWindowProperties() {
    qDebug() << "游戏窗口初始化...";
    this->setFixedSize(GAME_WIDTH, GAME_HEIGHT);
    this->setWindowTitle("Age Of Empires");
    this->setWindowIcon(QIcon());
}

void MainWidget::initOptions() {
    qDebug() << "窗口选项初始化...";
    //"设置"选项卡
    option = new Option();
    option->setModal(true);
    //"关于我们"选项卡
    aboutDialog = new AboutDialog(this);
    //倍速按钮组
    pbuttonGroup = new QButtonGroup(this);
    pbuttonGroup->addButton(ui->radioButton_1, 0);
    pbuttonGroup->addButton(ui->radioButton_2, 1);
    pbuttonGroup->addButton(ui->radioButton_4, 2);
    pbuttonGroup->addButton(ui->radioButton_8, 3);
    //绑定倍速按钮
    connect(ui->radioButton_1, SIGNAL(clicked()), this, SLOT(onRadioClickSlot()));
    connect(ui->radioButton_2, SIGNAL(clicked()), this, SLOT(onRadioClickSlot()));
    connect(ui->radioButton_4, SIGNAL(clicked()), this, SLOT(onRadioClickSlot()));
    connect(ui->radioButton_8, SIGNAL(clicked()), this, SLOT(onRadioClickSlot()));
    //绑定设置按钮
    connect(ui->option, &QPushButton::clicked, option, &QDialog::show);
    connect(option, &Option::changeMusic, this, &MainWidget::responseMusicChange);
    connect(option, &Option::request_ClearDebugText, this, &MainWidget::clearDebugText);
    connect(option, &Option::request_exportTreeBlock, this, &MainWidget::exportDebugTextTreeBlock);
    connect(option, &Option::request_exportTxt, this, &MainWidget::exportDebugTextTxt);
    connect(option, &Option::request_exportClear, this, &MainWidget::clearDebugTextFile);
    connect(option->btnTreeBlock,&QPushButton::clicked,option,&Option::on_exportTreeBlock_clicked);
    //隐藏组件
    option->hide();
    option->btnSelect->hide();
    option->btnLine->hide();
    option->btnPos->hide();
    option->btnOverlap->hide();
    aboutDialog->hide();
}

void MainWidget::initInfoPane() {
    qDebug() << "游戏实体属性框初始化...";
    sel = new SelectWidget(this);
    sel->move(20, 810);
    sel->show();

    ActWidget* acts_[ACT_WINDOW_NUM_FREE] = { ui->interact1, ui->interact2, ui->interact3, ui->interact4, ui->interact5, ui->interact6, ui->interact7, ui->interact8, ui->interact9, ui->interact10,ui->interact11,ui->interact12 };
    for (int i = 0; i < ACT_WINDOW_NUM_FREE; i++) {
        acts[i] = acts_[i];
        acts[i]->setStatus(0);
        acts[i]->setNum(i);
        acts[i]->hide();
        acts[i]->setAttribute(Qt::WA_Hover, true);
        acts[i]->installEventFilter(this);
        connect(acts[i], SIGNAL(actPress(int)), sel, SLOT(widgetAct(int)));
    }

    connect(ui->Game, SIGNAL(sendView(int, int, int)), sel, SLOT(getBuild(int, int, int)));
    connect(sel, SIGNAL(sendBuildMode(int)), ui->Game, SLOT(setBuildMode(int)));
}

void MainWidget::initGameTimer() {
    qDebug() << "初始化计时器...";
    timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);
    timer->start(40);
    //时间增加
    connect(timer, &QTimer::timeout, sel, &SelectWidget::frameUpdate);
    connect(timer, SIGNAL(timeout()), this, SLOT(FrameUpdate()));
}

void MainWidget::initPlayers() {
    qDebug() << "初始化玩家...";
    // 开辟玩家空间
    for (int i = 0; i < MAXPLAYER; i++) {
        player[i] = new Player(i);
    }
    //设置初始科技
    // player[0]->set_AllTechnology();
    player[1]->set_AllTechnology();
    //设置初始时代
    player[0]->setCiv(CIVILIZATION_STONEAGE);
    //设置初始资源
    // player[0]->changeResource(10000, 10000, 10000, 500);
    // player[1]->addArmy(AT_SCOUT , 35*BLOCKSIDELENGTH , 35*BLOCKSIDELENGTH);
}

void MainWidget::initMap(int MapJudge) {
    qDebug() << "初始化地图...";
    map = new Map;
    map->setPlayer(player);
    map->init(MapJudge);
    map->init_Map_Height();

    // 内存图开辟空间
    memorymap = new int* [MEMORYROW];
    for (int i = 0; i < MEMORYROW; i++) {
        memorymap[i] = new int[MEMORYCOLUMN];
    }
    map->loadResource();
    // buildInitialStock();
}

void MainWidget::initAI() {
    qDebug() << "加载AI...";
    UsrAi = new UsrAI();
    EnemyAi = new EnemyAI();
    connect(this, &MainWidget::startAI, UsrAi, &AI::startProcessing);
    connect(this, &MainWidget::startAI, EnemyAi, &AI::startProcessing);
    connect(UsrAi, &AI::cheatAttack, EnemyAi, &EnemyAI::onWaveAttack);
    connect(UsrAi, &UsrAI::cheatRes, this, &MainWidget::cheat_Player0Resource);
    UsrAi->start();
    EnemyAi->start();
}

void MainWidget::setupCore() {
    qDebug() << "加载内核...";
    core = new Core(map, player, memorymap, mouseEvent);
    sel->setCore(core);
    core->sel = sel;
}

void MainWidget::setupMouseTracking() {
    qDebug() << "设置鼠标跟踪...";
    ui->Game->setMouseTracking(true);
    ui->Game->setAttribute(Qt::WA_MouseTracking, true);
    ui->Game->installEventFilter(this);
}

void MainWidget::setupTipLabel() {
    qDebug() << "设置信息栏文本颜色...";
    QPalette pe;
    pe.setColor(QPalette::WindowText, Qt::green);
    ui->tip->setPalette(pe);
    tipLbl = ui->tip;
}

void MainWidget::initBGM() {
    qDebug() << "加载背景音乐...";
    bgm = SoundMap["BGM"];
    if (bgm != NULL) {
        bgm->setLoopCount(QSoundEffect::Infinite);
        responseMusicChange();
    }
}

void MainWidget::initViewMap() {
    qDebug() << "初始化小地图...";
    ui->mapView->setFriendlyFarmerList(&(player[0]->human));
    ui->mapView->setEnemyFarmerList(&(player[1]->human));
    ui->mapView->setFriendlyBuildList(&(player[0]->build));
    ui->mapView->setEnemyBuildList(&(player[1]->build));
    ui->mapView->setAnimalList(&(map->animal));
    ui->mapView->setResList(&(map->staticres));
}
//**************InitHelperFunctionEnd**************


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

void MainWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    QPixmap pix;
    pix = resMap["Interface"].front();
    painter.drawPixmap(0, 0, 1440, 45, pix);
    pix = resMap["Interface"].back();
    painter.drawPixmap(0, GAME_HEIGHT - 203.5, 1440, 203.5, pix);

}


// 初始化区块
void MainWidget::initBlock()
{
    for (int num = 0;num < BLOCK_COUNT;num++)
    {
        Block::allocateblock(num);
        loadResource(Block::getBlockname(num), Block::getblock(num));
        Block::allocategrayblock(num);
        loadGrayRes(Block::getblock(num), Block::getgrayblock(num));
        Block::allocateblackblock(num);
        loadBlackRes(Block::getblock(num), Block::getblackblock(num));
    }
}

// 初始化建筑
void MainWidget::initBuilding()
{
    for (int i = 0; i < 4; i++)
    {
        Building::allocatebuild(i);
        loadResource(Building::getBuildingname(i), Building::getBuild(i));
    }
    for (int i = 1; i < 3; i++)
    {
        for (int j = 0;j < 11;j++)
        {
            Building::allocatebuilt(i, j);
            loadResource(Building::getBuiltname(i, j), Building::getBuilt(i, j));
        }
    }

    for (int type = 0; type < 3; type++)
    {
        Building::allocatebuildFire(type);
        loadResource(Building::getBuildingFireName(type), Building::getBuildFire(type));
    }

    //市镇中心
    Building::setActNames(BUILDING_CENTER, 0, ACT_CREATEFARMER);
    Building::setActNames(BUILDING_CENTER, 1, ACT_UPGRADE_AGE);
    //谷仓
    Building::setActNames(BUILDING_GRANARY, 0, ACT_RESEARCH_WALL);
    Building::setActNames(BUILDING_GRANARY, 1, ACT_UPGRADE_TOWERBUILD);
    //仓库
    Building::setActNames(BUILDING_STOCK, 0, ACT_STOCK_UPGRADE_USETOOL);
    Building::setActNames(BUILDING_STOCK, 1, ACT_STOCK_UPGRADE_DEFENSE_INFANTRY);
    Building::setActNames(BUILDING_STOCK, 2, ACT_STOCK_UPGRADE_DEFENSE_ARCHER);
    Building::setActNames(BUILDING_STOCK, 3, ACT_STOCK_UPGRADE_DEFENSE_RIDER);
    //市场
    Building::setActNames(BUILDING_MARKET, 0, ACT_UPGRADE_WOOD);
    Building::setActNames(BUILDING_MARKET, 1, ACT_UPGRADE_STONE);
    Building::setActNames(BUILDING_MARKET, 2, ACT_UPGRADE_FARM);
    Building::setActNames(BUILDING_MARKET, 3, ACT_UPGRADE_GOLD);
    //军队
    Building::setActNames(BUILDING_ARMYCAMP, 0, ACT_ARMYCAMP_CREATE_CLUBMAN);
    Building::setActNames(BUILDING_ARMYCAMP, 5, ACT_ARMYCAMP_UPGRADE_CLUBMAN);
    Building::setActNames(BUILDING_ARMYCAMP, 3, ACT_ARMYCAMP_CREATE_SLINGER);
    Building::setActNames(BUILDING_RANGE, 0, ACT_RANGE_CREATE_BOWMAN);
    Building::setActNames(BUILDING_STABLE, 0, ACT_STABLE_CREATE_SCOUT);
    //船坞
    Building::setActNames(BUILDING_DOCK, 0, ACT_DOCK_CREATE_SAILING);
    Building::setActNames(BUILDING_DOCK, 1, ACT_DOCK_CREATE_WOOD_BOAT);
    Building::setActNames(BUILDING_DOCK, 2, ACT_DOCK_CREATE_SHIP);
}

// 初始化动物
void MainWidget::initAnimal()
{
    for (int num = 0;num < 5;num++)
    {
        if (num == ANIMAL_TREE)
        {
            Animal::allocateStand(num, 0);
            Animal::allocateDie(num, 0);
            loadResource(Animal::getAnimalName(num), Animal::getStand(num, 0));
            loadResource(Animal::getAnimalcarcassname(num), Animal::getDie(num, 0));
            continue;
        }
        else if (num == ANIMAL_FOREST)
        {
            Animal::allocateStand(num, 0);
            Animal::allocateDie(num, 0);
            loadResource(Animal::getAnimalName(num), Animal::getStand(num, 0));
            loadResource(Animal::getAnimalcarcassname(num), Animal::getDie(num, 0));
            continue;
        }
        if (num == ANIMAL_GAZELLE || num == ANIMAL_LION)
        {
            for (int i = 0;i <= 4;i++)
            {
                Animal::allocateRun(num, i);
                loadResource(Animal::getAnimalName(num) + "_Run_" + direction[i], Animal::getRun(num, i));
            }
            for (int i = 5;i < 8;i++)
            {
                Animal::allocateRun(num, i);
                flipResource(Animal::getRun(num, 8 - i), Animal::getRun(num, i));
            }
        }
        for (int i = 0;i <= 4;i++)
        {
            Animal::allocateAttack(num, i);
            Animal::allocateWalk(num, i);
            Animal::allocateStand(num, i);
            Animal::allocateDie(num, i);
            Animal::allocateDisappear(num, i);
            loadResource(Animal::getAnimalName(num) + "_Stand_" + direction[i], Animal::getStand(num, i));
            loadResource(Animal::getAnimalName(num) + "_Walk_" + direction[i], Animal::getWalk(num, i));
            loadResource(Animal::getAnimalName(num) + "_Attack_" + direction[i], Animal::getAttack(num, i));
            loadResource(Animal::getAnimalName(num) + "_Die_" + direction[i], Animal::getDie(num, i));
            loadResource(Animal::getAnimalName(num) + "_Disappear_" + direction[i], Animal::getDisappear(num, i));
        }
        for (int i = 5;i < 8;i++)
        {
            Animal::allocateAttack(num, i);
            Animal::allocateWalk(num, i);
            Animal::allocateStand(num, i);
            Animal::allocateDie(num, i);
            Animal::allocateDisappear(num, i);
            flipResource(Animal::getWalk(num, 8 - i), Animal::getWalk(num, i));
            flipResource(Animal::getStand(num, 8 - i), Animal::getStand(num, i));
            flipResource(Animal::getAttack(num, 8 - i), Animal::getAttack(num, i));
            flipResource(Animal::getDie(num, 8 - i), Animal::getDie(num, i));
            flipResource(Animal::getDisappear(num, 8 - i), Animal::getDisappear(num, i));
        }
    }
}

// 初始化不可移动的资源
void MainWidget::initStaticResource()
{
    for (int num = 0; num < 4; num++)
    {
        StaticRes::allocateStaticResource(num);
        loadResource(StaticRes::getStaticResName(num), StaticRes::getStaticResource(num));
    }
}

// 初始化农民状态
void MainWidget::initFarmer()
{
    //加载素材
    //"Villager","Lumber","Gatherer","Miner","Hunter","Farmer","Worker","Fisher"

    for (int statei = 0;statei < 8;statei++)
    {
        for (int i = 0;i <= 4;i++)
        {
            Farmer::allocateWalk(statei, i);
            Farmer::allocateStand(statei, i);
            Farmer::allocateDie(statei, i);
            Farmer::allocateDisappear(statei, i);
            loadResource(Farmer::getFarmerName(statei) + "_Stand_" + direction[i], Farmer::getStand(statei, i));
            loadResource(Farmer::getFarmerName(statei) + "_Walk_" + direction[i], Farmer::getWalk(statei, i));
            loadResource(Farmer::getFarmerName(statei) + "_Die_" + direction[i], Farmer::getDie(statei, i));
            loadResource(Farmer::getFarmerName(statei) + "_Disappear_" + direction[i], Farmer::getDisappear(statei, i));
        }
        for (int i = 5;i < 8;i++)
        {
            Farmer::allocateWalk(statei, i);
            Farmer::allocateStand(statei, i);
            Farmer::allocateDie(statei, i);
            Farmer::allocateDisappear(statei, i);
            flipResource(Farmer::getWalk(statei, 8 - i), Farmer::getWalk(statei, i));
            flipResource(Farmer::getStand(statei, 8 - i), Farmer::getStand(statei, i));
            flipResource(Farmer::getDie(statei, 8 - i), Farmer::getDie(statei, i));
            flipResource(Farmer::getDisappear(statei, 8 - i), Farmer::getDisappear(statei, i));
        }
    }
    // Work
    for (int statei = 0;statei < 8;statei++)
    {
        if (statei == 0)
        {
            continue;
        }
        for (int i = 0;i <= 4;i++)
        {
            Farmer::allocateWork(statei, i);
            loadResource(Farmer::getFarmerName(statei) + "_Work_" + direction[i], Farmer::getWork(statei, i));
        }
        for (int i = 5;i < 8;i++)
        {
            Farmer::allocateWork(statei, i);
            flipResource(Farmer::getWork(statei, 8 - i), Farmer::getWork(statei, i));
        }
    }
    // Attack
    for (int statei = 0;statei < 8;statei++)
    {
        if (statei == 2 || statei == 3 || statei == 5 || statei == 6)
        {
            continue;
        }
        for (int i = 0;i <= 4;i++)
        {
            Farmer::allocateAttack(statei, i);
            loadResource(Farmer::getFarmerName(statei) + "_Attack_" + direction[i], Farmer::getAttack(statei, i));
        }
        for (int i = 5;i < 8;i++)
        {
            Farmer::allocateAttack(statei, i);
            flipResource(Farmer::getAttack(statei, 8 - i), Farmer::getAttack(statei, i));
        }
    }
    // Carry 考虑如何对接
    for (int statei = 0;statei <= 6;statei++)
    {
        if (statei == 0 || statei == 5)
        {
            continue;
        }
        for (int i = 0;i <= 4;i++)
        {
            Farmer::allocateCarry(statei, i);
            loadResource(Farmer::getFarmerCarry(statei) + "_" + direction[i], Farmer::getCarry(statei, i));
        }
        for (int i = 5;i < 8;i++)
        {
            Farmer::allocateCarry(statei, i);
            flipResource(Farmer::getCarry(statei, 8 - i), Farmer::getCarry(statei, i));
        }
    }
    //船
    string shipName[] = { "","Wood_Boat","Sailing" };
    for (int type = 1;type <= 2;type++)
    {
        string& sN = shipName[type];
        for (int i = 0;i <= 4;i++)
        {
            Farmer::allocateShipStand(type, i);
            loadResource(sN + "_Stand_" + direction[i], Farmer::getShipStand(type, i));
        }
        for (int i = 5;i < 8;i++)
        {
            Farmer::allocateShipStand(type, i);
            flipResource(Farmer::getShipStand(type, 8 - i), Farmer::getShipStand(type, i));
        }
    }
}

void MainWidget::initArmy()
{
    //加载素材
    //"Archer","Axeman","Clubman","Scout"

    // Stand Walk Die
    for (int statei = 0;statei < 8;statei++)
    {
        for (int level = 0; level < 2;level++)
        {
            for (int i = 0;i <= 4;i++)
            {
                Army::allocateWalk(0, statei, level, i);
                Army::allocateStand(0, statei, level, i);
                Army::allocateDie(0, statei, level, i);
                Army::allocateDisappear(0, statei, level, i);
                Army::allocateAttack(0, statei, level, i);
                loadResource(Army::getArmyName(statei, level) + "_Attack_" + direction[i], Army::getAttack(0, statei, level, i));
                loadResource(Army::getArmyName(statei, level) + "_Disappear_" + direction[i], Army::getDisappear(0, statei, level, i));
                loadResource(Army::getArmyName(statei, level) + "_Stand_" + direction[i], Army::getStand(0, statei, level, i));
                loadResource(Army::getArmyName(statei, level) + "_Walk_" + direction[i], Army::getWalk(0, statei, level, i));
                loadResource(Army::getArmyName(statei, level) + "_Die_" + direction[i], Army::getDie(0, statei, level, i));

                Army::allocateWalk(1, statei, level, i);
                Army::allocateStand(1, statei, level, i);
                Army::allocateDie(1, statei, level, i);
                Army::allocateDisappear(1, statei, level, i);
                Army::allocateAttack(1, statei, level, i);
                loadResource("Enemy_" + Army::getArmyName(statei, level) + "_Attack_" + direction[i], Army::getAttack(1, statei, level, i));
                loadResource("Enemy_" + Army::getArmyName(statei, level) + "_Disappear_" + direction[i], Army::getDisappear(1, statei, level, i));
                loadResource("Enemy_" + Army::getArmyName(statei, level) + "_Stand_" + direction[i], Army::getStand(1, statei, level, i));
                loadResource("Enemy_" + Army::getArmyName(statei, level) + "_Walk_" + direction[i], Army::getWalk(1, statei, level, i));
                loadResource("Enemy_" + Army::getArmyName(statei, level) + "_Die_" + direction[i], Army::getDie(1, statei, level, i));
            }
            for (int i = 5;i < 8;i++)
            {
                Army::allocateWalk(0, statei, level, i);
                Army::allocateStand(0, statei, level, i);
                Army::allocateDie(0, statei, level, i);
                Army::allocateDisappear(0, statei, level, i);
                Army::allocateAttack(0, statei, level, i);
                flipResource(Army::getAttack(0, statei, level, 8 - i), Army::getAttack(0, statei, level, i));
                flipResource(Army::getDisappear(0, statei, level, 8 - i), Army::getDisappear(0, statei, level, i));
                flipResource(Army::getWalk(0, statei, level, 8 - i), Army::getWalk(0, statei, level, i));
                flipResource(Army::getStand(0, statei, level, 8 - i), Army::getStand(0, statei, level, i));
                flipResource(Army::getDie(0, statei, level, 8 - i), Army::getDie(0, statei, level, i));

                Army::allocateWalk(1, statei, level, i);
                Army::allocateStand(1, statei, level, i);
                Army::allocateDie(1, statei, level, i);
                Army::allocateDisappear(1, statei, level, i);
                Army::allocateAttack(1, statei, level, i);
                flipResource(Army::getAttack(1, statei, level, 8 - i), Army::getAttack(1, statei, level, i));
                flipResource(Army::getDisappear(1, statei, level, 8 - i), Army::getDisappear(1, statei, level, i));
                flipResource(Army::getWalk(1, statei, level, 8 - i), Army::getWalk(1, statei, level, i));
                flipResource(Army::getStand(1, statei, level, 8 - i), Army::getStand(1, statei, level, i));
                flipResource(Army::getDie(1, statei, level, 8 - i), Army::getDie(1, statei, level, i));
            }
        }
    }

}

void MainWidget::initMissile()
{
    //加载飞行物素材

    for (int statei = 0; statei < NUMBER_MISSILE; statei++)
    {
        Missile::allocatemissile(statei);
        loadResource(Missile::getMissilename(statei), Missile::getmissile(statei));
    }
}

// 删除区块资源
void MainWidget::deleteBlock()
{
    for (int i = 0;i < 1;i++)
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
        for (int j = 0;j < 10;j++)
        {
            Building::deallocatebuilt(i, j);
        }
    }

    for (int type = 0; type < 3; type++)
    {
        Building::deallocatebuildFire(type);
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
        if (num == 1 || num == 3)
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
            Animal::deallocateDisappear(num, i);
        }
        for (int i = 5; i < 8; i++)
        {
            Animal::deallocateAttack(num, i);
            Animal::deallocateWalk(num, i);
            Animal::deallocateStand(num, i);
            Animal::deallocateDie(num, i);
            Animal::deallocateDisappear(num, i);
        }
    }
}

// 删除不可移动的资源
void MainWidget::deleteStaticResource()
{
    for (int num = 0; num < 3; num++)
        StaticRes::deallocateStaticResource(num);
}

// 删除农民资源
void MainWidget::deleteFarmer()
{
    // 清理素材资源
    for (int statei = 0; statei < 7; statei++)
    {
        for (int i = 0; i < 8; i++)
        {
            Farmer::deallocateWalk(statei, i);
            Farmer::deallocateStand(statei, i);
            Farmer::deallocateDie(statei, i);
            Farmer::deallocateDisappear(statei, i);
        }
    }

    // 清理 Work 资源
    for (int statei = 0; statei < 7; statei++)
    {
        if (statei == 0)
        {
            continue;
        }

        for (int i = 0; i < 8; i++)
        {
            Farmer::deallocateWork(statei, i);
        }
    }

    // 清理 Attack 资源
    for (int statei = 0; statei < 7; statei++)
    {
        if (statei == 2 || statei == 3 || statei == 5 || statei == 6)
        {
            continue;
        }

        for (int i = 0; i < 8; i++)
        {
            Farmer::deallocateAttack(statei, i);
        }
    }

    // 清理 Carry 资源
    for (int statei = 0; statei <= 4; statei++)
    {
        if (statei == 0)
        {
            continue;
        }

        for (int i = 0; i < 8; i++)
        {
            Farmer::deallocateCarry(statei, i);
        }
    }

}

void MainWidget::deleteArmy()
{
    // 清理素材资源
    for (int statei = 0; statei < 7; statei++)
    {
        for (int level = 0;level < 2;level++)
        {
            for (int i = 0; i < 8; i++)
            {
                for (int re = 0; re < NOWPLAYER; re++)
                {
                    Army::deallocateWalk(re, statei, level, i);
                    Army::deallocateStand(re, statei, level, i);
                    Army::deallocateDie(re, statei, level, i);
                    Army::deallocateAttack(re, statei, level, i);
                    Army::deallocateDisappear(re, statei, level, i);
                }
            }
        }
    }
}

// 删除飞行物资源
void MainWidget::deleteMissile()
{
    for (int statei = 0; statei < NUMBER_MISSILE; statei++) Missile::deallocatemissile(statei);
}

bool MainWidget::eventFilter(QObject* watched, QEvent* event)
{
    for (int i = 0; i < 10; i++)
    {
        if (watched == acts[i] && !acts[i]->isHidden()) {
            if (event->type() == QEvent::HoverEnter) {
                ui->tip->setText(QString::fromStdString(actNames[sel->actions[i]]));
                if (acts[i]->getStatus() != 2)
                {
                    acts[i]->setStatus(0);
                    acts[i]->update();
                }
            }
            else if (event->type() == QEvent::MouseButtonRelease && acts[i]->getStatus() != 2) {
                acts[i]->setStatus(0);
            }
            else if (event->type() == QEvent::MouseButtonPress && acts[i]->getStatus() != 2) {
                acts[i]->setStatus(1);
            }
            else if (event->type() == QEvent::HoverLeave && acts[i]->getStatus() != 2)
            {
                ui->tip->setText("");
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void MainWidget::showPlayerResource(int playerRepresent)
{
    int wood, food, stone, gold;
    core->getPlayerNowResource(playerRepresent, wood, food, stone, gold);
    ui->resWood->setText(QString::number(wood));
    ui->resFood->setText(QString::number(food));
    ui->resStone->setText(QString::number(stone));
    ui->resGold->setText(QString::number(gold));
}

void MainWidget::statusUpdate()
{
    showPlayerResource(0);

    QFont currentFont = ui->score0->font();
    ui->score0->setTextFormat(Qt::RichText); // 确保使用富文本格式
    ui->score0->setText("<html><head/><body><p><span style=\" font-size:12pt; color:#00007f;\">"
        + QString::number(usrScore.getScore()) + "</span></p></body></html>");

    ui->score1->setTextFormat(Qt::RichText); // 确保使用富文本格式
    ui->score1->setText("<html><head/><body><p><span style=\" font-size:12pt; color:#aa0000;\">"
        + QString::number(enemyScore.getScore()) + "</span></p></body></html>");

    ui->mapView->screenL = ui->Game->getBlockDR();
    ui->mapView->screenU = ui->Game->getBlockUR();
    ui->statusLbl->setText(sel->getShowTime() + QString::fromStdString("\n"));
    ui->version->setText(QString::fromStdString(GAME_VERSION));
}

void MainWidget::gameDataUpdate()
{
    //
    if (!pause)
    {
        core->gameUpdate();
        //如果当前模式不是编辑器功能，那么不运行ai
        if(!EditorMode){
            core->infoShare();
            emit startAI();
        }
    }
    else
    {
        core->resetNowObject_Click(pause);
    }
    makeSound();
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
    return 0;
    return sel->getSecend() > GAME_LOSE_SEC;
}
bool MainWidget::isWin()
{
    auto* hero = player[0];
    return hero->getGold() >= GAME_WIN_GOLD;
}

void MainWidget::judgeVictory()
{
    if (isLoss())
    {
        //停止当前动作
        timer->stop();
        playSound("Lose");
        debugText("blue", " 游戏失败，未达成目标。最终得分为:" + QString::number(usrScore.getScore()));

        //弹出胜利提示
        if (isExamining || QMessageBox::information(this, QStringLiteral("游戏失败"), "很遗憾你没能成功保护部落。智慧之神为你惋惜~", QMessageBox::Ok))
        {
            //            setLose();
            ScoreSave("Lose");
            this->close();
        }
    }

    if (isWin())
    {
        //停止当前动作
        timer->stop();

        playSound("Win");
        debugText("blue", " 游戏胜利。最终得分为:" + QString::number(usrScore.getScore()));

        //弹出胜利提示
        if (isExamining || QMessageBox::information(this, QStringLiteral("游戏胜利"), "恭喜获胜，获得了纳西妲的青睐！", QMessageBox::Ok))
        {
            //            setWinning();
            ScoreSave("Win");
            this->close();
        }
    }
    else return;
}

void MainWidget::playSound()
{
    if (isExamining) return;
    soundPlayThread->AddSound(soundQueue);
    return;
}
void MainWidget::playSound(string s) {
    if (isExamining || SoundMap[s] == 0) return;
    SoundMap[s]->play();
    return;
}
void MainWidget::makeSound()
{
    if (soundQueue.empty()) return;

    if (!option->getSound())
    {
        queue<string> empty;
        swap(empty, soundQueue);
    }

    playSound();

    return;
}

void MainWidget::initEditor()
{
    //注册全局事件监听
    auto&e=::eventFilter;
    e->RegistReciver([&](){
        if(e->LeftMouseClicked()){
           int x=e->MouseX(),y=e->MouseY();
           mouseEvent->SetMemoeyMapX(x/4);
           mouseEvent->SetMemoryMapY(y/4);
           mouseEvent->SetMouseEventType(LEFT_PRESS);
           mouseEvent->SetDR(ui->Game->TranGlobalPosToDR(x,y));
           mouseEvent->SetUR(ui->Game->TranGlobalPosToUR(x,y));
        }
        else if(e->RightMouseClicked()){
            int x=e->MouseX(),y=e->MouseY();
            mouseEvent->SetMemoeyMapX(x/4);
            mouseEvent->SetMemoryMapY(y/4);
            mouseEvent->SetMouseEventType(RIGHT_PRESS);
            mouseEvent->SetDR(ui->Game->TranGlobalPosToDR(x,y));
            mouseEvent->SetUR(ui->Game->TranGlobalPosToUR(x,y));
        }
        //轮询编辑器
        if(EditorMode){
            updateEditor();
        }
    });
    //
    if(EditorMode){
        rectArea=new RectArea(ui->Game);
        circleArea=new CircleArea(ui->Game);
        lineArea=new LineArea(ui->Game);
    }
}

void MainWidget::ScoreSave(string gameResult)
{
    std::ofstream ScoreFile("GameScore.txt");
    if (ScoreFile.is_open())
    {
        std::string score = QString::number(usrScore.getScore()).toStdString();
        std::string time = QString::number(sel->getSecend()).toStdString();
        ScoreFile << gameResult << " " << score << " " << time;
        ScoreFile.close();
    }
    else
    {
        qDebug() << "open GameScore fail.";
    }
    return;
}
//**************槽函数***************
// 游戏帧更新
void MainWidget::FrameUpdate()
{
    judgeVictory();

    //打印debug栏
    respond_DebugMessage();

    if (!pause) gameframe++;
    g_frame = gameframe;
    sel->resetSecond();

    ui->lcdNumber->display(gameframe);

    if (mapmoveFrequency == 1 || mapmoveFrequency == 2) {
        paintUpdate();
    }
    else if (mapmoveFrequency == 4) {
        if (gameframe % 2 == 0 || pause) paintUpdate();
    }
    else if (mapmoveFrequency == 8) {
        if (gameframe % 3 == 0 || pause) paintUpdate();
    }
    else {
        qDebug() << "Speed setting error";
    }

    gameDataUpdate();

    return;
}
void MainWidget::onRadioClickSlot()
{
    static int interval = 40;
    switch (pbuttonGroup->checkedId())
    {
    case 0:
        timer->setInterval(interval);
        mapmoveFrequency = 1;
        break;
    case 1:
        mapmoveFrequency = 2;
        timer->setInterval(interval / mapmoveFrequency);
        break;
    case 2:
        mapmoveFrequency = 4;
        timer->setInterval(interval / mapmoveFrequency);
        break;
    case 3:
        mapmoveFrequency = 8;
        timer->setInterval(interval / mapmoveFrequency);
        nowobject = NULL;
        break;
    }
}

void MainWidget::cheat_Player0Resource()
{
    player[0]->changeResource(500, 500, 500, 500);
}

void MainWidget::on_stopButton_clicked()
{
    pause = !pause;

    if (pause) ui->stopButton->setText("继续");
    else ui->stopButton->setText("暂停");
}

void MainWidget::responseMusicChange()
{
    if (!isExamining && option->getMusic())
        bgm->play();
    else
        bgm->stop();
}
//***********************************************************************
//输出提示框
void MainWidget::respond_DebugMessage()
{
    std::map<QString, int>::iterator iter = debugMessageRecord.begin(), itere = debugMessageRecord.end();

    while (!debugMassagePackage.empty())
    {
        debugText(debugMassagePackage.front().color, debugMassagePackage.front().content);
        debugMassagePackage.pop();
    }

    while (iter != itere)
    {
        if (gameframe - iter->second > 200) iter = debugMessageRecord.erase(iter);
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
    else if (color == "black")
        ui->DebugTexter->insertHtml(COLOR_BLACK(sel->getShowTime() + content));

    ui->DebugTexter->insertPlainText("\n");
    QScrollBar* bar = ui->DebugTexter->verticalScrollBar();
    bar->setValue(bar->maximum());
}

void MainWidget::clearDebugText()
{
    ui->DebugTexter->clear();
}

void MainWidget::exportDebugTextTreeBlock()
{
    QFile file("TreeBlock.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
       qDebug() << "无法打开文件：" << file.errorString();
       return;
    }
    //导出我方人的位置
    using PD=std::array<int,2>;
    set<PD>pos;
    for(Human*human:player[0]->human){
        pos.insert({human->getBlockDR(),human->getBlockUR()});
    }
    //
    QTextStream out(&file);
    for(int i=0;i<MAP_L;++i){
       for(int j=0;j<MAP_U;++j){
           int tar=int(map->TreeBlock[i][j]);
           if(tar==0&&pos.count({i,j}))tar=3;
           out<<tar;
       }
       out<<endl;
    }

    //
    file.close();
    debugText("red","树林遮掩图导出成功!");
}

void MainWidget::exportDebugTextTxt()
{
    QString debugInfo = ui->DebugTexter->toPlainText();

    // 获取当前系统时间，用于命名文件
    QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");

    // 获取项目文件夹路径
    QString projectPath = QDir::currentPath();

    // 构建输出文件夹路径
    QString outputPath = QDir::cleanPath(projectPath + QDir::separator() + "output");

    // 创建输出文件夹（如果不存在）
    QDir outputDir(outputPath);
    if (!outputDir.exists()) {
        outputDir.mkpath(".");
    }

    // 构建文件名
    QString fileName = QString("%1/debug_info_%2.txt").arg(outputPath).arg(currentTime);

    // 打开文件以写入文本
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << debugInfo;
        file.close();
        qDebug() << "Debugging information has been saved to:" << fileName;
    }
    else {
        qDebug() << "fail to save Debugging information.";
    }
}

void MainWidget::clearDebugTextFile()
{
    // 获取项目文件夹路径
    QString projectPath = QDir::currentPath();

    // 构建输出文件夹路径
    QString outputPath = QDir::cleanPath(projectPath + QDir::separator() + "output");

    // 打开输出文件夹
    QDir outputDir(outputPath);

    // 遍历目录并删除文件
    QStringList fileList = outputDir.entryList(QDir::Files);
    foreach(QString fileName, fileList) {
        if (outputDir.remove(fileName)) {
            qDebug() << "have deleted:" << fileName;
        }
        else {
            qDebug() << "fail to delete debugging infomation.";
        }
    }
}

//***********************************************************************
//设置初始资源
void MainWidget::buildInitialStock()
{
    int minBDR = MAP_L / 2 - 10, minBUR = MAP_U / 2 - 10;
    int maxBDR = MAP_L / 2 + 6, maxBUR = MAP_U / 2 + 6;
    int lenth = 0, step;
    Point StockPoint, judPoint;
    vector<Point> findLab;
    bool tasking = true;
    int labSize, treeNum, maxtreeNum = 0;
    int lx, ly, mx, my;

    map->loadBarrierMap(true);
    map->reset_Map_Object_Resource();
    map->reset_resMap_AI();
    while (tasking)
    {
        lenth = maxBDR - minBDR;
        for (int y = minBUR; y <= maxBUR; y++)
        {
            if (y == minBUR || y == maxBUR) step = 1;
            else step = lenth;

            for (int x = minBDR; x <= maxBDR; x += lenth)
                if (!map->isBarrier(x, y, 5) && map->isFlat(x + 1, y + 1, 3)) findLab.push_back(Point(x + 1, y + 1));
        }

        labSize = findLab.size();

        if (labSize > 0)
        {
            for (int i = 0; i < labSize; i++)
            {
                judPoint = findLab[i];
                lx = max(judPoint.x - 5, 0);
                ly = max(judPoint.y - 5, 0);
                mx = min(judPoint.x + 8, MAP_L);
                my = min(judPoint.y + 8, MAP_U);
                treeNum = 0;

                for (int x = lx; x < mx; x++)
                    for (int y = ly; y < my; y++)
                        if (map->resMap_EnemyAI[x][y].type == RESOURCE_TREE) treeNum++;

                if (treeNum > 10)
                {
                    tasking = false;
                    if (treeNum > maxtreeNum)
                    {
                        maxtreeNum = treeNum;
                        StockPoint = judPoint;
                    }
                }
            }
        }
        findLab.clear();
        minBDR = max(minBDR - 1, 0);
        minBUR = max(minBUR - 1, 0);
        maxBDR = min(maxBDR + 1, MAP_L - 5);
        maxBUR = min(maxBUR + 1, MAP_U - 5);
    }

    //    player[0]->finishBuild(player[0]->addBuilding(BUILDING_STOCK , StockPoint.x, StockPoint.y , 100));
    return;
}



void MainWidget::on_option_2_clicked()
{
    aboutDialog->show();
}
