#include <cubos/engine/render/camera/draws_to.hpp>
#include <cubos/engine/render/camera/perspective_camera.hpp>
#include <cubos/engine/render/camera/plugin.hpp>

void cubos::engine::cameraPlugin(Cubos& cubos)
{
    cubos.component<PerspectiveCamera>();

    cubos.relation<DrawsTo>();
}
