/// @file
/// @brief Class @ref cubos::core::ecs::DynamicPlugin.
/// @ingroup core-ecs

#pragma once

#include <string>

#include <cubos/core/ecs/plugin_queue.hpp>

namespace cubos::core::ecs
{
    /// @brief Manages the loading and unloading of a plugin from a shared library.
    /// @ingroup core-ecs
    class CUBOS_CORE_API DynamicPlugin
    {
    public:
        ~DynamicPlugin();

        /// @brief Loads the plugin from a shared library with the given name.
        /// @param name Shared library name.
        /// @return Whether the plugin was loaded successfully.
        bool load(const std::string& name);

        /// @brief Unloads the plugin.
        ///
        /// Does nothing if the plugin was not loaded.
        void unload();

        /// @brief Gets the plugin function pointer.
        ///
        /// Aborts if the plugin was not loaded.
        ///
        /// @return Plugin function pointer.
        Plugin function() const;

    private:
        void* mHandle{nullptr};
        Plugin mPlugin;
    };
} // namespace cubos::core::ecs
