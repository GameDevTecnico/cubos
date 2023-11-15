#include <cubos/engine/utils/free_camera_controller/free_camera_controller.hpp>
#include <cubos/core/ecs/component/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::FreeCameraController)
{
    return core::ecs::ComponentTypeBuilder<FreeCameraController>("cubos::engine::FreeCameraController")
        .withField("speed", &FreeCameraController::speed)
        .withField("verticalAxis", &FreeCameraController::verticalAxis)
        .withField("horizontalXAxis", &FreeCameraController::horizontalXAxis)
        .withField("horizontalYAxis", &FreeCameraController::horizontalYAxis)
        .build();
}