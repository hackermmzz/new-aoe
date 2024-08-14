#include "AI.h"

QString AI::AIName[NOWPLAYER] = { "UsrAI", "EnemyAI" };

int AI::HumanMove(int SN, double DR0, double UR0){
    if(!isHuman(SN))
        return ACTION_INVALID_SN;
    if(DR0 > 2575 || DR0 < 0||UR0 > 2575 || UR0 < 0)
        return ACTION_INVALID_LOCATION;    
    return AddToIns(instruction(1,SN,DR0,UR0));
}

int AI::HumanAction(int SN,int obSN){
    if(!(isHuman(SN)||isBuilding(SN)) )
        return ACTION_INVALID_SN;
    return AddToIns(instruction(2,SN,obSN , true));
}

int AI::HumanBuild(int SN, int BuildingNum, int BlockDR, int BlockUR){
    if(!isHuman(SN)){
        return ACTION_INVALID_SN;
    }
    return AddToIns(instruction(3,SN,BlockDR,BlockUR,BuildingNum));
}

int AI::BuildingAction(int SN,int Action){
    if(!isBuilding(SN)){
        return ACTION_INVALID_SN;
    }
    return AddToIns(instruction(4,SN,Action));
}


//void AI::printInsRet(int id){
//    instruction tmp=getInsRet(id);
//    if(!tmp.isExist()){
//        qDebug()<<"ins:"<<id<<" Not Found!";
//        return;
//    }
//    if(tmp.type==1){
//        qDebug()<<"ins:"<<id
//               <<"\n   HumanMove("<<tmp.SN<<","<<tmp.DR<<","<<tmp.UR
//              <<")\n   return "<<tmp.ret;
//    }else if(tmp.type==2){
//        qDebug()<<"ins:"<<id
//               <<"\n   HumanAct("<<tmp.SN<<","<<tmp.obSN
//              <<")\n   return "<<tmp.ret;
//    }else if(tmp.type==3){
//        qDebug()<<"ins:"<<id
//               <<"\n   HumanBuild("<<tmp.SN<<","<<tmp.BlockDR<<","<<tmp.BlockUR<<","<<tmp.option
//              <<")\n   return "<<tmp.ret;
//    }else if(tmp.type==4){
//        qDebug()<<"ins:"<<id
//               <<"\n   BuildingAct("<<tmp.SN<<","<<tmp.option
//              <<")\n   return "<<tmp.ret;
//    }
//}


