#include "EventFilter.h"
#include"QMouseEvent"
#include<QDateTime>
void EventFilter::ResetState()
{
    mouseClicked=0;
    mouseUp=0;
    mouseDown=0;
    mouseMoveX=mouseMoveY=0;
}

void EventFilter::UpdateObject()
{
    for(auto*obj:object){
        //如果当前对象屏蔽事件,直接跳过
        if(obj->IsFilter())continue;
        //
        if(MouseClicked()){
            obj->onClick();
        }
        //
        if(MouseDown()){
            obj->onLeftMouseDown();
        }
        //
        if(MouseUp()){
            obj->onLeftMouseUp();
        }
        //
        if(mouseMoveX!=0||mouseMoveY!=0){
            obj->onMouseMove(mouseMoveX,mouseMoveY);
        }
    }
}

void EventFilter::SetMouseClicked()
{
    mouseClicked=1;
}

void EventFilter::SetMouseUp()
{
    mouseUp=true;
    mouseDown=false;
}

void EventFilter::SetMouseDown()
{
    mouseDown=true;
    mouseUp=false;
}

void EventFilter::EventProcess(QObject *obj, QEvent *event)
{
    //只处理GameWidget传来的事件
    if(strcmp(obj->metaObject()->className(),"GameWidget"))return;
    //更新鼠标位置
    switch (event->type()) {
        case QEvent::MouseButtonPress:case QEvent::MouseButtonRelease:case QEvent::MouseMove:
        {
            QMouseEvent*e=(QMouseEvent*)event;
            mouseX=e->x();
            mouseY=e->y();
            break;
        }
        default:
            break;
    }
    // 捕获鼠标按下事件
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent*e=(QMouseEvent*)event;
        if(e->button()==Qt::LeftButton){
            SetMouseDown();
        }
    }
    // 捕获鼠标释放事件
    else if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent*e=(QMouseEvent*)event;
        if(e->button()==Qt::LeftButton){
            SetMouseClicked();
            SetMouseUp();
        }
    }
    // 捕获鼠标移动事件
    else if (event->type() == QEvent::MouseMove) {
        QMouseEvent*e=(QMouseEvent*)event;
        int x=MouseX(),y=MouseY();
        static int lastx=-1,lasty=-1;
        if(lastx==-1){
            lastx=x,lasty=y;
        }
        //
        mouseMoveX=x-lastx;
        mouseMoveY=y-lasty;
        lastx=x;
        lasty=y;
    }
}

EventFilter::EventFilter()
{
    mouseClicked=mouseDown=0;
    mouseMoveX=mouseMoveY=0;
    mouseUp=1;
}

bool EventFilter::MouseDown()
{
    return mouseDown;
}

bool EventFilter::MouseUp()
{
    return mouseUp;
}

bool EventFilter::MouseClicked()
{
    return mouseClicked;
}

int EventFilter::MouseMoveDeltaX()
{
     return mouseMoveX;
}

int EventFilter::MouseMoveDeltaY()
{
    return mouseMoveY;
}

void EventFilter::AddObject(EventFilterBase *obj)
{
    object.push_back(obj);
}

void EventFilter::RemoveObject(EventFilterBase *obj)
{
    object.erase(std::find(object.begin(),object.end(),obj));
}

int EventFilter::MouseX()
{
    return mouseX;
}

int EventFilter::MouseY()
{
    return mouseY;
}


bool EventFilter::eventFilter(QObject *obj, QEvent *event)
{
    //
    {
        //更新事件
        EventProcess(obj,event);
        //更新所有组件
        UpdateObject();
        //重置所有一次性状态
        ResetState();
    }
    // 继续传递事件，不影响其他组件
    return QObject::eventFilter(obj, event);
}

EventFilterBase::EventFilterBase()
{
    extern EventFilter *eventFilter;
    eventFilter->AddObject(this);
    //
    filter=false;
}

void EventFilterBase::onClick()
{

}

void EventFilterBase::onLeftMouseDown()
{

}

void EventFilterBase::onLeftMouseUp()
{

}

void EventFilterBase::onMouseMove(int delta_x, int delta_y)
{

}

int EventFilterBase::MouseX()
{
    extern EventFilter *eventFilter;
    return eventFilter->MouseX();
}

int EventFilterBase::MouseY()
{
    extern EventFilter *eventFilter;
    return eventFilter->MouseY();
}

bool EventFilterBase::IsFilter()
{
    return filter;
}

void EventFilterBase::SetFilter(bool f)
{
    filter=f;
}

EventFilterBase::~EventFilterBase()
{
      extern EventFilter *eventFilter;
      eventFilter->RemoveObject(this);
}
