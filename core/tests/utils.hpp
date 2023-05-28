#pragma once

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
