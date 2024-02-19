#ifndef RESOURCE_H
#define RESOURCE_H


class Resource
{
public:
    Resource();

    bool get_Gatherable(){return gatherable;}
    int get_ResourceSort(){ return resourceSort; }
    int get_Cnt(){ return (int)(Cnt*MaxCnt); }
    void set_Gatherable(bool gatherable){this->gatherable = gatherable;}
    void updateCnt_byGather( double gather){  Cnt -= gather/MaxCnt ;}
    void updateCnt_byDecay(){ Cnt-=DecayRate; }
    bool isGather_finish(){ return Cnt<=0; }

protected:
    double Cnt = 1;

    int MaxCnt;

    bool gatherable = true;
    //false为不可采集 true为可以采集且不再具有其他功能

    double DecayRate=0;
    //腐烂速度

    int resourceSort;
    //指示资源的类型
    //1指代木头 2指代肉 3指代石头 4指代金子
    //eg:    HUMAN_WOOD

};

#endif // RESOURCE_H
