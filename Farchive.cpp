#include "Farchive.h"

FArchive::FArchive()
{
    write=0;
}

void FArchive::Serialize(int64_t&val)
{
    Serialize(static_cast<void*>(&val),sizeof(val));
}

void FArchive::Serialize(uint32_t&val)
{
    Serialize(static_cast<void*>(&val),sizeof(val));
}

void FArchive::Serialize(uint64_t&val)
{
    Serialize(static_cast<void*>(&val),sizeof(val));
}

void FArchive::Serialize(int32_t&val)
{
    Serialize(static_cast<void*>(&val),sizeof(val));
}

void FArchive::Serialize(double&val)
{
    Serialize(static_cast<void*>(&val),sizeof(val));
}

void FArchive::Serialize(float&val)
{
    Serialize(static_cast<void*>(&val),sizeof(val));
}

void FArchive::Serialize(bool&val)
{
    Serialize(static_cast<void*>(&val),sizeof(val));
}

void FArchive::Serialize(string&val)
{
    uint32_t siz=val.size();
    Serialize(siz);
    val.resize(siz);//写操作在这里不会起作用
    Serialize(static_cast<void*>(&val[0]),siz);
}

bool FArchive::IsRead() const
{
    return !IsWrite();
}

bool FArchive::IsWrite() const
{
    return write;
}

uint32_t FArchive::Bytes() const
{
    return data.size();
}

void* FArchive::GetData()
{
    return data.empty()?nullptr:&data[0];
}


