#include <cubos/core/ecs/system/query.hpp>

#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

#include <glm/glm.hpp>

using cubos::core::ecs::Query;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;

using namespace cubos::engine;

/*
// Semi-implicit euler integration
static void integrate(Position& position, PhysicsVelocity& physicsVelocity, const PhysicsMass& physicsMass, float damping, float duration)
{
    // We don’t integrate things with infinite mass.
    if (physicsMass.inverseMass <= 0.0f) return;

    // Work out the acceleration from the force.
    glm::vec3 resultingAcceleration = physicsVelocity.gravity;
    resultingAcceleration += physicsVelocity.totalForce * physicsMass.inverseMass;

    // Update linear velocity from the acceleration.
    physicsVelocity.velocity += resultingAcceleration * duration;

    // Impose drag.
    physicsVelocity.velocity *= glm::pow(damping, duration);

    // Update linear position.
    position.vec += physicsVelocity.velocity * duration;

    // Clear the forces.
    physicsVelocity.totalForce = glm::vec3{0.0F, 0.0F, 0.0F};
    physicsVelocity.added = false;
}

static void integrateSystem(Query<Write<Position>, Write<PhysicsVelocity>, Read<PhysicsMass>> query, Write<Accumulator> accumulator, Read<Damping> damping, Read<FixedDeltaTime> fixedDeltaTime, Read<DeltaTime> deltaTime)
{
    accumulator->value += deltaTime->value;

    while (accumulator->value >= fixedDeltaTime->value)
    {
        for (auto [entity, position, velocity, mass] : query)
        {
            integrate(*position, *velocity, *mass, damping->value, fixedDeltaTime->value);
        }

        accumulator->value -= fixedDeltaTime->value;
    }
}
*/

static void increaseAccumulator(Write<Accumulator> accumulator, Read<DeltaTime> deltaTime)
{
    accumulator->value += deltaTime->value;
}

static void decreaseAccumulator(Write<Accumulator> accumulator, Read<FixedDeltaTime> fixedDeltaTime)
{
    accumulator->value -= fixedDeltaTime->value;
}

static bool simulatePhysicsStep(Write<Accumulator> accumulator, Read<FixedDeltaTime> fixedDeltaTime)
{
    if (accumulator->value >= fixedDeltaTime->value)
    {
        return true;
    }
    return false;
}

static void integratePositionSystem(Query<Write<Position>, Write<PreviousPosition>, Write<PhysicsVelocity>, Read<PhysicsMass>> query, Read<Damping> damping, Read<Gravity> gravity, Read<FixedDeltaTime> fixedDeltaTime, Read<Substeps> substeps)
{
    // Possible components
    // prev position
    // AccumulatedTranslation
    float subDeltaTime = fixedDeltaTime->value/(float)substeps->value;

    for (auto [entity, position, prevPosition, velocity, mass] : query)
    {
        prevPosition->vec = position->vec;

        if (mass->inverseMass <= 0.0f) return;

        // Apply damping, gravity and other external forces
        velocity->velocity *= glm::pow(damping->value, subDeltaTime);

        // Apply forces
        glm::vec3 gravitationForce =
            mass->mass * gravity->value;
        glm::vec3 totalForce = gravitationForce + velocity->force;
        glm::vec3 deltaLinearVelocity = subDeltaTime * totalForce * mass->inverseMass;

        velocity->velocity += deltaLinearVelocity;
        position->vec += subDeltaTime * deltaLinearVelocity;
        //translation->vec = fixedDeltaTime->subDeltaTime * delta_lin_vel;
    }
}

static void applyCorrectionSystem(Query<Write<Position>, Write<AccumulatedCorrection>> query)
{
    for (auto [entity, position, correction] : query)
    {
        position->vec += correction->vec;
        correction->vec = glm::vec3(0,0,0);
    }
}

static void updateVelocitySystem(Query<Read<Position>, Read<PreviousPosition>, Write<PhysicsVelocity>> query, Read<FixedDeltaTime> fixedDeltaTime, Read<Substeps> substeps)
{
    float subDeltaTime = fixedDeltaTime->value/(float)substeps->value;

    for (auto [entity, position, prevPosition, velocity] : query)
    {
        velocity->velocity = (position->vec - prevPosition->vec) / subDeltaTime;
    }
}

void cubos::engine::physicsPlugin(Cubos& cubos)
{
    //int substeps = 5;

    cubos.addResource<FixedDeltaTime>();
    cubos.addResource<Substeps>();
    cubos.addResource<Accumulator>();
    cubos.addResource<Damping>();
    cubos.addResource<Gravity>();

    cubos.addComponent<PhysicsVelocity>();
    cubos.addComponent<PhysicsMass>();
    cubos.addComponent<AccumulatedCorrection>();
    cubos.addComponent<PreviousPosition>();

    // startup system to start subDeltaTime ?

    // executed every frame
    cubos.system(increaseAccumulator).tagged("cubos.physics.prepare");

    // tag that encapsulates a simulation step
    //cubos.tag("cubos.physics.simulation").after("cubos.physics.prepare");
    //cubos.tag("cubos.physics.simulation").runIf(simulatePhysicsStep); //.repeat();
    cubos.system(integratePositionSystem).tagged("cubos.physics.simulation.substeps.integrate").after("cubos.physics.prepare").runIf(simulatePhysicsStep);
    cubos.system(applyCorrectionSystem).tagged("cubos.physics.simulation.substeps.correct_position").after("cubos.physics.simulation.substeps.integrate").runIf(simulatePhysicsStep);
    cubos.system(updateVelocitySystem).tagged("cubos.physics.simulation.substeps.update_velocity").after("cubos.physics.simulation.substeps.correct_position").runIf(simulatePhysicsStep);
    cubos.system(decreaseAccumulator).tagged("cubos.physics.simulation.decrease_accumulator").after("cubos.physics.simulation.substeps.update_velocity").runIf(simulatePhysicsStep); // at the end of the step

    // tag for when user systems should apply force?
    
    // for each simulation step, we execute multiple substeps
    //cubos.tag("cubos.physics.simulation.substeps").during("cubos.physics.simulation");
    //cubos.tag("cubos.physics.simulation.substeps").repeat(substeps);
    
    // integrate position for a substep
    //cubos.tag("cubos.physics.simulation.substeps.integrate").during("cubos.physics.simulation.substeps");
    //cubos.system(integratePositionSystem).tagged("cubos.physics.simulation.substeps.integrate");
    //cubos.system(applyCorrectionSystem).tagged("cubos.physics.simulation.substeps.correct_position");
    //cubos.system(updateVelocitySystem).tagged("cubos.physics.simulation.substeps.update_velociy");

    // solve all constraints for a substep
    //cubos.tag("cubos.physics.simulation.substeps.constraints").during("cubos.physics.simulation.substeps");
    //cubos.tag("cubos.physics.simulation.substeps.constraints").after("cubos.physics.simulation.substeps.integrate");
    //cubos.system(distanceConstrainSolverSystem).tagged("cubos.physics.simulation.substeps.constraints");
    //cubos.system(springConstriantSolver).tagged("cubos.physics.simulation.substeps.constraints");

    //...

    // after executing the simulation, check for what bodies we can deactivate physics calculation
    //cubos.tag("cubos.physics.simulation.sleeping").during("cubos.physics.simulation");
    //cubos.tag("cubos.physics.simulation.sleeping").after("cubos.physics.simulation.substeps");
    //cubos.system(sleepingSystem).tagged("cubos.physics.simulation.sleeping");
    

    
    
    
    //cubos.system(integrateSystem);
}
