#pragma once

#include <cubos/core/ecs/entity/entity.hpp>
#include <cubos/core/ecs/system/accessors.hpp>
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

    /// @copydoc cubos::core::ecs::Read
    template <typename T>
    using Read = core::ecs::Read<T>;

    /// @copydoc cubos::core::ecs::Write
    template <typename T>
    using Write = core::ecs::Write<T>;

    /// @copydoc cubos::core::ecs::OptRead
    template <typename T>
    using OptRead = core::ecs::OptRead<T>;

    /// @copydoc cubos::core::ecs::OptWrite
    template <typename T>
    using OptWrite = core::ecs::OptWrite<T>;

    /// @copydoc cubos::core::ecs::EventReader
    template <typename T, unsigned int M = DEFAULT_FILTER_MASK>
    using EventReader = core::ecs::EventReader<T, M>;

    /// @copydoc cubos::core::ecs::EventWriter
    template <typename T>
    using EventWriter = core::ecs::EventWriter<T>;
} // namespace cubos::engine
