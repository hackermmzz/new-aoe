#include "ViewWidget.h"

ViewWidget::ViewWidget(QWidget *parent) : QWidget(parent)
{

    mainwidget=(MainWidget*)this->parentWidget();
}

void ViewWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    //////////////////////////////////////////////////////
    /*
     *by wlh
     */
    struct Info{
        QColor color;
        int w,h;
    };
    static vector<vector<Info>>data(MAP_L,vector<Info>(MAP_U));
    auto setdata=[&](int i,int j,const QColor&color,int w,int h)->void{
        if(i<0||j<0||i>=MAP_L||j>=MAP_U)return;
        data[i][j]={color,w,h};
    };
    //////////////////////////////////////////////////////
    for(int L = 0; L < MAP_L; L++)
    {
        for(int U = 0; U < MAP_U; U++)
        {
            if(mainwidget->map->cell[L][U].Explored)
            {
                if(mainwidget->map->cell[L][U].getMapType() == 1)
                    setdata(L,U,QColor(90, 140, 40),7,4);
                else if(mainwidget->map->cell[L][U].getMapType()==MAPTYPE_OCEAN)
                    setdata(L,U,QColor(0, 100, 255),7,4);
                else
                   setdata(L,U,QColor(99, 123, 47),7,4);
            }
        }
    }
    //遍历各list对象标注在地图上

    if(!animalList->empty()){
        auto animalIter = animalList->begin();
        for(; animalIter != animalList->end(); animalIter++)
        {
            int animalL = (*animalIter)->getDR() / 16 / gen5;
            int animalU = (*animalIter)->getUR() / 16 / gen5;
            if((*animalIter)->isTree())
                setdata(animalL,animalU,Qt::green,6,6);
            else
                setdata(animalL,animalU,Qt::yellow,6,6);
        }
    }
    if(!resList->empty()){
        auto resIter = resList->begin();
        for(; resIter != resList->end(); resIter++)
        {
            int resL = (*resIter)->getDR() / 16 / gen5;
            int resU = (*resIter)->getUR() / 16 / gen5;
            if((*resIter)->getNum() == NUM_STATICRES_Bush)
               setdata(resL,resU,Qt::green,6,6);
            else if((*resIter)->getNum() == NUM_STATICRES_Stone)
                setdata(resL,resU,Qt::gray,6,6);
            else if((*resIter)->getNum() == NUM_STATICRES_GoldOre)
                setdata(resL,resU,Qt::yellow,6,6);
        }
    }
    if(!enemyFarmerList->empty()){
        auto farmerIter = enemyFarmerList->begin();
        for(; farmerIter != enemyFarmerList->end(); farmerIter++)
        {
            if(!(*farmerIter)->isDie()){
                int farmerL = (*farmerIter)->getDR() / 16 / gen5;
                int farmerU = (*farmerIter)->getUR() / 16 / gen5;
                if(mainwidget->map->cell[farmerL][farmerU].Visible)
                    setdata(farmerL,farmerU,Qt::red,6,6);
            }
        }
    }
    if(!enemyBuildList->empty()){
        auto buildIter = enemyBuildList->begin();
        for(; buildIter != enemyBuildList->end(); buildIter++)
        {
            int buildL = (*buildIter)->getDR() / 16 / gen5;
            int buildU = (*buildIter)->getUR() / 16 / gen5;
            if(mainwidget->map->cell[buildL][buildU].Visible)
                setdata(buildL,buildU,Qt::red,8,8);
        }
    }

    if(!friendlyFarmerList->empty()){
        auto farmerIter = friendlyFarmerList->begin();
        for(; farmerIter != friendlyFarmerList->end(); farmerIter++)
        {
            if(!(*farmerIter)->isDie()){
                int farmerL = (*farmerIter)->getDR() / 16 / gen5;
                int farmerU = (*farmerIter)->getUR() / 16 / gen5;
                setdata(farmerL,farmerU,Qt::blue,6,6);
            }
        }
    }
    if(!friendlyBuildList->empty()){
        auto buildIter = friendlyBuildList->begin();
        for(; buildIter != friendlyBuildList->end(); buildIter++)
        {
            int buildL = (*buildIter)->getDR() / 16 / gen5;
            int buildU = (*buildIter)->getUR() / 16 / gen5;
            setdata(buildL,buildU,Qt::blue,8,8);
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
                setdata(L,U,Qt::black,7,4);
            }

        }
    }
    //
    double w=width(),h=height();
    double Sin=h/sqrt(h*h+w*w),Cos=w/sqrt(h*h+w*w);
    double fac=h/2/MAP_L/Sin;
    auto MapTo=[&](int L,int U)->array<int,2>{
        int paintY = Sin*(L-U+MAP_U)*fac;
        int paintX = Cos * (L + U)*fac;
        return {paintX,paintY};
    };
    //
    for(int L=0;L<MAP_L;++L){
        for(int U=0;U<MAP_U;++U){
            auto&&ret=MapTo(L,U);
            auto&d=data[L][U];
            painter.fillRect((QRect(ret[0],ret[1],d.w,d.h)),d.color);
        }
    }
    //绘制当前区域边框
    const static int EdgeW=160,EdgeH=60;
    auto&&ret=MapTo(screenL,screenU);
    QPen pen;
    pen.setColor(QColor(Qt::white));
    pen.setWidth(1);
    painter.setPen(pen);
    painter.drawRect(ret[0], ret[1], EdgeW, EdgeH);
}
