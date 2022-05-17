#include <cubos/core/data/deserializer.hpp>

using namespace cubos::core::data;

Deserializer::Deserializer(memory::Stream& stream) : stream(stream)
{
}

bool Deserializer::failed() const
{
    return this->failBit;
}
