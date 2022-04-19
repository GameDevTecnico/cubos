#include <cubos/engine/gl/pps/pass.hpp>

cubos::engine::gl::pps::PostProcessingPass::PostProcessingPass(cubos::core::io::Window& window)
    : window(window), renderDevice(window.getRenderDevice())
{
}
