#ifndef CUBOS_IO_INPUT_ACTION_HPP
#define CUBOS_IO_INPUT_ACTION_HPP

#include <cubos/io/input_context.hpp>
#include <cubos/io/input_sources/input_source.hpp>
#include <list>

namespace cubos::io
{

    class InputAction
    {
    public:
        std::list<InputSource*> inputSources;
        std::list<std::function<void(InputContext)>> functionBindings;
        std::string name;
        bool enable;

        InputAction(std::string actionName);
        void addInput(InputSource* source);
        void addBinding(std::function<void(InputContext)> binding);
        void processSources();
    };
} // namespace cubos::io

#endif // CUBOS_IO_INPUT_ACTION_HPP