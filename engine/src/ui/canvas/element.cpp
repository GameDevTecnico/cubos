#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/ui/canvas/draw_list.hpp>
#include <cubos/engine/ui/canvas/element.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::UIElement)
{
    return core::ecs::TypeBuilder<UIElement>("cubos::engine::UIElement")
        .withField("offset", &UIElement::offset)
        .withField("size", &UIElement::size)
        .withField("pivot", &UIElement::pivot)
        .withField("anchor", &UIElement::anchor)
        .withField("layer", &UIElement::layer)
        .build();
}

glm::vec2 cubos::engine::UIElement::bottomLeftCorner() const
{
    return glm::vec2{position.x - size.x * pivot.x, position.y - size.y * pivot.y};
}

glm::vec2 cubos::engine::UIElement::topRightCorner() const
{
    return glm::vec2{position.x + size.x * (1.0F - pivot.x), position.y + size.y * (1.0F - pivot.y)};
}

cubos::engine::UIDrawList::EntryBuilder cubos::engine::UIElement::drawUntyped(const UIDrawList::Type& type,
                                                                              const core::gl::VertexArray& vertexArray,
                                                                              size_t vertexOffset, size_t vertexCount,
                                                                              const void* data)
{
    return drawList.push(type, vertexArray, vertexOffset, vertexCount, data);
}
