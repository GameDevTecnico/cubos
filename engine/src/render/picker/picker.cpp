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

    const auto* data = this->pixelBuffer->map();
    auto r = static_cast<const uint16_t*>(data)[(y * this->size.x + x) * 2U + 0U];
    auto g = static_cast<const uint16_t*>(data)[(y * this->size.x + x) * 2U + 1U];
    auto id = (static_cast<uint32_t>(r) << 16U) | static_cast<uint32_t>(g);
    this->pixelBuffer->unmap();
    return id;
}
