#include "StaticRes.h"

std::list<ImageResource>* StaticRes::staticResource[3];
std::string StaticRes::StaticResname[3]={"Bush","Stone","GoldOre"};
std::string StaticRes::StaticResDisplayName[3] = {"浆果丛","石头","金矿"};

StaticRes::StaticRes()
{

}

StaticRes::StaticRes(int Num, double DR, double UR)
{
    this->Num=Num;
    this->DR=DR;
    this->UR=UR;
    BlockDR = DR/BLOCKSIDELENGTH;
    BlockUR = UR/BLOCKSIDELENGTH;
    setNowRes();
    setAttribute();

    this->globalNum=g_globalNum;
    g_Object.insert({this->globalNum,this});
    g_globalNum++;
}

StaticRes::StaticRes(int Num, int BlockDR, int BlockUR)
{
    this->Num=Num;
    this->BlockDR=BlockDR;
    this->BlockUR=BlockUR;

    setNowRes();
    setAttribute();
    setDetailPointAttrb_FormBlock();

    this->globalNum=g_globalNum;
    g_Object.insert({this->globalNum,this});
    g_globalNum++;
}

void StaticRes::nextframe()
{


}

void StaticRes::setAttribute()
{
    switch(Num){
    case NUM_STATICRES_Bush:
        MaxCnt = CNT_BUSH;
        resourceSort = HUMAN_GRANARYFOOD;
        BlockSizeLen = SIZELEN_SINGEL;
        break;
    case NUM_STATICRES_Stone:
        MaxCnt = CNT_STONE;
        resourceSort = HUMAN_STONE;
        BlockSizeLen = SIZELEN_SMALL;
        break;
    case NUM_STATICRES_GoldOre:
        MaxCnt = CNT_GOLDORE;
        resourceSort = HUMAN_GOLD;
        BlockSizeLen = SIZELEN_SMALL;
        break;
    default:
        incorrectNum = true;
        break;
    }
    setSideLenth();
    this->Cnt = MaxCnt;
}

void StaticRes::setNowRes()
{
    int listLen;
    nowlist = staticResource[Num];
    listLen = nowlist->size();

    nowres = next(nowlist->begin(), rand()%listLen);
    this->imageX=this->nowres->pix.width()/2.0;
    this->imageY=this->nowres->pix.width()/4.0;
    this->imageH=DR-UR;
}
