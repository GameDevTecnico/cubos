/// @file
/// @brief Component @ref cubos::engine::DirectionalShadowCaster.
/// @ingroup render-shadow-casters-plugin

#pragma once

#include <unordered_map>
#include <vector>

#include <cubos/core/ecs/entity/entity.hpp>
#include <cubos/core/ecs/entity/hash.hpp>
#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>
#include <cubos/engine/render/shadows/casters/caster.hpp>

namespace cubos::engine
{
    /// @brief Component which enables shadow casting on a directional light.
    /// @ingroup render-shadow-casters-plugin
    struct CUBOS_ENGINE_API DirectionalShadowCaster
    {
        CUBOS_REFLECT;

        static constexpr int MaxCascades = 10;

        cubos::engine::ShadowCaster baseSettings;

        /// @brief Target distance of each split from 0 to 1
        /// (interpolated between nearDistance and maxDistance).
        std::vector<float> splitDistances = {0.33F, 0.66F};

        float maxDistance = 0;  ///< Max distance (if 0, uses camera zFar).
        float nearDistance = 0; ///< Near distance of shadow frustum (if 0, uses camera zNear).

        /// @brief Directional shadow map data.
        class ShadowMap;

        std::unordered_map<core::ecs::Entity, std::shared_ptr<ShadowMap>, core::ecs::EntityHash>
            shadowMaps; ///< Caster's shadow maps for each camera.

        /// @brief Target size of the shadow map textures, in pixels.
        /// Use this to change the resolution of the shadow maps. Note that the
        /// real resolution may not match this, as it may be dynamically adjusted.
        glm::uvec2 size = {4096, 4096};

        /// @brief Update shadow maps to the target size and target number of cascades.
        /// @param rd Render device used to create the textures.
        void updateShadowMaps(cubos::core::gl::RenderDevice& rd);

        /// @brief Set splits to have the same distance between each other.
        /// @param numCascades Number of shadow map cascades.
        void setDefaultSplitDistances(int numCascades);

        /// @brief Get the real current distances of each split.
        /// @return Current distances of each split.
        const std::vector<float>& getCurrentSplitDistances() const;

        /// @brief Get the real current size of shadow maps.
        /// @return Size of the shadow map textures, in pixels.
        glm::uvec2 getCurrentSize() const;

    private:
        std::vector<float> mSplitDistances; ///< Current distance of each split.
        glm::uvec2 mSize = {0, 0};          ///< Used to check if size has been changed since the latest frame.
    };

    class DirectionalShadowCaster::ShadowMap
    {
    public:
        core::gl::Texture2DArray cascades;         ///< Cascades of the shadow map.
        core::gl::Texture2DArray previousCascades; ///< Used to check if the framebuffer needs to be recreated.
        core::gl::Framebuffer framebuffer;         ///< Framebuffer used by the shadow rasterizer.

        /// @brief Gets the size of the shadow map textures.
        /// @return Size of the shadow map textures, in pixels.
        glm::uvec2 size() const;

        /// @brief Recreates the shadow map textures.
        /// @param rd Render device used to create the textures.
        /// @param configSize Max size of the shadow map textures.
        /// @param numCascades Number of shadow map cascades.
        void resize(cubos::core::gl::RenderDevice& rd, glm::uvec2 configSize, int numCascades);

    private:
        /// @brief Size of the shadow map textures, in pixels.
        glm::uvec2 mSize = {0, 0};
    };
} // namespace cubos::engine
