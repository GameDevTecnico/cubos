# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [v0.4.0] - 2024-10-13

### Added

- PhysicsMaterial component (#1254, **@fallenatlas**).
- Components to support rigid body rotation (#865, **@fallenatlas**).
- Compute contact points and contact manifold for collision between boxes (#1243, **@fallenatlas**).
- Handle body rotation on physics integration (#1242, **@fallenatlas**).
- Binary Serializer and Deserializer (#1306, **@RiscadoA**).
- Type Client and Type Server (#1302, **@RiscadoA**).
- Deadzone for input axis (#844, **@kuukitenshi**).
- Generic Camera component to hold projection matrix (#1331, **@mkuritsu**).
- Initial application debugging through Tesseratos (#1303, **@RiscadoA**).
- Print stacktrace with *cpptrace* on calls to CUBOS_FAIL (#1172, **@RiscadoA**).
- Orthographic Camera component (#1182, **@mkuritsu**).
- Importer plugin (#1299, **@Scarface1809**).
- Handle body rotation on penetration solving (#1272, **@fallenatlas**).
- Cascaded shadow maps (#1187, **@tomas7770**).
- Global telemetry level (for tracing/logging) (#1265, **@roby2014**).
- Spans for tracing and profiling (#1265, **@roby2014**).
- Simple cross-platform multi-process utilities (**@RiscadoA**).
- Project opening and closing on Tesseratos (#1218, **@RiscadoA**).

### Changed

- Moved Glad and stb-image libs to another repositories, cubos-glad and cubos-stb, respectively (#1323, **@kuukitenshi**).
- Moved most tools from Tesseratos to the engine (#1322, **@RiscadoA**).
- Replaced OpenAL audio device with Miniaudio backend (#1005, **@Dageus**, **@diogomsmiranda**).

### Fixed
- Spot light angle mismatch between light and shadows (#1310, **@tomas7770**).
- Spot shadows cause light range cutoff (#1312, **@tomas7770**).
- Precision error in split screen size calculations (**@mkuritsu**).
- Incorrect loop condition in createTexture2DArray (**@tomas7770**).
- Use glTexImage3D instead of glTexStorage3D in createTexture2DArray (**@tomas7770**).

## [v0.3.0] - 2024-08-02

### Added

- UI canvas, elements, and color rectangles (#1116, **@DiogoMendonc-a**).
- Implemented file-based logging (#1154, **@roby2014**).
- Start and update methods to the Cubos class (#1213, **@RiscadoA**).
- Reset method to the Cubos class (#1225, **@RiscadoA**).
- Plugin injection to the Cubos class (#1214, **@RiscadoA**).
- Utility class used to load plugins from shared libraries (#1035, **@RiscadoA**).
- Implemented metrics and profiling tools (#1150, **@roby2014**).
- GL timer for profiling sections of rendering code (#1228, **@tomas7770**).
- Shadows plugin (#1185, **@tomas7770**).
- Network Address class (#1211, **@roby2014**).
- Image asset and UI Image component (#1270, **@DiogoMendonc-a**).
- Raycasting, collisions between rays and box/capsule colliders (#225, **@diogomsmiranda**).
- Change speed of the debug-camera using Tab and LShift, positive and negative respectively (#1159, **@diogomsmiranda**).
- Console plugin (#875, **@Scarface1809**).
- Friction calculation for penetration constraint (#1244, **@fallenatlas**).
- Bounciness calculation for penetration constraint (#1275, **@fallenatlas**).
- UI scaling modes (#1284, **@DiogoMendonc-a**).
- Shadow mapping with support for spot shadow casters (#1186, **@tomas7770**).
- TcpStream network class (#1209, **@roby2014**).
- TcpListener network class (#1210, **@roby2014**).
- UdpSocket network class (#1208, **@roby2014**).

### Changed

- Move Opt class to memory namespace (#1212, **@roby2014**).
- Moved from XPBD to TGS Soft for physics solving (#1269, **@fallenatlas**).
- Allow arbitrary input combinations for actions and axes (#417, #1279, **@luishfonseca**).
- Replaced fallback anonymous reflection types by a new anonymous reflection macro (#1163, **@RiscadoA**).

## [v0.2.0] - 2024-05-07

### Added

- Trait for representing inheritance relationships under reflection. (#693, **@roby2014**).
- Log the system order on the dispatcher (#414, **@RiscadoA**).
- Support for queries with multiple relation terms (#929, **@RiscadoA**).
- Support for queries with more than one unrelated targets (#930, **@RiscadoA**).
- Allow iterating over entity relations directly through the World (#1006, **@RiscadoA**).
- Observers, which allow reacting to component addition and removal (#724, **@RiscadoA**).
- Support for untyped resources (#1057, **@RiscadoA**).
- Addition and removal of resources through Commands (#325, **@RiscadoA**).
- New feature guide focused on queries (#995, **@RiscadoA**).
- Plugins system argument, which can be used to dynamically add and remove plugins (#1034, **@RiscadoA**).
- Task class, for use in asynchronous code (#1111, **@RiscadoA**).
- Fixed Time Step plugin (#989, **@joaomanita**).
- Access to unscaled delta time for utilities like the debug camera (#1020, **@RiscadoA**).
- Defaults plugin (#229, **@RiscadoA**).
- Global position, rotation and scale getters (#1002, **@DiogoMendonc-a**).
- Rotated box Gizmo (#878, **@DiogoMendonc-a**).
- Optional synchronous loading to asset bridges (**@RiscadoA**).
- Shader asset and bridge (#1058, **@tomas7770**).
- GBuffer plugin (#1061, **@RiscadoA**).
- Camera plugin (#1063, **@tomas7770**).
- RenderTarget plugin (#1059, **@tomas7770**).
- RenderPicker plugin (#1060, **@tomas7770**).
- ToneMapping plugin (#1087, **@RiscadoA**).
- RenderDepth plugin (#1082, **@RiscadoA**).
- HDR plugin (#1085, **@RiscadoA**).
- Lights plugin (#1135, **@RiscadoA**).
- Render Voxels plugin (#1119, **@RiscadoA**).
- Render Mesh plugin (#1110, **@RiscadoA**).
- GBuffer Rasterizer plugin (#1062, **@RiscadoA**).
- Deferred Shading plugin (#1086, **@RiscadoA**).
- SSAO plugin (#1088, **@RiscadoA**).
- Split Screen plugin for the new camera plugin (#1149, **@tomas7770**).
- Bloom plugin (#1089, **@RiscadoA**).
- Render Defaults plugin (#1158, **@RiscadoA**).
- Allow toggling vSync through setting (**@RiscadoA**).
- ECS Statistics tesseratos plugin (#1024, **@RiscadoA**).
- Show childOf hierarchy on the world inspector (#991, **@diogomsmiranda**).
- Rotation to the Transform Gizmo (#878, **@DiogoMendonc-a**).
- Search bar to World Inspector which allows filtering by components (#1105, **@diogomsmiranda**).
- Allow filtering by entity names in the World Inspector (#1106, **@diogomsmiranda**).
- Show relations on the Entity Inspector (#907, **@RiscadoA**).
- Allow adding and removing relations through the Entity Inspector (#1022, **@RiscadoA**).
- Possibility of building the core library as a shared library (#1052, **@RiscadoA**).
- Possibility of building the engine library as a shared library (#1053, **@RiscadoA**).
- Allow detecting rising/falling edges on Input actions (#958, **@diogomsmiranda**).
- Solve collisions with proper constraints (#1015, **@fallenatlas**).
- Substeps sub-loop in fixed step for physics simulation (**@fallenatlas**).
- Allow adding sub-scenes through the scene editor (#552, **@teres4**).

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
- Make Gizmos plugin draw to RenderTargets (#1157, **@RiscadoA**).
- Make ImGui plugin use RenderTargets (#1140, **@RiscadoA**).
- Change solver to use penetration constraint for collision solving (#1015, **@fallenatlas**).

### Removed

- Old renderer plugin (#1160, **@RiscadoA**).
- Old core debug renderer code (#668, **@RiscadoA**).
- Template-based serialization system (#1101, **@Dacops**).

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
- Scenes saving unnecessary non-overridden sub-scene components (#988, **@RiscadoA**).

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

## Non-Code Contributions

- **Nuno Baptista**, for the logo, the engine's visual identity and the editor's UX design.

## Previous Contributions

Before we started documenting changes, there were some key contributors who helped shape the engine.
Although they've moved on, their work is etched into the project's foundations.

- **@ratuspro**, for bringing the initial team together to work on this project and for his work on the input and settings managers and other utilities.
- **@joaopat98**, for his work on setting up the repo, coordinating the initial team efforts and writing our first renderer.
- **@DannyIsYog**, for his work on the input manager and for writing our first _ECS_ dispatcher.
- **@rsubtil**, for his work on bloom, SSAO, the dispatcher, and various essential engine features such as the _Cubos_ class and the window plugin.
- **@EdSwordsmith**, for introducing us to the _ECS_ design paradigm and writing its first version on the engine.
- **@JoseGraca96256**, for his work on the first audio device abstraction.

[unreleased]: https://github.com/GameDevTecnico/cubos/commits/main/
[v0.1.0]: https://github.com/GameDevTecnico/cubos/releases/tag/v0.1.0
[v0.2.0]: https://github.com/GameDevTecnico/cubos/releases/tag/v0.2.0
[v0.3.0]: https://github.com/GameDevTecnico/cubos/releases/tag/v0.3.0
[v0.4.0]: https://github.com/GameDevTecnico/cubos/releases/tag/v0.4.0
