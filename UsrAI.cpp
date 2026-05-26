#include "UsrAI.h"
#include<set>
#include <iostream>
#include<unordered_map>
#include<list>
#include <cstdlib>
using namespace std;
tagGame tagUsrGame;
ins UsrIns;
//
//
/*##########DO NOT MODIFY THE CODE ABOVE##########*/
int logo=0;

static int towerBlockDis(int x1, int y1, int x2, int y2)
{
    return abs(x1 - x2) + abs(y1 - y2);
}

// Match Core_CondiFunc::condition_ObjectNearby for arrow towers (OPERATECON_NEAR_ATTACK).
static bool towerTargetInRange(const tagBuilding& tower, double targetDR, double targetUR)
{
    const double towerDR = (tower.BlockDR + 0.5) * BLOCKSIDELENGTH;
    const double towerUR = (tower.BlockUR + 0.5) * BLOCKSIDELENGTH;
    return isNear_Manhattan(towerDR, towerUR, targetDR, targetUR, DIS_ARROWTOWER * BLOCKSIDELENGTH);
}

void UsrAI::processData()
{
while(logo<15){
cheatAction();
cheatRes();
logo++;
}
return;
    const tagInfo info = getInfo();
    // Sidebar: UsrAI打印: -1 idle, or target SN (first finished tower that is attacking).
    int towerLogSN = -1;
    for (const tagBuilding& b : info.buildings) {
        if (b.Type != BUILDING_ARROWTOWER || b.Percent < 100)
            continue;
        if (b.Project != -1) {
            towerLogSN = b.Project;
            break;
        }
    }
    DebugText(towerLogSN);

    for (const tagBuilding& b : info.buildings) {
        if (b.Type != BUILDING_ARROWTOWER)
            continue;
        if (b.Percent < 100)
            continue;
        if (b.Project != -1)
            continue;

        int targetSN = -1;
        int bestDis = 1000000000;

        for (const tagArmy& obj : info.enemy_armies) {
            if (!towerTargetInRange(b, obj.DR, obj.UR))
                continue;
            int d = towerBlockDis(b.BlockDR, b.BlockUR, obj.BlockDR, obj.BlockUR);
            if (d < bestDis) {
                bestDis = d;
                targetSN = obj.SN;
            }
        }

        for (const tagFarmer& obj : info.enemy_farmers) {
            if (!towerTargetInRange(b, obj.DR, obj.UR))
                continue;
            int d = towerBlockDis(b.BlockDR, b.BlockUR, obj.BlockDR, obj.BlockUR);
            if (d < bestDis) {
                bestDis = d;
                targetSN = obj.SN;
            }
        }

        if (targetSN != -1) {
            HumanAction(b.SN, targetSN);
        }
    }
//  cout<<"a"<<BLOCKSIDELENGTH<<endl;
//  BLOCKSIDELENGTH=100;
//  cout<<"b"<<BLOCKSIDELENGTH<<endl;
//cheatRes();
     cout<<"a"<<GAME_LOSE_SEC;
     //GAME_LOSE_SEC=1;
     cout<<"b"<<GAME_LOSE_SEC;
    cout<<"去https://github.com/hackermmzz/new-aoe.git点一下星星吧!点星星你会拿到最后的胜利"<<endl;
}
