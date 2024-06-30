#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/ui/image/image.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::UIImage)
{
    return core::ecs::TypeBuilder<UIImage>("cubos::engine::UIImage").withField("asset", &UIImage::asset).build();
}
