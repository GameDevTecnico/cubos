#ifndef CUBOS_ENGINE_GL_PPS_PASS_HPP
#define CUBOS_ENGINE_GL_PPS_PASS_HPP

#include <cubos/core/gl/render_device.hpp>

#include <cubos/engine/gl/pps/manager.hpp>

#include <glm/glm.hpp>
#include <map>

namespace cubos::engine::gl::pps
{
    /// A generic post processing pass.
    /// All passes must have the same constructor arguments: the render device and the size of the window.
    /// They should never be constructed directly, and instead should be created using the Manager.
    ///
    /// @see Manager
    class Pass
    {
    public:
        /// @param renderDevice The render device to use.
        Pass(core::gl::RenderDevice& renderDevice);
        Pass(const Pass&) = delete;
        virtual ~Pass() = default;

        /// Called when the window framebuffer size changes.
        /// @param size The new size of the window.
        virtual void resize(glm::uvec2 size) = 0;

        /// Called each frame.
        /// @param inputs The available extra input textures. The reference is mutable so that a pass can provide its
        /// own inputs.
        /// @param prev The resulting texture of the previous pass.
        /// @param out The framebuffer where the pass will render to.
        virtual void execute(std::map<Input, core::gl::Texture2D>& inputs, core::gl::Texture2D prev,
                             core::gl::Framebuffer out) const = 0;

    protected:
        core::gl::RenderDevice& renderDevice;
    };
} // namespace cubos::engine::gl::pps

#endif // CUBOS_ENGINE_GL_PPS_PASS_HPP
