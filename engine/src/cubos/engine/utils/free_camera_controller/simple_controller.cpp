#include <cubos/core/ecs/component/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

#include <cubos/engine/utils/free_camera_controller/simple_controller.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::SimpleController)
{
    return core::ecs::ComponentTypeBuilder<SimpleController>("cubos::engine::SimpleController")
        .withField("speed", &SimpleController::speed)
        .withField("verticalAxis", &SimpleController::verticalAxis)
        .withField("horizontalXAxis", &SimpleController::horizontalXAxis)
        .withField("horizontalZAxis", &SimpleController::horizontalZAxis)
        .build();
}