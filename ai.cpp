#include "AI.h"

int AI::HumanMove(int SN, double L0, double U0){
    Coordinate* self=g_Object[SN];
//        self->setActSpeed();
//        g_Object[SN]->setActSpeed();
    if(!isHuman(self))
        return ACTION_INVALID_SN;
    if(L0 > 2575 || L0 < 0||U0 > 2575 || U0 < 0)
        return ACTION_INVALID_LOCATION;    
    return AddToIns(instruction(1,self,L0,U0));
}

int AI::HumanAction(int SN,int obSN){
    Coordinate* self=g_Object[SN];
    Coordinate* obj=g_Object[obSN];
    if(!isHuman(self))
        return ACTION_INVALID_SN;
    if(obj->getSort()!=SORT_ANIMAL&&obj->getSort()!=SORT_STATICRES){
        return ACTION_INVALID_OBSN;
    }
    if(self->getSort()!=SORT_FARMER){
        if(obj->getSort()==SORT_STATICRES||isAnimalTree(obj)){
            return ACTION_INVALID_OBSN;
        }
    }
    return AddToIns(instruction(2,self,obj));
}

int AI::HumanBuild(int SN,int BuildingNum,int BlockL,int BlockU){
    Coordinate* self=g_Object[SN];
    if(!isHuman(self)||self->getSort()!=SORT_FARMER){
        return ACTION_INVALID_SN;
    }
    int woodcost=-1;
    int stonecost=-1;
    int size=0;
    bool granny=false;
    bool market=false;
    for(tagBuilding building:getGameInfo().buildings){
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
        if(getGameInfo().civilizationStage<CIVILIZATION_TOOLAGE||!granny){
            return ACTION_INVALID_BUILDINGNUM;
        }
        woodcost=BUILD_MARKET_WOOD;
        size=3;
        break;
    case BUILDING_ARROWTOWER:
        stonecost=BUILD_ARROWTOWER_STONE;
        size=2;
        break;
    case BUILDING_FARM:
        if(getGameInfo().civilizationStage<CIVILIZATION_TOOLAGE||!market){
            return ACTION_INVALID_BUILDINGNUM;
        }
        woodcost=BUILD_FARM_WOOD;
        size=3;
        break;
    case BUILDING_ARMYCAMP:
        woodcost=BUILD_ARMYCAMP_WOOD;
        size=3;
        break;
    case BUILDING_STABLE:
        woodcost=BUILD_STABLE_WOOD;
        size=3;
        break;
    case BUILDING_RANGE:
        woodcost=BUILD_RANGE_WOOD;
        size=3;
        break;
    case BUILDING_WALL:
        stonecost=BUILD_WALL_STONE;
        size=2;
        break;
    case BUILDING_CENTER:
        woodcost=BUILD_CENTER_WOOD;
        size=3;
        break;
    default:
        return ACTION_INVALID_BUILDINGNUM;
        break;
    }
    if(getGameInfo().Wood<woodcost||getGameInfo().Stone<stonecost){
        return ACTION_INVALID_RESOURCE;
    }
    if(BlockL<0||BlockU<0||BlockL+size>=72||BlockU+size>=72){
        return ACTION_INVALID_LOCATION;
    }
    return AddToIns(instruction(3,self,BlockL,BlockU,BuildingNum));
}

