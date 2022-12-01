##Plugins {#engine-plugins}

The \ref cubos::engine::Cubos "Cubos" class is designed to be extended using
plugins. Plugins are simple functions which receive a reference to the \ref
cubos::engine::Cubos "Cubos" object and can add resources, components, systems
and even other plugins to it.

If a plugin is added twice, it will only be called once, so when you write
a plugin, its safe to add any plugins you depend on, without worrying about
the user already having added them.

Here is a list of the plugins that are included in the engine:
- \subpage engine-plugins-env-settings - adds support for loading settings from
  environment variables.
- \subpage engine-plugins-file-settings - adds support for loading settings
  from files.
- \subpage engine-plugins-assets - adds support for loading assets.
- \subpage engine-plugins-window - opens a window and provides input events.
- \subpage engine-plugins-renderer - provides 3D rendering components and
  systems.
- \subpage engine-plugins-collisions - provides 3D collision detection
  components and systems.
