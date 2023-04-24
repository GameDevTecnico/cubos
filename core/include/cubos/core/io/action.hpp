#ifndef CUBOS_CORE_IO_ACTION_HPP
#define CUBOS_CORE_IO_ACTION_HPP

#include <list>

#include <cubos/core/io/context.hpp>
#include <cubos/core/io/sources/source.hpp>

namespace cubos::core::io
{
    /// Class used to map between inputSources and the functions they should call when triggered
    /// Example usage:
    ///
    ///     io::InputManager::init(window);
    ///     auto ShootAction = io::InputManager::createAction("shoot_action");
    ///     ShootAction->addBinding(shoot);
    ///     ShootAction->addSource(new io::DoubleAxis(new io::ButtonPress(io::MouseButton::Right));
    class Action
    {
    public:
        /// The constructor for an Action with a name
        /// @param name name of the Action being created
        Action(std::string name);

        /// Gets the name of the action
        std::string getName() const;

        /// Adds a new source to the Action
        /// @param source Source to add
        void addSource(Source* source);

        /// Adds a new bindings to the Action
        /// @param binding Function to add
        void addBinding(std::function<void(Context)> binding);

        /// Checks if any Input Source was triggered.
        /// If so, calls all the binded functions
        void processSources();

    private:
        std::string name;
        std::list<Source*> inputSources;
        std::list<std::function<void(Context)>> functionBindings;
        bool enable;
    };
} // namespace cubos::core::io

#endif // CUBOS_CORE_IO_ACTION_HPP
