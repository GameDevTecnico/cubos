# Events {#examples-engine-events}

@brief Using the @ref cubos::core::ecs::EventReader and @ref cubos::core::ecs::EventWriter

This example shows how the @ref cubos::core::ecs::EventReader and the @ref cubos::core::ecs::EventWriter can be used and configured, in a simple scene, to emit and read events. These are in the core, meaning, you don't need to add them to cubos.

Firstly, we need to create and register the event we want to emit. Here, our event has one variable, however, you can give it any number of variables of any type you want.

@snippet events/main.cpp Event struct
@snippet events/main.cpp Adding event

To receive these events, we can make a simple system, using the @ref cubos::core::ecs::EventReader and iterating through all the events it has. This will be the layout of all our reader systems (A, C, D).

@snippet events/main.cpp Event reader system

Now, to emit these events, we will use the @ref cubos::core::ecs::EventWriter. This system will emit 3 events on the first frame and another 3 on the second frame. By setting the value of the ShouldQuit resource to true on the second frame, the engine stops before reaching the third frame. 

@snippet events/main.cpp Event writer system

Lastly, let's set the order we want these system to execute in.

@snippet events/main.cpp Adding systems

These are the expected results with this order.

@snippet events/main.cpp Expected results

There are a couple of things to note here. First, the order in which the systems appear to receive the event. C receives the event, followed by D, this happens because even though A comes before C it also come before B, which is where the event is emitted, this means that C and D can read the event emitted by B on that same frame, while A will only read it on the next frame. This also explains why on the second run, A is never displayed, indeed, the engine quit before A got a chance to receive it's so desired events. This shows how the results of the execution of systems that use events may vary with the order set for them, so special care should be taken when defining this.