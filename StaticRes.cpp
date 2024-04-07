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
}

StaticRes::StaticRes(int Num, int BlockDR, int BlockUR)
{
    this->Num=Num;
    this->BlockDR=BlockDR;
    this->BlockUR=BlockUR;

    setDetailPointAttrb_FormBlock();
}

void StaticRes::nextframe()
{

}

int StaticRes::getSort()
{
    return SORT_STATICRES;
}

void StaticRes::setAttribute()
{

}
