#ifndef AI_H
#define AI_H
#include "GlobalVariate.h"
#include "Coordinate.h"

class AI:public QThread
{
public:
/*##########INTERFACE BEGINS HERE##########*/
    int HumanMove(int SN, double L0, double U0){
        Coordinate* self=g_Object[SN];
        if(!isHuman(self))
            return ACTION_INVALID_SN;
        if(L0 > 2575 || L0 < 0||U0 > 2575 || U0 < 0)
            return ACTION_INVALID_LOCATION;
        instructions.push(instruction(1,self,L0,U0));
        return ACTION_SUCCESS;
    }

    int HumanAction(int SN,int obSN){
        Coordinate* self=g_Object[SN];
        Coordinate* obj=g_Object[obSN];
        if(!isHuman(self))
            return ACTION_INVALID_SN;
        if(obj->getSort()!=SORT_ANIMAL&&obj->getSort()!=SORT_STATICRES){
            return ACTION_INVALID_OBSN;
        }
        instructions.push(instruction(2,self,obj));
        return ACTION_SUCCESS;
    }

    int HumanBuild(int SN,int BuildingNum,int BlockL,int BlockU){
        Coordinate* self=g_Object[SN];
        if(!isHuman(self)){
            return ACTION_INVALID_SN;
        }
        int woodcost=-1;
        int stonecost=-1;
        int size=0;
        bool granny=false;
        bool market=false;
        for(tagBuilding building:AIGame.buildings){
            if(building.Type==BUILDING_GRANARY){
                granny=true;
            }
            if(building.Type==BUILDING_MARKET){
                market=true;
            }
            if(market&&granny){break;}
        }
        switch (BuildingNum) {
        case BUILDING_HOME:
            woodcost=BUILD_HOUSE_WOOD;
            size=2;
            break;
        case BUILDING_STOCK:
            woodcost=BUILD_STOCK_WOOD;
            size=3;
            break;
        case BUILDING_GRANARY:
            woodcost=BUILD_GRANARY_WOOD;
            size=3;
            break;
        case BUILDING_MARKET:
            if(AIGame.civilizationStage<CIVILIZATION_TOOLAGE||!granny){
                return ACTION_INVALID_BUILDINGNUM;
            }
            woodcost=BUILD_MARKET_WOOD;
            size=4;
            break;
        case BUILDING_ARROWTOWER:
            stonecost=BUILD_ARROWTOWER_STONE;
            size=2;
            break;
        case BUILDING_FARM:
            if(AIGame.civilizationStage<CIVILIZATION_TOOLAGE||!market){
                return ACTION_INVALID_BUILDINGNUM;
            }
            woodcost=BUILD_FARM_WOOD;
            size=3;
            break;
        default:
            return ACTION_INVALID_BUILDINGNUM;
            break;
        }
        if(AIGame.Wood<woodcost||AIGame.Stone<stonecost){
            return ACTION_INVALID_RESOURCE;
        }
        if(BlockL<0||BlockU<0||BlockL+size>=72||BlockU+size>=72){
            return ACTION_INVALID_LOCATION;
        }
        instructions.push(instruction(3,self,BlockL,BlockU,BuildingNum));
    }

    int BuildingAction(int SN,int Action){
        Coordinate* self=g_Object[SN];
        if(!isBuilding(self)){
            return ACTION_INVALID_SN;
        }
        int foodcost=-1;
        int woodcost=-1;
        int stonecost=-1;
        switch (Action) {
        case BUILDING_CENTER_CREATEFARMER:
            if(self->getNum()!=BUILDING_GRANARY){
                return ACTION_INVALID_ACTION;
            }
            foodcost=BUILDING_CENTER_CREATEFARMER_FOOD;
            break;
        case BUILDING_CENTER_UPGRADE:
            if(self->getNum()!=BUILDING_CENTER){
                return ACTION_INVALID_ACTION;
            }
            foodcost=BUILDING_CENTER_UPGRADE_FOOD;
            break;
        case BUILDING_GRANARY_ARROWTOWER:
            if(self->getNum()!=BUILDING_GRANARY||AIGame.civilizationStage<CIVILIZATION_TOOLAGE){
                return ACTION_INVALID_ACTION;
            }
            foodcost=BUILDING_GRANARY_ARROWTOWER_FOOD;
            break;
        case BUILDING_MARKET_WOOD_UPGRADE:
            if(self->getNum()!=BUILDING_MARKET){
                return ACTION_INVALID_ACTION;
            }
            foodcost=BUILDING_MARKET_WOOD_UPGRADE_FOOD;
            woodcost=BUILDING_MARKET_WOOD_UPGRADE_WOOD;
            break;
        case BUILDING_MARKET_STONE_UPGRADE:
            if(self->getNum()!=BUILDING_MARKET){
                return ACTION_INVALID_ACTION;
            }
            foodcost=BUILDING_MARKET_STONE_UPGRADE_FOOD;
            stonecost=BUILDING_MARKET_STONE_UPGRADE_STONE;
            break;
        case BUILDING_MARKET_FARM_UPGRADE:
            if(self->getNum()!=BUILDING_MARKET){
                return ACTION_INVALID_ACTION;
            }
            foodcost=BUILDING_MARKET_FARM_UPGRADE_FOOD;
            woodcost=BUILDING_MARKET_FARM_UPGRADE_WOOD;
            break;
        case BUILDING_CANCEL:
            if(self->getNum()!=BUILDING_MARKET||self->getNum()!=BUILDING_GRANARY
                    ||self->getNum()!=BUILDING_CENTER||self->getNum()!=BUILDING_GRANARY)
                return ACTION_INVALID_ACTION;
            break;
        default:
            return ACTION_INVALID_ACTION;
            break;
        }
        if(AIGame.Wood<=woodcost||AIGame.Meat<=foodcost||AIGame.Stone<=stonecost){
            return ACTION_INVALID_RESOURCE;
        }
        instructions.push(instruction(4,self,Action));
    }
/*###########INTERFACE ENDS HERE###########*/
    AI();
    void processData();
    void run() override{
        if(AIGame.GameFrame>10&&AIfinished){
            qDebug()<<g_frame<<":"<<"调用AI";
            processData();
        }
        }
private:
    bool isHuman(Coordinate* self){
        return (self!=nullptr&&(self->getSort()==SORT_FARMER||self->getSort()==SORT_ARMY));
    }
    bool isBuilding(Coordinate* self){
        return (self!=nullptr&&(self->getSort()==SORT_FARM||self->getSort()==SORT_BUILDING));
    }
};

#endif // AI_H
