#ifndef RESOURCE_H
#define RESOURCE_H


class Resource
{
public:
    Resource();

    bool get_Gatherable(){return gatherable;}
    void set_Gatherable(bool gatherable){this->gatherable = gatherable;}

protected:
    double Cnt;

    int MaxCnt;

    bool gatherable = true;
    //false为不可采集 true为可以采集且不再具有其他功能

    double DecayRate=0;
    //腐烂速度

};

#endif // RESOURCE_H
