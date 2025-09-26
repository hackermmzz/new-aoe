#include "networkplugin.h"
#include<QNetworkReply>
#include<QTimer>
#include<QJsonDocument>
#include<QJsonObject>
NetworkPlugin::NetworkPlugin(QObject*parent) : QThread(parent)
{

}

void NetworkPlugin::postJson(QString url,map<QString,QString> header, QJsonObject json,uint timeout)
{
    auto task=[=](){
        QUrl postUrl(url);
        QNetworkRequest postRequest(postUrl);
        //设置请求头
        for(auto itr=header.begin();itr!=header.end();++itr){
            postRequest.setRawHeader(itr->first.toLocal8Bit(),itr->second.toLocal8Bit());
        }
        //data必须是json格式
        postRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        //获取数据
        QJsonDocument jsonDoc(json);
        QString data= jsonDoc.toJson(QJsonDocument::Indented);
        QByteArray postData;
        postData.append(data);
        //
        QEventLoop loop;
        connect(manager,&QNetworkAccessManager::finished,&loop,&QEventLoop::quit);
        //发送数据
        QNetworkReply*reply=manager->post(postRequest, postData);
        //设置超时器
        QTimer timer;
        timer.setSingleShot(true);
        connect(&timer,&QTimer::timeout,[&](){
            loop.quit();
            reply->abort();
        });
        timer.start(timeout*1000);
        // 处理响应
            if (reply->error() == QNetworkReply::NoError) {
                QByteArray response = reply->readAll();
                qDebug()<<response;
            } else {
                qDebug()<<reply->errorString();
            }
        //等待执行完成
        loop.exec();
        // 释放资源
        reply->deleteLater();
    };
    //
    AddTask(task);
}

void NetworkPlugin::waitDone()
{
    shouldExit=true;
    wait();
}

NetworkPlugin::~NetworkPlugin()
{
    delete manager;
}

void NetworkPlugin::run()
{
    //创建网络管理器
    manager=new QNetworkAccessManager(NULL);
    //
    while(!shouldExit || !taskQueue.empty()){
        lock.lock();
        //
        bool flag=0;
        TaskType task;
        if(!taskQueue.empty()){
            task=taskQueue.front();
            taskQueue.pop();
            flag=1;
        }
        //
        lock.unlock();
        //
        if(flag)task();
    }
}

void NetworkPlugin::AddTask(const NetworkPlugin::TaskType &task)
{
    //
    lock.lock();
    taskQueue.push(task);
    lock.unlock();
}
