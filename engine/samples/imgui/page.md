# ImGui {#examples-engine-imgui}

@brief Using the @ref imgui-plugin plugin.

@see Full source code [here](https://github.com/GameDevTecnico/cubos/tree/main/engine/samples/imgui).

The ImGui plugin allows you to integrate the [Dear ImGui](https://github.com/ocornut/imgui) library with CUBOS., making it quick, straightforward, and effortless to create user interfaces and interactive debugging tools within your applications.

![](imgui/img1.png)

First, let's start by including the header files we need.

@snippet imgui/main.cpp Including plugin headers

Then, make sure to add the plugin.

@snippet imgui/main.cpp Adding the plugin

Once the ImGui plugin is added, you can create systems to display ImGui windows and widgets. Here's an example of how to create an ImGui window.

@snippet imgui/main.cpp ImGui window example

Ensure that you add your system with the `cubos.imgui` tag; otherwise, the ImGui elements from that system won't be be visible.

@snippet imgui/main.cpp Adding the system

Pretty simple right? You're now equipped to craft and utilize ImGui's functions to design your cool user interface.

You can find more about how to use [Dear ImGui](https://github.com/ocornut/imgui) stuff [here](https://github.com/ocornut/imgui/tree/master/docs).