/// @file
/// @brief Implements the debugger functionality of Cubos.

#include <cubos/core/memory/stream.hpp>

namespace cubos::core::ecs
{
    class Cubos;

    /// @brief Receives debugging commands from the given stream and executes them on the given application.
    ///
    /// This function will block until the stream is closed or the application is stopped.
    ///
    /// @param stream Stream to receive debugging commands from and send responses to.
    /// @param cubos Application to debug.
    /// @return Whether the application should continue running.
    bool debugger(memory::Stream& stream, Cubos& cubos);
} // namespace cubos::core::ecs