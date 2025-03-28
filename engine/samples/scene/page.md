# Scene {#examples-engine-scene}

@brief Using the @ref scene-plugin plugin.

@see Full source code [here](https://github.com/GameDevTecnico/cubos/tree/main/engine/samples/scene).

This example shows how the @ref scene-plugin plugin can be used to create
scene assets and spawn them on the world.

The plugin function is included from the @ref engine/scene/plugin.hpp header.

@snippet scene/main.cpp Adding the plugin

Let's start by taking a look at a scene file.
@include scene/assets/sub.cubos

Scene files are JSON files with the extension `.cubos`. A scene file describes an entity, and, optionally, its components, relations, children, grandchildren, and so on.

Components can be added to an entity by simply adding a field in their JSON object. For example, in this scene, the root entity has a single component, `Num`, with a value of 3. Additionally, it has a child entity, with name `#child` - sub-entity names must always start with `#`. 

This child entity has its own component `Num`, this time with a value of `4`, and a relation `OwnedBy` with the root entity of the scene as a target. Relations are identified by their type name, followed by `#` and the name of the entity they target. If the name is empty, then, they target the root.

@note Make sure these component and relation names match the ones in your application and that they have been registered with @ref cubos::engine::Cubos "Cubos".

Let's look at a different scene file now, this time with *inheritance*. Inheritance allows us to instantiate scenes within other scenes.

@include scene/assets/main.cubos

In this scene, as before, we have a root entity, this time with `Num` set to `0`. This entity has four child entities, `#foo`, `#bar`, `#sub1` and `#sub2`, similarly to the previous scene. The big difference here is that we use inheritance in the `#sub1` and `#sub2` entities, by adding the `inherit` field.

When the scene is loaded, all properties defined in the inherited scenes are added to their respective entities. In this case, both entities refer to the scene with id `cd007ba2-ee0d-44fd-bf36-85c829dbe66f`, which is the scene we've previously looked that.

The entities in that scene are instantiated in this new scene, with their names prepended by `#sub1` and `#sub2` respectively. So, the `#sub1` and `#sub2` entities both represent the roots of the sub-scene we imported, and `#sub1#child` and `#sub2#child` to the `#child` entity we looked at before.

Also take a look at the `DistanceTo` relation: it is a symmetric relation, so it doesn't make a different whether we put it in `#sub1` or `#sub2`.

Now that we have our scene file, let's get our application to load it.
The first thing we're going to need is a reference to the scene asset. 
For the purposes of this sample we can simply use an hardcoded reference to the asset.

@snippet scene/main.cpp Setting the asset

Then we'll need a system that spawns that scene.
To do this we simply get the Scene object from the asset, and then spawn its entities.
@ref cubos::core::ecs::Commands::spawn "Commands::spawn" will create in the world a copy of every entity defined in the scene's blueprint.
It won't remove the entities already there, so if you want to close a scene, you'll have to do it yourself.

We use the `named` method to give the name `"scene"` to the root entity of the scene. This will cause all child entities to also have their name prepended by this name. 

@snippet scene/main.cpp Spawning the scene

In this case, we'll run this system at startup, since we want to spawn it a single time.
Since it's a startup system, we'll have to tag it with `cubos.assets` to make sure it runs only after the scene bridge has been registered.
On a real game, you could have, for example, a scene for an enemy which you spawn multiple times, instead of just once at startup.

@snippet scene/main.cpp Printing the scene

This sample also contains a system which prints the components and relations of the spawned entities.
If you run it, it should give you the same information we defined in the files.
