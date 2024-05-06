# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- Log the system order on the dispatcher (#414, **@RiscadoA**).
- Negative tags to the dispatcher (#390, **@RiscadoA**).
- Support for queries with multiple relation terms (#929, **@RiscadoA**).
- Support for queries with more than one unrelated targets (#930, **@RiscadoA**).
- Allow iterating over entity relations directly through the World (#1006, **@RiscadoA**).
- Observers, which allow reacting to component addition and removal (#724, **@RiscadoA**).
- Fixed Time Step plugin (#989, **@joaomanita**).
- New feature guide focused on queries (#995, **@RiscadoA**).
- ECS Statistics tesseratos plugin (#1024, **@RiscadoA**).
- Global position, rotation and scale getters (#1002, **@DiogoMendonc-a**).
- Show childOf hierarchy on the world inspector (#991, **@diogomsmiranda**).
- Rotation to the Transform Gizmo (#878, **@DiogoMendonc-a**).
- Rotated box Gizmo (#878, **@DiogoMendonc-a**).
- Shader asset and bridge (#1058, **@tomas7770**).
- GBuffer plugin (#1061, **@RiscadoA**).
- Defaults plugin (#229, **@RiscadoA**).
- Support for untyped resources (#1057, **@RiscadoA**).
- Addition and removal of resources through Commands (#325, **@RiscadoA**).
- Access to unscaled delta time for utilities like the debug camera (#1020, **@RiscadoA**).
- RenderTarget plugin (#1059, **@tomas7770**).
- RenderPicker plugin (#1060, **@tomas7770**).
- ToneMapping plugin (#1087, **@RiscadoA**).
- RenderDepth plugin (#1082, **@RiscadoA**).
- Task class, for use in asynchronous code (#1111, **@RiscadoA**).
- HDR plugin (#1085, **@RiscadoA**).
- Camera plugin (#1063, **@tomas7770**).
- Lights plugin (#1135, **@RiscadoA**).
- Search bar to World Inspector which allows filtering by components (#1105, **@diogomsmiranda**).
- Allow filtering by entity names in the World Inspector (#1106, **@diogomsmiranda**).
- Possibility of building the core library as a shared library (#1052, **@RiscadoA**).
- Possibility of building the engine library as a shared library (#1053, **@RiscadoA**).
- Plugins system argument, which can be used to dynamically add and remove plugins (#1034, **@RiscadoA**).
- Show relations on the Entity Inspector (#907, **@RiscadoA**).
- Allow adding and removing relations through the Entity Inspector (#1022, **@RiscadoA**).
- Optional synchronous loading to asset bridges (**@RiscadoA**).
- Render Voxels plugin (#1119, **@RiscadoA**).
- Render Mesh plugin (#1110, **@RiscadoA**).
- Optional synchronous loading to asset bridges (**@RiscadoA**).
- Allow toggling vSync through setting (**@RiscadoA**).
- GBuffer Rasterizer plugin (#1062, **@RiscadoA**).
- Deferred Shading plugin (#1086, **@RiscadoA**).
- SSAO plugin (#1088, **@RiscadoA**).
- Split screen for the new renderer (#1149, **@tomas7770**).
- Trait for representing inheritance relationships under reflection. (#693, **@roby2014**).
- Bloom plugin (#1089, **@RiscadoA**).

### Changed

- Make reflection work for all types, even those without reflection implemented (#1092, **@RiscadoA**).
- Move away from string tags to typed tags (#918, **@joaomanita**).
- Force plugins to be added only once (#422, **@RiscadoA**).
- Return direct references to resources instead of guards on World (#922, **@RiscadoA**).
- Stop using old serialization to load Settings (#1100, **@RiscadoA**).
- Make transform gizmo screen size independent from entity's distance to camera (#1017, **@DiogoMendonc-a**)
- Palette & Grid saving/loading now use streams instead of serialization (#572, **@Dacops**).
- Split Dispatcher into Planner and Schedule (#1117, **@RiscadoA**).
- Use Pixel Pack Buffers to speed up entity picking (**@RiscadoA**).

### Fixed

- Crash in multiple samples due to missing plugin dependencies (**@RiscadoA**).
- Cursor offset in tesseratos when on retina displays (#998, **@diogomsmiranda**).
- Random failures in the ECS stress test (#948, **@RiscadoA**).
- System condition being ignored by Cubos (#1032, **@RiscadoA**).
- Wrong tranform gizmo position for entities with parents (#1003, **@DiogoMendonc-a**).
- Free camera controller angle not being bounded (#1016, **@diogomsmiranda**).
- Crash when meshing an empty voxel grid (**@RiscadoA**).
- Double destruction in AnyVector::swapErase (**@RiscadoA**).
- Crash due to DataInspector BeginTable calls being unchecked (#1036, **@RiscadoA**).
- Segmentation fault when closing window with deferred renderer plugin (#959, **@RiscadoA**).

## [v0.1.0] - 2024-02-17

### Added

- This CHANGELOG file! It's time we start keeping track of what we have done.
- Serialization system based on reflection (#435, **@RiscadoA**, **@Dacops**).
- Trait for describing enum types under reflection (#579, **@roby2014**).
- New JSON asset bridge based on the new serialization system (#856, **@Dacops**).
- Relations to the ECS (#656, **@RiscadoA**, also, there's a blog post on this!).
- Queries which can be built dynamically (#882, **@RiscadoA**).
- Broadphase collision detection for boxes and capsules (#905, **@luishfonseca**, **@fallenatlas**).
- Physics plugin which for now only implements particle physics (#200, **@fallenatlas**).
- Bootstrap the Gizmo plugin (#512, **@DiogoMendonc-a**).
- Transform gizmo, used to move selected entities (#213, **@DiogoMendonc-a**)
- Free camera controller plugin (#720, **@diogomsmiranda**).
- Split screen plugin, which sets the viewports of active cameras automatically (#760, **@joaomanita**).
- Screen picker plugin (#868, **@tomas7770**).
- Entity picking through the entity selector plugin (#871, **@tomas7770**).
- Toolbox plugin, which manages other TESSERATOS windows (#519, **@teres4**).
- Voxel palette editor plugin (#662, **@roby2014**).
- Scene editor plugin (#265, #551, **@DiogoMendonc-a**, **@teres4**, **@roby2014**).
- Metrics panel plugin, which only shows FPS statistics for now (#275, **@Scarface1809**).
- Parent-child hierarchies to the transform plugin, at last (#334, **@luishfonseca**, **@RiscadoA**).
- New trait type for handling bit masks (**@RiscadoA**).
- Buttons to add/remove components through the entity inspector (#906, **@RiscadoA**).
- Narrow-phase collision detection plugin (#524, **@fallenatlas**).
- Ephemeral trait, used to indicate that relations and components shouldn't be persisted (**@RiscadoA**).
- Collider gizmos plugin, used to view collision shapes (**@RiscadoA**).
- Delta Time multiplier, can be used to adjust simulation speed (#866, **@RiscadoA**).
- Collision Solver plugin (#532, **@fallenatlas**).
- Allow detecting rising/falling edges on Input actions (#958, **@diogomsmiranda**).
- Penetration constraint plugin (#1015, **@fallenatlas**).
- Substeps subloop in fixed step for physics simulation (**@fallenatlas**).

### Changed

- Also track test coverage of engine (#941, **@luishfonseca**)
- Start using reflection on most of the ECS (#462, **@RiscadoA**).
- Change the ECS to be archetype-based (#819, **@RiscadoA**)
- Move the Cubos class to the core (#900, **@RiscadoA**).
- Switch to new system syntax (#896, **@RiscadoA**).
- Change Gizmo plugin to use the Screen picker plugin (#870, **@tomas7770**).
- Default to debug camera when no other camera is active (#840, **@RiscadoA**).
- Bump `glad` from `d08b1aa` to `73eaae0`.
- Bump `json` from `v3.11.2` to `v3.11.3`.
- Change solver to use penetration constraint for collision solving (#1015, **@fallenatlas**).

### Deprecated

- Template-based serialization system (#571, **@RiscadoA**).
- Bridges based on the deprecated serialization system (#436, **@Dacops**, **@RiscadoA**).

### Removed

- `quadrados-gen`, as we no longer need code generation with the new reflection-based ECS (#849, **@RiscadoA**).
- Dependency on `spdlog` and `fmt` (#830, **@RiscadoA**).

### Fixed

- Engine tests are now compilable and run in the CI (#566, **@RiscadoA**).
- Crash when selected entity is destroyed (#477, **@DiogoMendonc-a**).
- Crash when using OpenGL < 4.3 (#740, **@diogomsmiranda**).
- Crash when a renderable grid has a null asset (#956, **@RiscadoA**).
- Bug where other PRs doc previews are deleted when you merge a single PR (#563, **@RiscadoA**).
- Fix spotlight transform and cutoff math (**@RiscadoA**).
- Gravity not working for all entities if an entity had inverse mass smaller or equal to zero (**@fallenatlas**).

## Previous Contributions

Before we started documenting changes, there were some key contributors who helped shape the engine.
Although they've moved on, their work is etched into the project's foundations.

- **@ratuspro**, for bringing the initial team together to work on this project and for his work on the input and settings managers and other utilities.
- **@joaopat98**, for his work on setting up the repo, coordinating the initial team efforts and writing our first renderer.
- **@DannyIsYog**, for his work on the input manager and for writing our first *ECS* dispatcher.
- **@rsubtil**, for his work on bloom, SSAO, the dispatcher, and various essential engine features such as the *Cubos* class and the window plugin.
- **@EdSwordsmith**, for introducing us to the *ECS* design paradigm and writing its first version on the engine.
- **@JoseGraca96256**, for his work on the first audio device abstraction.

[unreleased]: https://github.com/GameDevTecnico/cubos/commits/main/
[v0.1.0]: https://github.com/GameDevTecnico/cubos/releases/tag/v0.1.0
