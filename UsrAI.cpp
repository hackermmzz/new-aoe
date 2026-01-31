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
    cheatAction();
    static bool res=0;
    if(!res)
    res=1,cheatRes();
    //return;
    auto info=getInfo();
    static bool xx=0;
    static tagFarmer f;
    for(auto x :info.farmers){
        if(x.FarmerSort==FARMERTYPE_FARMER){
            if(xx&&f.SN==x.SN){
                f=x;
            }else if(!xx){
                f=x;
                xx=1;
            }
        }
    }
    if(f.WorkObjectSN==-1)
    HumanBuild(f.SN,BUILDING_COLLAGE,112,106);
}
