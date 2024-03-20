/// @file
/// @brief Resource system argument specializations.
/// @ingroup core-ecs-system-arguments

#pragma once

#include <cubos/core/ecs/system/access.hpp>
#include <cubos/core/ecs/system/fetcher.hpp>
#include <cubos/core/ecs/world.hpp>

namespace cubos::core::ecs
{
    template <typename T>
    class SystemFetcher<T&>
    {
    public:
        static inline constexpr bool ConsumesOptions = false;

        SystemFetcher(World& world, const SystemOptions& /*options*/)
            : mWorld{world}
        {
        }

        void analyze(SystemAccess& access) const
        {
            access.dataTypes.insert(mWorld.types().id(reflection::reflect<T>()));
        }

        T& fetch(const SystemContext& /*ctx*/)
        {
            return mWorld.resource<T>();
        }

    private:
        World& mWorld;
    };

    template <typename T>
    class SystemFetcher<const T&>
    {
    public:
        static inline constexpr bool ConsumesOptions = false;

        SystemFetcher(World& world, const SystemOptions& /*options*/)
            : mWorld{world}
        {
        }

        void analyze(SystemAccess& access) const
        {
            access.dataTypes.insert(mWorld.types().id(reflection::reflect<T>()));
        }

        const T& fetch(const SystemContext& /*ctx*/)
        {
            return mWorld.resource<T>();
        }

    private:
        const World& mWorld;
    };
} // namespace cubos::core::ecs
