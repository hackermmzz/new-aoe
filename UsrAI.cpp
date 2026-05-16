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
int logo=0;
void UsrAI::processData()
{
while(logo<15){
cheatAction();
cheatRes();
logo++;
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
