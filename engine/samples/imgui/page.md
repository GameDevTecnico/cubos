# ImGui {#examples-engine-imgui}

@brief Using the @ref imgui-plugin plugin and the @ref cubos::engine::DataInspector "DataInspector" resource.

@see Full source code [here](https://github.com/GameDevTecnico/cubos/tree/main/engine/samples/imgui).

The ImGui plugin allows you to integrate the [Dear ImGui](https://github.com/ocornut/imgui) library with **Cubos**, making it quick, straightforward, and effortless to create user interfaces and interactive debugging tools within your applications.

![](imgui/img1.png)

First, let's start by including the header files we need.

@snippet imgui/main.cpp Including plugin headers

Then, make sure to add the plugin.

@snippet imgui/main.cpp Adding the plugin

When you're using ImGui directly in an application or a library, you must also make sure to initialize the ImGui context on it.
To do so, you can add a startup system just like the one below:

@snippet imgui/main.cpp ImGui initialization

You can read more about this in the documentation of @ref cubos::engine::ImGuiContextHolder.
Now, you can create systems to display ImGui windows and widgets. Here's a system which opens an ImGui window, and its demo.

@snippet imgui/main.cpp ImGui Demo

Ensure that you add your system with the `cubos.imgui` tag; otherwise, the ImGui elements from that system won't be be visible.

Pretty simple right? You're now equipped to craft and utilize ImGui's functions to design your cool user interface.

Now, we'll also show you how you can use the @ref cubos::engine::DataInspector resource in combination with @ref imgui-plugin plugin to inspect/modify data in real time.

To start off, we'll need to have some sort of dummy data shared across our application, so we can inspect/modify later. Let's create a `DummyResource` with some fields and fill it with random data.

@note In case you did not read about @ref core-reflection yet, you should take a look at that first, since we'll use some of it's concepts here.

@snippet imgui/main.cpp Creating a dummy resource

@snippet imgui/main.cpp Filling the dummy resource

Well now, using the @ref cubos::engine::DataInspector is pretty easy, all you have to do is access the resource on your system, and use the functions `DataInspector::edit` and `DataInspector::edit`.

@snippet imgui/main.cpp DataInspector window example

![](imgui/img2.png)

You can find more about how to use [Dear ImGui](https://github.com/ocornut/imgui) stuff [here](https://github.com/ocornut/imgui/tree/master/docs).