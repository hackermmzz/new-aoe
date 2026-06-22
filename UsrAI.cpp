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

void UsrAI::processData(){

    tagInfo x=getInfo();
    while(lunx<15&&x.Gold<150000)
    {
        cheatRes();
        cheatAction();
        ++lunx;
    }
    return;

}
