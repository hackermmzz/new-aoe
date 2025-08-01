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
    virtual void onLeftMouseDown()=0;//摁下才算触发
    virtual void onLeftMouseUp()=0;//释放完成一次区域限制
    virtual void onMouseMove(int delta_x,int delta_y)=0;//鼠标移动改变区域的范围
    virtual void Draw()=0;//区域的绘制
    virtual void onRightMouseClick()=0;//选定目标,设置目标的区域为该区域
    //获取子类组件名称
    template<class T>
    std::string GetName(){
        return T::Name();
    }
};


#endif // AREASELECTED_H
