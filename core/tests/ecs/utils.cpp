#include "utils.hpp"

#include <cubos/core/ecs/component/reflection.hpp>
#include <cubos/core/ecs/relation/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/unordered_map.hpp>
#include <cubos/core/reflection/external/vector.hpp>

CUBOS_REFLECT_IMPL(IntegerComponent)
{
    return cubos::core::ecs::ComponentTypeBuilder<IntegerComponent>("IntegerComponent")
        .withField("value", &IntegerComponent::value)
        .build();
}

CUBOS_REFLECT_IMPL(ParentComponent)
{
    return cubos::core::ecs::ComponentTypeBuilder<ParentComponent>("ParentComponent")
        .withField("id", &ParentComponent::id)
        .build();
}

CUBOS_REFLECT_IMPL(DetectDestructorComponent)
{
    using namespace cubos::core::reflection;

    return Type::create("DetectDestructorComponent")
        .with(ConstructibleTrait::typed<DetectDestructorComponent>()
                  .withDefaultConstructor()
                  .withMoveConstructor()
                  .build())
        .with(FieldsTrait().withField("detect", &DetectDestructorComponent::detect));
}

CUBOS_REFLECT_IMPL(EntityArrayComponent)
{
    return cubos::core::ecs::ComponentTypeBuilder<EntityArrayComponent>("EntityArrayComponent")
        .withField("vec", &EntityArrayComponent::vec)
        .build();
}

CUBOS_REFLECT_IMPL(EntityDictionaryComponent)
{
    return cubos::core::ecs::ComponentTypeBuilder<EntityDictionaryComponent>("EntityDictionaryComponent")
        .withField("map", &EntityDictionaryComponent::map)
        .build();
}

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
