#include <cubos/core/memory/stream.hpp>

using cubos::core::memory::Stream;

/// [Include]
#include <cubos/core/data/ser/json.hpp>

using cubos::core::data::JSONSerializer;
/// [Include]

/// [Usage]
#include <glm/vec3.hpp>

#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/map.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/vector.hpp>

int main()
{
    std::vector<std::vector<std::vector<glm::ivec3>>> vec{{{{1, 2, 3}, {4, 5, 6}}}, {{{7, 8, 9}, {10, 11, 12}}}};
    JSONSerializer ser{};
    ser.write(vec);

    auto json = ser.output();
    Stream::stdOut.print(json.dump());
}
/// [Usage]

/// [Output]
// [[[{"x":1,"y":2,"z":3},{"x":4,"y":5,"z":6}]],[[{"x":7,"y":8,"z":9},{"x":10,"y":11,"z":12}]]]
/// [Output]