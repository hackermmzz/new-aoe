#include "UsrAI.h"
#include<set>
#include <iostream>
#include<unordered_map>
#include<list>
using namespace std;
tagGame tagUsrGame;
ins UsrIns;
//
//
/*##########DO NOT MODIFY THE CODE ABOVE##########*/
void UsrAI::processData()
{
    //    return;
    cheatAction();
    static bool res = 0;
    if (!res)
        res = 1, cheatRes();
    return;
    auto info = getInfo();
    static bool xx = 0;
    static tagArmy f;
    for (auto x : info.armies) {
        if (x.Sort == AT_STONE_THROWER) {
            if (xx && f.SN == x.SN) {
                f = x;
            }
            else if (!xx) {
                f = x;
                xx = 1;
                PinPointStrike(f.SN, f.DR, f.UR + 32.0 * 5);
            }
        }
    }
}
