#include <cubos/engine/gl/pps/pass.hpp>

cubos::engine::gl::pps::Pass::Pass(cubos::core::io::Window& window)
    : window(window), renderDevice(window.getRenderDevice())
{
}
