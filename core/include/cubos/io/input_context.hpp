#ifndef CUBOS_IO_INPUT_CONTEXT_HPP
#define CUBOS_IO_INPUT_CONTEXT_HPP

#include <glm/glm.hpp>
#include <variant>
namespace cubos::io
{

    class InputContext
    {
    public:
        std::variant<float, glm::vec2> value;
        InputContext()
        {
        }

        InputContext(glm::vec2 value)
        {
            this->value = value;
        }

        InputContext(float value)
        {
            this->value = value;
        }

        template <class T> T getValue()
        {
            return std::get<T>(this->value);
        }
    };
} // namespace cubos::io

#endif // CUBOS_IO_INPUT_CONTEXT_HPP