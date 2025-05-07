/// @file
/// @brief Reflection declaration for C-strings.
/// @ingroup core-reflection

#pragma once

#include <cubos/core/reflection/traits/array.hpp>
#include <cubos/core/reflection/traits/constructible_utils.hpp>
#include <cubos/core/reflection/type.hpp>

CUBOS_REFLECT_EXTERNAL_TEMPLATE((typename T, std::size_t size), (T[size]))
{
    // We are supplying, first, the element type, and then a getter for the length and another for
    // the address of an element.
    const Type& element = reflect<T>();
    auto arrayTrait = ArrayTrait(
        element, [](const void* /*instance*/) -> std::size_t { return size; },
        [](const void* instance, std::size_t index) -> uintptr_t {
            return reinterpret_cast<uintptr_t>(static_cast<const T*>(instance) + index);
        });

    Type& type = Type::create(reflect<T>().name() + "[" + std::to_string(size) + "]").with(arrayTrait);

    if (element.has<ConstructibleTrait>())
    {
        const auto& elConstructibleTrait = element.get<ConstructibleTrait>();

        auto constructibleTrait = ConstructibleTrait{
            sizeof(T) * size,
            alignof(T),
            [destructor = elConstructibleTrait.destructor()](void* instance) {
                auto* arr = static_cast<T*>(instance);
                for (std::size_t i = 0; i < size; ++i)
                {
                    destructor(&arr[i]);
                }
            },
        };

        if (elConstructibleTrait.hasDefaultConstruct())
        {
            constructibleTrait =
                memory::move(constructibleTrait)
                    .withDefaultConstructor(
                        [defaultConstructor = elConstructibleTrait.defaultConstructor()](void* instance) {
                            auto* arr = static_cast<T*>(instance);
                            for (std::size_t i = 0; i < size; ++i)
                            {
                                defaultConstructor(&arr[i]);
                            }
                        });
        }

        if (elConstructibleTrait.hasCopyConstruct())
        {
            constructibleTrait = memory::move(constructibleTrait)
                                     .withCopyConstructor([copyConstructor = elConstructibleTrait.copyConstructor()](
                                                              void* instance, const void* other) {
                                         auto* arr = static_cast<T*>(instance);
                                         const auto* otherArr = static_cast<const T*>(other);
                                         for (std::size_t i = 0; i < size; ++i)
                                         {
                                             copyConstructor(&arr[i], &otherArr[i]);
                                         }
                                     });
        }

        if (elConstructibleTrait.hasMoveConstruct())
        {
            constructibleTrait = memory::move(constructibleTrait)
                                     .withMoveConstructor([moveConstructor = elConstructibleTrait.moveConstructor()](
                                                              void* instance, void* other) {
                                         auto* arr = static_cast<T*>(instance);
                                         auto* otherArr = static_cast<T*>(other);
                                         for (std::size_t i = 0; i < size; ++i)
                                         {
                                             moveConstructor(&arr[i], &otherArr[i]);
                                         }
                                     });
        }

        return type.with(constructibleTrait);
    }
    else
    {
        return type;
    }
}
