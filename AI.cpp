#include "ai.h"

std::vector<QString> AI::AIName= { "UsrAI", "EnemyAI" };

AI::~AI() {
    stopThread = true;
    condition.wakeAll();
    wait();
}

int AI::HumanMove(int SN, double DR0, double UR0){
    return AddToIns(instruction(INS_HUMANMOVE,SN,Double::FromDouble(DR0),Double::FromDouble(UR0)));
}

int AI::HumanAction(int SN,int obSN){
    return AddToIns(instruction(INS_HUMANACTION,SN,obSN , true));
}

int AI::HumanBuild(int SN, int BuildingNum, int BlockDR, int BlockUR){
    return AddToIns(instruction(INS_HUMANBUILD,SN,BlockDR,BlockUR,BuildingNum));
}

int AI::BuildingAction(int SN,int Action){
    return AddToIns(instruction(INS_BUILDINGACTION,SN,Action));
}

int AI::PinPointStrike(int SN, double DR0, double UR0)
{
    return AddToIns(instruction(INS_PINPOINT_STRIKE,SN,Double::FromDouble(DR0),Double::FromDouble(UR0)));
}

void AI::cheatAction() {
    if(!IsExamining){
        is_cheatAction = true;
    }
}

void AI::startProcessing() {
    if (!mutex.tryLock()) {
        return;
    }
    stopThread = false;
    condition.wakeAll();
    mutex.unlock();
}

void AI::stopProcessing() {
    QMutexLocker locker(&mutex);
    stopThread = true;
    condition.wakeAll();
}

void AI::run() {
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

bool AI::trylock() {
    return aiLock.tryLock();
}

void AI::unlock() {
    aiLock.unlock();
}

bool AI::isHuman(int SN) {
    int type = SN / 10000;
    return g_Object[SN] && (type == SORT_ARMY || type == SORT_FARMER);
}

bool AI::isBuilding(int SN) {
    int sort = SN / 10000;
    return g_Object[SN] && (sort == SORT_BUILDING || sort == SORT_Building_Resource);
}

double AI::calDistance(double DR1, double UR1, double DR2, double UR2)
{
    return pow(pow(DR1 - DR2, 2) + pow(UR1 - UR2, 2), 0.5);
}

void AI::DebugText(string debugStr)
{
    call_debugText("black", " " + AIName[id] + "打印：" + QString::fromStdString(debugStr), id);
}

void AI::DebugText(QString debugStr)
{
    call_debugText("black", " " + AIName[id] + "打印：" + debugStr, id);
}

void AI::DebugText(char *debugStr)
{
    call_debugText("black", " " + AIName[id] + "打印：" + QString::fromUtf8(debugStr), id);
}

void AI::DebugText(int debugInt)
{
    call_debugText("black", " " + AIName[id] + "打印：" + QString::number(debugInt), id);
}

void AI::DebugText(double debugdouble)
{
    call_debugText("black", " " + AIName[id] + "打印：" + QString::number(debugdouble), id);
}


