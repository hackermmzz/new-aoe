#ifndef USRAI_H
#define USRAI_H

#include "AI.h"

extern tagGame tagUsrGame;
extern ins UsrIns;
/*##########DO NOT MODIFY THE CODE ABOVE##########*/

class UsrAI:public AI
{
public:
    UsrAI(){this->id=0;}
    ~UsrAI(){}

private:
    void processData() override;
    tagInfo getInfo(){return tagUsrGame.getInfo();}
    int AddToIns(instruction ins) override
    {
        UsrIns.lock.lock();
        ins.id=UsrIns.g_id;
        UsrIns.g_id++;
        UsrIns.instructions.push(ins);
        UsrIns.lock.unlock();
        return ins.id;
    }
    void clearInsRet() override
    {
        tagUsrGame.clearInsRet();
    }
    /*##########DO NOT MODIFY THE CODE IN THE CLASS##########*/
    bool tryBuildBuilding(int buildingType, int requiredWood, int maxCount, int reqiredBuilding,int is_settler=0);
    //my function
    void InitAll();
    void InitActionNode();
    void UpdateAll();
    vector<int>AllocateFarmers(int n,int x,int y,bool lock=1);
    void AllocateFarmers(vector<int>&v,int n, int x, int y, bool lock=1);
    void FreeFarmers(const vector<int>sn);
    array<int,2> FindSpace(int size,int x,int y,bool land);
    array<int,2> FindForBuilding(int type,int x,int y);
    array<int,2> FindForBuilding(int tar,int src,int x,int y,int dis);
    bool InMyLand(int x,int y);
    bool IsYanAn(int x,int y);//判断2x2的方块是否沿岸
    int Dis(tagObj&t0,tagObj&t1);
    //
};

/*##########YOUR CODE BEGINS HERE##########*/
    
/*##########YOUR CODE ENDS HERE##########*/
#endif // USRAI_H
