#ifndef BROADCAST_H
#define BROADCAST_H

#include<functional>
#include<vector>
using namespace std;
class BroadCast
{
public:
    using TaskType=function<void(void)>;
protected:
    vector<TaskType> tasks;
public:
    BroadCast();
    void subscribe(TaskType task);
    template<class T,class Ret,class...ARG>
    void subscribe(T*object,Ret (T::*MemberFun)(ARG...),ARG...arg);
    template<class T,class Ret>
    void subscribe(T*object,Ret (T::*MemberFun)(void));
    void broadcast();
};

template<class T, class Ret>
void BroadCast::subscribe(T *object, Ret (T::*MemberFun)())
{
    subscribe([=](){
        (object->*MemberFun)();
    });
}

template<class T, class Ret, class...ARG>
void BroadCast::subscribe(T *object, Ret (T::*MemberFun)(ARG...),ARG...arg)
{
    subscribe([=](){
        (object->*MemberFun)(arg...);
    });
}

#endif // BROADCAST_H
