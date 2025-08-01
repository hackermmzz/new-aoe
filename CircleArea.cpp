#include "CircleArea.h"



CircleArea::CircleArea(GameWidget *widget_)
{
    widget=widget_;
}


void CircleArea::onLeftMouseDown()
{
    triger=1;
    current.dr=widget->tranDR(MouseX(),MouseY())+widget->DR;
    current.ur=widget->tranUR(MouseX(),MouseY())+widget->UR;
    current.rad=0;
}

void CircleArea::onLeftMouseUp()
{
    triger=0;
    if(current.rad!=0){
        area.push_back(current);
        //将关联对象与当前区域进行关联
        for(auto*coor:coordinate){
            relation[coor]=current;
        }
        coordinate.clear();
    }
}

void CircleArea::onMouseMove(int delta_x, int delta_y)
{
    if(triger){
        current.rad+=delta_x;
    }

}

void CircleArea::Draw()
{
    for(auto&ele:area)
        for(auto&line:GetCircle(ele))
            widget->AddLine(line[0],line[1],line[2],line[3],Qt::gray);
    if(triger)
        for(auto&line:GetCircle(current))
            widget->AddLine(line[0],line[1],line[2],line[3],Qt::green);
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

void CircleArea::onRightMouseClick()
{
    // 选择对象
    Coordinate*obj=Core::getObject(MouseX()/4,MouseY()/4);
    if(obj!=0)coordinate.insert(obj);
}

vector<array<double, 4> > CircleArea::GetCircle(CircleAreaData &data)
{
    const static int freq=30;
    vector<array<double,4>>ret;
    vector<array<double,2>>points;
    //计算出所有的点
    for(double deg=0.0;deg<=360.0;deg+=360.0/freq){
        double r=deg*3.1415926/180;
        double x=cos(r)*data.rad+data.dr;
        double y=sin(r)*data.rad+data.ur;
        points.push_back({x,y});
    }
    //连点成线
    for(int i=0;i<points.size()-1;++i){
        auto&p0=points[i],&p1=points[i+1];
        ret.push_back({p0[0],p0[1],p1[0],p1[1]});
    }
    //
    return ret;
}

CircleAreaData *CircleArea::GetPosIn(double dr, double ur)
{
    for(auto itr=area.rbegin();itr!=area.rend();++itr){
        double dr_=dr-itr->dr,ur_=ur-itr->ur;
        double dis=sqrt(dr_*dr_+ur_*ur_);
        if(dis<=itr->rad)return &*itr;
    }
    return NULL;
}


string CircleArea::Name()
{
    return "Circle";
}
