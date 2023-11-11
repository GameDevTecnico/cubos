#include "utils.hpp"

#include <cubos/core/ecs/relation/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

using cubos::core::reflection::ConstructibleTrait;
using cubos::core::reflection::FieldsTrait;
using cubos::core::reflection::Type;

CUBOS_REFLECT_IMPL(EmptyRelation)
{
    return cubos::core::ecs::RelationTypeBuilder<EmptyRelation>("EmptyRelation").build();
}

CUBOS_REFLECT_IMPL(IntegerRelation)
{
    return cubos::core::ecs::RelationTypeBuilder<IntegerRelation>("IntegerRelation")
        .withField("value", &IntegerRelation::value)
        .build();
}

CUBOS_REFLECT_IMPL(DetectDestructorRelation)
{
    return Type::create("DetectDestructorRelation")
        .with(ConstructibleTrait::typed<DetectDestructorRelation>()
                  .withDefaultConstructor()
                  .withMoveConstructor()
                  .build())
        .with(FieldsTrait().withField("detect", &DetectDestructorRelation::detect));
}
