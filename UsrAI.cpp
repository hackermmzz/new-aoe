#include "UsrAI.h"
#include<set>
#include <iostream>
#include<unordered_map>
using namespace std;
tagGame tagUsrGame;
ins UsrIns;

/*##########DO NOT MODIFY THE CODE ABOVE##########*/
tagInfo info;

void UsrAI::processData()
{
    info = getInfo();
    int treeSN = -1;
    int stoneSN = -1;
    int animalSN = -1;
    int fishSN = -1;
    for (tagResource& resource : info.resources) {
        if (resource.Type == RESOURCE_TREE) {
            treeSN = resource.SN;
        }
        else if (resource.Type == RESOURCE_STONE) {
            stoneSN = resource.SN;
        }
        else if (resource.Type == RESOURCE_GAZELLE) {
            animalSN = resource.SN;
        }
        else if (resource.Type == RESOURCE_FISH) {
            fishSN = resource.SN;
        }
    }
    int SNs[4] = { treeSN, stoneSN, animalSN, fishSN };
    int i = 0;
    // 先让农民去收集各类资源
    for (tagFarmer& farmer : info.farmers) {
        if (farmer.NowState == HUMAN_STATE_IDLE && farmer.FarmerSort == 0) {
            HumanAction(farmer.SN, SNs[i]);
            i++;
        }
    }
    // 1000帧后，让砍树和挖石头的人去打猎
    if (info.GameFrame > 1000) {
        for (tagFarmer& farmer : info.farmers) {
            if (farmer.NowState == HUMAN_STATE_WORKING && farmer.FarmerSort == 0) {
                if (farmer.WorkObjectSN == treeSN || farmer.WorkObjectSN == stoneSN) {
                    HumanAction(farmer.SN, animalSN);
                }
            }
        }
    }
}
