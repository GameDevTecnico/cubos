#include <cubos/memory/serializer.hpp>

using namespace cubos::core::memory;

Serializer::Serializer(Stream& stream) : stream(stream)
{
}

void Serializer::flush()
{
}

void Serializer::write(const std::string& str, const char* name)
{
    this->write(str.c_str(), name);
}
