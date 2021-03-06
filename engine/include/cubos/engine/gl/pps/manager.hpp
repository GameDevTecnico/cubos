
#ifndef CUBOS_ENGINE_GL_PPS_MANAGER_HPP
#define CUBOS_ENGINE_GL_PPS_MANAGER_HPP

#include <cubos/core/io/window.hpp>
#include <cubos/core/gl/render_device.hpp>

#include <glm/glm.hpp>
#include <map>

namespace cubos::engine::gl::pps
{
    class Pass;

    /// Possible renderer outputs which can then be used as input for a post processing pass.
    enum class Input
    {
        Lighting, ///< Renderer output with lighting applied.
        Position, ///< GBuffer texture with the world position of the pixels.
        Normal,   ///< GBuffer texture with the world normal of the pixels.
    };

    /// Class responsible for managing the post processing passes.
    /// This class is renderer agnostic, which means that it can be used with any renderer.
    ///
    /// Passes are executed in the order they are added, and take as input the output of the previous pass and the
    /// outputs of the renderer.
    /// @see Pass
    class Manager final
    {
    public:
        /// @param renderDevice The render device to use.
        /// @param size The size of the window.
        Manager(core::gl::RenderDevice& renderDevice, glm::uvec2 size);
        ~Manager();

        /// Called when the window framebuffer size changes.
        /// @param size The new size of the window.
        void resize(glm::uvec2 size);

        /// Provides a texture input to be used by the passes.
        /// @param input The input identifier.
        /// @param texture The texture to provide.
        void provideInput(Input input, core::gl::Texture2D texture);

        /// Adds a pass to the manager.
        /// @tparam T The type of the pass to add.
        /// @return The ID of the pass.
        template <typename T> size_t addPass();

        /// Removes a pass from the manager.
        /// @param id The ID of the pass.
        void removePass(size_t id);

        /// Applies all post processing passes sequentially, and outputs the result to the screen.
        /// The lighting input must have been provided before calling this function, since it acts as the input for the
        /// first pass.
        void execute(core::gl::Framebuffer out);

        /// Gets the number of passes in the manager.
        /// @return The number of passes.
        size_t passCount() const;

    private:
        core::gl::RenderDevice& renderDevice;        ///< The render device to use.
        glm::uvec2 size;                             ///< The current size of the window.
        std::map<Input, core::gl::Texture2D> inputs; ///< The inputs provided to the passes.
        std::map<size_t, Pass*> passes;              ///< The passes present in the manager.
        size_t nextId;                               ///< The next ID to use for a pass.
        core::gl::Texture2D intermediateTex[2];      ///< Intermediate textures used for the passes.
        core::gl::Framebuffer intermediateFb[2];     ///< Intermediate framebuffers used for the passes.
    };

    // Implementation.

    template <typename T> size_t Manager::addPass()
    {
        size_t id = this->nextId++;
        this->passes[id] = new T(this->renderDevice, this->size);
        return id;
    }
} // namespace cubos::engine::gl::pps

#endif // CUBOS_ENGINE_GL_PPS_COPY_PASS_HPP
