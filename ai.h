#ifndef AI_H
#define AI_H

#include <QMutex>
#include <QThread>
#include <QWaitCondition>
#include "assert.h"
#include "GlobalVariate.h"

class AI : public QThread
{
    Q_OBJECT

public:
    explicit AI(QObject *parent = nullptr) : QThread(parent), stopThread(false) {;}

    ~AI(){
        stopThread = true;
        condition.wakeAll();
        wait();
    }

    int HumanMove(int SN, double L0, double U0);
    int HumanAction(int SN, int obSN);
    int HumanBuild(int SN, int BuildingNum, int BlockL, int BlockU);
    int BuildingAction(int SN, int Action);
//    void printInsRet(int id);

public slots:
    void startProcessing() {
        if(!mutex.tryLock()){
            return;
        }
        stopThread = false;
        condition.wakeAll();
        mutex.unlock();
    }

    void stopProcessing() {
        QMutexLocker locker(&mutex);
        stopThread = true;
        condition.wakeAll();
    }

protected:
    int id;
    void run() override {
        while(true){
            QMutexLocker locker(&mutex);
            if(stopThread)
                return;
            if (g_frame > 10) {
                processData();
            }
            condition.wait(&mutex);
        }
    }

    virtual void processData() = 0;

    bool trylock() {
        return aiLock.tryLock();
    }

    void unlock() {
        aiLock.unlock();
    }

    virtual int AddToIns(instruction ins) = 0;
    virtual void clearInsRet() = 0;

private:
    QMutex aiLock;
    QMutex mutex;
    QWaitCondition condition;
    bool stopThread = false;

    bool isHuman(int SN) {
        int type = SN / 10000;
        return g_Object[SN] && (type == SORT_ARMY || type == SORT_FARMER);
    }

    bool isBuilding(int SN) {
        return g_Object[SN] && SN / 10000 == SORT_BUILDING;
    }

public:
    double calDistance(double DR1, double UR1, double DR2, double UR2) {
        return pow(pow(DR1 - DR2, 2) + pow(UR1 - UR2, 2), 0.5);
    }

    void DebugText(std::string debugStr) {
        call_debugText("black", " AI" + QString::number(id) + "打印：" + QString::fromStdString(debugStr));
    }

    void DebugText(int debugInt) {
        call_debugText("black", " AI" + QString::number(id) + "打印：" + QString::number(debugInt));
    }

    void DebugText(double debugDouble) {
        call_debugText("black", " AI" + QString::number(id) + "打印：" + QString::number(debugDouble));
    }
};

#endif // AI_H
