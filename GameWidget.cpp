#include "GameWidget.h"
#include "ui_GameWidget.h"
#include "Map.h"
#include <QDateTime>
#include<QDateTime>
GameWidget::GameWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GameWidget)
{
    ui->setupUi(this);
    mainwidget=(MainWidget*)this->parentWidget();
    setFocusPolicy(Qt::StrongFocus);
    connect(mainwidget,SIGNAL(mapmove()),this,SLOT(movemap()));
}

GameWidget::~GameWidget()
{
    delete ui;
}
void GameWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.setPen(Qt::black);

    painter.fillRect(rect(), Qt::black);

    //检测点x y是否落在屏幕内
    static QRect winRect(0,0,GAMEWIDGET_WIDTH,GAMEWIDGET_HEIGHT);
    static auto InRect=[&](int x,int y)->bool{
      if(x>=winRect.x()&&x<=winRect.x()+winRect.width()&&y>=winRect.y()&&y<=winRect.y()+winRect.height())return 1;
      return 0;
    };
    static auto RectInRect=[&](int x,int y,int w,int h)->bool{
        return (InRect(x,y)||InRect(x+w,y)||InRect(x+w,y+h)||InRect(x,y+h));
    };

    //地图绘制部分
    int x1=BlockDR;//x1，y1作为参考坐标，用来每次循环初始化x2，y2的值
    int y1=BlockUR;//x2，y2则是引导绘制，用来实际判断cell中的内容
    int x2;
    int y2;


    for(int i = 0; i < GAMEWIDGET_HEIGHT / (mainwidget->map->cell[0][0].block[0]->front().pix.height() / 2.0) + 6; i++)
    {
        x2=x1;
        y2=y1;
        if(i%2==1)
        {
            y1--;
        }else if(i%2==0)
        {
            x1++;
        }
        //此处改动不采用nowres来显示图片
        for(int j = 0; j < GAMEWIDGET_WIDTH / mainwidget->map->cell[0][0].block[0]->front().pix.width() + 1; j++) // 行绘制
        {
            if(x2>=MAP_L||y2>=MAP_U||x2<0||y2<0)
            {
                x2++;
                y2++;
                continue;
            }
            Block&block=mainwidget->map->cell[x2][y2];
            int x,y,w,h;
            QPixmap*pix=0;
            // 此处以下的drawPixmap函数中，添加偏移量OffsetX/Y以对齐各地块
            if(i%2==0)x=-32+64*j + block.getOffsetX(),y=-16+16*i + block.getOffsetY();
            if(i%2==1)x=64*j + block.getOffsetX(),y=-16+16*i + block.getOffsetY();
            w=Block::block[block.Num]->front().pix.width(),h=Block::block[block.Num]->front().pix.height();
            list<ImageResource>*targetList=0;
            if(mainwidget->map->cell[x2][y2].Visible == true && mainwidget->map->cell[x2][y2].Explored == true)targetList=Block::block[block.Num];
            else if(mainwidget->map->cell[x2][y2].Visible == false && mainwidget->map->cell[x2][y2].Explored == true)targetList=Block::grayblock[block.Num];
            else if(mainwidget->map->cell[x2][y2].Visible == false && mainwidget->map->cell[x2][y2].Explored == false)targetList=Block::blackblock[block.Num];
            //如果没超出屏幕，那么绘制
            if(RectInRect(x,y,w,h)){
                //如果是海洋
                if(block.getMapType()==MAPTYPE_OCEAN){
                    static QPixmap*ocean=nullptr;
                    static QPixmap*grayOcean=0;
                    if(!ocean)
                    {
                        ocean=new QPixmap(resMap["Sea_Deep"].front());
                        grayOcean=new QPixmap(applyTransparencyEffect(*ocean,0.5));
                    }
                    auto&block=mainwidget->map->cell[x2][y2];
                    if(block.Visible&&block.Explored)pix=ocean;
                    else if(block.Explored)pix=grayOcean;
                    else pix=0;
                }
                else pix=&(targetList->front().pix);
                //绘制
                if(pix)
                painter.drawPixmap(x,y,w,h,*pix);
            }
            x2++;
            y2++;
        }
    }

    //清除内存图内容
    emptymemorymap();

    //绘制列表清空
    drawlist.clear();
    static auto CheckInScreen=[&](Coordinate*coor)->bool{
        int tx = tranX(coor->getDR()-DR, coor->getUR()-UR), ty = tranY(coor->getDR()-DR, coor->getUR()-UR);
        // BlockDR、BlockUR
        int tmpBlockDR = coor->getDR() / BLOCKSIDELENGTH, tmpBlockUR = coor->getUR() / BLOCKSIDELENGTH;
        int x=tx - coor->getimageX() + mainwidget->map->cell[tmpBlockDR][tmpBlockUR].getOffsetX();
        int y=coor->getimageY() - coor->getNowRes()->pix.height() + ty +  mainwidget->map->cell[tmpBlockDR][tmpBlockUR].getOffsetY();
        int w=coor->getNowRes()->pix.width(),h=coor->getNowRes()->pix.height();
        return RectInRect(x,y,w,h);
    };

    //地图资源相关 树木石块等
    std::list<StaticRes*> *sr=&mainwidget->map->staticres;
    if(!sr->empty())
    {
        std::list<StaticRes*>::iterator sriter=sr->begin();
        while(sriter!=sr->end())
        {
            if(CheckInScreen(*sriter)&&(*sriter)->getexplored())
                insert((*sriter),&drawlist);
            else (*sriter)->setNotInWidget();
            sriter++;
        }
    }
    std::list<Animal *> *ar=&mainwidget->map->animal;
    if(!ar->empty())
    {
        std::list<Animal *>::iterator ariter=ar->begin();
        while(ariter!=ar->end())
        {
            if(CheckInScreen(*ariter)&&(*ariter)->getexplored())
                insert((*ariter),&drawlist);
            else (*ariter)->setNotInWidget();
            ariter++;
        }
    }
    //玩家的建筑部分 人物部分
    for(int i=0;i<MAXPLAYER;i++)
    {
        std::list<Building *> *b=&(mainwidget->player[i]->build);
        std::list<Building *>::iterator biter=b->begin();
        while(!b->empty()&&biter!=b->end())
        {
            Coordinate *p=*biter;
            if(CheckInScreen(*biter)&&(*biter)->getvisible())
                insert(p,&drawlist);
            else (*biter)->setNotInWidget();
            biter++;
        }

        std::list<Human *> *h=&(mainwidget->player[i]->human);
        std::list<Human *>::iterator hiter=h->begin();
        while(!h->empty()&&hiter!=h->end())
        {
            Coordinate *p=*hiter;
            if(CheckInScreen(*hiter)&&(*hiter)->getvisible()&&!(*hiter)->getTransported())
                insert(p,&drawlist);
            else (*hiter)->setNotInWidget();
            hiter++;
        }

        //绘制投掷物
        std::list<Missile *> *mis = &(mainwidget->player[i]->missile);
        std::list<Missile *>::iterator misiter = mis->begin();
        while(!mis->empty() && misiter!=mis->end())
        {
            Coordinate* p = *misiter;
            insert(p , &drawlist);
            misiter++;
        }
    }

    if(nowobject!=NULL)
    {
        painter.setPen(Qt::white);
        int width=nowobject->getCrashLength()*4;
        int height=nowobject->getCrashLength()*2;
        int tempBlockDR = nowobject->getDR() / BLOCKSIDELENGTH, tempBlockUR = nowobject->getUR() / BLOCKSIDELENGTH;
        int X=tranX(nowobject->getDR()-DR,nowobject->getUR()-UR)-nowobject->getCrashLength()*2 + mainwidget->map->cell[tempBlockDR][tempBlockUR].getOffsetX();
        int Y=tranY(nowobject->getDR()-DR,nowobject->getUR()-UR) - height / 2 + mainwidget->map->cell[tempBlockDR][tempBlockUR].getOffsetY();
        QPolygonF diamond;
        diamond << QPointF(X+width/2, Y);
        diamond << QPointF(X+width, Y+height/2);
        diamond << QPointF(X+width/2, Y+height);
        diamond << QPointF(X, Y+height/2);
        painter.drawPolygon(diamond);
    }
    Building* buildOb = NULL;
    //drawlist正常绘制
    if(!drawlist.empty())
    {
        std::list<Coordinate *>::iterator iter=drawlist.begin();
        while(iter!=drawlist.end())
        {
            // x、y坐标偏移量
            int tx = tranX((*iter)->getDR()-DR, (*iter)->getUR()-UR), ty = tranY((*iter)->getDR()-DR, (*iter)->getUR()-UR);
            // BlockDR、BlockUR
            int tmpBlockDR = (*iter)->getDR() / BLOCKSIDELENGTH, tmpBlockUR = (*iter)->getUR() / BLOCKSIDELENGTH;
            int x=tx - (*iter)->getimageX() + mainwidget->map->cell[tmpBlockDR][tmpBlockUR].getOffsetX();
            int y=(*iter)->getimageY() - (*iter)->getNowRes()->pix.height() + ty +  mainwidget->map->cell[tmpBlockDR][tmpBlockUR].getOffsetY();
            int w=(*iter)->getNowRes()->pix.width(),h=(*iter)->getNowRes()->pix.height();
            painter.drawPixmap(x,y,w,h,(*iter)->getNowRes()->pix);
            (*iter)->printer_ToBuilding((void**)&buildOb);
            if(buildOb != NULL && buildOb->getFireNowList() != NULL)
            {
                painter.drawPixmap(\
                    tx - buildOb->getFireImageX() + mainwidget->map->cell[tmpBlockDR][tmpBlockUR].getOffsetX(),
                    buildOb->getFireImageY() - buildOb->getFireNowRes()->pix.height() + ty + mainwidget->map->cell[tmpBlockDR][tmpBlockUR].getOffsetY(),
                    buildOb->getFireNowRes()->pix.width(),
                    buildOb->getFireNowRes()->pix.height(),
                    buildOb->getFireNowRes()->pix
                );
            }

            if(Core::objClickedCaptureState==1){//如果需要捕捉点击对象
                int xx=Core::mouseEventStore.memoryMapX,yy=Core::mouseEventStore.memoryMapY;
                int x=tranX((*iter)->getDR()-DR, (*iter)->getUR()-UR)-(*iter)->getimageX();
                int y=(*iter)->getimageY()-(*iter)->getNowRes()->pix.height()+tranY((*iter)->getDR()-DR,(*iter)->getUR()-UR) + /*(*iter)->getMapHeightOffsetY()*/ mainwidget->map->cell[tmpBlockDR][tmpBlockUR].getOffsetY();
                if(xx>=x/4&&xx<=(x+w)/4&&yy>=y/4&&yy<=(y+h)/4){
                    Core::objClickedCaptureState=2;
                    Core::objCapture=*iter;
                }
            }
            (*iter)->setInWidget();
            iter++;
        }
    }

    if(Core::objClickedCaptureState==1){
        Core::objClickedCaptureState=2;
        Core::objCapture=0;
    }

}

void GameWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Z){
        //回滚状态
        ResumePreState();
    }
}


void GameWidget::SaveCurrentState(void *state)
{
    const static int MAXSIZE=119;//最多回滚119次
    if(AllState.size()>MAXSIZE)AllState.pop_back();
    AllState.push_front(state);
}

void *GameWidget::RollBackState()
{
    if(AllState.size()==0)return 0;
    auto ans=AllState.front();
    AllState.pop_front();
    return ans;
}

void GameWidget::ResumePreState()
{
    GameState*state=(GameState*)RollBackState();
    if(state==0)return;
    //////////////////////
    auto&cell=mainwidget->map->cell;
    auto&heightMap=mainwidget->map->m_heightMap;
    for(int i=0;i<MAP_L;++i){
        for(int j=0;j<MAP_U;++j){
            cell[i][j]=(state->cell)[i][j];
        }
    }

    for(int i=0;i<GENERATE_L;++i){
        for(int j=0;j<GENERATE_U;++j){
            heightMap[i][j]=(state->m_heightMap)[i][j];
        }
    }

    mainwidget->player[0]->human=state->myHuman;
    mainwidget->player[0]->build=state->myBuilding;
    mainwidget->player[1]->human=state->enemyHuman;
    mainwidget->player[1]->build=state->enemyBuilding;
    mainwidget->map->animal=state->animal;
    mainwidget->map->staticres=state->resource;
    ////释放内存
    delete state;
}

void GameWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        mainwidget->leftMousePress=1;
        mainwidget->mouseEvent->memoryMapX=event->x()/4;
        mainwidget->mouseEvent->memoryMapY=event->y()/4;
        mainwidget->mouseEvent->mouseEventType=LEFT_PRESS;
        mainwidget->mouseEvent->DR=tranDR(event->x(),event->y())+DR;
        mainwidget->mouseEvent->UR=tranUR(event->x(),event->y())+UR;
        if(buildMode >= 0){
            int hoverDR = (tranDR(event->x(), event->y()) + DR) / BLOCKSIDELENGTH;
            int hoverUR = (tranUR(event->x(), event->y()) + UR) / BLOCKSIDELENGTH;
            if(buildMode == BUILDING_GRANARY || buildMode == BUILDING_STOCK || buildMode == BUILDING_MARKET || buildMode == BUILDING_FARM\
                 || buildMode == BUILDING_ARMYCAMP || buildMode == BUILDING_RANGE || buildMode == BUILDING_STABLE)
            {
                hoverDR--;
                hoverUR--;
            }
            if(buildMode == BUILDING_HOME || buildMode == BUILDING_ARROWTOWER)
            {
                hoverUR--;
            }
            emit sendView(hoverDR, hoverUR, buildMode);
            buildMode = -1;
            QApplication::restoreOverrideCursor();
        }
    }
    else if(event->button()==Qt::RightButton)
    {
        mainwidget->mouseEvent->memoryMapX=event->x()/4;
        mainwidget->mouseEvent->memoryMapY=event->y()/4;
        mainwidget->mouseEvent->mouseEventType=RIGHT_PRESS;
        double tDR = tranDR(event->x(),event->y());
        double tUR = tranUR(event->x(),event->y());
        mainwidget->mouseEvent->DR = tDR + DR;
        mainwidget->mouseEvent->UR = tUR + UR;

        if(buildMode >= 0)
        {
            buildMode = -1;
            QApplication::restoreOverrideCursor();
        }
    }
}

