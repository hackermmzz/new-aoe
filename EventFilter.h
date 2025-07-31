#ifndef EVENTFILTER_H
#define EVENTFILTER_H
#include<QWidget>
#include<QEvent>
#include<iostream>
#include<list>
using std::list;
//任何需要使用EventFilter的派生类必须继承该类
class EventFilterBase{
protected:
    bool filter;
public:
    EventFilterBase();
    virtual void onClick();
    virtual void onLeftMouseDown();
    virtual void onLeftMouseUp();
    virtual void onMouseMove(int delta_x,int delta_y);
    int MouseX();
    int MouseY();
    bool IsFilter();
    void SetFilter(bool f);
    ~EventFilterBase();
};
//
class EventFilter:public QObject
{
private:
    list<EventFilterBase*>object;
    bool mouseDown;
    bool mouseUp;
    bool mouseClicked;
    int mouseMoveX;
    int mouseMoveY;
    int mouseX;
    int mouseY;
    void ResetState();
    void UpdateObject();
    void SetMouseClicked();
    void SetMouseUp();
    void SetMouseDown();
    void EventProcess(QObject *obj, QEvent*event);
public:
    EventFilter();
    bool MouseDown();
    bool MouseUp();
    bool MouseClicked();
    int MouseMoveDeltaX();
    int MouseMoveDeltaY();
    void AddObject(EventFilterBase*obj);
    void RemoveObject(EventFilterBase*obj);
    int MouseX();
    int MouseY();
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // EVENTFILTER_H
