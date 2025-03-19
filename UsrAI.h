#ifndef USRAI_H
#define USRAI_H

#include "AI.h"

extern tagGame tagUsrGame;
extern ins UsrIns;
/*##########璇峰嬁淇敼浠ヤ笂閮ㄥ垎##########*/
class UsrAI:public AI
{
public:
    UsrAI(){this->id=0;}
    ~UsrAI(){}


private:
  /***************AI閫氫俊鍑芥暟锛岃鍕夸慨鏀?**************/
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
    void BuildHome();//閫犳埧瀛?
    void ditu();//鍦板浘
    tagFarmer GetFarmer();//閫犲啘姘?
    void GetFarmer(tagFarmer&sn);//鎵惧啘姘?
    void RecycleFarmer(tagFarmer&farmer);
    int GetBuildingWidth(int Type,int reservced);
    pair<int,int>GetFittestBlockToBuildHouse(int houseType,int x,int y,int interval,int reserved0,int reserved1);
    bool CheckTaretBlockCanBuildTargetTypeBuilding(int houseType,int x,int y,int interval);
    pair<int,int>GetResourceCenter(vector<tagResource>&rsc);
    void  CreateFarmer();
    void CutDownTree();
    //void CreateArmy();
    void GoToWork();
    bool CheckCanBuildDock( int l, int u);
    void BuildDock();
    void PickGold();
    void GuoHe();
   // void UpdateWeapon();
    void PickFish();
   // pair<int,int>GetBlockToBuildDock(int houseType,int x,int y);
    pair<int,int>FindNearstBeach(int x,int y);
    void CreatBoat();
    pair<int,int> FindNearstBeachToBuild(int houseType,int x,int y,int reserved);
    bool CheckSea(int x,int y);
    //void SailingAttack();
    void Fish();
    //pair<double,double>FindNearstGold(double x,double y);
    tagFarmer boat();
    void boat(tagFarmer&sn);
    //pair<int x,int y>FindNearstSea(int x,int y);
};


#endif // USRAI_H
