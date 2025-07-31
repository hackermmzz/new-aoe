#ifndef CIRCLEAREA_H
#define CIRCLEAREA_H

#include"AreaSelected.h"
#include"GameWidget.h"
//
struct CircleAreaData{
    //
    double dr,ur;
    double rad;
};
//
class CircleArea:public AreaSelected
{
private:
    bool triger;
    CircleAreaData current;
    GameWidget*widget;
    vector<CircleAreaData>area;
public:
    CircleArea(GameWidget*widget);
    virtual void onClick();
    virtual void onLeftMouseDown();
    virtual void onLeftMouseUp();
    virtual void onMouseMove(int delta_x,int delta_y);
    virtual void Draw();
    vector<array<double,4>>GetCircle(CircleAreaData&data);
    CircleAreaData* GetPosIn(double dr, double ur);
    virtual  string GetName();
    static string Name();
};

#endif // CIRCLEAREA_H
