#ifndef CUBOS_IO_ACTION_HPP
#define CUBOS_IO_ACTION_HPP

#include <cubos/io/context.hpp>
#include <cubos/io/sources/source.hpp>

#include <list>

namespace cubos::io
{
    /// Class used to map between inputSources and the functions they should call when triggered
    /// Example usage:
    ///
    ///     io::InputManager::init(window);
    ///     auto ShootAction = io::InputManager::createAction("shoot_action");
    ///     ShootAction->addBinding(shoot);
    ///     ShootAction->addInput(new io::DoubleAxis(new io::ButtonPress(io::MouseButton::Right));
    class Action
    {
    public:
        std::string name; ///< Name of the Action.

        /// The constructor for an Action with a name
        /// @param name name of the Action being created
        Action(std::string name);

        /// Adds a new source to the Action
        /// @param source Source to add
        void addInput(Source* source);

        /// Adds a new bindings to the Action
        /// @param binding Function to add
        void addBinding(std::function<void(Context)> binding);

        /// Checks if any Input Source was triggered.
        /// If so, calls all the binded functions
        void processSources();

    private:
        std::list<Source*> inputSources;
        std::list<std::function<void(Context)>> functionBindings;
        bool enable;
    };
} // namespace cubos::io

#endif // CUBOS_IO_ACTION_HPP
