#include <cubos/core/ecs/system/query.hpp>

#include <cubos/engine/physics/plugins/gravity.hpp>

#include <glm/glm.hpp>

using cubos::core::ecs::Query;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;

using namespace cubos::engine;

static void applyGravitySystem(Query<Write<PhysicsVelocity>, Read<PhysicsMass>> query, Read<Gravity> gravity)
{
    for (auto [entity, velocity, mass] : query)
    {
        if (mass->inverseMass <= 0.0f) return;

        // Apply gravity force
        glm::vec3 gravitationForce = mass->mass * gravity->value;

        velocity->force = velocity->force + gravitationForce;
    }
}

void cubos::engine::gravityPlugin(Cubos& cubos)
{
    cubos.addResource<Gravity>();

    cubos.system(applyGravitySystem).tagged("cubos.physics.apply_forces");
}
