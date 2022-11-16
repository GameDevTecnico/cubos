# Entity Component System {#core-ecs}

[Wikipedia](https://en.wikipedia.org/wiki/Entity_component_system) defines *ECS* as:

> Entity Component System (ECS) is a software architectural pattern mostly used in video game development for the representation of game world objects. An ECS comprises entities composed from components of data, with systems which operate on entities' components.
> ECS follows the principle of composition over inheritance, meaning that every entity is defined not by a type hierarchy, but by the components that are associated with it. Systems act globally over all entities which have the required components. 

The *ECS* thus is a vital part of the engine, since all of the rest is structured around it in some way. This sets the engine apart from other engines which follow a more traditional model, like *Unity*, *Unreal* and *Godot*.

CUBOS. ECS has six main parts:
- \subpage core-ecs-world - the main object that holds all of the *ECS* state. (For example a Racetrack could be a world that would hold the track, the terrain, all the car, etc...)
- \subpage core-ecs-entities - sets of components that populate the game world (For example a Car is an entity made up of components such as Velocity, Position, etc...)
- \subpage core-ecs-components -  structures of data that make up the entities (For example one component of the Car is the Velocity which stores the information of how fast the Car is going)
- \subpage core-ecs-resources - singleton-like objects which are stored per world and which do not belong to a specific entity (For example the DeltaTime is a resource that is used by the systems to calculate the time between frames)
- \subpage core-ecs-systems - logic that operates on the components data and resources, let it be creating, destroying or changing (For example we can have a system that is chaging the velocity of the car, increasing it if the player presses the accelarator or decreasing it if the player presses the breaks).
- \subpage core-ecs-dispatcher - decides when each system is called and knows which systems are independent from each other so they can be called at the same time (parallel computing) (For example in the race game we might have 2 systems, one that changes the velocity and one that moves the car's position, it makes sense for the dispatcher to first update the velocity and then update the car's position)

One important thing to note is that in an *ECS* the data is completely decoupled from the logic. What this means is that entities and components do not and should not possess any functionality other than storing data. All of the logic is relegated to the systems. 