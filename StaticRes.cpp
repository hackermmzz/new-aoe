#include "StaticRes.h"

std::list<ImageResource>* StaticRes::staticResource[10];
std::string StaticRes::StaticResname[10]={"Bush","Stone","GoldOre","Fish"};
std::string StaticRes::StaticResDisplayName[10] = {"浆果丛","石头","金矿","渔场"};


StaticRes::StaticRes(int Num, double DR, double UR)
{
    this->Num=Num;

    setDRUR(DR, UR);
    updateBlockByDetail();
    setNowRes();
    setAttribute();

    this->globalNum=g_globalNum;
    g_Object.insert({this->globalNum,this});
    g_globalNum++;
}

StaticRes::StaticRes(int Num, int BlockDR, int BlockUR)
{
    this->Num=Num;
    setBlockDRUR(BlockDR, BlockUR);

    setNowRes();
    setAttribute();
    setDetailPointAttrb_FormBlock();

    this->globalNum=10000*getSort()+g_globalNum;
    g_Object.insert({this->globalNum,this});
    g_globalNum++;
}

void StaticRes::nextframe(){
    //只有渔场才能动态更新每一帧
    if(Num!=NUM_STATICRES_Fish)return;
    nowres=next(nowres);
    if(nowres==nowlist->end())nowres=nowlist->begin();
}

void StaticRes::setAttribute()
{
    switch(Num){
    case NUM_STATICRES_Bush:
        MaxCnt = CNT_BUSH;
        resourceSort = HUMAN_GRANARYFOOD;
        BlockSizeLen = SIZELEN_SINGEL;
        crashLength = CRASHBOX_SINGLEOB;
        break;
    case NUM_STATICRES_Stone:
        MaxCnt = CNT_STONE;
        resourceSort = HUMAN_STONE;
        BlockSizeLen = SIZELEN_SMALL;
        crashLength = CRASHBOX_SMALLBLOCK;
        break;
    case NUM_STATICRES_GoldOre:
        MaxCnt = CNT_GOLDORE;
        resourceSort = HUMAN_GOLD;
        BlockSizeLen = SIZELEN_SMALL;
        crashLength = CRASHBOX_SMALLBLOCK;
        break;
    case NUM_STATICRES_Fish:
        MaxCnt = CNT_FISH;
        resourceSort = HUMAN_DOCKFOOD;
        BlockSizeLen = SIZELEN_SMALL;
        crashLength = CRASHBOX_SMALLBLOCK;
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

    updateImageXYByNowRes();
    this->imageH=DR-UR;
}
