#include "LineArea.h"


LineArea::LineArea(GameWidget *widget_)
{
    widget=widget_;
    triger=0;
    down=0;
}


void LineArea::onLeftMouseDown()
{
    triger=1;
    down=1;
}

void LineArea::onLeftMouseUp()
{
    down=0;
}


void LineArea::onMouseMove(int delta_x, int delta_y)
{
    if(triger&&down){
        current.data.push_back({widget->TranGlobalPosToDR(MouseX(),MouseY()),widget->TranGlobalPosToUR(MouseX(),MouseY())});
    }
}

void LineArea::Draw()
{
    for(int i=0;i<lines.size();++i){
        auto&data=lines[i].data;
        for(int j=0;j+1<data.size();++j){
            auto&p0=data[j],&p1=data[j+1];
            widget->AddLine(p0[0],p0[1],p1[0],p1[1],Qt::gray);
        }
    }
    if(triger)
    for(int j=0;j+1<current.data.size();++j){
        auto&p0=current.data[j],&p1=current.data[j+1];
        widget->AddLine(p0[0],p0[1],p1[0],p1[1],Qt::green);
    }
    //绘制所有待关联的对象
    for(auto*obj:coordinate){
        widget->AddEdge(obj->getDR(),obj->getUR(),obj->getCrashLength(),obj->getCrashLength(),Qt::red);
    }
    //
    for(auto&ele:relation){
        auto*obj=ele.first;
        if(coordinate.count(obj)==0){
            widget->AddEdge(obj->getDR(),obj->getUR(),obj->getCrashLength(),obj->getCrashLength(),Qt::darkRed);
        }
    }
}

void LineArea::onRightMouseClick()
{
    // 选择对象
    Coordinate*obj=Core::getObject(MouseX()/4,MouseY()/4);
    if(obj!=0)coordinate.insert(obj);
    else{
        //如果没选到对象,默认就是保存本次区域
        triger=0;
        if(current.data.size()>=2){
            lines.push_back(current);
            //将关联对象与当前区域进行关联
            for(auto*coor:coordinate){
                relation[coor]=current;
            }
            coordinate.clear();
            //
            current.data.clear();
        }
    }
}

LineAreaData *LineArea::GetPosIn(double dr, double ur)
{

}


string LineArea::Name()
{
    return "Line";
}