int AI::BuildingAction(int SN,int Action){
    Coordinate* self=g_Object[SN];
    if(!isBuilding(self)){
        return ACTION_INVALID_SN;
    }
    Building* building=dynamic_cast<Building*>(self);
    if(building->getActSpeed()>0){
        return ACTION_INVALID_ACTION;
    }
    int foodcost=-1;
    int woodcost=-1;
    int stonecost=-1;
    if(Action==BUILDING_CANCEL){
        return AddToIns(instruction(0,self,0));
    }
    if(self->getNum()==BUILDING_CENTER){
        switch (Action) {
        case BUILDING_CENTER_CREATEFARMER:
            foodcost=BUILDING_CENTER_CREATEFARMER_FOOD;
            break;
        case BUILDING_CENTER_UPGRADE:
            foodcost=BUILDING_CENTER_UPGRADE_FOOD;
            break;
        default:
            return ACTION_INVALID_ACTION;
        }

    }else if(self->getNum()==BUILDING_GRANARY){
        switch (Action) {
        case BUILDING_GRANARY_ARROWTOWER:
            if(getGameInfo().civilizationStage<CIVILIZATION_TOOLAGE){
                return ACTION_INVALID_ACTION;
            }
            foodcost=BUILDING_GRANARY_ARROWTOWER_FOOD;
            break;
        default:
            return ACTION_INVALID_ACTION;
        }

    }else if(self->getNum()==BUILDING_MARKET){
        switch(Action){
        case BUILDING_MARKET_WOOD_UPGRADE:
            foodcost=BUILDING_MARKET_WOOD_UPGRADE_FOOD;
            woodcost=BUILDING_MARKET_WOOD_UPGRADE_WOOD;
            break;
        case BUILDING_MARKET_STONE_UPGRADE:
            foodcost=BUILDING_MARKET_STONE_UPGRADE_FOOD;
            stonecost=BUILDING_MARKET_STONE_UPGRADE_STONE;
            break;
        case BUILDING_MARKET_FARM_UPGRADE:
            foodcost=BUILDING_MARKET_FARM_UPGRADE_FOOD;
            woodcost=BUILDING_MARKET_FARM_UPGRADE_WOOD;
            break;
        case BUILDING_MARKET_GOLD_UPGRADE:
            foodcost=BUILDING_MARKET_GOLD_UPGRADE_FOOD;
            woodcost=BUILDING_MARKET_GOLD_UPGRADE_WOOD;
            break;
        default:
            return ACTION_INVALID_ACTION;
        }

    }else if(self->getNum()==BUILDING_STOCK){
        switch (Action) {
        case BUILDING_STOCK_UPGRADE_USETOOL:
            foodcost=BUILDING_STOCK_UPGRADE_USETOOL_FOOD;
            break;
        case BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY:
            foodcost=BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY_FOOD;
            break;
        case BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER:
            foodcost=BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER_FOOD;
            break;
        case BUILDING_STOCK_UPGRADE_DEFENSE_RIDER:
            foodcost=BUILDING_STOCK_UPGRADE_DEFENSE_RIDER_FOOD;
            break;
        default:
            return ACTION_INVALID_ACTION;
        }
    }else if(self->getNum()==BUILDING_ARMYCAMP){
        switch (Action) {
        case BUILDING_ARMYCAMP_CREATE_CLUBMAN:
            foodcost=BUILDING_ARMYCAMP_CREATE_CLUBMAN_FOOD;
            break;
        case BUILDING_ARMYCAMP_CREATE_SLINGER:
            foodcost=BUILDING_ARMYCAMP_CREATE_SLINGER_FOOD;
            stonecost=BUILDING_ARMYCAMP_CREATE_SLINGER_STONE;
            break;
        case BUILDING_ARMYCAMP_UPGRADE_CLUBMAN:
            foodcost=BUILDING_ARMYCAMP_UPGRADE_CLUBMAN_FOOD;
            break;
        default:
            return ACTION_INVALID_ACTION;
        }
    }else if(self->getNum()==BUILDING_RANGE){
        switch (Action) {
        case BUILDING_RANGE_CREATE_BOWMAN:
            foodcost=BUILDING_RANGE_CREATE_BOWMAN_FOOD;
            woodcost=BUILDING_RANGE_CREATE_BOWMAN_WOOD;
            break;
        default:
            return ACTION_INVALID_ACTION;
        }
    }else if(self->getNum()==BUILDING_STABLE){
        switch (Action) {
        case BUILDING_STABLE_CREATE_SCOUT:
            foodcost=BUILDING_STABLE_CREATE_SCOUT_FOOD;
            break;
        default:
            return ACTION_INVALID_ACTION;
        }
    }

    if(getGameInfo().Wood<=woodcost||getGameInfo().Meat<=foodcost||getGameInfo().Stone<=stonecost){
        return ACTION_INVALID_RESOURCE;
    }
    return AddToIns(instruction(4,self,Action));
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

void AI::clearInsRet(){

}

