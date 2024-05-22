#ifndef AI_H
#define AI_H
#include <QMutex>
#include "assert.h"
#include "GlobalVariate.h"
#include "Coordinate.h"
#include "Building.h"

class AI:public QThread
{
public:
/*##########INTERFACE BEGINS HERE##########*/
    int HumanMove(int SN, double L0, double U0);

    int HumanAction(int SN,int obSN);

    int HumanBuild(int SN,int BuildingNum,int BlockL,int BlockU);

    int BuildingAction(int SN,int Action);

    instruction getInsRet(int id);

    void printInsRet(int id);

    void clearInsRet();
/*###########INTERFACE ENDS HERE###########*/

    AI(){}
    ~AI(){}
    virtual void processData(){}
    void run() override{
        if(!trylock()) return;  ///如果锁未被释放，则直接返回
        if(AIGame.GameFrame>10){
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
protected:
    QMutex aiLock;
    int id;
    int AddToIns(instruction ins){
        if(id==0){
            UsrIns.lock.lock();
            ins.id=UsrIns.g_id;
            UsrIns.g_id++;
            UsrIns.instructions.push(ins);
            UsrIns.lock.unlock();
            return ins.id;
        }else if(id==1){

            return -1;
        }
    }
    bool isHuman(Coordinate* self){
        return (self!=nullptr&&(self->getSort()==SORT_FARMER||self->getSort()==SORT_ARMY));
    }
    bool isBuilding(Coordinate* self){
        return (self!=nullptr&&(self->getSort()==SORT_BUILDING));
    }
    bool isAnimalTree(Coordinate* self){
        return self->getSort()==SORT_ANIMAL&&(self->getNum()==ANIMAL_FOREST||self->getNum()==ANIMAL_TREE);
    }
};

#endif // AI_H
