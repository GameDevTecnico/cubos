#include <cubos/engine/render/shadows/casters/caster.hpp>
#include <cubos/engine/render/shadows/casters/directional_caster.hpp>
#include <cubos/engine/render/shadows/casters/plugin.hpp>
#include <cubos/engine/render/shadows/casters/point_caster.hpp>
#include <cubos/engine/render/shadows/casters/spot_caster.hpp>

void cubos::engine::shadowCastersPlugin(Cubos& cubos)
{
    cubos.component<SpotShadowCaster>();
    cubos.component<DirectionalShadowCaster>();
    cubos.component<PointShadowCaster>();
}
