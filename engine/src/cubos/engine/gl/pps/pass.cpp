#include <cubos/engine/gl/pps/pass.hpp>

cubos::engine::gl::PostProcessingPass::PostProcessingPass(cubos::core::io::Window& window)
    : window(window), renderDevice(window.getRenderDevice())
{
}
