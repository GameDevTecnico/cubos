/// @file
/// @brief Class @ref cubos::core::ecs::Plugins.
/// @ingroup core-ecs-system-arguments

#pragma once

#include <cubos/core/ecs/plugin_queue.hpp>
#include <cubos/core/ecs/system/fetcher.hpp>

namespace cubos::core::ecs
{
    /// @brief System argument used to add or remove Cubos plugins at runtime.
    /// @ingroup core-ecs-system-arguments
    class CUBOS_CORE_API Plugins final
    {
    public:
        /// @brief Constructs.
        /// @param queue Plugin queue.
        Plugins(PluginQueue* queue);

        /// @brief Adds a plugin to the application. Will be executed on the next frame.
        ///
        /// The plugin's startup systems will run before the frame begins.
        ///
        /// @param plugin Plugin.
        void add(Plugin plugin);

        /// @brief Removes a plugin from the application. Will be executed on the next frame.
        ///
        /// This will revert all configuration changes made by the plugin. It won't revert any effects caused by the
        /// plugin on the world, such as adding or removing entities.
        ///
        /// @param plugin Plugin.
        void remove(Plugin plugin);

    private:
        PluginQueue* mQueue;
    };

    template <>
    class SystemFetcher<Plugins>
    {
    public:
        static inline constexpr bool ConsumesOptions = false;

        SystemFetcher(World& /*world*/, const SystemOptions& /*options*/)
        {
        }

        void analyze(SystemAccess& /*access*/) const
        {
        }

        static Plugins fetch(const SystemContext& ctx)
        {
            return {ctx.pluginQueue};
        }
    };
} // namespace cubos::core::ecs
