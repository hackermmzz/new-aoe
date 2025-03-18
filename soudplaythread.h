#ifndef SOUDPLAYTHREAD_H
#define SOUDPLAYTHREAD_H
#include <qthread.h>
#include <qdebug.h>
#include<iostream>
#include<QMutex>
#include<queue>
#include<GlobalVariate.h>
using namespace std;
class SoudPlayThread:public QThread
{
private:
    QMutex mutex;
    queue<string> soundQue;
public:
    SoudPlayThread();
    void AddSound(queue<string>&sounds);
protected:
    void run();
};

#endif // SOUDPLAYTHREAD_H
