#pragma once

/// Utility macro which adds two subcases to the current test case, one for true and one for false.
#define PARAMETRIZE_TRUE_OR_FALSE(what, var)                                                                           \
    do                                                                                                                 \
    {                                                                                                                  \
        SUBCASE(what "is true")                                                                                        \
        {                                                                                                              \
            var = true;                                                                                                \
        }                                                                                                              \
        SUBCASE(what "is false")                                                                                       \
        {                                                                                                              \
            var = false;                                                                                               \
        }                                                                                                              \
    } while (false)

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
