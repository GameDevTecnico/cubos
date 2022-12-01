# Environment Settings {#engine-plugins-env-settings}

The \ref cubos::engine::plugins::env_settings "env_settings" plugin adds a
startup system which runs on the stage `"env-settings"`. This system reads
the environment argument variables and adds any settings it finds to the \ref
cubos::core::Settings "core::Settings" resource.

The state is by default put as the first stage, so it will be executed before
any other startup systems.

Settings are read as key-value pairs, where the key is the name of the setting
and the value is the value of the setting. Example input:

```bash
    ./mygame renderer.ssao=true window.vsync=true window.width=1920 window.height=1080
```
