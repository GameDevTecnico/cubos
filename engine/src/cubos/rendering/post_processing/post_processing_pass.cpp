#include "cubos/rendering/post_processing/post_processing_pass.hpp"

cubos::rendering::PostProcessingPass::PostProcessingPass(cubos::io::Window& window)
    : window(window), renderDevice(window.getRenderDevice())
{
}
