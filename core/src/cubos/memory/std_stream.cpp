#include <cubos/memory/std_stream.hpp>

using namespace cubos::core::memory;

StdStream::StdStream(FILE* file, bool close)
{
    this->file = file;
    this->close = close;
}

StdStream::StdStream(StdStream&& other)
{
    this->file = other.file;
    this->close = other.close;
    other.file = nullptr;
    other.close = false;
}

StdStream::~StdStream()
{
    if (this->close)
        fclose(this->file);
}

size_t StdStream::read(void* data, size_t size)
{
    return fread(data, size, 1, this->file);
}

size_t StdStream::write(const void* data, size_t size)
{
    return fwrite(data, size, 1, this->file);
}

size_t StdStream::tell() const
{
    return ftell(this->file);
}

void StdStream::seek(int64_t offset, SeekOrigin origin)
{
    switch (origin)
    {
    case SeekOrigin::Begin:
        fseek(this->file, offset, SEEK_SET);
        break;
    case SeekOrigin::Current:
        fseek(this->file, offset, SEEK_CUR);
        break;
    case SeekOrigin::End:
        fseek(this->file, offset, SEEK_END);
        break;
    }
}

bool StdStream::eof() const
{
    return feof(this->file);
}

char StdStream::peek() const
{
    auto c = fgetc(this->file);
    ungetc(c, this->file);
    return static_cast<char>(c);
}
