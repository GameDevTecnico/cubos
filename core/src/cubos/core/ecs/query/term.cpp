#include <cubos/core/ecs/query/term.hpp>

using cubos::core::ecs::QueryTerm;

QueryTerm QueryTerm::makeEntity(int target)
{
    return {
        .type = DataTypeId::Invalid,
        .entity = {.target = target},
    };
}

QueryTerm QueryTerm::makeWithComponent(DataTypeId type, int target)
{
    return {
        .type = type,
        .component = {.target = target, .without = false, .optional = false},
    };
}

QueryTerm QueryTerm::makeWithoutComponent(DataTypeId type, int target)
{
    return {
        .type = type,
        .component = {.target = target, .without = true, .optional = false},
    };
}

QueryTerm QueryTerm::makeOptComponent(DataTypeId type, int target)
{
    return {
        .type = type,
        .component = {.target = target, .without = false, .optional = true},
    };
}

bool QueryTerm::isEntity() const
{
    return type == DataTypeId::Invalid;
}

bool QueryTerm::isComponent(const Types& types) const
{
    return type != DataTypeId::Invalid && types.isComponent(type);
}
