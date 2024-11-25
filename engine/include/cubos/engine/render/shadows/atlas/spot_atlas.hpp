/// @file
/// @brief Resource @ref cubos::engine::SpotShadowAtlas.
/// @ingroup render-shadow-atlas-plugin

#pragma once

#include <map>

#include <glm/vec2.hpp>

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>
#include <cubos/engine/render/shadows/atlas/slot.hpp>

namespace cubos::engine
{
    /// @brief Resource which stores the shadow map atlas for spot lights,
    /// a large texture that holds the shadow maps for each shadow caster
    /// in a quadtree structure, reducing texture switching.
    /// @ingroup render-shadow-atlas-plugin
    class CUBOS_ENGINE_API SpotShadowAtlas
    {
    public:
        CUBOS_REFLECT;

        /// @brief Gets the size of the shadow atlas texture.
        /// @return Size of the shadow atlas texture, in pixels.
        glm::uvec2 getSize() const;

        /// @brief Recreates the shadow atlas texture.
        /// @param rd Render device used to create the texture.
        void resize(cubos::core::gl::RenderDevice& rd);

        /// @brief Configured size of the shadow atlas texture, in pixels.
        /// Use this to change the resolution of the atlas. Note that the
        /// texture isn't immediately resized; use @ref getSize() to get the
        /// actual texture size.
        glm::uvec2 configSize = {4096, 4096};

        /// @brief Whether the shadow atlas texture has already been cleared this frame.
        bool cleared = false;

        /// @brief Stores shadow maps for each spot shadow caster component.
        core::gl::Texture2D atlas{nullptr};

        /// @brief Stores the sizes, offsets, and caster ids of the shadow maps
        /// in the atlas.
        std::vector<std::shared_ptr<cubos::engine::ShadowMapSlot>> slots;

        /// @brief Maps shadow caster ids to their corresponding slots.
        std::map<int, std::shared_ptr<cubos::engine::ShadowMapSlot>> slotsMap;

    private:
        /// @brief Size of the shadow atlas texture, in pixels.
        glm::uvec2 mSize = {0, 0};
    };
} // namespace cubos::engine
