#include <cubos/core/ecs/query/term.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/type.hpp>

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

QueryTerm QueryTerm::makeRelation(DataTypeId type, int fromTarget, int toTarget)
{
    return {
        .type = type,
        .relation = {.fromTarget = fromTarget, .toTarget = toTarget},
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

bool QueryTerm::isRelation(const Types& types) const
{
    return type != DataTypeId::Invalid && types.isRelation(type);
}

bool QueryTerm::compare(const Types& types, const QueryTerm& other) const
{
    if (type != other.type)
    {
        return false;
    }

    if (this->isEntity())
    {
        return entity.target == other.entity.target;
    }

    if (this->isComponent(types))
    {
        return component.target == other.component.target && component.without == other.component.without &&
               component.optional == other.component.optional;
    }

    if (this->isRelation(types))
    {
        return relation.fromTarget == other.relation.fromTarget && relation.toTarget == other.relation.toTarget;
    }

    CUBOS_UNREACHABLE();
}

std::vector<QueryTerm> QueryTerm::resolve(const Types& types, const std::vector<QueryTerm>& baseTerms,
                                          std::vector<QueryTerm>& otherTerms)
{
    // This code assumes otherTerms comes from the query argument types, and thus, all of its targets
    // should be unspecified (-1).

    std::vector<QueryTerm> terms{};
    int defaultTarget = 0;

    // For each base term.
    auto otherTermIt = otherTerms.begin();
    for (auto baseTerm : baseTerms)
    {
        // Check if any of the remaining other terms has the same type as the base term.
        auto it = otherTermIt;
        for (; it != otherTerms.end(); ++it)
        {
            if (baseTerm.type == it->type)
            {
                break;
            }
        }

        // If the base term is a negative component, its impossible to merge it with any other term.
        if (baseTerm.isComponent(types) && baseTerm.component.without)
        {
            it = otherTerms.end();
        }

        // If there is a matching term, merge it with the base term.
        if (it != otherTerms.end())
        {
            // Add all the other terms until the matching term to the result.
            for (; otherTermIt != it; ++otherTermIt)
            {
                auto& otherTerm = *otherTermIt;

                // Change the other term's target to the default, as it was unspecified.
                if (otherTerm.isEntity())
                {
                    CUBOS_ASSERT(otherTerm.entity.target == -1); // See comment at the beginning of the function.
                    otherTerm.entity.target = defaultTarget;
                }
                else if (otherTerm.isComponent(types))
                {
                    CUBOS_ASSERT(otherTerm.component.target == -1); // See comment at the beginning of the function.
                    otherTerm.component.target = defaultTarget;
                }
                else if (otherTerm.isRelation(types))
                {
                    CUBOS_ASSERT(otherTerm.relation.fromTarget == -1 &&
                                 otherTerm.relation.toTarget == -1); // See comment at the beginning of the function.
                    otherTerm.relation.fromTarget = defaultTarget;
                    otherTerm.relation.toTarget = ++defaultTarget;
                }
                else
                {
                    CUBOS_UNREACHABLE();
                }

                // Add it to the result.
                terms.emplace_back(otherTerm);
            }

            auto& otherTerm = *otherTermIt;
            ++otherTermIt;

            // Merge data from the other term into the base term.
            if (baseTerm.isEntity())
            {
                CUBOS_ASSERT(otherTerm.entity.target == -1); // See comment at the beginning of the function.

                if (baseTerm.entity.target == -1)
                {
                    baseTerm.entity.target = defaultTarget;
                }

                otherTerm.entity.target = baseTerm.entity.target;
            }
            else if (baseTerm.isComponent(types))
            {
                CUBOS_ASSERT(otherTerm.component.target == -1); // See comment at the beginning of the function.

                // Code above should ensure terms never match if they have different without values.
                CUBOS_ASSERT(baseTerm.component.without == otherTerm.component.without);

                if (baseTerm.entity.target == -1)
                {
                    baseTerm.entity.target = defaultTarget;
                }

                baseTerm.component.optional = otherTerm.component.optional;
                otherTerm.entity.target = baseTerm.entity.target;
            }
            else if (baseTerm.isRelation(types))
            {
                CUBOS_ASSERT(otherTerm.relation.fromTarget == -1 &&
                             otherTerm.relation.toTarget == -1); // See comment at the beginning of the function.

                if (baseTerm.relation.fromTarget == -1)
                {
                    baseTerm.relation.fromTarget = defaultTarget;
                }

                if (baseTerm.relation.toTarget == -1)
                {
                    baseTerm.relation.toTarget = ++defaultTarget;
                }

                otherTerm.relation.fromTarget = baseTerm.relation.fromTarget;
                otherTerm.relation.toTarget = baseTerm.relation.toTarget;
            }
        }

        // Update either the defaultTarget or the term's target, depending on it being specified.
        if (baseTerm.isEntity())
        {
            if (baseTerm.entity.target == -1)
            {
                baseTerm.entity.target = defaultTarget;
            }
            else
            {
                defaultTarget = baseTerm.entity.target;
            }
        }
        else if (baseTerm.isComponent(types))
        {
            if (baseTerm.component.target == -1)
            {
                baseTerm.component.target = defaultTarget;
            }
            else
            {
                defaultTarget = baseTerm.component.target;
            }
        }
        else if (baseTerm.isRelation(types))
        {
            if (baseTerm.relation.fromTarget == -1)
            {
                baseTerm.relation.fromTarget = defaultTarget;
            }
            else
            {
                defaultTarget = baseTerm.relation.fromTarget;
            }

            if (baseTerm.relation.toTarget == -1)
            {
                baseTerm.relation.toTarget = ++defaultTarget;
            }
            else
            {
                defaultTarget = baseTerm.relation.toTarget;
            }
        }
        else
        {
            CUBOS_UNREACHABLE();
        }

        // Add the base term to the result.
        terms.emplace_back(baseTerm);
    }

    // Add the remaining other terms.
    for (; otherTermIt != otherTerms.end(); ++otherTermIt)
    {
        auto& otherTerm = *otherTermIt;

        // Change the other term's target to the default, as it was unspecified.
        if (otherTerm.isEntity())
        {
            CUBOS_ASSERT(otherTerm.entity.target == -1); // See comment at the beginning of the function.
            otherTerm.entity.target = defaultTarget;
        }
        else if (otherTerm.isComponent(types))
        {
            CUBOS_ASSERT(otherTerm.component.target == -1); // See comment at the beginning of the function.
            otherTerm.component.target = defaultTarget;
        }
        else if (otherTerm.isRelation(types))
        {
            CUBOS_ASSERT(otherTerm.relation.fromTarget == -1 &&
                         otherTerm.relation.toTarget == -1); // See comment at the beginning of the function.
            otherTerm.relation.fromTarget = defaultTarget;
            otherTerm.relation.toTarget = ++defaultTarget;
        }
        else
        {
            CUBOS_UNREACHABLE();
        }

        // Add it to the result.
        terms.emplace_back(otherTerm);
    }

    return terms;
}

std::string QueryTerm::toString(const Types& types, const std::vector<QueryTerm>& terms)
{
    std::string result;

    for (const auto& term : terms)
    {
        if (!result.empty())
        {
            result += ", ";
        }

        if (term.isEntity())
        {
            result += "Entity(" + std::to_string(term.entity.target) + ")";
        }
        else if (term.isComponent(types))
        {
            if (term.component.optional)
            {
                result += '?';
            }
            else if (term.component.without)
            {
                result += '!';
            }

            result += types.type(term.type).name() + "(" + std::to_string(term.component.target) + ")";
        }
        else if (term.isRelation(types))
        {
            result += types.type(term.type).name() + "(" + std::to_string(term.relation.fromTarget) + ", " +
                      std::to_string(term.relation.toTarget) + ")";
        }
        else
        {
            CUBOS_UNREACHABLE();
        }
    }

    return result;
}
