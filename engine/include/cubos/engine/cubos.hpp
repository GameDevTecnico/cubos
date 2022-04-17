#ifndef CUBOS_ENGINE_CUBOS_HPP
#define CUBOS_ENGINE_CUBOS_HPP

namespace cubos::engine
{
    /// Initializes the game engine.
    /// @param argc The number of arguments passed to the program.
    /// @param argv The array of arguments passed to the program.
    void init(int argc, char** argv);

    /// Runs the game engine.
    void run();
} // namespace cubos::engine

#endif // CUBOS_ENGINE_CUBOS_HPP
