#include <glm/glm.hpp>

#include <cubos/engine/physics/plugins/gravity.hpp>

using namespace cubos::engine;

void cubos::engine::gravityPlugin(Cubos& cubos)
{
    cubos.addResource<Gravity>();

    cubos.system("apply gravity")
        .tagged("cubos.physics.apply_forces")
        .call([](Query<Velocity&, Force&, const Mass&> query, const Gravity& gravity) {
            for (auto [velocity, force, mass] : query)
            {
                if (mass.inverseMass <= 0.0F)
                {
                    return;
                }

                // Apply gravity force
                glm::vec3 gravitationForce = mass.mass * gravity.value;

                force.addForce(gravitationForce);
            }
        });
}
