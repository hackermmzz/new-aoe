#ifndef LINEAREA_H
#define LINEAREA_H
#include<areaselected.h>
#include<GameWidget.h>
//
struct LineAreaData{
    using PD=array<double,2>;
    vector<PD>data;
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
    map<Coordinate*,LineAreaData>relation;
    LineArea(GameWidget*widget);
    virtual void onLeftMouseDown();
    virtual void onLeftMouseUp();
    virtual void onMouseMove(int delta_x,int delta_y);
    virtual void Draw();
    virtual void onRightMouseClick();
    LineAreaData *GetPosIn(double dr,double ur);
    static string Name();
};

#endif // LINEAREA_H
