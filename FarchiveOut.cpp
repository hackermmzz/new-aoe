#include "FarchiveOut.h"

FArchiveOut::FArchiveOut()
{
    write=1;
}

void FArchiveOut::Serialize(void *src, uint32_t bytes)
{
    if (bytes == 0)
            return;

    const uint8_t* pSrc = reinterpret_cast<const uint8_t*>(src);
    size_t oldSize = data.size();

    // vector尾部扩容，保留原有全部旧数据，新增bytes字节空间
    data.resize(oldSize + bytes);

    // 将外部src内存拷贝到vector新开辟的尾部空间
    memcpy(&data[oldSize], pSrc, bytes);
}

void FArchiveOut::Clear()
{
    data.clear();
}

uint32_t FArchiveOut::Bytes() const
{
    return data.size();
}
