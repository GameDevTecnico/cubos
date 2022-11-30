# Dispatcher {#core-ecs-dispatcher}

The Dispatcher is used to organize the order for when and what systems are called each frame and to organize the systems in a way to know which systems do not share resources between them (don't access the same entities and/or components) so they can be called at the same time if the systems allows multithreading.

## How the Dispatcher is organized {#core-ecs-dispatcher-organization}

The dispatcher has a vector of **stages**, each stage stores all the systems that can be run parallel to each other (don't share resources). 

The stages are run on the order they are in the vector. Each stage has a vector of **systems** that are run in the order they are in the vector.

The dispathcer already contains some pre-set stages used by the engine and you can add your own stages to the dispatcher or use the existing ones.
The pre-set stages are (in order):
- **Transform** - This stage is used to update the transform of the entities.
- **Main** - This stage is pre-set for the user to use for their own systems.
- **Render** - This stage is used to render the entities.

If you want to make a more complex system order you can add your own stages in any position (before or after the pre-set stages).

## How to add systems to the dispatcher {#core-ecs-dispatcher-adding-systems}

TODO How to add systems to the dispatcher
TODO stages:
    TODO Introduction to what a stage is
    TODO Deafult stages
    TODO How to create stages
    TODO How to change the order of stages