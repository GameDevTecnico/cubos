# Scene {#examples-engine-scene}

@brief Using the @ref scene-plugin plugin.

This example shows how the @ref scene-plugin plugin can be used to create
scene assets and spawn them on the world.

The plugin function is included from the @ref engine/scene/plugin.hpp header.

@snippet scene/main.cpp Adding the plugin

Let's start by taking a look at a scene file.
@include assets/sub.cubos

Scene files are JSON files with the extension `.cubos`. They must have two fields: `imports` and `entities`.
The `entities` field is an object where each field identifies and describes the components of an entity.
In this scene we have two entities, `root` and `child`.
`root` has a single component, `num`, with a value of 1.
`child` has two components, a `parent` and a `num`.
In this sample, `num` is used so we can later identify the entities.

@note Make sure these component names match the ones in your application and that they have been registered with @ref cubos::engine::Cubos "Cubos".

Let's look at a different scene file now, this time with `imports`. Imports allows us to instantiate scenes within other scenes.

@include assets/main.cubos

This file imports the asset with id `cd007ba2-ee0d-44fd-bf36-85c829dbe66f`, which is the scene we looked at in the previous file, under the name `sub1`.
It then imports the very same scene again, but this time with the name `sub2` instead.
This effectively instantiates the entities of the previous scene twice in this new scene, each with their names prefixed with either `sub1.` or `sub2.`

Under `entities`, we can override the entities in the sub-scenes to edit components or add new ones.
For example, by referencing `sub1.root` we are making local changes to the `root` entity of that instance of the subscene.
The result of the changes we make to both `sub1.root` and `sub2.root` is that the parent of these entities will be set to be the `main` entity.

Now that we have our scene file, let's get our application to load it.
The first thing we're going to need is a reference to the scene asset. 
For the purposes of this sample we can simply use an hardcoded reference to the asset.

@snippet scene/main.cpp Setting the asset

Then we'll need a system that spawns that scene.
To do this we simply get the Scene object from the asset, and then spawn its entities.
@ref cubos::core::ecs::Commands::spawn "Commands::spawn" will create in the world a copy of every entity defined in the scene's blueprint.
It won't remove the entities already there, so if you want to close a scene, you'll have to do it yourself.

@snippet scene/main.cpp Spawning the scene

In this case, we'll run this system at startup, since we want to spawn it a single time.
Since it's a startup system, we'll have to tag it with `cubos.assets` to make sure it runs only after the scene bridge has been registered.
On a real game, you could have, for example, a scene for an enemy which you spawn multiple times, instead of just once at startup.

@snippet scene/main.cpp Adding the system

This sample will output the list of every entity in the scene, so you can check that everything is working as expected.
If you run it, it should give you a list that has:
- an entity with `num` set to 0, with no parent. This is the `main` entity.
- two entities with `num` set to 1, with same parent, who has `num` set to 0. These are the `root` entities of each instance of the subscene.
- two entities with `num` set to 2, with different parents, but both of them having `num` set to 1.  These are the `child` entities of each instance of the subscene.
