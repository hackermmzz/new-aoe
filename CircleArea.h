#ifndef CIRCLEAREA_H
#define CIRCLEAREA_H

#include"AreaSelected.h"
#include"GameWidget.h"
//
struct CircleAreaData{
    //
    double dr,ur;
    double rad;
    int areaType;  // 0=原有区域(灰色), 1=巡逻区域(蓝色)
    
    CircleAreaData() : dr(0), ur(0), rad(0), areaType(0) {}
};
//
class CircleArea:public AreaSelected
{
private:
    bool triger;
    CircleAreaData current;
    GameWidget*widget;
    vector<CircleAreaData>area;
    set<Coordinate*>coordinate;
public:
    multimap<Coordinate*,CircleAreaData>relation;
    CircleArea(GameWidget*widget);
    virtual void onLeftMouseDown();
    virtual void onLeftMouseUp();
    virtual void onMouseMove(int delta_x,int delta_y);
    virtual void Draw();
    virtual void onRightMouseClick();
    virtual void onRightMouseDown();
    vector<array<double,4>>GetCircle(CircleAreaData&data);
    CircleAreaData* GetPosIn(double dr, double ur);
    static string Name();
    
    // 新增功能
    void setCurrentAreaType(int type);  // 设置当前绘制的区域类型
    QColor getAreaColor(int areaType);  // 根据区域类型获取颜色
    void setTargetUnits(const vector<class Coordinate*>& units);  // 设置要绑定的单位

};

#endif // CIRCLEAREA_H
