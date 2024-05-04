#include <glm/glm.hpp>

#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/physics/plugins/gravity.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(Gravity)
{
    return core::ecs::TypeBuilder<Gravity>("cubos::engine::Gravity").build();
}

void cubos::engine::gravityPlugin(Cubos& cubos)
{
    cubos.depends(physicsPlugin);

    cubos.resource<Gravity>();

    cubos.system("apply gravity")
        .tagged(physicsApplyForcesTag)
        .call([](Query<Velocity&, Force&, const Mass&> query, const Gravity& gravity) {
            for (auto [velocity, force, mass] : query)
            {
                if (mass.inverseMass <= 0.0F)
                {
                    continue;
                }

                // Apply gravity force
                glm::vec3 gravitationForce = mass.mass * gravity.value;

                force.add(gravitationForce);
            }
        });
}
