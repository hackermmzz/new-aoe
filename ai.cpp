#include "AI.h"

int AI::HumanMove(int SN, double L0, double U0){
    if(!isHuman(SN))
        return ACTION_INVALID_SN;
    if(L0 > 2575 || L0 < 0||U0 > 2575 || U0 < 0)
        return ACTION_INVALID_LOCATION;    
    return AddToIns(instruction(1,g_Object[SN],L0,U0));
}

int AI::HumanAction(int SN,int obSN){
    if(!isHuman(SN))
        return ACTION_INVALID_SN;
    return AddToIns(instruction(2,g_Object[SN],g_Object[obSN]));
}

int AI::HumanBuild(int SN,int BuildingNum,int BlockL,int BlockU){
    if(!isHuman(SN)){
        return ACTION_INVALID_SN;
    }
    return AddToIns(instruction(3,g_Object[SN],BlockL,BlockU,BuildingNum));
}

int AI::BuildingAction(int SN,int Action){
    if(!isBuilding(SN)){
        return ACTION_INVALID_SN;
    }
    return AddToIns(instruction(4,g_Object[SN],Action));
}

instruction AI::getInsRet(int ins_id){
    if(getGameInfo().ins_ret.find(ins_id)==getGameInfo().ins_ret.end()){
        return instruction();
    }else{
        return getGameInfo().ins_ret[ins_id];
    }
}

void AI::printInsRet(int id){
    instruction tmp=getInsRet(id);
    if(!tmp.isExist()){
        qDebug()<<"ins:"<<id<<" Not Found!";
        return;
    }
    if(tmp.type==1){
        qDebug()<<"ins:"<<id
               <<"\n   HumanMove("<<tmp.self->getglobalNum()<<","<<tmp.L<<","<<tmp.U
              <<")\n   return "<<tmp.ret;
    }else if(tmp.type==2){
        qDebug()<<"ins:"<<id
               <<"\n   HumanAct("<<tmp.self->getglobalNum()<<","<<tmp.obj->getglobalNum()
              <<")\n   return "<<tmp.ret;
    }else if(tmp.type==3){
        qDebug()<<"ins:"<<id
               <<"\n   HumanBuild("<<tmp.self->getglobalNum()<<","<<tmp.BL<<","<<tmp.BU<<","<<tmp.option
              <<")\n   return "<<tmp.ret;
    }else if(tmp.type==4){
        qDebug()<<"ins:"<<id
               <<"\n   BuildingAct("<<tmp.self->getglobalNum()<<","<<tmp.option
              <<")\n   return "<<tmp.ret;
    }
}


