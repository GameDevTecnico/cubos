#include <cubos/engine/rendering/post_processing/post_processing_pass.hpp>

cubos::engine::rendering::PostProcessingPass::PostProcessingPass(cubos::core::io::Window& window)
    : window(window), renderDevice(window.getRenderDevice())
{
}
