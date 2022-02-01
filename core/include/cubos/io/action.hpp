#ifndef CUBOS_IO_ACTION_HPP
#define CUBOS_IO_ACTION_HPP

#include <cubos/io/context.hpp>
#include <cubos/io/sources/source.hpp>
#include <list>

namespace cubos::io
{

    class InputAction
    class Action
    {
    public:
        std::list<Source*> inputSources;
        std::list<std::function<void(Context)>> functionBindings;
        std::string name;
        bool enable;

        Action(std::string actionName);
        void addInput(Source* source);
        void addBinding(std::function<void(Context)> binding);
        void processSources();
    };
} // namespace cubos::io

#endif // CUBOS_IO_ACTION_HPP