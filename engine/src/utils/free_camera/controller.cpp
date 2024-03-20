#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

#include <cubos/engine/utils/free_camera/controller.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::FreeCameraController)
{
    return core::ecs::TypeBuilder<FreeCameraController>("cubos::engine::FreeCameraController")
        .withField("enabled", &FreeCameraController::enabled)
        .withField("unscaledDeltaTime", &FreeCameraController::unscaledDeltaTime)
        .withField("vertical", &FreeCameraController::vertical)
        .withField("lateral", &FreeCameraController::lateral)
        .withField("longitudinal", &FreeCameraController::longitudinal)
        .withField("speed", &FreeCameraController::speed)
        .withField("sens", &FreeCameraController::sens)
        .withField("phi", &FreeCameraController::phi)
        .withField("theta", &FreeCameraController::theta)
        .build();
}