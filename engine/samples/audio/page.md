# Audio {#examples-engine-audio}

@brief Using the @ref audio-plugin plugin

@see Full source code [here](https://github.com/GameDevTecnico/cubos/tree/main/engine/samples/audio)

This sample shows how the @ref audio-plugin can be used to load audio samples and interact with them, using all supported file formats.

Let's start by creating a Listener (represented through the camera).

@snippet audio/main.cpp Adding an AudioListener.

In order to produce sound, we also need to create an AudioSource.

@snippet audio/main.cpp Adding the AudioSource

How does this source know what to play? You can assign any audio asset, as long as in the supported format, to the AudioSource component.

@snippet audio/main.cpp Adding the asset

We add a system which cycles through 3 assets and allows to play/pause and stop them through user input.

The way we signal cubos to play, pause and stop an asset is through the use of the `AudioPlay`, `AudioPause`, and `AudioStop` components; just add them and the actions should happen immediatly.

@snippet audio/main.cpp Manipulating audio assets

That way, using our selected inputs, if you press `Enter`, you will skip to the next asset; `Spacebar` will play or pause the asset; and pressing `Backspace` will stop the current audio asset.

You can play around with the audio samples and listen to each one using the plugin!
