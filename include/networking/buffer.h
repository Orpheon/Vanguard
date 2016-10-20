#ifndef BUFFER_H
#define BUFFER_H

#include <cstdint>
#include <cstdlib>
#include <cstring>

class Buffer
{
    public:
        Buffer(void *data_, uint64_t datalen_);
        virtual ~Buffer();
        void* getdata() {return data;}
        uint64_t length() {return datalen;}
    protected:
        void *data;
        uint64_t datalen;
        uint64_t pos;
    private:
};

class ReadBuffer : public Buffer
{
    public:
        ReadBuffer(void *data_, uint64_t datalen_);
        virtual ~ReadBuffer();
        template<class T> T read()
        {
            if (datalen-pos < sizeof(T))
            {
                // Pulling too much data
                throw -1;
            }
            T r = (*reinterpret_cast<T*>(reinterpret_cast<char*>(data)+pos));
            pos += sizeof(T);
            return r;
        }
    protected:
    private:
};

class WriteBuffer : public Buffer
{
    public:
        WriteBuffer();
        virtual ~WriteBuffer();
        void enlarge(uint64_t newsize);
        template<class T> void write(T input)
        {
            if (datalen-pos < sizeof(T))
            {
                // Buffer too small, resize
                enlarge(datalen*2.0);
            }
            std::memcpy(reinterpret_cast<char*>(data)+pos, &input, sizeof(T));
        }
    protected:
    private:
        uint64_t STARTING_SIZE = 128;
};
#endif // BUFFER_H
