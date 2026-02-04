#include "EventFilter.h"
#include"QMouseEvent"
#include<QDateTime>
#include<functional>
using std::function;
void EventFilter::ResetState()
{
    leftmouseClicked=0;
    leftmouseUp=0;
    leftmouseDown=0;
   // leftmousePress=0;
    rightmouseClicked=0;
    rightmouseDown=0;
    rightmouseUp=0;
   // rightmousePress=0;
    mouseMoveX=mouseMoveY=0;
    //
    wheelClicked=0;
    wheeldown=0;
    wheelup=0;
    wheelScroll=0;
}

void EventFilter::UpdateObject()
{
    function<void(list<EventFilterBase*>&,bool,bool,bool,bool,bool,bool,bool,bool,bool,bool,bool,bool,bool,bool)>dfs=
    [&](list<EventFilterBase*>&objects,
            bool LeftMouseClicked,bool LeftMouseDown,bool LeftMouseUp,bool LeftMousePress,
            bool RightMouseClicked,bool RightMouseDown,bool RightMouseUp,bool RightMousePress,
            bool MouseMove,
            bool WheelClicked,bool WheelDown,bool WheelUp,bool WheelPress,
            bool WheelScroll
            )->void{
        for(auto*obj:objects){
            //如果当前对象屏蔽事件,直接跳过(子孙也收不到消息)
            if(obj->IsFilter())continue;
            //存储传递给子孙节点的数据
            bool mouseclicked=LeftMouseClicked;
            bool leftmousedown=LeftMouseDown;
            bool leftmouseup=LeftMouseUp;
            bool rightmouseclick=RightMouseClicked;
            bool rightmousedown=RightMouseDown;
            bool rightmouseup=RightMouseUp;
            bool mousemove=MouseMove;
            bool wheelclicked=WheelClicked;
            bool wheeldown=WheelDown;
            bool wheelup=WheelUp;
            bool wheelscroll=WheelScroll;
            bool leftmousepress=LeftMousePress;
            bool rightmousepress=RightMousePress;
            bool wheelpress=WheelPress;
            //鼠标左键点击
            if(LeftMouseClicked){
                obj->onClick();
                if(obj->GetAbord()){
                    mouseclicked=0;
                }
            }
            //鼠标左键摁下
            if(LeftMouseDown){
                obj->onLeftMouseDown();
                if(obj->GetAbord()){
                    leftmousedown=0;
                }
            }
            //鼠标左键释放
            if(LeftMouseUp){
                obj->onLeftMouseUp();
                if(obj->GetAbord()){
                    leftmouseup=0;
                }
            }
            //鼠标左键持续摁下
            if(LeftMousePress){
                obj->onLeftMousePress();
                if(obj->GetAbord()){
                    leftmousepress=0;
                }
            }
            //鼠标邮件点击
            if(RightMouseClicked){
                obj->onRightMouseClick();
                if(obj->GetAbord()){
                    rightmouseclick=0;
                }
            }
            //鼠标邮件摁下
            if(RightMouseDown){
                obj->onRightMouseDown();
                if(obj->GetAbord()){
                    rightmousedown=0;
                }
            }
            //鼠标右键释放
            if(RightMouseUp){
                obj->onRightMouseUp();
                if(obj->GetAbord()){
                    rightmouseup=0;
                }
            }
            //鼠标右键持续摁下
            if(RightMousePress){
                obj->onRightMousePress();
                if(obj->GetAbord()){
                    rightmousepress=0;
                }
            }
            //鼠标移动
            if(MouseMove){
                obj->onMouseMove(MouseMoveDeltaX(),MouseMoveDeltaY());
                if(obj->GetAbord()){
                    mousemove=0;
                }
            }
            //滚轮点击
            if(WheelClicked){
                obj->onWheelClick();
                if(obj->GetAbord()){
                    wheelclicked=0;
                }
            }
            //滚轮按下
            if(WheelDown){
                obj->onWheelDown();
                if(obj->GetAbord()){
                    wheeldown=0;
                }
            }
            //滚轮释放
            if(WheelUp){
                obj->onWheelUp();
                if(obj->GetAbord()){
                    wheelup=0;
                }
            }
            //滚轮滚动
            if(WheelScroll){
                obj->onWheelScroll(WheelScrollDelta());
                if(obj->GetAbord()){
                    wheelscroll=0;
                }
            }
            //滚轮持续摁下
            if(WheelPress){
                obj->onWheelPress();
                if(obj->GetAbord()){
                    wheelpress=0;
                }
            }
            //更新状态,遍历孩子
            dfs(obj->GetChild(),
                mouseclicked,leftmousedown,leftmouseup,leftmousepress,
                rightmouseclick,rightmousedown,rightmouseup,rightmousepress,
                mousemove,
                wheelclicked,wheeldown,wheelup,wheelpress,
                wheelscroll
                );
        }
    };
    //////////////////////////////
    dfs(object,
        LeftMouseClicked(),LeftMouseDown(),LeftMouseUp(),LeftMousePress(),
        RightMouseClicked(),RightMouseDown(),RightMouseUp(),RightMousePress(),
        MouseMove(),
        WheelClicked(),WheelDown(),WheelUp(),WheelPress(),
        WheelScroll()
        );
}

