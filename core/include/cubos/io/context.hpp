#ifndef CUBOS_IO_CONTEXT_HPP
#define CUBOS_IO_CONTEXT_HPP

#include <glm/glm.hpp>
#include <variant>
namespace cubos::io
{
    /// Context is used to pass parameters created by input
    /// Actions that are relevant to the gameplay logic.
    /// Currently it supports glm::vec2 and float.
    class Context
    {
    public:
        /// Empty Context constructor
        Context();

        /// Creates a context with a glm::vec2 object
        /// @param value glm::vec2 to be stored in the context
        Context(glm::vec2 value);

        /// Creates a context with a float
        /// @param value float to be stored in the context
        Context(float value);

        /// Get the value stores inside the Context.
        /// Assumes the value stored is of type T.
        /// @return returns the value if stored value is of type T. Otherwise, crashes.
        template <class T> T getValue();

    private:
        std::variant<float, glm::vec2> value;
    };

    template <class T> T Context::getValue()
    {
        return std::get<T>(this->value);
    }
} // namespace cubos::io

#endif // CUBOS_IO_CONTEXT_HPP