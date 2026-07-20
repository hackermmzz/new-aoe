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
int lunx=0;

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
}
