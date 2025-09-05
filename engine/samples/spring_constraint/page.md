SpringConstraint {#examples-engine-spring_constraint}

@brief Using the @ref physics-solver-plugin plugin.

@see Full source code here

This example demonstrates the behaviour of the @ref cubos::engine::SpringConstraint relation in multiple physics scenarios.

The @ref cubos::engine::SpringConstraint enforces a spring-like connection between two entities. The spring tries to maintain a specified rest length, applying forces to pull or push the entities toward each other. Damping can be applied to reduce oscillations. Each entity requires the position of the local anchor points (localAnchor1 and localAnchor2), which can be set to {0.0F, 0.0F, 0.0F} for the center of the grid.

The sample includes four scenarios:

SimpleSpring: One entity is fixed and the other is free with an initial position. The free entity oscillates around the fixed one due to the spring force.

Double Spring: Adds a second free entity connected to the first, creating a chained spring system.

PushPull: One entity collides with a constrained spring entity, demonstrating spring response.

To switch between these scenarios, press the space-bar.


