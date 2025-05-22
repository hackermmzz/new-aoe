#ifndef AI_H
#define AI_H

#include <QMutex>
#include <QThread>
#include <QWaitCondition>
#include "assert.h"
#include "GlobalVariate.h"
#include<iostream>
extern bool is_cheatAction;

class AI : public QThread
{
    Q_OBJECT

public:
    explicit AI(QObject* parent = nullptr) : QThread(parent), stopThread(false) { ; }

    ~AI() {
        stopThread = true;
        condition.wakeAll();
        wait();
    }

    int HumanMove(int SN, double DR0, double UR0);
    int HumanAction(int SN, int obSN);
    int HumanBuild(int SN, int BuildingNum, int BlockDR, int BlockUR);
    int BuildingAction(int SN, int Action);
    //    void printInsRet(int id);
    void cheatAction() { is_cheatAction = true; }

public slots:
    void startProcessing() {
        if (!mutex.tryLock()) {
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
        while (true) {
            QMutexLocker locker(&mutex);
            if (stopThread)
                return;
            if (g_frame > 10) {
                ProcessDataWork = 1;
                processData();
                ProcessDataWork = 0;
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
    static QString AIName[NOWPLAYER];

    bool isHuman(int SN) {
        int type = SN / 10000;
        return g_Object[SN] && (type == SORT_ARMY || type == SORT_FARMER);
    }

    bool isBuilding(int SN) {
        int sort = SN / 10000;
        return g_Object[SN] && (sort == SORT_BUILDING || sort == SORT_Building_Resource);
    }

public:
    int ProcessDataWork = 0;

    double calDistance(double DR1, double UR1, double DR2, double UR2)
    {
        return pow(pow(DR1 - DR2, 2) + pow(UR1 - UR2, 2), 0.5);
    }

    void DebugText(std::string debugStr)
    {
        call_debugText("black", " " + AIName[id] + "打印：" + QString::fromStdString(debugStr), id);
    }

    void DebugText(QString debugStr)
    {
        call_debugText("black", " " + AIName[id] + "打印：" + debugStr, id);
    }
    void DebugText(char* debugStr)
    {
        call_debugText("black", " " + AIName[id] + "打印：" + QString::fromUtf8(debugStr), id);
    }


    void DebugText(int debugInt)
    {
        call_debugText("black", " " + AIName[id] + "打印：" + QString::number(debugInt), id);
    }

    void DebugText(double debugDouble)
    {
        call_debugText("black", " " + AIName[id] + "打印：" + QString::number(debugDouble), id);
    }
signals:
    void cheatRes();
    void cheatAttack(int wave);
};

#endif // AI_H
