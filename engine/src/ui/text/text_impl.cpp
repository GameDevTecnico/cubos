#include "text_impl.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(UITextImpl)
{
    return cubos::core::ecs::TypeBuilder<UITextImpl>("cubos::engine::UITextImpl")
        .withField("vertexCount", &UITextImpl::vertexCount)
        .build();
}
