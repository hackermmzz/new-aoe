#ifndef FARCHIVE_H
#define FARCHIVE_H

#include<vector>
#include"string"
using namespace std;
using Byte=unsigned char;
class FArchive
{
protected:
    vector<Byte>data;
    bool write;
public:
    FArchive();
    template<class T>
    void Serialize(T&val);
    template<class T>
    void Serialize(T&val,uint32_t bytes);
    void Serialize(int32_t&val);
    void Serialize(int64_t&val);
    void Serialize(uint32_t&val);
    void Serialize(uint64_t&val);
    void Serialize(double&val);
    void Serialize(float&val);
    void Serialize(bool&val);
    void Serialize(string&val);
    virtual void Serialize(void*dst,uint32_t bytes)=0;
    bool IsRead()const;
    bool IsWrite()const;
    void* GetData();
    virtual void Clear()=0;
    virtual uint32_t Bytes()const =0;
};

template<class T>
void FArchive::Serialize(T &val)
{
    val.Serialize(this);
}

template<class T>
void FArchive::Serialize(T &val, uint32_t bytes)
{
    Serialize(&val,bytes);
}

#endif // FARCHIVE_H
