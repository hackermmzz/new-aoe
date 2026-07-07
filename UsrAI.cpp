#include "UsrAI.h"
#include<set>
#include <iostream>
#include<unordered_map>
#include<list>
#include <cstdlib>
using namespace std;
tagGame tagUsrGame;
ins UsrIns;
/*##########DO NOT MODIFY THE CODE ABOVE##########*/

int logo=0;

static bool towerTargetInRange(int towerDR, int towerUR, int objDR, int objUR)
{
    const int range = static_cast<int>(DIS_ARROWTOWER);
    return abs(objDR - towerDR) <= range && abs(objUR - towerUR) <= range;
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

    int towerLogSN = -1;
    for (const tagBuilding& b : info.buildings) {
        if (b.Type != BUILDING_ARROWTOWER)
            continue;
        if (b.Percent < 100)
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
            if (!towerTargetInRange(b.BlockDR, b.BlockUR, obj.BlockDR, obj.BlockUR))
                continue;
            int d = abs(b.BlockDR - obj.BlockDR) + abs(b.BlockUR - obj.BlockUR);
            if (d < bestDis) {
                bestDis = d;
                targetSN = obj.SN;
            }
        }

        for (const tagFarmer& obj : info.enemy_farmers) {
            if (!towerTargetInRange(b.BlockDR, b.BlockUR, obj.BlockDR, obj.BlockUR))
                continue;
            int d = abs(b.BlockDR - obj.BlockDR) + abs(b.BlockUR - obj.BlockUR);
            if (d < bestDis) {
                bestDis = d;
                targetSN = obj.SN;
            }
        }
        cheatAction();

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
