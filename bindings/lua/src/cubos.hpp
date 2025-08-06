/// @dir
/// @brief @ref lua-bindings-plugin plugin directory.

/// @file
/// @brief Base cubos lua api.
/// @ingroup lua-bindings-plugin
#pragma once

#include <lua.hpp>

#include <cubos/engine/prelude.hpp>

namespace cubos::bindings::lua
{
    /// @brief Injects the cubos table into the lua VM.
    /// @param state The current lua state.
    /// @param cubos The @ref cubos::engine::Cubos instance to be stored.
    /// @ingroup lua-bindings-plugin
    void injectCubos(lua_State* state, cubos::engine::Cubos* cubos);

    /// @brief Gets the injected cubos table from the lua VM.
    /// @param state The current lua state.
    /// @returns Pointer to the stored @ref cubos::engine::Cubos instance.
    /// @ingroup lua-bindings-plugin
    cubos::engine::Cubos* getCubos(lua_State* state);
} // namespace cubos::bindings::lua
