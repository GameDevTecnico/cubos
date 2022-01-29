#include <cubos/rendering/renderer.hpp>

using namespace cubos::gl;
using namespace cubos::rendering;

Renderer::Renderer(io::Window& window) : window(window), renderDevice(window.getRenderDevice())
{
}

void Renderer::flush()
{
    drawRequests.clear();
}