#include "UsrAI.h"
tagGame tagUsrGame;
ins UsrIns;
/*##########请勿修改以上部分##########*/
void UsrAI:: processData(){
    tagInfo info =getInfo();
    if(info.GameFrame%100==0){
        cheatRes();
    }
    if(info.farmers.size()>0){
        qDebug()<<info.farmers.at(0).NowState;
    }
}
