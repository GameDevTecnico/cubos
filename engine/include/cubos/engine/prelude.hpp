#pragma once

#include <cubos/core/ecs/entity/entity.hpp>
#include <cubos/core/ecs/system/event/reader.hpp>
#include <cubos/core/ecs/system/event/writer.hpp>
#include <cubos/core/ecs/system/query.hpp>
#include <cubos/core/ecs/system/system.hpp>

namespace cubos::engine
{
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
