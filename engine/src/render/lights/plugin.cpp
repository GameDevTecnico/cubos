#include <cubos/engine/render/lights/directional.hpp>
#include <cubos/engine/render/lights/environment.hpp>
#include <cubos/engine/render/lights/plugin.hpp>
#include <cubos/engine/render/lights/point.hpp>
#include <cubos/engine/render/lights/spot.hpp>

void cubos::engine::lightsPlugin(Cubos& cubos)
{
    cubos.resource<RenderEnvironment>();

    cubos.component<DirectionalLight>();
    cubos.component<PointLight>();
    cubos.component<SpotLight>();
}
