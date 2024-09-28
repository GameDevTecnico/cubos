#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/traits/enum.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/render/camera/orthographic.hpp>

using cubos::core::reflection::EnumTrait;
using cubos::core::reflection::Type;
using cubos::engine::OrthographicCamera;

CUBOS_REFLECT_EXTERNAL_IMPL(OrthographicCamera::Axis)
{
    return Type::create("cubos::engine::CameraAxis")
        .with(EnumTrait{}
                  .withVariant<OrthographicCamera::Axis::Vertical>("Vertical")
                  .withVariant<OrthographicCamera::Axis::Horizontal>("Horizontal"));
}

CUBOS_REFLECT_IMPL(OrthographicCamera)
{
    return core::ecs::TypeBuilder<OrthographicCamera>("cubos::engine::OrthographicCamera")
        .withField("size", &OrthographicCamera::size)
        .withField("axis", &OrthographicCamera::axis)
        .withField("zNear", &OrthographicCamera::zNear)
        .withField("zFar", &OrthographicCamera::zFar)
        .build();
}
