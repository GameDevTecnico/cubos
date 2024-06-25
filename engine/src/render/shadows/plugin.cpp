#include <cubos/engine/render/shadows/caster.hpp>
#include <cubos/engine/render/shadows/directional_caster.hpp>
#include <cubos/engine/render/shadows/plugin.hpp>
#include <cubos/engine/render/shadows/point_caster.hpp>
#include <cubos/engine/render/shadows/spot_caster.hpp>

void cubos::engine::shadowsPlugin(Cubos& cubos)
{
    cubos.component<SpotShadowCaster>();
    cubos.component<DirectionalShadowCaster>();
    cubos.component<PointShadowCaster>();
}
