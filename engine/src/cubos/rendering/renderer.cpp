#include <cubos/rendering/renderer.hpp>
#include "cubos/io/window.hpp"

using namespace cubos::gl;
using namespace cubos::rendering;

Renderer::Renderer(io::Window& window) : window(window), renderDevice(window.getRenderDevice())
{
}
