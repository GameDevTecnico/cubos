/// @file
/// @brief Resource system argument specializations.
/// @ingroup core-ecs-system-arguments

#pragma once

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
            : mResource{world.write<T>().get()}
        {
        }

        void analyze(SystemAccess& /*access*/) const
        {
            // FIXME: when we add resources to the world type registry, we should add its type id to the access object.
        }

        T& fetch(CommandBuffer& /*cmdBuffer*/)
        {
            return mResource;
        }

    private:
        T& mResource;
    };

    template <typename T>
    class SystemFetcher<const T&>
    {
    public:
        static inline constexpr bool ConsumesOptions = false;

        SystemFetcher(World& world, const SystemOptions& /*options*/)
            : mResource{world.read<T>().get()}
        {
        }

        void analyze(SystemAccess& /*access*/) const
        {
            // FIXME: when we add resources to the world type registry, we should add its type id to the access object.
        }

        const T& fetch(CommandBuffer& /*cmdBuffer*/)
        {
            return mResource;
        }

    private:
        const T& mResource;
    };
} // namespace cubos::core::ecs
