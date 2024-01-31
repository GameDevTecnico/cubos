/// @file
/// @brief Class @ref cubos::core::ecs::SystemFetcher.
/// @ingroup core-ecs-system

#pragma once

namespace cubos::core::ecs
{
    class World;
    class CommandBuffer;
    struct SystemOptions;
    struct SystemAccess;

    /// @brief Type meant to be specialized which implements for each argument type the necessary logic to extract it
    /// from the world.
    /// @tparam T Argument type.
    /// @ingroup core-ecs-system
    template <typename T>
    class SystemFetcher
    {
    public:
        /// @brief Indicates whether this argument type consumes system options.
        static inline constexpr bool ConsumesOptions = false;

        /// @brief Called when a system is constructed for the first time.
        /// @param world World to extract data from.
        /// @param options Argument options.
        SystemFetcher(World& world, const SystemOptions& options)
        {
            (void)world;
            (void)options;

            // This should never be instantiated. This constructor is only defined for documentation purposes.
            static_assert(AlwaysFalse<T>, "Invalid system argument type");
        }

        /// @brief Called to determine the access patterns of the argument.
        /// @param[out] access Access patterns to add info to.
        void analyze(SystemAccess& access) const
        {
            (void)access;

            // This should never be instantiated. This method is only defined for documentation purposes.
            static_assert(AlwaysFalse<T>, "Invalid system argument type");
        }

        /// @brief Called each system run to fetch the data from the world.
        /// @param cmdBuffer Command buffer.
        T fetch(CommandBuffer& cmdBuffer)
        {
            (void)cmdBuffer;

            // This should never be instantiated. This method is only defined for documentation purposes.
            static_assert(AlwaysFalse<T>, "Invalid system argument type");
        }

    private:
        // Looks dumb, but this is necessary to make the static_assert's only trigger when the template is instantiated.
        // Reference: https://stackoverflow.com/questions/44059557/whats-the-right-way-to-call-static-assertfalse
        template <typename U>
        static constexpr bool AlwaysFalse = false;
    };
} // namespace cubos::core::ecs
