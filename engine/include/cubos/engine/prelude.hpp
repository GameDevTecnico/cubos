#pragma once

#include <cubos/core/ecs/cubos.hpp>
#include <cubos/core/ecs/entity/entity.hpp>
#include <cubos/core/ecs/system/system.hpp>

namespace cubos::engine
{
    /// @copydoc cubos::core::ecs::Tag
    using Tag = core::ecs::Tag;

    /// @copydoc cubos::core::ecs::Cubos
    using Cubos = core::ecs::Cubos;

    /// @copydoc cubos::core::ecs::DeltaTime
    using DeltaTime = core::ecs::DeltaTime;

    /// @copydoc cubos::core::ecs::ShouldQuit
    using ShouldQuit = core::ecs::ShouldQuit;

    /// @copydoc cubos::core::ecs::Arguments
    using Arguments = core::ecs::Arguments;

    /// @copydoc cubos::core::ecs::Query
    template <typename... ComponentTypes>
    using Query = core::ecs::Query<ComponentTypes...>;

    /// @copydoc cubos::core::ecs::Entity
    using Entity = core::ecs::Entity;

    /// @copydoc cubos::core::ecs::Commands
    using Commands = core::ecs::Commands;

    /// @copydoc cubos::core::ecs::Opt
    template <typename T>
    using Opt = core::ecs::Opt<T>;

    /// @copydoc cubos::core::ecs::EventReader
    template <typename T, unsigned int M = DEFAULT_FILTER_MASK>
    using EventReader = core::ecs::EventReader<T, M>;

    /// @copydoc cubos::core::ecs::EventWriter
    template <typename T>
    using EventWriter = core::ecs::EventWriter<T>;
} // namespace cubos::engine
