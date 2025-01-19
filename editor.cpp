#include "editor.h"
#include "ui_editor.h"

Editor::Editor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Editor)
{
    ui->setupUi(this);

    // 创建控件
    terrainButton = new QPushButton("地形", this);
    terrainMenu = new QMenu(this);
    terrainMenu->addAction("草地");
    terrainMenu->addAction("海洋");
    terrainMenu->addAction("沙漠");
    terrainButton->setMenu(terrainMenu);

    heightComboBox = new QComboBox(this);
    heightComboBox->addItem("1");
    heightComboBox->addItem("2");
    heightComboBox->addItem("3");
    heightComboBox->addItem("4");
    heightComboBox->addItem("5");

    resourceButton = new QPushButton("资源", this);
    resourceMenu = new QMenu(this);
    QAction *treeAction = resourceMenu->addAction("树");
    resourceMenu->addAction("房子");
    resourceMenu->addAction("我方箭塔");
    resourceMenu->addAction("敌方箭塔");
    connect(treeAction, &QAction::triggered, this, &Editor::onTreeSelected);
    resourceButton->setMenu(resourceMenu);

    // 创建主控件布局
    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(terrainButton);
    layout->addWidget(heightComboBox);
    layout->addWidget(resourceButton);

    mainWidget->setLayout(layout);



}



Editor::~Editor()
{
    delete ui;
}
