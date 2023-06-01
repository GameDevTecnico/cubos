#pragma once

#include <functional>

/// Utility macro which adds two subcases to the current test case, one for true and one for false.
#define PARAMETRIZE_TRUE_OR_FALSE(what, var)                                                                           \
    do                                                                                                                 \
    {                                                                                                                  \
        SUBCASE(what " is true")                                                                                        \
        {                                                                                                              \
            var = true;                                                                                                \
        }                                                                                                              \
        SUBCASE(what " is false")                                                                                       \
        {                                                                                                              \
            var = false;                                                                                               \
        }                                                                                                              \
    } while (false)

/// Counts the number of arguments in a variadic macro - handles up to 4 arguments.
#define COUNT_ARGS(...) COUNT_ARGS_IMPL(__VA_ARGS__, 4, 3, 2, 1)
#define COUNT_ARGS_IMPL(_1, _2, _3, _4, n, ...) n

/// Turns a list of argument types into a list of argument declarations.
/// For example, DEFINE_MOCK_IMPL_DECL(int, float) becomes int _0, float _1.
#define MAKE_DECLARATIONS(...) __VA_OPT__(MAKE_DECLARATIONS_N(COUNT_ARGS(__VA_ARGS__), __VA_ARGS__))
#define MAKE_DECLARATIONS_N(n, ...) MAKE_DECLARATIONS_N_IMPL(n, __VA_ARGS__)
#define MAKE_DECLARATIONS_N_IMPL(n, ...) MAKE_DECLARATIONS_##n(__VA_ARGS__)
#define MAKE_DECLARATIONS_1(a) a _0
#define MAKE_DECLARATIONS_2(a, b) a _0, b _1
#define MAKE_DECLARATIONS_3(a, b, c) a _0, b _1, c _2
#define MAKE_DECLARATIONS_4(a, b, c, d) a _0, b _1, c _2, d _3

/// Turns a list of argument types into a list of argument names.
/// For example, DEFINE_MOCK_IMPL_NAME(int, float) becomes _0, _1.
#define MAKE_NAMES(...) __VA_OPT__(MAKE_NAMES_N(COUNT_ARGS(__VA_ARGS__), __VA_ARGS__))
#define MAKE_NAMES_N(n, ...) MAKE_NAMES_N_IMPL(n, __VA_ARGS__)
#define MAKE_NAMES_N_IMPL(n, ...) MAKE_NAMES_##n(__VA_ARGS__)
#define MAKE_NAMES_1(a) _0
#define MAKE_NAMES_2(a, b) _0, _1
#define MAKE_NAMES_3(a, b, c) _0, _1, _2
#define MAKE_NAMES_4(a, b, c, d) _0, _1, _2, _3

/// Overrides a virtual method with a mock implementation which calls a std::function field
/// instead.
#define MOCK_OVERRIDE(ret, name, ...)                                                                                  \
    std::function<ret(__VA_ARGS__)> name##When;                                                                        \
                                                                                                                       \
    ret name(MAKE_DECLARATIONS(__VA_ARGS__)) override                                                                  \
    {                                                                                                                  \
        return this->name##When(MAKE_NAMES(__VA_ARGS__));                                                              \
    }

/// Overrides a virtual const method with a mock implementation which calls a std::function field
/// instead.
#define MOCK_CONST_OVERRIDE(ret, name, ...)                                                                            \
    std::function<ret(__VA_ARGS__)> name##When;                                                                        \
                                                                                                                       \
    ret name(MAKE_DECLARATIONS(__VA_ARGS__)) const override                                                            \
    {                                                                                                                  \
        return this->name##When(MAKE_NAMES(__VA_ARGS__));                                                              \
    }

/// Utility type to check if its destructor is called.
/// Useful for testing data structures.
class DetectDestructor
{
public:
    DetectDestructor(bool* destructed = nullptr)
        : mDestructed(destructed)
    {
    }

    DetectDestructor(DetectDestructor&& other)
        : mDestructed(other.mDestructed)
    {
        other.mDestructed = nullptr;
    }

    ~DetectDestructor()
    {
        if (mDestructed != nullptr)
        {
            *mDestructed = true;
        }
    }

private:
    bool* mDestructed;
};
