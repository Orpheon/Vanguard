#include "networking/buffer.h"

Buffer::Buffer(void *data_, uint64_t datalen_) : data(data_), datalen(datalen_), pos(0)
{
    //ctor
}
Buffer::~Buffer()
{
    //dtor
}

ReadBuffer::ReadBuffer(void *data_, uint64_t datalen_) : Buffer(data_, datalen_)
{
    //ctor
}
ReadBuffer::~ReadBuffer()
{
    //dtor
}

WriteBuffer::WriteBuffer() : Buffer(std::malloc(32), 32)
{
    //ctor
}
WriteBuffer::~WriteBuffer()
{
    std::free(data);
}
void WriteBuffer::enlarge(uint64_t newsize)
{
    void *newdata = std::malloc(newsize);
    std::memcpy(newdata, data, datalen);
    std::free(data);
    data = newdata;
    datalen = newsize;
}
