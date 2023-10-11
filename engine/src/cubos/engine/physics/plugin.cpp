#include <cubos/core/ecs/system/query.hpp>

#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

#include <glm/glm.hpp>

using cubos::core::ecs::Query;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;

using namespace cubos::engine;

// Semi-implicit euler integration
static void integrate(Position& position, PhysicsVelocity& physicsVelocity, const PhysicsMass& physicsMass, float duration)
{
    // We don’t integrate things with infinite mass.
    if (physicsMass.inverseMass <= 0.0f) return;

    // Work out the acceleration from the force.
    glm::vec3 resultingAcceleration = physicsVelocity.acceleration;
    resultingAcceleration += physicsVelocity.totalForce * physicsMass.inverseMass;

    // Update linear velocity from the acceleration.
    physicsVelocity.velocity += resultingAcceleration * duration;

    // Impose drag.
    physicsVelocity.velocity *= glm::pow(physicsVelocity.damping, duration);

    // Update linear position.
    position.vec += physicsVelocity.velocity * duration;

    // Clear the forces.
    physicsVelocity.totalForce = glm::vec3{0.0F, 0.0F, 0.0F};
    physicsVelocity.added = false;
}

static void integrateSystem(Query<Write<Position>, Write<PhysicsVelocity>, Read<PhysicsMass>> query, Write<Accumulator> accumulator, Read<FixedDeltaTime> fixedDeltaTime, Read<DeltaTime> deltaTime)
{
    accumulator->value += deltaTime->value;

    while (accumulator->value >= fixedDeltaTime->value)
    {
        for (auto [entity, position, velocity, mass] : query)
        {
            integrate(*position, *velocity, *mass, fixedDeltaTime->value);
        }

        accumulator->value -= fixedDeltaTime->value;
    }
}

void cubos::engine::physicsPlugin(Cubos& cubos)
{
    cubos.addResource<FixedDeltaTime>();
    cubos.addResource<Accumulator>();

    cubos.addComponent<PhysicsVelocity>();
    cubos.addComponent<PhysicsMass>();

    cubos.system(integrateSystem);
}
