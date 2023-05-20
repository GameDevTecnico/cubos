#include <cubos/core/memory/std_stream.hpp>

using namespace cubos::core::memory;

StdStream::StdStream(FILE* file, bool close)
{
    mFile = file;
    mClose = close;
}

StdStream::StdStream(StdStream&& other) noexcept
{
    mFile = other.mFile;
    mClose = other.mClose;
    other.mFile = nullptr;
    other.mClose = false;
}

StdStream::~StdStream()
{
    if (mClose)
    {
        fclose(mFile);
    }
}

std::size_t StdStream::read(void* data, std::size_t size)
{
    return fread(data, 1, size, mFile);
}

std::size_t StdStream::write(const void* data, std::size_t size)
{
    return fwrite(data, 1, size, mFile);
}

std::size_t StdStream::tell() const
{
    auto pos = ftell(mFile);
    return pos == -1 ? SIZE_MAX : static_cast<std::size_t>(pos);
}

void StdStream::seek(ptrdiff_t offset, SeekOrigin origin)
{
    switch (origin)
    {
    case SeekOrigin::Begin:
        fseek(mFile, static_cast<long>(offset), SEEK_SET);
        break;
    case SeekOrigin::Current:
        fseek(mFile, static_cast<long>(offset), SEEK_CUR);
        break;
    case SeekOrigin::End:
        fseek(mFile, static_cast<long>(offset), SEEK_END);
        break;
    }
}

bool StdStream::eof() const
{
    return feof(mFile) != 0;
}

char StdStream::peek() const
{
    auto c = fgetc(mFile);
    ungetc(c, mFile);
    return static_cast<char>(c);
}
