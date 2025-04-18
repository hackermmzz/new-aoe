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
    void ditu();
        void BuildHome();
        void RecycleFarmer(tagFarmer &farmer);
        void GetFarmer(tagFarmer &farmer);
        tagFarmer GetFarmer();
        pair<int, int> GetFittestBlockToBuildHouse(int houseType, int x, int y);
        int GetBuildingWidth(int Type);
        bool CheckTaretBlockCanBuildTargetTypeBuilding(int houseType, int x, int y);
        void CreateFarmer();
        bool CheckSea(int x,int y);
        void BuildDock();
        pair<double,double>FindNearstBeach(int x,int y);
        pair<int,int> FindNearstBeachToBuild(int houseType,int x,int y);
        bool CheckCanBuildDock(int x, int y);
        void CreatBoat();
        void CutDownTree();
        void Fish();
        void savemassage();
        void Guohe();
        void boat(tagFarmer&farmer);
        tagFarmer boat();
        void Kaoan(tagFarmer wb);
        void move_to_another_side();
        void Human_move_to_boat(tagFarmer farmer,tagFarmer wb);
        pair<int,int> find_nearest_gold(int x,int y);
        void KillEnemy();
        void KillFish();
        void GoToBoat();
        pair<double,double>FindNearstGold(double x,double y);
        pair<int, int> findOppositeShore(int startX, int startY);
};


#endif // USRAI_H
