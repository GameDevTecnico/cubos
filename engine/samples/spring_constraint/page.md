# DistanceConstraint {#examples-engine-distance_constraint}

@brief Using the @ref physics-solver-plugin plugin.

@see Full source code [here](https://github.com/GameDevTecnico/cubos/tree/main/engine/samples/distance_constraint).

This example shows the behaviour of the @ref cubos::engine::DistanceConstraint relation in multiple physics scenarios.

The @ref cubos::engine::DistanceConstraint enforces a distance between two entities, whether fixed or a in range. To specify the fixed type add the relation with the isRigid
attribute set to true and specify a fixedDistance value. For the range type set the same attribute to false and specify a minDistance and maxDistance instead.
The relation also requires the position of the local anchor points in each entity (localAnchor1 and localAnchor2), set these to {0.0F, 0.0F, 0.0F} for the center of the grid.

The sample includes four scenarios: A simple Circle scenario where one entity is fixed and another has an initial velocity but is constrained to first one, resulting in an "orbiting" motion. A pendulum scenerio with the same setup as the first but the second entity has a simulated gravity instead of a constant velocity. A double pendulum scenario which adds another entity constrained to the original pendulum. And a PushPull scenario where one constrained entity is pushed by a "free" entity until it hits minimum distance with the fixed entity, being repelled outwards and pushing the "free" entity back.

To switch between these scenarios press the space-bar.

