#include "StaticRes.h"
std::string StaticRes::StaticResname[3]={"","",""};
std::string StaticRes::StaticResDisplayName[3] = {"","",""};

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
