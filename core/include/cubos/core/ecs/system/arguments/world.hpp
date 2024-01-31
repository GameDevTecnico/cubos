/// @file
/// @brief World system argument specializations.
/// @ingroup core-ecs-system-arguments

#pragma once

#include <cubos/core/ecs/system/access.hpp>
#include <cubos/core/ecs/system/fetcher.hpp>

namespace cubos::core::ecs
{
    template <>
    class SystemFetcher<World&>
    {
    public:
        static inline constexpr bool ConsumesOptions = false;

        SystemFetcher(World& world, const SystemOptions& /*options*/)
            : mWorld{world}
        {
        }

        static void analyze(SystemAccess& access)
        {
            access.usesWorld = true;
        }

        World& fetch(CommandBuffer& /*cmdBuffer*/)
        {
            return mWorld;
        }

    private:
        World& mWorld;
    };

    template <>
    class SystemFetcher<const World&>
    {
    public:
        static inline constexpr bool ConsumesOptions = false;

        SystemFetcher(World& world, const SystemOptions& /*options*/)
            : mWorld{world}
        {
        }

        static void analyze(SystemAccess& access)
        {
            access.usesWorld = true;
        }

        const World& fetch(CommandBuffer& /*cmdBuffer*/)
        {
            return mWorld;
        }

    private:
        const World& mWorld;
    };
} // namespace cubos::core::ecs
