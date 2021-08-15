#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <sys/types.h>

namespace io {

enum whence {
    SEEK_SET,
    SEEK_CUR,
    SEEK_END,
};

class Read {
public:
    virtual ~Read() { }
    virtual bool isSeekable(void) const = 0;
    virtual size_t tell(void) const = 0;
    virtual bool seek(off_t offset, whence whence) = 0;
    virtual size_t read(void* ptr, size_t size, size_t nmem) = 0;
    virtual int error(void) const = 0;
    virtual bool eof(void) const = 0;
};

class BufReader :
    public Read
{
public:
    BufReader(const void* buf, off_t size)
        : _buffer((const char* )buf)
        , _size(size > 0 ? size : 0)
        , _pos(0) {
    }
    virtual ~BufReader() { }
    virtual bool isSeekable(void) const { return true; }
    virtual size_t tell(void) const { return _pos; }
    virtual bool seek(off_t offset, whence whence) {
        off_t target;
        switch (whence) {
        case SEEK_SET:
            target = offset;
            break;
        case SEEK_CUR:
            target = _pos + offset;
            break;
        case SEEK_END:
            target = _size + offset;
            break;
        default:
            return false;
        }
        if (target < 0 || target > _size)
            return false;
        _pos = target;
        return true;
    }
    virtual size_t read(void* ptr, size_t size, size_t nmem) {
        off_t count = (off_t)(size * nmem);
        off_t final = _pos + count;
        if (count <= 0 || final < _pos)
            return 0;
        if (final > _size)
            count = _size - _pos;
        memcpy(ptr, _buffer + _pos, count);
        _pos += count;
        return count;
    }
    virtual int error(void) const { return 0; }
    virtual bool eof(void) const { return _pos == _size; }

private:
    const char* _buffer;
    const off_t _size;
    off_t _pos;
};

class BinaryReader {
public:
    BinaryReader(Read* reader)
        : _reader(reader)
    {
    }

    template<typename T>
    bool read(T* out) {
        return true;
    }

private:
    Read* _reader;
};

}
