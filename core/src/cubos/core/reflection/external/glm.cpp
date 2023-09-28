#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>

#define AUTO_IMPL(type, name)                                                                                          \
    CUBOS_REFLECT_EXTERNAL_IMPL(type)                                                                                  \
    {                                                                                                                  \
        return Type::create(name).with(ConstructibleTrait::typed<type>()                                               \
                                           .withDefaultConstructor()                                                   \
                                           .withCopyConstructor()                                                      \
                                           .withMoveConstructor()                                                      \
                                           .build());                                                                  \
    }

AUTO_IMPL(glm::quat, "glm::quat");
AUTO_IMPL(glm::dquat, "glm::dquat");
