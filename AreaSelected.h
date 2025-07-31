#ifndef AREASELECTED_H
#define AREASELECTED_H
#include"config.h"
#include"digitalConfig.h"
#include<qevent.h>
#include<QWidget>
#include<vector>
#include<eventfilter.h>
using std::vector;

class AreaSelected: public EventFilterBase
{
public:
    virtual void onClick()=0;
    virtual void onLeftMouseDown()=0;
    virtual void onLeftMouseUp()=0;
    virtual void onMouseMove(int delta_x,int delta_y)=0;
    virtual void Draw()=0;
    virtual std::string GetName()=0;
};


#endif // AREASELECTED_H
