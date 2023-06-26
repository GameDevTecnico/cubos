/// @file
/// @brief Sample which showcases how deserialization and its hooks work.

#include <glm/vec3.hpp>

#include <cubos/core/data/des/json.hpp>
#include <cubos/core/memory/stream.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/vector.hpp>

using cubos::core::data::JSONDeserializer;
using cubos::core::memory::Stream;

static std::string line()
{
    std::string line;
    Stream::stdIn.readUntil(line, "\n");
    return line;
}

int main()
{
    JSONDeserializer des{};

    // Ask the user to enter a JSON string for a glm::ivec3, loop while the input is invalid.
    while (true)
    {
        Stream::stdOut.printf(
            "Enter a single line JSON string for a glm::ivec3 - an object with three integer fields x, y and z:\n");
        if (!des.parse(line()))
        {
            Stream::stdOut.printf("Invalid JSON string, try again.\n\n");
            continue;
        }

        glm::ivec3 vec;
        if (!des.read(vec))
        {
            Stream::stdOut.printf("Invalid glm::ivec3, try again.\n\n");
            continue;
        }

        Stream::stdOut.printf("Parsed glm::ivec3 with the fields x = {}, y = {}, z = {}\n\n", vec.x, vec.y, vec.z);
        break;
    }

    // Set an hook for glm::ivec3, so that it is deserialized from a single number, where all components are the same.
    des.hook<glm::ivec3>([&des](glm::ivec3& v) {
        int number;
        if (!des.read(number))
        {
            return false;
        }

        v.x = v.y = v.z = number;
        return true;
    });

    // Ask the user to enter a JSON string for an array of glm::ivec3, loop while the input is invalid.
    while (true)
    {
        Stream::stdOut.printf("Enter a single line JSON string for an array of glm::ivec3, where each\n"
                              "glm::ivec3 is deserialized from a single number, so that all components\n"
                              "are the same (e.g.:, 1 -> glm::ivec3{1, 1, 1}):\n");
        if (!des.parse(line()))
        {
            Stream::stdOut.printf("Invalid JSON string, try again.\n\n");
            continue;
        }

        std::vector<glm::ivec3> vec;
        if (!des.read(vec))
        {
            Stream::stdOut.printf("Invalid array of glm::ivec3, try again.\n\n");
            continue;
        }

        Stream::stdOut.printf("Parsed array of glm::ivec3 with the following elements:\n");
        for (const auto& v : vec)
        {
            Stream::stdOut.printf("    x = {}, y = {}, z = {}\n", v.x, v.y, v.z);
        }
        break;
    }
}
