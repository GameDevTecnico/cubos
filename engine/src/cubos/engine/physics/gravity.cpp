#include <glm/glm.hpp>

#include <cubos/core/ecs/system/query.hpp>

#include <cubos/engine/physics/plugins/gravity.hpp>

using namespace cubos::engine;

static void applyGravitySystem(Query<PhysicsVelocity&, const PhysicsMass&> query, const Gravity& gravity)
{
    for (auto [velocity, mass] : query)
    {
        if (mass.inverseMass <= 0.0F)
        {
            return;
        }

        // Apply gravity force
        glm::vec3 gravitationForce = mass.mass * gravity.value;

        velocity.force += gravitationForce;
    }
}

void cubos::engine::gravityPlugin(Cubos& cubos)
{
    cubos.addResource<Gravity>();

    cubos.system(applyGravitySystem).tagged("cubos.physics.apply_forces");
}