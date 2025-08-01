#ifndef RECTAREA_H
#define RECTAREA_H
#include"AreaSelected.h"
#include"GameWidget.h"
//
struct RectAreaData{
    //
    double dr,ur;
    double w,h;
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
    map<Coordinate*,RectAreaData>relation;
    RectArea(GameWidget*widget);
    virtual void onLeftMouseDown();
    virtual void onLeftMouseUp();
    virtual void onMouseMove(int delta_x,int delta_y);
    virtual void Draw();
    virtual void onRightMouseClick();
    RectAreaData* GetPosIn(double dr,double ur);
    static string Name();
};

#endif // RECTAREA_H
