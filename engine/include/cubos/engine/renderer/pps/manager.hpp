/// @file
/// @brief Class @ref cubos::engine::PostProcessingManager.
/// @ingroup renderer-plugin

#pragma once

#include <map>

#include <glm/glm.hpp>

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/io/window.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    class PostProcessingPass;

    /// @brief Possible renderer outputs which can then be used as input for a post processing pass.
    /// @ingroup renderer-plugin
    enum class PostProcessingInput
    {
        Lighting, ///< Renderer output with lighting applied.
        Position, ///< GBuffer texture with the world position of the pixels.
        Normal,   ///< GBuffer texture with the world normal of the pixels.
    };

    /// @brief Responsible for managing the post processing passes.
    ///
    /// This class is renderer agnostic. It can be used with any renderer implementation.
    /// Passes are executed in the order they are added, and take as input the output of the
    /// previous pass and the outputs of the renderer.
    ///
    /// @see PostProcessingPass
    /// @ingroup renderer-plugin
    class CUBOS_ENGINE_API PostProcessingManager final
    {
    public:
        /// @brief Constructs.
        /// @param renderDevice Render device to use.
        /// @param size Size of the window.
        PostProcessingManager(core::gl::RenderDevice& renderDevice, glm::uvec2 size);
        ~PostProcessingManager();

        /// @brief Called when the window framebuffer size changes.
        /// @param size New size of the window.
        void resize(glm::uvec2 size);

        /// @brief Provides a texture input to be used by the passes.
        /// @param input Input identifier.
        /// @param texture Texture to provide.
        void provideInput(PostProcessingInput input, core::gl::Texture2D texture);

        /// @brief Adds a pass to the manager.
        /// @tparam T Type of the pass to add.
        /// @return ID of the pass.
        template <typename T>
        std::size_t addPass();

        /// @brief Removes a pass from the manager.
        /// @param id ID of the pass.
        void removePass(std::size_t id);

        /// @brief Applies all post processing passes sequentially, and outputs the result to the
        /// given framebuffer.
        ///
        /// The lighting input must have been provided before calling this function, since it acts as the input for the
        /// first pass.
        ///
        /// @param out Framebuffer to output to.
        void execute(const core::gl::Framebuffer& out);

        /// @brief Gets the number of passes in the manager.
        /// @return Number of passes.
        std::size_t passCount() const;

    private:
        core::gl::RenderDevice& mRenderDevice;                      ///< Render device to use.
        glm::uvec2 mSize;                                           ///< Current size of the window.
        std::map<PostProcessingInput, core::gl::Texture2D> mInputs; ///< Inputs provided to the passes.
        std::map<std::size_t, PostProcessingPass*> mPasses;         ///< Passes present in the manager.
        std::size_t mNextId;                                        ///< Next ID to use for a pass.
        core::gl::Texture2D mIntermediateTex[2];                    ///< Intermediate textures used for the passes.
        core::gl::Framebuffer mIntermediateFb[2];                   ///< Intermediate framebuffers used for the passes.
    };

    // Implementation.

    template <typename T>
    std::size_t PostProcessingManager::addPass()
    {
        std::size_t id = mNextId++;
        mPasses[id] = new T(mRenderDevice, mSize);
        return id;
    }
} // namespace cubos::engine
