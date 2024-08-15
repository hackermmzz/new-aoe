#include "Coordinate.h"

vector<Point> Coordinate::viewLab[5][12];


/*******行动相关*******/
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

//设置当前交互对象
void Coordinate::set_interAct(int interSort, int interNum, bool interRepresent, bool interBui_builtUp)
{
    interactSort = interSort;
    interactNum = interNum;
    interact_sameRepresent = interRepresent;
    interactBui_builtUp = interBui_builtUp;
}

void Coordinate::resetINterAct()
{
    interactSort = -1;
    interactNum = -1;
    interact_sameRepresent = false;
    interactBui_builtUp = false;
}


/*******可见性相关*******/
void Coordinate::setViewLab( int blockSize , int visionLen )
{
    Point viewBlock ;
    vector<Point>& pointLab = viewLab[blockSize][visionLen];
    int lx,mx,my;
    int x0 = (blockSize - 1)/2;  //计算的原点
    int y0 = x0 , yr , R , vL;

    lx = - visionLen + 1;
    mx =2*x0 + visionLen;
    my = mx;

    if(blockSize == 1 && visionLen == 2)
    {
        viewBlock.y = y0;
        mx++; my++; lx--;
        for(int x = lx ; x < mx ; x++)
        {
            viewBlock.x = x;
            pointLab.push_back(viewBlock);
        }
        for(int y = y0+1 ; y<my; y++)
        {
            mx--; lx++;
            Coordinate::addViewLab(viewLab[blockSize][visionLen] , lx , mx , y , 2*y0 - y);
        }
    }
    else if(visionLen <= 4)
    {
        viewBlock.y = y0;
        for(int x = lx ; x < mx; x++)
        {
            viewBlock.x = x;
            pointLab.push_back(viewBlock);
        }
        for(int y = y0+1 ; y<my; y++)
        {
            if(y == my -1){mx-=1;lx+=1;}
            Coordinate::addViewLab(viewLab[blockSize][visionLen] , lx , mx , y , 2*y0 - y);
        }
    }
    else
    {
        R = visionLen;
        viewBlock.y = y0;
        for(int x = lx ; x < mx; x++)
        {
            viewBlock.x = x;
            pointLab.push_back(viewBlock);
        }

        for(int y = y0+1; y<my; y++)
        {
            yr = y - y0;
            vL = (int)(sqrt((double)( R*R - yr*yr ))+0.5);

            lx = -vL +1;
            mx = 2*x0 + vL;
            Coordinate::addViewLab(viewLab[blockSize][visionLen] , lx , mx , y , 2*y0 - y);
        }
    }
}

void Coordinate::addViewLab( vector<Point>& blockLab , int lx , int mx , int y , int y_mirr )
{
    Point blockPoint(0,y),blockPoint_mirr(0,y_mirr);

    for(int x = lx; x<mx; x++)
    {
        blockPoint.x = blockPoint_mirr.x = x;
        blockLab.push_back(blockPoint);
        blockLab.push_back(blockPoint_mirr);
    }
}

vector<Point> Coordinate::getViewLab()
{
    if(viewLab[(int)BlockSizeLen][getVision()].empty() && BlockSizeLen>0 && getVision() > 1)
        setViewLab((int)BlockSizeLen , getVision());

    return viewLab[(int)BlockSizeLen][getVision()];
}


/*******image资源相关信息*******/
//用于限制nowres切换，以降低图像资源循环速度
bool Coordinate::isNowresShift()
{
    if(nowres_step == nowres_changeRecord)
    {
        nowres_changeRecord = 0;
        return true;
    }
    else
    {
        nowres_changeRecord++;
        return false;
    }
}

void Coordinate::updateImageXYByNowRes()
{
    this->imageX=this->nowres->pix.width()/2.0;
    this->imageY=this->nowres->pix.width()/4.0;
}

/*******坐标相关*******/
void Coordinate::setDetailPointAttrb_FormBlock()
{
    setDRUR( (BlockDR + BlockSizeLen/2.0)*BLOCKSIDELENGTH, (BlockUR + BlockSizeLen/2.0)*BLOCKSIDELENGTH );
    setSideLenth();
}



