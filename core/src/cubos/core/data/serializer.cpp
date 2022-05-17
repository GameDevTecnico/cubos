#include <cubos/core/data/serializer.hpp>

using namespace cubos::core::data;

Serializer::Serializer(memory::Stream& stream) : stream(stream)
{
}

void Serializer::flush()
{
}

bool Serializer::failed() const
{
    return this->failBit;
}
