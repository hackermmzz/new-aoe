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
    vector<int>AllocateFarmers(int n,int x,int y,bool lock=1,bool force=0);
    void AllocateFarmers(vector<int>&v,int n, int x, int y, bool lock=1);
    void FreeFarmers(const vector<int>sn);
    array<int,2> FindSpace(int size,int x,int y,bool land,int index=-1);
    array<int,2> FindForBuilding(int type,int x,int y,int index=-1);
    array<int,2> FindForBuilding(int tar,int src,int x,int y,int dis,int index=-1);
    array<int,2> FindForBuildingInEnemyLand(int type,int x,int y);
    bool InMyLand(int x,int y);
    bool IsYanAn(int x,int y);//判断2x2的方块是否沿岸
    int Dis(const tagObj&t0,const tagObj&t1);
    double Distance(const tagObj&t0,const tagObj&t1);
    double Distance(const array<double,2>&p0,const array<double,2>&p1);
    template<class T>
    bool checkWorkObjIsTargetType(int sn,vector<T>&v);
    template<class T,class U>
    int Count(const vector<T>&v,U op);
    template<class T,class U>
    vector<T> findTargetObj(const vector<T>&v,U op);
    bool checkIsSeaSide(int i,int j,bool landF=1);
    bool checkTargetBlockIsEnemySideSea(double i,double j);
    void GoToCutTree(int sn);//控制闲散人员去砍树
    bool checkIsKaoAn(int i,int j,int index);
    void BuildBuilding(const vector<int>&farmer,int x,int y,int type,const function<bool(void)>&check=[](){return true;});
    bool IsBuilt(int x,int y);
    bool IsExistBuildTask(int x,int y);
    vector<Point>GetUnExplore(int x,int y,bool land=1);
    bool CheckLandPosition(int i,int j,int ti,int tj);
    bool CheckOceanPosition(int i,int j,int ti,int tj);
    bool CheckGoldReach(int x,int y);
    tagObj *GetObj(int sn);
    template<class U,class T>
    bool checkCondition(const U&buf,const T&op);
    //
};

/*##########YOUR CODE BEGINS HERE##########*/

/*##########YOUR CODE ENDS HERE##########*/
#endif // USRAI_H
