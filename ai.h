#ifndef AI_H
#define AI_H
#include <QMutex>
#include "assert.h"
#include "GlobalVariate.h"
#include "Building.h"

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
    AI(){;}
    ~AI(){;}
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
