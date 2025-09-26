#ifndef LINEAREA_H
#define LINEAREA_H
#include"AreaSelected.h"
#include <GameWidget.h>
//
struct LineAreaData{
    using PD=array<double,2>;
    vector<PD>data;
    int areaType;  // 0=原有区域(灰色), 1=巡逻区域(蓝色)
    
    LineAreaData() : areaType(0) {}
};
//
class LineArea:public AreaSelected
{
public:
private:
    bool down;
    bool triger;
    LineAreaData current;
    GameWidget*widget;
    vector<LineAreaData>lines;
    set<Coordinate*>coordinate;
public:
    multimap<Coordinate*,LineAreaData>relation;
    LineArea(GameWidget*widget);
    virtual void onLeftMouseDown();
    virtual void onLeftMouseUp();
    virtual void onMouseMove(int delta_x,int delta_y);
    virtual void Draw();
    virtual void onRightMouseClick();
    virtual void onRightMouseDown();
    LineAreaData *GetPosIn(double dr,double ur);
    static string Name();
    
    // 新增功能
    void setCurrentAreaType(int type);  // 设置当前绘制的区域类型
    QColor getAreaColor(int areaType);  // 根据区域类型获取颜色
    void setTargetUnits(const vector<class Coordinate*>& units);  // 设置要绑定的单位
};

#endif // LINEAREA_H
