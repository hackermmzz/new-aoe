#include "FarchiveIn.h"

FArchiveIn::FArchiveIn()
{
    write=0;
    pos=0;
}

FArchiveIn::FArchiveIn(const void *data0, uint32_t bytes):FArchiveIn()
{
    if(bytes==0)return;
    data.resize(bytes);
    memcpy(static_cast<void*>(&(data[0])),data0,bytes);
}

void FArchiveIn::Serialize(void* dst, uint32_t bytes)
{
    if(bytes == 0) return;
    // 边界检查：防止越界读(加快速度，不检查）
    // 从data[Pos]拷贝到外部dst
    memcpy(dst, &data[pos], bytes);
    pos += bytes;
}

void FArchiveIn::Clear()
{
    data.clear();
    pos=0;
}

uint32_t FArchiveIn::Bytes() const
{
    return data.size()-pos;
}
