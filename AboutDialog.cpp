#include "AboutDialog.h"
#include "ui_AboutDialog.h"
#include "GlobalVariate.h"
AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    setFixedSize(600,500);
    pix = QPixmap(RESPATH+"/About_Background_001.png");

    setWindowTitle("关于我们");
    ui->nameLabel->setText("项目名称： newAOE");
    ui->versionLabel->setText("当前版本： " + QString::fromStdString(GAME_VERSION));
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, 600, 500, pix);
}
