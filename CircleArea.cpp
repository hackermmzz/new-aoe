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
    // current.areaType 需要在MainWidget中设置
}

void CircleArea::onLeftMouseUp()
{
    triger=0;
    if(current.rad!=Double::Zero()){
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

void CircleArea::onMouseMove(int delta_x, int delta_y)
{
    if(triger){
        current.rad+=delta_x;
    }

}

void CircleArea::Draw()
{
    // 绘制已完成的区域，根据区域类型使用不同颜色
    for(auto&ele:area) {
        QColor color = getAreaColor(ele.areaType);
        for(auto&line:GetCircle(ele))
            widget->AddLine(line[0],line[1],line[2],line[3],color);
    }
    
    // 绘制正在创建的区域，使用绿色
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

void CircleArea::onRightMouseDown()
{
    // 如果正在绘制区域，则取消当前选择
    if(triger) {
        triger = false;
        current.dr = current.ur = current.rad = 0;
        coordinate.clear();
    }
}

vector<array<Double, 4> > CircleArea::GetCircle(CircleAreaData &data)
{
    const static int freq=30;
    vector<array<Double,4>>ret;
    vector<array<Double,2>>points;
    //计算出所有的点
    for(Double deg=Double::Zero();deg<=Double(360);deg+=Double(360)/freq){
        Double r=deg*Double("3.1415926")/180;
        Double x=Double::FromDouble(cos(double(r)))*data.rad+data.dr;
        Double y=Double::FromDouble(sin(double(r)))*data.rad+data.ur;
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

CircleAreaData *CircleArea::GetPosIn(Double dr, Double ur)
{
    for(auto itr=area.rbegin();itr!=area.rend();++itr){
        Double dr_=dr-itr->dr,ur_=ur-itr->ur;
        Double dis=sqrt(dr_*dr_+ur_*ur_);
        if(dis<=itr->rad)return &*itr;
    }
    return NULL;
}


string CircleArea::Name()
{
    return "Circle";
}

// 设置当前绘制的区域类型
void CircleArea::setCurrentAreaType(int type) {
    current.areaType = type;
}

// 根据区域类型获取颜色
QColor CircleArea::getAreaColor(int areaType) {
    switch(areaType) {
        case 1: return Qt::blue;    // 巡逻区域
        default: return Qt::gray;   // 原有区域
    }
}

// 设置要绑定的单位
void CircleArea::setTargetUnits(const vector<Coordinate*>& units) {
    coordinate.clear();
    for (Coordinate* unit : units) {
        coordinate.insert(unit);
    }
}
