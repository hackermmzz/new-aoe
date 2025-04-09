#include "UsrAI.h"
tagGame tagUsrGame;
ins UsrIns;
/*##########请勿修改以上部分##########*/
#include "set"
#include "QDebug"
#include<iostream>
using namespace std;
using Pos=pair<int,int>;
double mid=64*BLOCKSIDELENGTH;
int house;
int id_Wrong_lastFrame = -1;
int Map[128][128];
int CenterX,CenterY;
tagInfo info;
set<int>FarmerbeUsed;
bool DockIsBuild;
bool IsCangkuBuild=false;
bool UpdateToFTB;
bool IsGuoHe;
bool IsGuCangBuilt;
int ret=999;

void UsrAI::processData()
{

}


