#ifndef NETWORKPLUGIN_H
#define NETWORKPLUGIN_H
#include<QNetworkAccessManager>
#include <QThread>
#include<vector>
#include<functional>
#include<queue>
#include<mutex>
#include<QEventLoop>
#include <QDebug>
using namespace std;
//不支持多线程
class NetworkPlugin : public QThread
{
using TaskType=function<void(void)>;
    Q_OBJECT
public:
    explicit NetworkPlugin(QObject*parent=NULL);
    void postJson(QString url,map<QString,QString>header, QJsonObject json,uint timeout=30);
    void waitDone();//等待所有请求发送完毕
    ~NetworkPlugin();
protected:
    void run() override;
private:
    void AddTask(const TaskType&task);
private:
    QNetworkAccessManager *manager;
    queue<TaskType>taskQueue;
    mutex lock;
    bool shouldExit;
};

#endif // NETWORKPLUGIN_H
