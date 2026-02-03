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

    ~AI();

    int HumanMove(int SN, double DR0, double UR0);
    int HumanAction(int SN, int obSN);
    int HumanBuild(int SN, int BuildingNum, int BlockDR, int BlockUR);
    int BuildingAction(int SN, int Action);
    //    void printInsRet(int id);
    void cheatAction();

public slots:
    void startProcessing();

    void stopProcessing();

protected:
    int id;
    void run() override;

    virtual void processData() = 0;

    bool trylock();

    void unlock();

    virtual int AddToIns(instruction ins) = 0;
    virtual void clearInsRet() = 0;

private:
    QMutex aiLock;
    QMutex mutex;
    QWaitCondition condition;
    bool stopThread = false;
    static std::vector<QString> AIName;

    bool isHuman(int SN);

    bool isBuilding(int SN);

public:
    int ProcessDataWork = 0;

    double calDistance(double DR1, double UR1, double DR2, double UR2);

    void DebugText(std::string debugStr);

    void DebugText(QString debugStr);
    void DebugText(char* debugStr);


    void DebugText(int debugInt);

    void DebugText(double debugDouble);
signals:
    void cheatRes();
    void cheatAttack(int wave);
};

#endif // AI_H
