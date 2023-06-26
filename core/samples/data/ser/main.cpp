/// @file
/// @brief Sample which showcases how serialization and its hooks work.

#include <glm/vec3.hpp>

#include <cubos/core/data/ser/json.hpp>
#include <cubos/core/memory/stream.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/vector.hpp>

using cubos::core::data::JSONSerializer;
using cubos::core::memory::Stream;

int main()
{
    JSONSerializer ser{};

    // Serialize a glm::ivec3 - it is an ObjectType, so the serializer will serialize its fields by default.
    glm::ivec3 vec = {1, 2, 3};
    ser.write(vec);
    Stream::stdOut.printf("glm::ivec3 serialized with the default implementation:\n{}\n", ser.string(2));

    // Set an hook for glm::ivec3, so that it is serialized as a comma-separated string instead.
    ser.hook<glm::ivec3>([&ser](const glm::ivec3& v) {
        ser.write(std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z));
    });

    // Serialize the same glm::ivec3 again, now with the custom hook applied.
    ser.write(vec);
    Stream::stdOut.printf("glm::ivec3 serialized with a custom hook:\n{}\n", ser.string(2));

    // Serialize a vector of glm::ivec3 - it is an ArrayType, so the serializer will serialize its elements by default.
    // The elements themselves will be serialized with the hook we set above.
    std::vector<glm::ivec3> vecs = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    ser.write(vecs);
    Stream::stdOut.printf("std::vector<glm::ivec3> serialized with the glm::ivec3 hook:\n{}\n", ser.string(2));
}
