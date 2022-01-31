#ifndef CUBOS_IO_CONTEXT_HPP
#define CUBOS_IO_CONTEXT_HPP

#include <glm/glm.hpp>
#include <variant>
namespace cubos::io
{

    class Context
    {
    public:
        std::variant<float, glm::vec2> value;
        Context()
        {
        }

        Context(glm::vec2 value)
        {
            this->value = value;
        }

        Context(float value)
        {
            this->value = value;
        }

        template <class T> T getValue()
        {
            return std::get<T>(this->value);
        }
    };
} // namespace cubos::io

#endif // CUBOS_IO_CONTEXT_HPP