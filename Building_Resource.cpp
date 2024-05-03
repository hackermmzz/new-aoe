#include "Building_Resource.h"

/**********************虚函数**************************/
/**********更新*************/
void Building_Resource::nextframe()
{
    setNowRes();
    if(Percent<100)
    {
        nowres = nowlist->begin();
        advance(nowres , (int)(Percent/25));
    }
    else
    {
        if(!gatherable) changeToGatherAble();
        nowres++;
        if(nowres==nowlist->end())  nowres=nowlist->begin(); //读到最后回到最初
    }

    initGatherer();

    this->imageX=this->nowres->pix.width()/2.0;
    this->imageY=this->nowres->pix.width()/4.0;
}

void Building_Resource::setAttribute()
{
    if(Num == BUILDING_FARM)
    {
        MaxBlood = BLOOD_BUILD_FARM;
        Foundation=FOUNDATION_MIDDLE;
        resourceSort = HUMAN_GRANARYFOOD;
        gatherable = false;
        setMaxCnt();
    }
//    qDebug()<<MaxCnt;
    Cnt = MaxCnt;
}

/**********************虚函数**************************/
bool Building_Resource::isGathererAsLandlord(Coordinate* gatherer)
{
    //无主农田，设置地主
    if(this->gatherer == NULL)
    {
        setGatherer(gatherer);
        return true;
    }

    //返回传入采集者是否是地主
    return gatherer == this->gatherer;
}
