/// @file
/// @brief Resource @ref cubos::engine::ShadowAtlas.
/// @ingroup render-shadow-atlas-plugin

#pragma once

#include <map>

#include <glm/vec2.hpp>

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Resource which stores shadow map atlases, large textures that
    /// hold the shadow maps for each shadow caster in a quadtree structure,
    /// reducing texture switching. There is an atlas for spot lights, and another
    /// for point lights.
    /// @ingroup render-shadow-atlas-plugin
    class CUBOS_ENGINE_API ShadowAtlas
    {
    public:
        CUBOS_REFLECT;

        /// @brief Gets the size of the spot shadow atlas texture.
        /// @return Size of the spot shadow atlas texture, in pixels.
        glm::uvec2 getSpotAtlasSize() const;

        /// @brief Gets the size of the point shadow atlas texture.
        /// @return Size of the point shadow atlas texture, in pixels.
        glm::uvec2 getPointAtlasSize() const;

        /// @brief Recreates the shadow atlas textures.
        /// @param rd Render device used to create the textures.
        void resize(cubos::core::gl::RenderDevice& rd);

        /// @brief Configured size of the spot shadow atlas texture, in pixels.
        /// Use this to change the resolution of the spot atlas. Note that the
        /// texture isn't immediately resized; use @ref getSpotAtlasSize() to get the
        /// actual texture size.
        glm::uvec2 configSpotAtlasSize = {4096, 4096};

        /// @brief Configured size of the point shadow atlas texture, in pixels.
        /// Use this to change the resolution of the point atlas. Note that the
        /// texture isn't immediately resized; use @ref getPointAtlasSize() to get the
        /// actual texture size.
        glm::uvec2 configPointAtlasSize = {1024, 1024};

        /// @brief Whether the shadow atlas textures have already been cleared this frame.
        bool cleared = false;

        /// @brief Stores shadow maps for each spot shadow caster component.
        core::gl::Texture2D spotAtlas{nullptr};

        /// @brief Stores shadow maps for each point shadow caster component.
        /// Each texture of the array corresponds to a face of a cubemap.
        core::gl::Texture2DArray pointAtlas{nullptr};

        /// @brief Slot for a shadow map in the shadow atlas.
        struct Slot
        {
            glm::vec2 size;   ///< Shadow map size, in normalized coordinates.
            glm::vec2 offset; ///< Shadow map offset, in normalized coordinates.
            int casterId;     ///< Id of the shadow caster (-1 if none).

            /// @brief Constructs.
            /// @param size Shadow map size, in normalized coordinates.
            /// @param offset Shadow map offset, in normalized coordinates.
            /// @param casterId Id of the shadow caster (-1 if none).
            Slot(glm::vec2 size, glm::vec2 offset, int casterId)
                : size(size)
                , offset(offset)
                , casterId(casterId){};
        };

        /// @brief Stores the sizes, offsets, and caster ids of the shadow maps
        /// in the spot atlas.
        std::vector<std::shared_ptr<Slot>> spotAtlasSlots;

        /// @brief Stores the sizes, offsets, and caster ids of the shadow maps
        /// in the point atlas.
        std::vector<std::shared_ptr<Slot>> pointAtlasSlots;

        /// @brief Maps shadow caster ids to their corresponding slots.
        std::map<int, std::shared_ptr<Slot>> slotsMap;

    private:
        /// @brief Size of the spot shadow atlas texture, in pixels.
        glm::uvec2 mSpotAtlasSize = {0, 0};

        /// @brief Size of the point shadow atlas texture, in pixels.
        glm::uvec2 mPointAtlasSize = {0, 0};
    };
} // namespace cubos::engine
