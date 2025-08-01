#include "Rectarea.h"
#include<iostream>
#include"GlobalVariate.h"
using namespace std;



RectArea::RectArea(GameWidget *widget_)
{
    triger=0;
    widget=widget_;
}

void RectArea::onLeftMouseDown()
{
    triger=1;
    current.dr=widget->TranGlobalPosToDR(MouseX(),MouseY());
    current.ur=widget->TranGlobalPosToUR(MouseX(),MouseY());
    current.h=current.w=0;

}

void RectArea::onLeftMouseUp()
{
    triger=0;
    if(current.w!=0&&current.h!=0){
        area.push_back(current);
        //将关联对象与当前区域进行关联
        for(auto*coor:coordinate){
            relation[coor]=current;
        }
        coordinate.clear();
    }
}

void RectArea::onMouseMove(int delta_x, int delta_y)
{
    if(triger){
        current.w+=delta_x;
        current.h+=delta_y;
    }

}

void RectArea::Draw()
{
    for(auto&ele:area)
    widget->AddEdge(ele.dr,ele.ur,ele.w,ele.h,Qt::gray);
    if(triger){
        widget->AddEdge(current.dr,current.ur,current.w,current.h,Qt::green);
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

void RectArea::onRightMouseClick()
{
   // 选择对象
   Coordinate*obj=Core::getObject(MouseX()/4,MouseY()/4);
   if(obj!=0)coordinate.insert(obj);
}


RectAreaData *RectArea::GetPosIn(double dr, double ur)
{
    for(auto itr=area.rbegin();itr!=area.rend();++itr){
        RectAreaData&data=*itr;
        if(dr>=data.dr&&ur>=data.ur&&dr<=data.dr+data.w&&ur<=data.ur+data.h){
            return &*itr;
        }
    }
    return NULL;
}

string RectArea::Name()
{

     return "Rect";
}
