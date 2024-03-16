#include "Resource.h"

Resource::Resource()
{
}


int Resource::get_ReturnBuildingType()
{
    int buildingType;
    switch (resourceSort)
    {
    case HUMAN_WOOD:
    case HUMAN_GOLD:
    case HUMAN_STONE:
    case HUMAN_STOCKFOOD:
        buildingType = BUILDING_STOCK;
        break;
    case HUMAN_GRANARYFOOD:
        buildingType = BUILDING_GRANARY;
        break;
    default:
        buildingType = BUILDING_CENTER;
        break;
    }

    return buildingType;
}
