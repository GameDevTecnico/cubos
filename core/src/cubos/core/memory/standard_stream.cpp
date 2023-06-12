#include <cubos/core/memory/standard_stream.hpp>

using namespace cubos::core::memory;

StandardStream::StandardStream(FILE* file, bool close)
{
    mFile = file;
    mClose = close;
}

StandardStream::StandardStream(StandardStream&& other) noexcept
{
    mFile = other.mFile;
    mClose = other.mClose;
    other.mFile = nullptr;
    other.mClose = false;
}

StandardStream::~StandardStream()
{
    if (mClose)
    {
        fclose(mFile);
    }
}

std::size_t StandardStream::read(void* data, std::size_t size)
{
    return fread(data, 1, size, mFile);
}

std::size_t StandardStream::write(const void* data, std::size_t size)
{
    return fwrite(data, 1, size, mFile);
}

std::size_t StandardStream::tell() const
{
    auto pos = ftell(mFile);
    return pos == -1 ? SIZE_MAX : static_cast<std::size_t>(pos);
}

void StandardStream::seek(ptrdiff_t offset, SeekOrigin origin)
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

bool StandardStream::eof() const
{
    return feof(mFile) != 0;
}

char StandardStream::peek() const
{
    auto c = fgetc(mFile);
    ungetc(c, mFile);
    return static_cast<char>(c);
}
