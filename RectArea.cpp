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
    // current.areaType 需要在MainWidget中设置
}

void RectArea::onLeftMouseUp()
{
    triger=0;
    if(current.w!=0&&current.h!=0){
        area.push_back(current);
        //将关联对象与当前区域进行关联
        for(auto*coor:coordinate){
            // 检查是否已存在相同类型的区域，如果存在则替换，否则添加
            bool found = false;
            for(auto it = relation.equal_range(coor); it.first != it.second; ++it.first) {
                if(it.first->second.areaType == current.areaType) {
                    it.first->second = current;  // 替换相同类型的区域
                    found = true;
                    break;
                }
            }
            if(!found) {
                relation.insert({coor, current});  // 添加新区域
            }
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
    //
    using Point=array<double,2>;
    auto AddLine=[&](RectAreaData&current,QColor color)->void{
        using Data=array<double,2>;
        Data point[4];
        point[0]={current.dr,current.ur};
        point[1]={current.dr+current.w,current.ur};
        point[2]={current.dr+current.w,current.ur+current.h};
        point[3]={current.dr,current.ur+current.h};
        for(int i=0;i<4;++i){
            auto&p0=point[i],&p1=point[(i+1)%4];
            widget->AddLine(p0[0],p0[1],p1[0],p1[1],color);
        }
    };
    //
    for(auto&ele:area)
    {
        AddLine(ele,Qt::gray);
    }
    if(triger){
        AddLine(current,Qt::green);
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

void RectArea::onRightMouseDown()
{
    // 如果正在绘制区域，则取消当前选��
    if(triger) {
        triger = false;
        current.dr = current.ur = current.w = current.h = 0;
        coordinate.clear();
    }
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

// 设置当前绘制的区域类型
void RectArea::setCurrentAreaType(int type) {
    current.areaType = type;
}

// 根据区域类型获取颜色
QColor RectArea::getAreaColor(int areaType) {
    switch(areaType) {
        case 1: return Qt::blue;    // 巡逻区域
        default: return Qt::gray;   // 原有区域
    }
}

// 设置要绑定的单位
void RectArea::setTargetUnits(const vector<Coordinate*>& units) {
    coordinate.clear();
    for (Coordinate* unit : units) {
        coordinate.insert(unit);
    }
}
