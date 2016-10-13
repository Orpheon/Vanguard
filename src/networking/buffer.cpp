#include "buffer.h"
#include <cstdlib>

Buffer::Buffer(void *data_, uint64_t datalen_) : data(data_), datalen(datalen_), pos(0)
{
    //ctor
}
Buffer::~Buffer()
{
    std::free(data);
}

ReadBuffer::ReadBuffer(void *data_, uint64_t datalen_) : Buffer(data_, datalen_)
{
    //ctor
}
ReadBuffer::~ReadBuffer()
{
    //dtor
}
