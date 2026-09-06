#ifndef FARCHIVEOUT_H
#define FARCHIVEOUT_H

#include"farchive.h"
class FArchiveOut:public FArchive
{
public:
    using FArchive::Serialize;
    FArchiveOut();
    virtual void Serialize(void*src,uint32_t bytes);
    virtual void Clear();
    virtual uint32_t Bytes()const;
};

#endif // FARCHIVEOUT_H
