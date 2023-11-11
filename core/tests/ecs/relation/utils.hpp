#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include "../utils.hpp"

// A relation which doesn't store anything.
struct EmptyRelation
{
    CUBOS_REFLECT;
};

// A relation which stores a single integer.
struct IntegerRelation
{
    CUBOS_REFLECT;

    int value;
};

// A relation which can be used to test if destructors are called correctly.
struct DetectDestructorRelation
{
    CUBOS_REFLECT;

    DetectDestructor detect;
};
