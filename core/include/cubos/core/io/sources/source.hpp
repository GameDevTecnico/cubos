#ifndef CUBOS_CORE_IO_SOURCE_HPP
#define CUBOS_CORE_IO_SOURCE_HPP

#include <cubos/core/event.hpp>
#include <cubos/core/io/window.hpp>
#include <cubos/core/io/keyboard.hpp>
#include <map>
#include <string>
#include <memory>
#include <variant>
#include <list>
#include <glm/glm.hpp>

namespace cubos::core::io
{
    class Context;

    /// Class used as a decorator for source objects
    /// Handles events subscription and context creation
    class Source
    {
    public:
        /// Subscribes this object to callbacks in the Input Manager
        /// @see unsubscribeEvents
        virtual void subscribeEvents() = 0;

        /// Unsubscribes this object to callbacks in the Input Manager
        /// @see subscribeEvents
        virtual void unsubscribeEvents() = 0;

        /// Checks if this source as been triggered since last check
        /// @return true if source was triggered, otherwise false
        virtual bool isTriggered() = 0;

        /// Creates the context related to this source
        /// @return context created by this source
        virtual Context createContext() = 0;
    };
} // namespace cubos::core::io
#endif // CUBOS_CORE_IO_SOURCE_HPP
