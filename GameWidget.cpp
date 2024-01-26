#include "GameWidget.h"
#include "ui_GameWidget.h"

GameWidget::GameWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GameWidget)
{
    ui->setupUi(this);
    mainwidget=(MainWidget*)this->parentWidget();
    connect(mainwidget,SIGNAL(mapmove()),this,SLOT(movemap()));
}

GameWidget::~GameWidget()
{
    delete ui;
}

void GameWidget::drawMap(double x, double y, double width, double height, QPixmap pixmap, double offsetX, double offsetY)
{

}
void GameWidget::paintEvent(QPaintEvent *)
{
    // 创建QPainter对象
    QPainter painter(this);

    // 绘制黑色背景
    painter.setPen(Qt::black);
    painter.fillRect(rect(), Qt::black);

    // 地图绘制部分
    int x1=BlockDR;// x1，y1作为参考坐标，用来每次循环初始化x2，y2的值
    int y1=BlockUR;// x2，y2则是引导绘制，用来实际判断cell中的内容
    int x2;
    int y2;

//    for(int i = 0; i < MAP_L; i++)
//    {
//        for(int j = 0; j < MAP_U; j++)
//        {
//            mainwidget->map->cell[i][j].Visible = false;
//        }
//    }

    for(int i=0;i<GAMEWIDGET_HEIGHT/(mainwidget->map->cell[0][0].block[0]->front().pix.height()/2.0)+2;i++)
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
        for(int j=0;j<GAMEWIDGET_WIDTH/mainwidget->map->cell[0][0].block[0]->front().pix.width()+1;j++) // 行绘制
        {
            if(x2>=MAP_L||y2>=MAP_U||x2<0||y2<0)
            {
                x2++;
                y2++;
                continue;
            }
            if(i%2==0)
            {
                if(mainwidget->map->cell[x2][y2].Visible == true && mainwidget->map->cell[x2][y2].Explored == true)
                    painter.drawPixmap(-32+64*j,-16+16*i,BLOCKPIXEL_X,BLOCKPIXEL_Y,Block::block[mainwidget->map->cell[x2][y2].Num]->front().pix);
                else if(mainwidget->map->cell[x2][y2].Visible == false && mainwidget->map->cell[x2][y2].Explored == true)
                {
                    painter.drawPixmap(-32+64*j,-16+16*i,BLOCKPIXEL_X,BLOCKPIXEL_Y,Block::grayblock[mainwidget->map->cell[x2][y2].Num]->front().pix);
                }
                else if(mainwidget->map->cell[x2][y2].Visible == false && mainwidget->map->cell[x2][y2].Explored == false)
                {
                    painter.drawPixmap(-32+64*j,-16+16*i,BLOCKPIXEL_X,BLOCKPIXEL_Y,Block::blackblock[mainwidget->map->cell[x2][y2].Num]->front().pix);
                }
            }
            if(i%2==1)
            {
                if(mainwidget->map->cell[x2][y2].Visible == true && mainwidget->map->cell[x2][y2].Explored == true)
                    painter.drawPixmap(64*j,-16+16*i,BLOCKPIXEL_X,BLOCKPIXEL_Y,Block::block[mainwidget->map->cell[x2][y2].Num]->front().pix);
                else if(mainwidget->map->cell[x2][y2].Visible == false && mainwidget->map->cell[x2][y2].Explored == true)
                {
                    painter.drawPixmap(64*j,-16+16*i,BLOCKPIXEL_X,BLOCKPIXEL_Y,Block::grayblock[mainwidget->map->cell[x2][y2].Num]->front().pix);
                }
                else if(mainwidget->map->cell[x2][y2].Visible == false && mainwidget->map->cell[x2][y2].Explored == false)
                {
                    painter.drawPixmap(64*j,-16+16*i,BLOCKPIXEL_X,BLOCKPIXEL_Y,Block::blackblock[mainwidget->map->cell[x2][y2].Num]->front().pix);
                }
            }
            x2++;
            y2++;
        }
    }

    //清除内存图内容
    emptymemorymap();

    //绘制列表清空
    drawlist.clear();


    //玩家的建筑部分 人物部分
    for(int i=0;i<MAXPLAYER;i++)
    {
        std::list<Building *> *b=&(mainwidget->player[i]->build);
        std::list<Building *>::iterator biter=b->begin();
        while(!b->empty()&&biter!=b->end())
        {
            Coordinate *p=*biter;
            insert(p,&drawlist);
            biter++;
        }

        std::list<Human *> *h=&(mainwidget->player[i]->human);
        std::list<Human *>::iterator hiter=h->begin();
        while(!h->empty()&&hiter!=h->end())
        {
            Coordinate *p=*hiter;
            insert(p,&drawlist);
            hiter++;
        }
    }

    //地图资源相关 树木石块等
    std::list<StaticRes*> *sr=&mainwidget->map->staticres;
    if(!sr->empty())
    {
        std::list<StaticRes*>::iterator sriter=sr->begin();
        while(sriter!=sr->end())
        {
            insert((*sriter),&drawlist);
            sriter++;
        }
    }
    std::list<Animal *> *ar=&mainwidget->map->animal;
    if(!ar->empty())
    {
        std::list<Animal *>::iterator ariter=ar->begin();
        while(ariter!=ar->end())
        {
            insert((*ariter),&drawlist);
            ariter++;
        }
    }


    //drawlist正常绘制
    if(!drawlist.empty())
    {
        std::list<Coordinate *>::iterator iter=drawlist.begin();
        while(iter!=drawlist.end())
        {
            painter.drawPixmap(tranX((*iter)->getDR()-DR,(*iter)->getUR()-UR)-(*iter)->getimageX(),(*iter)->getimageY()-(*iter)->getNowRes()->pix.height()+tranY((*iter)->getDR()-DR,(*iter)->getUR()-UR),(*iter)->getNowRes()->pix.width(),(*iter)->getNowRes()->pix.height(),(*iter)->getNowRes()->pix);
            drawmemory(tranX((*iter)->getDR()-DR,(*iter)->getUR()-UR)-(*iter)->getimageX(),(*iter)->getimageY()-(*iter)->getNowRes()->pix.height()+tranY((*iter)->getDR()-DR,(*iter)->getUR()-UR),(*(*iter)->getNowRes()),(*iter)->getglobalNum());
            iter++;
        }

    }
}

void GameWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        mainwidget->mouseEvent->memoryMapX=event->x()/4;
        mainwidget->mouseEvent->memoryMapY=event->y()/4;
        mainwidget->mouseEvent->mouseEventType=LEFT_PRESS;
        mainwidget->mouseEvent->DR=tranDR(event->x(),event->y())+DR;
        mainwidget->mouseEvent->UR=tranUR(event->x(),event->y())+UR;

    }
    else if(event->button()==Qt::RightButton)
    {
        mainwidget->mouseEvent->memoryMapX=event->x()/4;
        mainwidget->mouseEvent->memoryMapY=event->y()/4;
        mainwidget->mouseEvent->mouseEventType=RIGHT_PRESS;
        mainwidget->mouseEvent->DR=tranDR(event->x(),event->y())+DR;
        mainwidget->mouseEvent->UR=tranUR(event->x(),event->y())+UR;
    }
}

void GameWidget::mouseReleaseEvent(QMouseEvent* event)
{

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
    DR=X*gen5/4.0+Y*gen5/2.0;
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
void GameWidget::drawmemory(int X, int Y, ImageResource res, int globalNum)
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

