#ifndef EVENTFILTER_H
#define EVENTFILTER_H
#include<QWidget>
#include<QEvent>
#include<iostream>
#include<list>
#include<functional>
using std::function;
using std::list;
//任何需要使用EventFilter的派生类必须继承该类
class EventFilterBase{
private:
    bool aborded;
protected:
    bool filter;
    list<EventFilterBase*>child;
public:
    EventFilterBase();
    virtual void onClick();
    virtual void onLeftMouseDown();
    virtual void onLeftMouseUp();
    virtual void onRightMouseUp();
    virtual void onRightMouseDown();
    virtual void onRightMouseClick();
    virtual void onMouseMove(int delta_x,int delta_y);
    virtual void onLeftMousePress();
    virtual void onRightMousePress();
    virtual void onWheelDown();
    virtual void onWheelScroll(int delta);
    virtual void onWheelUp();
    virtual void onWheelClick();
    virtual void onWheelPress();
    int MouseX();
    int MouseY();
    bool IsFilter();
    void SetFilter(bool f);
    void abort();
    bool GetAbord();//一次获取立刻置false
    list<EventFilterBase*>& GetChild();
    ~EventFilterBase();
};
//
class EventFilter:public QObject
{
private:
    list<EventFilterBase*>object;
    list<function<void()>>reciverFun;
    bool leftmouseDown;
    bool leftmouseUp;
    bool leftmouseClicked;
    bool leftmousePress;
    bool rightmouseDown;
    bool rightmouseUp;
    bool rightmouseClicked;
    bool rightmousePress;
    bool wheeldown;
    bool wheelup;
    bool wheelClicked;
    bool wheelPress;
    int mouseMoveX;
    int mouseMoveY;
    int mouseX;
    int mouseY;
    int wheelScroll;
    void ResetState();
    void UpdateObject();
    void UpdateReciverFun();
    void SetLeftMouseUp();
    void SetLeftMouseDown();
    void SetRightMouseUp();
    void SetRightMouseDown();
    void SetWheelDown();
    void SetWheelUp();
    void EventProcess(QObject *obj, QEvent*event);
public:
    EventFilter();
    bool LeftMouseDown();
    bool LeftMouseUp();
    bool LeftMouseClicked();
    bool RightMouseDown();
    bool RightMouseUp();
    bool RightMouseClicked();
    bool LeftMousePress();
    bool RightMousePress();
    bool MouseMove();
    bool WheelDown();
    bool WheelUp();
    bool WheelClicked();
    bool WheelPress();
    bool WheelScroll();
    int MouseMoveDeltaX();
    int MouseMoveDeltaY();
    int WheelScrollDelta();
    void AddObject(EventFilterBase*obj);
    void RegistReciver(const function<void()>&fun);
    void RemoveObject(EventFilterBase*obj);
    int MouseX();
    int MouseY();
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // EVENTFILTER_H
