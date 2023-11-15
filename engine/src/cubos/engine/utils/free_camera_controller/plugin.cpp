#include <cubos/engine/utils/free_camera_controller/free_camera_controller.hpp>
#include <cubos/engine/utils/free_camera_controller/plugin.hpp>

using namespace cubos::engine;

using cubos::core::ecs::Query;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;

static void updateFreeCameraController(Query<Read<FreeCameraController>, Write<Position>> entities)
{
    for(auto [entity, controller, position] : entities)
    {
        position->vec.x += controller->horizontalXAxis * controller->speed;
        position->vec.y += controller->horizontalYAxis * controller->speed;
        position->vec.z += controller->verticalAxis * controller->speed;
    }
}

void updateVertical(float vertical, cubos::core::ecs::Write<cubos::engine::FreeCameraController> controller)
{
    controller->verticalAxis = vertical;
}

void updateHorizontalX(float horizontalX, cubos::core::ecs::Write<cubos::engine::FreeCameraController> controller)
{
    controller->horizontalXAxis = horizontalX;
}

void updateHorizontalY(float horizontalY, cubos::core::ecs::Write<cubos::engine::FreeCameraController> controller)
{
    controller->horizontalYAxis = horizontalY;
}

void cubos::engine::freeCameraControllerPlugin(Cubos& cubos)
{
    cubos.addComponent<FreeCameraController>();

    cubos.system(updateFreeCameraController);
}