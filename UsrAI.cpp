#include "UsrAI.h"
tagGame tagUsrGame;
ins UsrIns;
/*##########请勿修改以上部分##########*/
tagInfo info;
double mid=36*BLOCKSIDELENGTH;
int tmpFrame=(int)1e6;

void UsrAI::processData()
{
    info=getInfo();
    qDebug()<<info.farmers.at(0).NowState;
}
