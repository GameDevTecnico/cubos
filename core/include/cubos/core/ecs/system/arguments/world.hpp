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

        void analyze(SystemAccess& access) const // NOLINT(readability-convert-member-functions-to-static)
        {
            access.usesWorld = true;
        }

        World& fetch(const SystemContext& /*ctx*/)
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

        void analyze(SystemAccess& access) const // NOLINT(readability-convert-member-functions-to-static)
        {
            access.usesWorld = true;
        }

        const World& fetch(const SystemContext& /*ctx*/)
        {
            return mWorld;
        }

    private:
        const World& mWorld;
    };
} // namespace cubos::core::ecs
