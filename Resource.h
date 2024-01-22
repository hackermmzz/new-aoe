#ifndef RESOURCE_H
#define RESOURCE_H


class Resource
{
public:
    Resource();

protected:
    double Cnt;

    double MaxCnt;

    bool gatherable = true;
    //false为不可采集 true为可以采集且不再具有其他功能

    double DecayRate=0;
    //腐烂速度

};

#endif // RESOURCE_H
