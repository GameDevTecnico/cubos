#include <cstring>

#include <cubos/core/memory/buffer_stream.hpp>

using namespace cubos::core::memory;

BufferStream::BufferStream(void* buffer, std::size_t size, bool readOnly)
{
    mBuffer = buffer;
    mSize = size;
    mPosition = 0;
    mReadOnly = readOnly;
    mReachedEof = false;
    mOwned = false;
}

BufferStream::BufferStream(const void* buffer, std::size_t size)
{
    // Casting from const void* to void* isn't a problem because the pointer will never be written to: the buffer is set
    // as read-only.
    mBuffer = const_cast<void*>(buffer);
    mSize = size;
    mPosition = 0;
    mReadOnly = true;
    mReachedEof = false;
    mOwned = false;
}

BufferStream::BufferStream(std::size_t size)
{
    if (size == 0)
    {
        abort();
    }

    mBuffer = new char[size];
    mSize = size;
    mPosition = 0;
    mReadOnly = false;
    mReachedEof = false;
    mOwned = true;
}

BufferStream::~BufferStream()
{
    if (mOwned)
    {
        delete[] static_cast<char*>(mBuffer);
    }
}

BufferStream::BufferStream(const BufferStream& other)
{
    mSize = other.mSize;
    mPosition = other.mPosition;
    mReadOnly = other.mReadOnly;
    mReachedEof = other.mReachedEof;
    mOwned = other.mOwned;
    if (mOwned)
    {
        mBuffer = new char[mSize];
        memcpy(mBuffer, other.mBuffer, mSize);
    }
    else
    {
        mBuffer = other.mBuffer;
    }
}

BufferStream::BufferStream(BufferStream&& other) noexcept
{
    mBuffer = other.mBuffer;
    mSize = other.mSize;
    mPosition = other.mPosition;
    mReadOnly = other.mReadOnly;
    mReachedEof = other.mReachedEof;
    mOwned = other.mOwned;
    other.mBuffer = nullptr;
    other.mSize = 0;
    other.mPosition = 0;
    other.mReadOnly = true;
    other.mReachedEof = true;
    other.mOwned = false;
}

const void* BufferStream::getBuffer() const
{
    return mBuffer;
}

std::string BufferStream::string() const
{
    return std::string{static_cast<const char*>(mBuffer), mPosition};
}

std::size_t BufferStream::read(void* data, std::size_t size)
{
    std::size_t bytesRemaining = mSize - mPosition;
    if (size > bytesRemaining)
    {
        size = bytesRemaining;
        mReachedEof = true;
    }
    memcpy(data, static_cast<char*>(mBuffer) + mPosition, size);
    mPosition += size;
    return size;
}

std::size_t BufferStream::write(const void* data, std::size_t size)
{
    if (mReadOnly)
    {
        return 0;
    }

    std::size_t bytesRemaining = mSize - mPosition;
    if (size > bytesRemaining)
    {
        if (mOwned)
        {
            // Expand the buffer.
            std::size_t newSize = mSize * 2;
            while (newSize < mPosition + size)
            {
                newSize *= 2;
            }

            char* newBuffer = new char[newSize];
            memcpy(newBuffer, static_cast<char*>(mBuffer), mSize);
            delete[] static_cast<char*>(mBuffer);
            mBuffer = newBuffer;
            mSize = newSize;
        }
        else
        {
            size = bytesRemaining;
            mReachedEof = true;
        }
    }
    memcpy(static_cast<char*>(mBuffer) + mPosition, data, size);
    mPosition += size;
    return size;
}

std::size_t BufferStream::tell() const
{
    return mPosition;
}

void BufferStream::seek(ptrdiff_t offset, SeekOrigin origin)
{
    if (origin == SeekOrigin::Current)
    {
        if (offset < 0 && static_cast<std::size_t>(-offset) > mPosition)
        {
            mPosition = 0;
        }
        else if (offset < 0)
        {
            mPosition -= static_cast<std::size_t>(-offset);
        }
        else
        {
            mPosition += static_cast<std::size_t>(offset);
        }
    }
    else if (origin == SeekOrigin::End)
    {
        if (offset < 0 && static_cast<std::size_t>(-offset) > mSize)
        {
            mPosition = 0;
        }
        else if (offset < 0)
        {
            mPosition = mSize - static_cast<std::size_t>(-offset);
        }
        else
        {
            mPosition = mSize + static_cast<std::size_t>(offset);
        }
    }
    else
    {
        if (offset < 0)
        {
            mPosition = 0;
        }
        else
        {
            mPosition = static_cast<std::size_t>(offset);
        }
    }

    if (mPosition > mSize)
    {
        mPosition = mSize;
    }

    mReachedEof = false;
}

bool BufferStream::eof() const
{
    return mReachedEof;
}

char BufferStream::peek() const
{
    if (mPosition == mSize)
    {
        return '\0';
    }
    return ((char*)mBuffer)[mPosition];
}
