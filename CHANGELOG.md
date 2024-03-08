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
- Rotation Gizmo (#878, **@DiogoMendonc-a**).

### Fixed

- Crash in multiple samples due to missing plugin dependencies (**@RiscadoA**).
- Cursor offset in tesseratos when on retina displays (#998, **@diogomsmiranda**).
- Random failures in the ECS stress test (#948, **@RiscadoA**).
- System condition being ignored by Cubos (#1032, **@RiscadoA**).
- Wrong tranform gizmo position for entities with parents (#1003, **@DiogoMendonc-a**).
- Free camera controller angle not being bounded (#1016, **@diogomsmiranda**).

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