void EventFilter::UpdateReciverFun()
{
    for(auto&fun:reciverFun){
        fun();
    }
}


void EventFilter::SetLeftMouseUp()
{
    //判断是否生成点击
    if(leftmousePress)leftmouseClicked=true;
    else leftmouseClicked=false;
    //更新其他受影响的状态
    leftmouseUp=true;
    leftmouseDown=false;
    leftmousePress=false;
}

void EventFilter::SetLeftMouseDown()
{
    leftmouseDown=true;
    leftmouseClicked=false;
    leftmouseUp=false;
    leftmousePress=true;
}


void EventFilter::SetRightMouseUp()
{
    //

    if(rightmousePress)rightmouseClicked=true;
    else rightmouseClicked=false;
    //
    rightmouseDown=false;
    rightmouseUp=true;
    rightmousePress=false;
}

void EventFilter::SetRightMouseDown()
{
    rightmouseDown=true;
    rightmouseUp=false;
    rightmouseClicked=false;
    rightmousePress=true;
}

void EventFilter::SetWheelDown()
{
    wheelClicked=false;
    wheeldown=true;
    wheelPress=true;
    wheelup=false;
}

void EventFilter::SetWheelUp()
{
    if(wheelPress)wheelClicked=true;
    else wheelClicked=false;
    //
    wheeldown=false;
    wheelPress=false;
    wheelup=true;
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
            SetLeftMouseDown();
        }else if(e->button()==Qt::RightButton){
            SetRightMouseDown();
        }else if(e->button()==Qt::MiddleButton){
            SetWheelDown();
        }
    }
    // 捕获鼠标释放事件
    else if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent*e=(QMouseEvent*)event;
        if(e->button()==Qt::LeftButton){
            SetLeftMouseUp();
        }else if(e->button()==Qt::RightButton){
            SetRightMouseUp();
        }else if(e->button()==Qt::MiddleButton){
            SetWheelUp();
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
    //捕获滚轮滚动
    else if(event->type()==QEvent::Wheel){
        QWheelEvent*e=(QWheelEvent*)event;
        wheelScroll=e->angleDelta().y();
    }
}

EventFilter::EventFilter()
{
    ResetState();
    //
    leftmousePress=rightmousePress=false;
}

bool EventFilter::LeftMouseDown()
{
    return leftmouseDown;
}

bool EventFilter::LeftMouseUp()
{
    return leftmouseUp;
}

bool EventFilter::LeftMouseClicked()
{
    return leftmouseClicked;
}

bool EventFilter::RightMouseDown()
{
    return rightmouseDown;
}

bool EventFilter::RightMouseUp()
{
    return rightmouseUp;
}

bool EventFilter::RightMouseClicked()
{
    return rightmouseClicked;
}

bool EventFilter::LeftMousePress()
{
    return leftmousePress;
}

bool EventFilter::RightMousePress()
{
    return rightmousePress;
}

bool EventFilter::MouseMove()
{
    return MouseMoveDeltaX()!=0||MouseMoveDeltaY()!=0;
}

bool EventFilter::WheelDown()
{
    return wheeldown;
}

bool EventFilter::WheelUp()
{
    return wheelup;
}

bool EventFilter::WheelClicked()
{
    return wheelClicked;
}

bool EventFilter::WheelPress()
{
    return wheelPress;
}

bool EventFilter::WheelScroll()
{
    return wheelScroll!=0;
}

int EventFilter::MouseMoveDeltaX()
{
     return mouseMoveX;
}

int EventFilter::MouseMoveDeltaY()
{
    return mouseMoveY;
}

int EventFilter::WheelScrollDelta()
{
    return wheelScroll;
}

void EventFilter::AddObject(EventFilterBase *obj)
{
    object.push_back(obj);
}

void EventFilter::RegistReciver(const function<void ()> &fun)
{
    reciverFun.push_back(fun);
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
        //更新接收函数
        UpdateReciverFun();
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
    aborded=false;
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

void EventFilterBase::onRightMouseUp()
{

}

void EventFilterBase::onRightMouseDown()
{

}

void EventFilterBase::onRightMouseClick()
{

}

void EventFilterBase::onMouseMove(int delta_x, int delta_y)
{

}

void EventFilterBase::onLeftMousePress()
{

}

void EventFilterBase::onRightMousePress()
{

}

void EventFilterBase::onWheelDown()
{

}

void EventFilterBase::onWheelUp()
{

}

void EventFilterBase::onWheelClick()
{

}

void EventFilterBase::onWheelPress()
{

}

void EventFilterBase::onWheelScroll(int delta)
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

bool EventFilterBase::GetAbord()
{
    bool ret=aborded;
    aborded=false;
    return ret;
}

list<EventFilterBase *> &EventFilterBase::GetChild()
{
    return child;
}

EventFilterBase::~EventFilterBase()
{
      extern EventFilter *eventFilter;
      eventFilter->RemoveObject(this);
}
