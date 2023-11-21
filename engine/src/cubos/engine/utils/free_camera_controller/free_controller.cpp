#include <cubos/engine/utils/free_camera_controller/free_controller.hpp>
#include <cubos/core/ecs/component/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::FreeController)
{
    return core::ecs::ComponentTypeBuilder<FreeController>("cubos::engine::FreeController")
        .withField("speed", &FreeController::speed)
        .withField("sens", &FreeController::sens)
        .withField("verticalAxis", &FreeController::verticalAxis)
        .withField("horizontalXAxis", &FreeController::horizontalXAxis)
        .withField("horizontalZAxis", &FreeController::horizontalZAxis)
        .withField("phi", &FreeController::phi)
        .withField("theta", &FreeController::theta)
        .build();
}