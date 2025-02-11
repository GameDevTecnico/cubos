/// @file
/// @brief Class @ref cubos::core::ecs::PluginQueue.
/// @ingroup core-ecs

#pragma once

#include <mutex>
#include <vector>

#include <cubos/core/api.hpp>

namespace cubos::core::ecs
{
    class Cubos;

    /// @brief Function pointer type representing a plugin.
    /// @ingroup core-ecs
    using Plugin = void (*)(Cubos&);

    /// @brief Stores plugin operations to be executed later.
    /// @ingroup core-ecs
    struct CUBOS_CORE_API PluginQueue final
    {
        std::vector<Plugin> toAdd;
        std::vector<Plugin> toRemove;
        std::vector<Plugin> toDestroy;
        std::mutex mutex;
    };
} // namespace cubos::core::ecs
