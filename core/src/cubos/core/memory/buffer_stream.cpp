#include <cubos/core/memory/buffer_stream.hpp>

#include <cstring>

using namespace cubos::core::memory;

BufferStream::BufferStream(void* buffer, size_t size, bool readOnly)
{
    this->buffer = buffer;
    this->size = size;
    this->position = 0;
    this->readOnly = readOnly;
    this->reachedEof = false;
    this->owned = false;
}

BufferStream::BufferStream(const void* buffer, size_t size)
{
    // Casting from const void* to void* isn't a problem because the pointer will never be written to: the buffer is set
    // as read-only.
    this->buffer = const_cast<void*>(buffer);
    this->size = size;
    this->position = 0;
    this->readOnly = true;
    this->reachedEof = false;
    this->owned = false;
}

BufferStream::BufferStream(size_t size)
{
    if (size == 0)
    {
        abort();
    }

    this->buffer = new char[size];
    this->size = size;
    this->position = 0;
    this->readOnly = false;
    this->reachedEof = false;
    this->owned = true;
}

BufferStream::~BufferStream()
{
    if (this->owned)
    {
        delete[] static_cast<char*>(this->buffer);
    }
}

BufferStream::BufferStream(BufferStream&& other)
{
    this->buffer = other.buffer;
    this->size = other.size;
    this->position = other.position;
    this->readOnly = other.readOnly;
    this->reachedEof = other.reachedEof;
    this->owned = other.owned;
    other.buffer = nullptr;
    other.size = 0;
    other.position = 0;
    other.readOnly = true;
    other.reachedEof = true;
    other.owned = false;
}

size_t BufferStream::read(void* data, size_t size)
{
    size_t bytesRemaining = this->size - this->position;
    if (size > bytesRemaining)
    {
        size = bytesRemaining;
        this->reachedEof = true;
    }
    memcpy(data, static_cast<char*>(this->buffer) + this->position, size);
    this->position += size;
    return size;
}

size_t BufferStream::write(const void* data, size_t size)
{
    if (this->readOnly)
        return 0;

    size_t bytesRemaining = this->size - this->position;
    if (size > bytesRemaining)
    {
        if (this->owned)
        {
            // Expand the buffer.
            size_t newSize = this->size * 2;
            while (newSize < this->position + size)
            {
                newSize *= 2;
            }

            char* newBuffer = new char[newSize];
            memcpy(newBuffer, static_cast<char*>(this->buffer), this->size);
            delete[] static_cast<char*>(this->buffer);
            this->buffer = newBuffer;
            this->size = newSize;
        }
        else
        {
            size = bytesRemaining;
            this->reachedEof = true;
        }
    }
    memcpy(static_cast<char*>(this->buffer) + this->position, data, size);
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

    this->reachedEof = false;
}

bool BufferStream::eof() const
{
    return this->reachedEof;
}

char BufferStream::peek() const
{
    if (this->position == this->size)
        return '\0';
    return ((char*)this->buffer)[this->position];
}
