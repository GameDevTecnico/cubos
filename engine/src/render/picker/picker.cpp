#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/render/picker/picker.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::RenderPicker)
{
    return core::ecs::TypeBuilder<RenderPicker>("cubos::engine::RenderPicker")
        .withField("size", &RenderPicker::size)
        .withField("cleared", &RenderPicker::cleared)
        .build();
}

uint32_t cubos::engine::RenderPicker::read(unsigned int x, unsigned int y) const
{
    if (x >= this->size.x || y >= this->size.y)
    {
        return UINT32_MAX; // Out of bounds.
    }

    y = size.y - y - 1;

    // The pixel pack buffer holds data in the RGBA32UInt format.
    glm::uvec4 pixel;
    this->pixelBuffer->copyTo(&pixel, (y * this->size.x + x) * 16U, 16U);
    return (pixel.r << 16U) | pixel.g;
}
