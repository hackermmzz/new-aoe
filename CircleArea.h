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
    set<Coordinate*>coordinate;
public:
    map<Coordinate*,CircleAreaData>relation;
    CircleArea(GameWidget*widget);
    virtual void onLeftMouseDown();
    virtual void onLeftMouseUp();
    virtual void onMouseMove(int delta_x,int delta_y);
    virtual void Draw();
    virtual void onRightMouseClick();
    vector<array<double,4>>GetCircle(CircleAreaData&data);
    CircleAreaData* GetPosIn(double dr, double ur);
    static string Name();

};

#endif // CIRCLEAREA_H
