#include "Rectarea.h"
#include<iostream>
using namespace std;



RectArea::RectArea(GameWidget *widget_)
{
    triger=0;
    widget=widget_;
}

void RectArea::onClick()
{

}

void RectArea::onLeftMouseDown()
{
    triger=1;
    current.dr=widget->tranDR(MouseX(),MouseY())+widget->DR;
    current.ur=widget->tranUR(MouseX(),MouseY())+widget->UR;
    current.h=current.w=0;

}

void RectArea::onLeftMouseUp()
{
    triger=0;
    area.push_back(current);
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
    widget->AddEdge(ele.dr,ele.ur,ele.w,ele.h);
    if(triger)
        widget->AddEdge(current.dr,current.ur,current.w,current.h);
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

string RectArea::GetName()
{
   return Name();
}

string RectArea::Name()
{

     return "Rect";
}
