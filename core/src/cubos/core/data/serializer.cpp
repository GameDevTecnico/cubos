#include <cubos/core/data/serializer.hpp>

using namespace cubos::core::data;

Serializer::Serializer(memory::Stream& stream) : stream(stream)
{
    this->failBit = false;
}

void Serializer::flush()
{
}

bool Serializer::failed() const
{
    return this->failBit;
}
