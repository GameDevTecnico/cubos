#include <cubos/io/context.hpp>

using namespace cubos::io;

Context::Context()
{
}

Context::Context(glm::vec2 value)
{
    this->value = value;
}

Context::Context(float value)
{
    this->value = value;
}