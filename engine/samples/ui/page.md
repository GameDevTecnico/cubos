# UI {#examples-engine-ui}

@brief Using the @ref ui-canvas-plugin plugin.

@see Full source code [here](https://github.com/GameDevTecnico/cubos/tree/main/engine/samples/ui).

This example shows how the @ref ui-canvas-plugin plugin can be used to create ui elements on screen. It also uses the @ref ui-color-rect-plugin plugin to draw those elements to screen.

@snippet ui/main.cpp Set up Canvas

We start by setting up a @ref cubos::engine::UICanvas. This will serve as a target to which the elements will be drawn to.

@snippet ui/main.cpp Set up Background

Now we can set up a background, with a @ref cubos::engine::UIElement. We want it to cover the whole screen, so we'll add both a @ref cubos::engine::UIHorizontalStretch and a @ref cubos::engine::UIVerticalStretch componenets. These will make sure our background's size will always match that of the canvas. For extra style, we can add a little border around it, by setting the margins of the components to 20.

We can add a @ref cubos::engine::UIColorRect to that entity and have the background be drawn in white.

Finally, we have to set this UIElement as a child of the entity holding the canvas.

Besides the stretch components, the @ref cubos::engine::UIElement has some fields to control its placement.

@snippet ui/main.cpp Set up Panel

Here we create a small panel to the right side of the screen. The anchor is set to `1, 0.5`. This will mean that the panel's pivot will be placed on the very right edge of its parent, and in the middle vertically. Then the pivot itself is set to also be `1, 0.5`. This will mean that the UIElement will "grow" to the left horizontally, and equally up and down vertically. This is because by setting that value, we are effectively telling the UI element its pivot will be on its right edge, in the middle point of it. We set the offset to be `0, -50`, so that we leave a little space to the right of the it before the edge of its parent. This space will be constant regardless of the parent's size. Finally, we se the size of the element to be `200, 600`.

We set this panel as child of the background, and we'll paint it red.

Next let's add an image.

@snippet ui/main.cpp Set up Logo

To add an image, it's a simple matter of adding a @ref cubos::engine::UIImage component to an element, with the handle to an image asset.
It will then be drawn onto that element's rect.

We might, however, want to make sure that the image will retain its aspect ratio without having to manually adjust the element's size. The solution for that is the @ref cubos::engine::UINativeAspectRatio component. What it does is size down the element to have a size that is in line with the source file's aspect ratio.

@note @ref cubos::engine::UINativeAspectRatio will behave differently when attached to a @ref cubos::engine::UIHorizontalStretch or a @ref cubos::engine::UIVerticalStretch component. It will keep the dimension that is being stretched, and alter the other one, which can result in either sizing up or down the element. In case both UIHorizontalStretch and UIVerticalStretch are present, UINativeAspectRatio will return to sizing the element down.

@snippet ui/main.cpp Set up Longo Logo

For this we will the use the extended version of our logo, which is rectangular. You'll notice that the UIElement it's being attached to is a square, but due to the UINativeAspectRatio component, that square will be sized down into proper shape.

@note The @ref cubos::engine::UINativeAspectRatio is transformative. If you desire to keep the original size of the element information for other uses, it might be best to have the image instead be on a child entity of that element, and use both @ref cubos::engine::UIHorizontalStretch and @ref cubos::engine::UIVerticalStretch to achieve the same effect.


![](ui_output.png)