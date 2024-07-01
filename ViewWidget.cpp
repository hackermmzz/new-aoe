#include "ViewWidget.h"

ViewWidget::ViewWidget(QWidget *parent) : QWidget(parent)
{

    mainwidget=(MainWidget*)this->parentWidget();
}

void ViewWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    for(int L = 0; L < 72; L++)
    {
        for(int U = 0; U < 72; U++)
        {
            int paintY = 1.23 * (72 + L - U);
            int paintX = 3.55 * ((double)L + (double)U);
            if(mainwidget->map->cell[L][U].Explored)
            {
                if(mainwidget->map->cell[L][U].getMapType() == 1) painter.fillRect(QRect(paintX, paintY, 7, 4), QBrush(QColor(90, 140, 40)));
                else painter.fillRect(QRect(paintX, paintY, 7, 4), QBrush(QColor(99, 123, 47)));
//                else painter.fillRect(QRect(paintX, paintY, 7, 4), QBrush(QColor(255,222,173)));
            }
        }
    }
    int outlineX = 3.55 * ((double)screenL + (double)screenU);
    int outlineY = 1.23 * (72 + screenL - screenU);
    //遍历各list对象标注在地图上

    if(!animalList->empty()){
        auto animalIter = animalList->begin();
        for(; animalIter != animalList->end(); animalIter++)
        {
            int animalL = (*animalIter)->getDR() / 16 / gen5;
            int animalU = (*animalIter)->getUR() / 16 / gen5;
            int animalY = 1.23 * (MAP_L + animalL - animalU);
            int animalX = 3.55 * (animalL + animalU);
            if((*animalIter)->isTree()) painter.fillRect(QRect(animalX, animalY, 6, 6), QBrush(Qt::green));
            else painter.fillRect(QRect(animalX, animalY, 6, 6), QBrush(Qt::yellow));

        }
    }
    if(!resList->empty()){
        auto resIter = resList->begin();
        for(; resIter != resList->end(); resIter++)
        {
            int resL = (*resIter)->getDR() / 16 / gen5;
            int resU = (*resIter)->getUR() / 16 / gen5;
            int resY = 1.23 * (MAP_L + resL - resU);
            int resX = 3.55 * (resL + resU);
            painter.fillRect(QRect(resX, resY, 6, 6), QBrush(Qt::gray));

        }
    }
    if(!enemyFarmerList->empty()){
        auto farmerIter = enemyFarmerList->begin();
        for(; farmerIter != enemyFarmerList->end(); farmerIter++)
        {
            int farmerL = (*farmerIter)->getDR() / 16 / gen5;
            int farmerU = (*farmerIter)->getUR() / 16 / gen5;
            int farmerY = 1.23 * (MAP_L + farmerL - farmerU);
            int farmerX = 3.55 * (farmerL + farmerU);
            if(mainwidget->map->cell[farmerL][farmerU].Visible) painter.fillRect(QRect(farmerX, farmerY, 6, 6), QBrush(Qt::red));

        }
    }
    if(!enemyBuildList->empty()){
        auto buildIter = enemyBuildList->begin();
        for(; buildIter != enemyBuildList->end(); buildIter++)
        {
            int buildL = (*buildIter)->getDR() / 16 / gen5;
            int buildU = (*buildIter)->getUR() / 16 / gen5;
            int buildY = 1.23 * (MAP_L + buildL - buildU);
            int buildX = 3.55 * (buildL + buildU);
            if(mainwidget->map->cell[buildL][buildU].Visible) painter.fillRect(QRect(buildX, buildY, 8, 8), QBrush(Qt::red));

        }
    }

    if(!friendlyFarmerList->empty()){
        auto farmerIter = friendlyFarmerList->begin();
        for(; farmerIter != friendlyFarmerList->end(); farmerIter++)
        {
            int farmerL = (*farmerIter)->getDR() / 16 / gen5;
            int farmerU = (*farmerIter)->getUR() / 16 / gen5;
            int farmerY = 1.23 * (MAP_L + farmerL - farmerU);
            int farmerX = 3.55 * (farmerL + farmerU);
            painter.fillRect(QRect(farmerX, farmerY, 6, 6), QBrush(Qt::blue));

        }
    }
    if(!friendlyBuildList->empty()){
        auto buildIter = friendlyBuildList->begin();
        for(; buildIter != friendlyBuildList->end(); buildIter++)
        {
            int buildL = (*buildIter)->getDR() / 16 / gen5;
            int buildU = (*buildIter)->getUR() / 16 / gen5;
            int buildY = 1.23 * (MAP_L + buildL - buildU);
            int buildX = 3.55 * (buildL + buildU);
            painter.fillRect(QRect(buildX, buildY, 8, 8), QBrush(Qt::blue));

        }
    }
    for(int L = 0; L < MAP_L; L++)
    {
        for(int U = 0; U < MAP_U; U++)
        {
            int paintY = 1.23 * (MAP_L + L - U);
            int paintX = 3.55 * (L + U);
            if(!(mainwidget->map->cell[L][U].Explored))
            {
                painter.fillRect(QRect(paintX, paintY, 7, 4), QBrush(Qt::black));
            }

        }
    }
    //绘制当前区域边框
    painter.fillRect(QRect(outlineX, outlineY, 160, 1), QBrush(Qt::white));
    painter.fillRect(QRect(outlineX, outlineY + 61, 160, 1), QBrush(Qt::white));
    painter.fillRect(QRect(outlineX, outlineY, 1, 61), QBrush(Qt::white));
    painter.fillRect(QRect(outlineX + 160, outlineY, 1, 61), QBrush(Qt::white));
}
