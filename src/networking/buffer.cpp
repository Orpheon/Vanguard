#include "buffer.h"

Buffer::Buffer(void *data_, uint64_t datalen_) : data(data_), datalen(datalen_), pos(0)
{
    //ctor
}
Buffer::~Buffer()
{
    // TODO: free data somehow
}

ReadonlyBuffer::ReadonlyBuffer(void *data_, uint64_t datalen_) : Buffer(data_, datalen_)
{
    //ctor
}
ReadonlyBuffer::~ReadonlyBuffer()
{
    // TODO: free data somehow
}
