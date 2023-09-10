/// @file
/// @brief Class @ref cubos::core::reflection::ConstructibleTrait.
/// @ingroup core-reflection

#pragma once

#include <cstddef>

namespace cubos::core::reflection
{
    /// @brief Describes how a reflected type may be constructed and destructed.
    /// @see See @ref examples-core-reflection-traits-constructible for an example of using this
    /// trait.
    /// @ingroup core-reflection
    class ConstructibleTrait
    {
    public:
        /// @brief Function pointer to the destructor of a type.
        /// @param instance Pointer to the instance to destruct.
        using Destructor = void (*)(void* instance);

        /// @brief Function pointer to the default constructor of a type.
        /// @param instance Pointer to the location to construct the instance at.
        using DefaultConstructor = void (*)(void* instance);

        /// @brief Function pointer to the copy constructor of a type.
        /// @param instance Pointer to the location to construct the instance at.
        /// @param other Pointer to the instance to copy construct from.
        using CopyConstructor = void (*)(void* instance, const void* other);

        /// @brief Function pointer to the move constructor of a type.
        /// @param instance Pointer to the location to construct the instance at.
        /// @param other Pointer to the instance to move construct from.
        using MoveConstructor = void (*)(void* instance, void* other);

        /// @brief Builder for @ref ConstructibleTrait.
        template <typename T>
        class Builder;

        /// @brief Constructs.
        /// @param size Size of the type in bytes.
        /// @param alignment Alignment of the type in bytes (must be a power of two).
        /// @param destructor Function pointer to the destructor of the type.
        ConstructibleTrait(std::size_t size, std::size_t alignment, void (*destructor)(void*));

        /// @brief Returns a trait builder for the given type.
        /// @tparam T Type to build a trait for.
        /// @return Trait builder.
        template <typename T>
        static Builder<T> builder();

        /// @brief Sets the default constructor of the type.
        ///
        /// Aborts if the default constructor has already been set.
        ///
        /// @param defaultConstructor Function pointer to the default constructor of the type.
        /// @return Reference to this object.
        ConstructibleTrait& withDefaultConstructor(DefaultConstructor defaultConstructor);

        /// @brief Sets the copy constructor of the type.
        ///
        /// Aborts if the copy constructor has already been set.
        ///
        /// @param copyConstructor Function pointer to the copy constructor of the type.
        /// @return Reference to this object.
        ConstructibleTrait& withCopyConstructor(CopyConstructor copyConstructor);

        /// @brief Sets the move constructor of the type.
        ///
        /// Aborts if the copy constructor has already been set.
        ///
        /// @param moveConstructor Function pointer to the move constructor of the type.
        /// @return Reference to this object.
        ConstructibleTrait& withMoveConstructor(MoveConstructor moveConstructor);

        /// @brief Returns the size of the type in bytes.
        /// @return Size of the type in bytes.
        std::size_t size() const;

        /// @brief Returns the alignment of the type in bytes.
        /// @return Alignment of the type in bytes.
        std::size_t alignment() const;

        /// @brief Destructs an instance of the type.
        /// @param instance Pointer to the instance to destruct.
        void destruct(void* instance) const;

        /// @brief Default constructs an instance of the type.
        /// @param instance Pointer to the location to construct the instance at.
        /// @return Whether default construction is supported.
        bool defaultConstruct(void* instance) const;

        /// @brief Copy constructs an instance of the type.
        /// @param instance Pointer to the location to construct the instance at.
        /// @param other Pointer to the instance to copy construct from.
        /// @return Whether copy construction is supported.
        bool copyConstruct(void* instance, const void* other) const;

        /// @brief Move constructs an instance of the type.
        /// @param instance Pointer to the location to construct the instance at.
        /// @param other Pointer to the instance to move construct from.
        /// @return Whether move construction is supported.
        bool moveConstruct(void* instance, void* other) const;

    private:
        std::size_t mSize;
        std::size_t mAlignment;
        Destructor mDestructor;
        DefaultConstructor mDefaultConstructor;
        CopyConstructor mCopyConstructor;
        MoveConstructor mMoveConstructor;
    };

    template <typename T>
    class ConstructibleTrait::Builder
    {
    public:
        /// @brief Constructs.
        Builder()
            : mTrait(sizeof(T), alignof(T), [](void* instance) { static_cast<T*>(instance)->~T(); })
        {
        }

        /// @brief Returns the constructed trait.
        /// @return Constructed trait.
        ConstructibleTrait build() &&
        {
            return mTrait;
        }

        /// @brief Sets the copy constructor of the type.
        /// @return Builder.
        Builder&& withDefaultConstructor() &&
        {
            mTrait.withDefaultConstructor([](void* instance) { new (instance) T(); });
            return static_cast<Builder&&>(*this);
        }

        /// @brief Sets the copy constructor of the type.
        /// @return Builder.
        Builder&& withCopyConstructor() &&
        {
            mTrait.withCopyConstructor(
                [](void* instance, const void* other) { new (instance) T(*static_cast<const T*>(other)); });
            return static_cast<Builder&&>(*this);
        }

        /// @brief Sets the move constructor of the type.
        /// @return Builder.
        Builder&& withMoveConstructor() &&
        {
            mTrait.withMoveConstructor(
                [](void* instance, void* other) { new (instance) T(static_cast<T&&>(*static_cast<T*>(other))); });
            return static_cast<Builder&&>(*this);
        }

    private:
        ConstructibleTrait mTrait;
    };

    template <typename T>
    ConstructibleTrait::Builder<T> ConstructibleTrait::builder()
    {
        return Builder<T>();
    }
} // namespace cubos::core::reflection
