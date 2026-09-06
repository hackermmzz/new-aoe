#ifndef FARCHIVEIN_H
#define FARCHIVEIN_H

#include"farchive.h"
class FArchiveIn:public FArchive
{
protected:
    int64_t pos;
public:
    using FArchive::Serialize;
    FArchiveIn();
    FArchiveIn(const void*data,uint32_t bytes);
    virtual void Serialize(void *dst, uint32_t bytes);
    virtual void Clear();
    virtual uint32_t Bytes()const;
};

#endif // FARCHIVEIN_H
