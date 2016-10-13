#ifndef BUFFER_H
#define BUFFER_H

#include <cstdint>

class Buffer
{
    public:
        Buffer(void *data_, uint64_t datalen_);
        virtual ~Buffer();
    protected:
    private:
        void *data;
        uint64_t datalen;
        uint64_t pos;
};

class ReadonlyBuffer : public Buffer
{
    public:
        ReadonlyBuffer(void *data_, uint64_t datalen_);
        virtual ~ReadonlyBuffer();
        template<class T> T read()
        {
            if (datalen-pos < sizeof(T))
            {
                // Pulling too much data
                throw -1;
            }
            T r = (*reinterpret_cast<T*>(data+pos));
            pos += sizeof(T);
            return r;
        }
    protected:
    private:
};
#endif // BUFFER_H
