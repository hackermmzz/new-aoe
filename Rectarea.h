#ifndef RECTAREA_H
#define RECTAREA_H
#include"AreaSelected.h"
#include"GameWidget.h"
//
struct RectAreaData{
    //
    double dr,ur;
    double w,h;
    int areaType;  // 0=原有区域(灰色), 1=巡逻区域(蓝色)
    
    RectAreaData() : dr(0), ur(0), w(0), h(0), areaType(0) {}
};
//
class RectArea:public AreaSelected
{
private:
    bool triger;
    RectAreaData current;
    GameWidget*widget;
    vector<RectAreaData>area;
    set<Coordinate*>coordinate;
public:
    multimap<Coordinate*,RectAreaData>relation;
    RectArea(GameWidget*widget);
    virtual void onLeftMouseDown();
    virtual void onLeftMouseUp();
    virtual void onMouseMove(int delta_x,int delta_y);
    virtual void Draw();
    virtual void onRightMouseClick();
    virtual void onRightMouseDown();
    RectAreaData* GetPosIn(double dr,double ur);
    static string Name();
    
    // 新增功能
    void setCurrentAreaType(int type);  // 设置当前绘制的区域类型
    QColor getAreaColor(int areaType);  // 根据区域类型获取颜色
    void setTargetUnits(const vector<class Coordinate*>& units);  // 设置要绑定的单位
};

#endif // RECTAREA_H