bool GameWidget::judgeinWindow(double x, double y)
{
    if(x>0&&x<GAMEWIDGET_WIDTH&&y>0&&y<GAMEWIDGET_HEIGHT)
    {
        return 1;
    }
    return 0;
}

void GameWidget::mouseReleaseEvent(QMouseEvent* event)
{
    mainwidget->mouseEvent->mouseEventType=NULL_MOUSEEVENT;
    mainwidget->leftMousePress=0;
}

//坐标间的相互转化
int GameWidget::tranX(int DR, int UR)
{
    int X;
    //    X=L*2.0/gen5+U*2.0/gen5;
    X=(DR+UR)*2/gen5;
    return X;
}

int GameWidget::tranY(int DR, int UR)
{
    int Y;
    //    Y=L/gen5-U/gen5;
    Y=(DR-UR)/gen5;
    return Y;
}

int GameWidget::tranDR(int X, int Y)
{
    int DR;
    DR = X * gen5 / 4.0 + Y * gen5 / 2.0;
    return DR;
}

int GameWidget::tranUR(int X, int Y)
{
    int UR;
    UR=X*gen5/4.0-Y*gen5/2.0;
    return UR;
}
//根据当前对象高度插入drawlist
void GameWidget::insert(Coordinate *p, std::list<Coordinate *> *drawlist)
{
    if(drawlist->empty())
    {
        drawlist->push_back(p);
    }
    else
    {
        std::list<Coordinate *>::iterator iter=drawlist->begin();
        while(1)//H越大说明越靠下，应该先打印H小的，所以H小的在前边
        {
            if(p->getimageH()<(*iter)->getimageH())
            {
                drawlist->insert(iter,p);
                break;
            }
            if((*iter)==drawlist->back())
            {
                drawlist->push_back(p);
                break;
            }
            iter++;
        }
    }

}

//绘制内存图
void GameWidget::drawmemory(int X, int Y,  ImageResource&res, int globalNum)
{
    for(int i=0;i<res.pix.width();i++)
    {
        for(int j=0;j<res.pix.height();j++)
        {
            int mx,my;
            mx=i+X;
            my=j+Y;

            if(mx>=0&&my>=0&&mx<GAMEWIDGET_WIDTH&&my<GAMEWIDGET_HEIGHT)//
            {
                if(res.memorymap.getMemoryMap(i,j)!=0)
                {
                    mainwidget->memorymap[mx/4][my/4]=globalNum;
                }
            }
        }
    }
}

//清除内存图
void GameWidget::emptymemorymap()
{
    for(int i=0;i<MEMORYROW;i++)
    {
        for(int j=0;j<MEMORYCOLUMN;j++)
        {
            mainwidget->memorymap[i][j]=0;
        }
    }
}

//地图移动
void GameWidget::movemap()
{
    //此处采用相对坐标，只相对于当前窗口 所以在纵向数据判断处 应该加上的是下窗口的大小
    int x=this->mapFromGlobal(QCursor().pos()).x();
    int y=this->mapFromGlobal(QCursor().pos()).y();
    if(BlockDR+22<0)
    {
        BlockDR++;
    }
    if(BlockUR<0)
    {
        BlockUR++;
    }
    if(BlockDR+GAMEWIDGET_MIDBLOCKL>MAP_L-1)
    {
        BlockDR--;
    }
    if(BlockUR+GAMEWIDGET_MIDBLOCKU>MAP_U-1)
    {
        BlockUR--;
    }
    if(x<2)
    {
        BlockDR--;
        BlockUR--;
    }
    if(x>GAME_WIDTH-20)
    {
        BlockDR++;
        BlockUR++;
    }
    if(y<-44)
    {
        BlockUR++;
        BlockDR--;
    }
    if(y>GAME_HEIGHT-50-45)//此处先用常数 其中45代表上边框的宽
    {
        BlockUR--;
        BlockDR++;
    }
    if(x<2&&y<-44)
    {
        BlockDR++;
    }
    if(x<-44&&y>GAME_HEIGHT-50-45)
    {
        BlockUR++;
    }
    if(x>GAME_WIDTH-20&&y<-44)
    {
        BlockUR--;
    }
    if(x>GAME_WIDTH-20&&y>GAME_HEIGHT-50-45)
    {
        BlockDR--;
    }
    DR=(BlockDR+0.5)*16*gen5;
    UR=(BlockUR+0.5)*16*gen5;

}

void GameWidget::UpdateData()
{

}

void GameWidget::setBuildMode(int buildMode)
{
    this->buildMode = buildMode;
}
