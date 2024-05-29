#include "Coordinate.h"

Coordinate::Coordinate()
{

}

void Coordinate::nextframe()
{

}

int Coordinate::getSort()
{
    return SORT_COORDINATE;
}

int Coordinate::getMapHeightOffsetY()
{
    return MapHeightOffsetY;
}

void Coordinate::setMapHeightOffsetY(int m_MapHeightOffsetY)
{
    MapHeightOffsetY = m_MapHeightOffsetY;
}

int Coordinate::ActNameToActNum(int actName)
{
    /**
      将actName转换为actNum
    */
    switch (actName) {
    case ACT_CREATEFARMER:  return BUILDING_CENTER_CREATEFARMER;
    case ACT_UPGRADE_AGE:   return BUILDING_CENTER_UPGRADE;
    case ACT_UPGRADE_WOOD : return BUILDING_MARKET_WOOD_UPGRADE;
    case ACT_UPGRADE_STONE :    return BUILDING_MARKET_STONE_UPGRADE;
    case ACT_UPGRADE_FARM : return BUILDING_MARKET_FARM_UPGRADE;
    case ACT_UPGRADE_GOLD : return BUILDING_MARKET_GOLD_UPGRADE;
    case ACT_STOCK_UPGRADE_USETOOL :    return BUILDING_STOCK_UPGRADE_USETOOL;
    case ACT_STOCK_UPGRADE_DEFENSE_INFANTRY:    return BUILDING_STOCK_UPGRADE_DEFENSE_INFANTRY;
    case ACT_STOCK_UPGRADE_DEFENSE_ARCHER : return BUILDING_STOCK_UPGRADE_DEFENSE_ARCHER;
    case ACT_STOCK_UPGRADE_DEFENSE_RIDER :  return BUILDING_STOCK_UPGRADE_DEFENSE_RIDER;
    case ACT_ARMYCAMP_CREATE_CLUBMAN :  return BUILDING_ARMYCAMP_CREATE_CLUBMAN;
    case ACT_ARMYCAMP_CREATE_SLINGER :  return BUILDING_ARMYCAMP_CREATE_SLINGER;
    case ACT_ARMYCAMP_UPGRADE_CLUBMAN : return BUILDING_ARMYCAMP_UPGRADE_CLUBMAN;
    case ACT_RANGE_CREATE_BOWMAN :  return BUILDING_RANGE_CREATE_BOWMAN;
    case ACT_STABLE_CREATE_SCOUT :  return BUILDING_STABLE_CREATE_SCOUT;
    case ACT_RESEARCH_WALL :    return BUILDING_GRANARY_WALL;
    case ACT_UPGRADE_TOWERBUILD:    return BUILDING_GRANARY_ARROWTOWER;

//    case ACT_BUILD_HOUSE :
//    case ACT_BUILD_GRANARY :
//    case ACT_BUILD_STOCK :
//    case ACT_BUILD_CANCEL :
//    case ACT_BUILD_FARM :
//    case ACT_BUILD_MARKET :
//    case ACT_BUILD_ARROWTOWER :
//    case ACT_BUILD_ARMYCAMP :
//    case ACT_BUILD_RANGE :
//    case ACT_BUILD_STABLE :
    default:
        return -1;
        break;
    }



}
