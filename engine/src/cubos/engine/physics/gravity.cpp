#include <glm/glm.hpp>

#include <cubos/core/ecs/system/query.hpp>

#include <cubos/engine/physics/plugins/gravity.hpp>

using namespace cubos::engine;

static void applyGravitySystem(Query<Write<PhysicsVelocity>, Read<PhysicsMass>> query, Read<Gravity> gravity)
{
    for (auto [entity, velocity, mass] : query)
    {
        if (mass->inverseMass <= 0.0F)
        {
            return;
        }

        // Apply gravity force
        glm::vec3 gravitationForce = mass->mass * gravity->value;

        velocity->force += gravitationForce;
    }
}

void cubos::engine::gravityPlugin(Cubos& cubos)
{
    cubos.addResource<Gravity>();

    cubos.system(applyGravitySystem).tagged("cubos.physics.apply_forces");
}
