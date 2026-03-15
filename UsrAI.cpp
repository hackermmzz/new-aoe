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
    cheatRes();
    tagInfo info=getInfo();
    cout<<info.Meat<<" "<<info.Wood<<" "<<info.Gold<<" "<<info.Stone<<endl;

}
