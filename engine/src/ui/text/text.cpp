#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/traits/enum.hpp>

#include <cubos/engine/ui/text/text.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(UIText)
{
    return core::ecs::TypeBuilder<UIText>("cubos::engine::UIText")
        .withField("text", &UIText::text)
        .withField("color", &UIText::color)
        .withField("fontSize", &UIText::fontSize)
        .withField("fontAtlas", &UIText::fontAtlas)
        .build();
}
