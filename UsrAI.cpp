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

void UsrAI::processData()
{
    cheatAction();
    static int x=0;
    if(x<10)
    cheatRes(),++x;


}
