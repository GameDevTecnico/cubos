# File Settings {#engine-plugins-file-settings}

The \ref cubos::engine::plugins::file_settings "file_settings" plugin adds a
startup system which runs on the stage `"file-settings"`. This system reads the
settings file specified by the `settings.path` setting (default:
`"settings.json"`), and adds any settings it finds to the \ref
cubos::core::Settings "core::Settings" resource.

The `"file-settings"` stage is by default put after the `"env-settings"` stage,
or, if the `"env-settings"` stage is not present, as the first stage.

Settings are stored as a *JSON* object, where the keys are the names of the
settings and the values are the values of the settings. Example input:

```json
{
    "renderer": {
        "ssao": true
    },
    "window": {
        "vsync": true,
        "width": 1920,
        "height": 1080
    }
}
```

Which is equivalent to:
```json
{
    "renderer.ssao": true,
    "window.vsync": true,
    "window.width": 1920,
    "window.height": 1080
}
```
