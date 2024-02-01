# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- This CHANGELOG file! It's time we start keeping track of what we have done.
- Serialization system based on reflection (#435).
- Trait for describing enum types under reflection (#579).
- New JSON asset bridge based on the new serialization system (#856).
- Relations to the ECS (#656, also, there's a blog post on this!).
- Queries which can be built dynamically (#882).
- Broadphase collision detection for boxes and capsules (#905).
- Physics plugin which for now only implements particle physics (#200).
- Bootstrap the Gizmo plugin (#512).
- Transform gizmo, used to move selected entities (#213)
- Free camera controller plugin (#720).
- Split screen plugin, which sets the viewports of active cameras automatically (#760).
- Screen picker plugin (#868).
- Entity picking through the entity selector plugin (#871).
- Toolbox plugin, which manages other TESSERATOS windows (#519).
- Voxel palette editor plugin (#662).
- Scene editor plugin (#265, #551).
- Metrics panel plugin, which only shows FPS statistics for now (#275).

### Changed

- Also track test coverage of engine (#941)
- Start using reflection on most of the ECS (#462).
- Change the ECS to be archetype-based (#819)
- Move the Cubos class to the core (#900).
- Switch to new system syntax (#896).
- Bump `glad` from `d08b1aa` to `73eaae0`.
- Bump `json` from `v3.11.2` to `v3.11.3`.
- Change Gizmo plugin to use the Screen picker plugin (#870).

### Deprecated

- Template-based serialization system (#571).
- Bridges based on the deprecated serialization system (#436).

### Removed

- `quadrados-gen`, as we no longer need code generation with the new reflection-based ECS (#849).
- Dependency on `spdlog` and `fmt` (#830).

### Fixed

- Engine tests are now compilable and run in the CI (#566).
- Crash when selected entity is destroyed (#477).
- Crash when using OpenGL < 4.3 (#740).
- Bug where other PRs doc previews are deleted when you merge a single PR (#563).

[unreleased]: https://github.com/GameDevTecnico/cubos/commits/main/
