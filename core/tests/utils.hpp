#pragma once

/// Utility type to check if its destructor is called.
/// Useful for testing data structures.
class DetectDestructor
{
public:
    DetectDestructor(bool& destructed)
        : mDestructed(destructed)
        , mMoved(false)
    {
    }

    DetectDestructor(DetectDestructor&& other)
        : mDestructed(other.mDestructed)
        , mMoved(false)
    {
        other.mMoved = true;
    }

    ~DetectDestructor()
    {
        if (!mMoved)
        {
            mDestructed = true;
        }
    }

private:
    bool& mDestructed;
    bool mMoved; ///< Needed to prevent detecting destructor calls on moved values.
};
