#ifndef AI_H
#define AI_H
#include <QMutex>
#include "assert.h"
#include "GlobalVariate.h"

class AI:public QThread
{
public:
/*##########INTERFACE BEGINS HERE##########*/

    int HumanMove(int SN, double L0, double U0);

    int HumanAction(int SN,int obSN);

    int HumanBuild(int SN,int BuildingNum,int BlockL,int BlockU);

    int BuildingAction(int SN,int Action);

    virtual instruction getInsRet(int id)=0;

    void printInsRet(int id);

    double calDistance( double DR1 , double UR1 , double DR2 , double UR2 )
        { return pow(pow(DR1 - DR2, 2) + pow(UR1 - UR2, 2), 0.5); }

    void DebugText( string debugStr )
        { call_debugText("black" , " AI"+QString::number(id) + "打印：" + QString::fromStdString(debugStr)); }

    void DebugText( int debugInt )
        { call_debugText("black" , " AI"+QString::number(id) + "打印：" + QString::number(debugInt)); }

    void DebugText( double debugDouble )
        { call_debugText("black" , " AI"+QString::number(id) + "打印：" + QString::number(debugDouble)); }

/*###########INTERFACE ENDS HERE###########*/
    virtual void processData()=0;
    void run() override{
        if(!trylock()) return;  ///如果锁未被释放，则直接返回
        if(g_frame>10){
            processData();
        }
        unlock();
    }
    bool trylock(){
        return aiLock.tryLock();
    }
    void unlock(){
        aiLock.unlock();
    }
    AI(){}
    ~AI(){}
protected:
    QMutex aiLock;
    int id;
    virtual int AddToIns(instruction ins){
    }
    virtual void clearInsRet()=0;
private:
    bool isHuman(int SN){
        int type=SN/10000;
        return g_Object[SN]&&(type==SORT_ARMY||type==SORT_FARMER);
    }
    bool isBuilding(int SN){
        return g_Object[SN]&&SN/10000==SORT_BUILDING;
    }
};

#endif // AI_H
