#include <cubos/core/ecs/component/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

#include <cubos/engine/utils/free_camera_controller/controller.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::FreeCameraController)
{
    return core::ecs::ComponentTypeBuilder<FreeCameraController>("cubos::engine::FreeCameraController")
        .withField("speed", &FreeCameraController::speed)
        .withField("sens", &FreeCameraController::sens)
        .withField("phi", &FreeCameraController::phi)
        .withField("theta", &FreeCameraController::theta)
        .build();
}