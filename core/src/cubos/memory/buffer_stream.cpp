#include <cubos/memory/buffer_stream.hpp>

#include <cstring>

using namespace cubos::memory;

BufferStream::BufferStream(void* buffer, size_t size, bool readOnly)
{
    this->buffer = buffer;
    this->size = size;
    this->position = 0;
    this->readOnly = readOnly;
}

BufferStream::BufferStream(const void* buffer, size_t size)
{
    // Casting from const void* to void* isn't a problem because the pointer will never be written to: the buffer is set
    // as read-only.
    this->buffer = const_cast<void*>(buffer);
    this->size = size;
    this->position = 0;
    this->readOnly = true;
}

size_t BufferStream::read(void* data, size_t size)
{
    size_t bytesRemaining = this->size - this->position;
    if (size > bytesRemaining)
        size = bytesRemaining;
    memcpy(data, this->buffer + this->position, size);
    this->position += size;
    return size;
}

size_t BufferStream::write(const void* data, size_t size)
{
    if (this->readOnly)
        return 0;

    size_t bytesRemaining = this->size - this->position;
    if (size > bytesRemaining)
        size = bytesRemaining;
    memcpy(this->buffer + this->position, data, size);
    this->position += size;
    return size;
}

size_t BufferStream::tell() const
{
    return this->position;
}

void BufferStream::seek(int64_t offset, SeekOrigin origin)
{
    if (origin == SeekOrigin::Current)
    {
        if (offset < 0 && -offset > this->position)
            this->position = 0;
        else
            this->position += offset;
    }
    else if (origin == SeekOrigin::End)
    {
        if (offset < 0 && -offset > this->size)
            this->position = 0;
        else
            this->position = this->size + offset;
    }
    else
    {
        if (offset < 0)
            this->position = 0;
        else
            this->position = offset;
    }

    if (this->position > this->size)
        this->position = this->size;
}

bool BufferStream::eof() const
{
    return this->position == this->size;
}

char BufferStream::peek() const
{
    if (this->position == this->size)
        return '\0';
    return *(char*)(this->buffer + this->position);
}
