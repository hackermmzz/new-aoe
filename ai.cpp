#include "AI.h"

QString AI::AIName[NOWPLAYER] = { "UsrAI", "EnemyAI" };

int AI::HumanMove(int SN, double DR0, double UR0){ 
    return AddToIns(instruction(1,SN,DR0,UR0));
}

int AI::HumanAction(int SN,int obSN){
    return AddToIns(instruction(2,SN,obSN , true));
}

int AI::HumanBuild(int SN, int BuildingNum, int BlockDR, int BlockUR){
    return AddToIns(instruction(3,SN,BlockDR,BlockUR,BuildingNum));
}

int AI::BuildingAction(int SN,int Action){
    return AddToIns(instruction(4,SN,Action));
}


