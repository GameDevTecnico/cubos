#include <cubos/core/data/serializer.hpp>

using namespace cubos::core::data;

Serializer::Serializer()
{
    this->failBit = false;
}

void Serializer::flush()
{
    // Do nothing.
}

bool Serializer::failed() const
{
    return this->failBit;
}
