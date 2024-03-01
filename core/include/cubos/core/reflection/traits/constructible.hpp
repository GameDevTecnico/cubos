/// @file
/// @brief Class @ref cubos::core::reflection::ConstructibleTrait.
/// @ingroup core-reflection

#pragma once

#include <cstddef>

#include <cubos/core/api.hpp>
#include <cubos/core/memory/move.hpp>

namespace cubos::core::reflection
{
    /// @brief Describes how a reflected type may be constructed and destructed.
    /// @see See @ref examples-core-reflection-traits-constructible for an example of using this
    /// trait.
    /// @ingroup core-reflection
    class CUBOS_CORE_API ConstructibleTrait
    {
    public:
        /// @brief Function pointer to the destructor of a type.
        using Destructor = void (*)(void* instance);

        /// @brief Function pointer to the default constructor of a type.
        using DefaultConstructor = void (*)(void* instance);

        /// @brief Function pointer to the copy constructor of a type.
        using CopyConstructor = void (*)(void* instance, const void* other);

        /// @brief Function pointer to the move constructor of a type.
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
        static Builder<T> typed();

        /// @brief Sets the default constructor of the type.
        ///
        /// Aborts if the default constructor has already been set.
        ///
        /// @param defaultConstructor Function pointer to the default constructor of the type.
        /// @return Trait.
        ConstructibleTrait&& withDefaultConstructor(DefaultConstructor defaultConstructor) &&;

        /// @brief Sets the copy constructor of the type.
        ///
        /// Aborts if the copy constructor has already been set.
        ///
        /// @param copyConstructor Function pointer to the copy constructor of the type.
        /// @return Trait.
        ConstructibleTrait&& withCopyConstructor(CopyConstructor copyConstructor) &&;

        /// @brief Sets the move constructor of the type.
        ///
        /// Aborts if the copy constructor has already been set.
        ///
        /// @param moveConstructor Function pointer to the move constructor of the type.
        /// @return Trait.
        ConstructibleTrait&& withMoveConstructor(MoveConstructor moveConstructor) &&;

        /// @brief Returns the size of the type in bytes.
        /// @return Size of the type in bytes.
        std::size_t size() const;

        /// @brief Returns the alignment of the type in bytes.
        /// @return Alignment of the type in bytes.
        std::size_t alignment() const;

        /// @brief Checks if default construction is supported.
        /// @return Whether the operation is supported.
        bool hasDefaultConstruct() const;

        /// @brief Checks if copy construction is supported.
        /// @return Whether the operation is supported.
        bool hasCopyConstruct() const;

        /// @brief Checks if move construction is supported.
        /// @return Whether the operation is supported.
        bool hasMoveConstruct() const;

        /// @brief Destructs an instance of the type.
        /// @param instance Pointer to the instance to destruct.
        void destruct(void* instance) const;

        /// @brief Default constructs an instance of the type.
        /// @note Aborts if @ref hasDefaultConstruct() returns false.
        /// @param instance Pointer to the location to construct the instance at.
        void defaultConstruct(void* instance) const;

        /// @brief Copy constructs an instance of the type.
        /// @note Aborts if @ref hasCopyConstruct() returns false.
        /// @param instance Pointer to the location to construct the instance at.
        /// @param other Pointer to the instance to copy construct from.
        void copyConstruct(void* instance, const void* other) const;

        /// @brief Move constructs an instance of the type.
        /// @note Aborts if @ref hasMoveConstruct() returns false.
        /// @param instance Pointer to the location to construct the instance at.
        /// @param other Pointer to the instance to move construct from.
        void moveConstruct(void* instance, void* other) const;

    private:
        std::size_t mSize;
        std::size_t mAlignment;
        Destructor mDestructor;
        DefaultConstructor mDefaultConstructor{nullptr};
        CopyConstructor mCopyConstructor{nullptr};
        MoveConstructor mMoveConstructor{nullptr};
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

        /// @brief Sets the default constructor of the type.
        /// @return Builder.
        Builder&& withDefaultConstructor() &&
        {
            mTrait = memory::move(mTrait).withDefaultConstructor([](void* instance) { new (instance) T(); });
            return memory::move(*this);
        }

        /// @brief Sets the copy constructor of the type.
        /// @return Builder.
        Builder&& withCopyConstructor() &&
        {
            mTrait = memory::move(mTrait).withCopyConstructor(
                [](void* instance, const void* other) { new (instance) T(*static_cast<const T*>(other)); });
            return memory::move(*this);
        }

        /// @brief Sets the move constructor of the type.
        /// @return Builder.
        Builder&& withMoveConstructor() &&
        {
            mTrait = memory::move(mTrait).withMoveConstructor(
                [](void* instance, void* other) { new (instance) T(memory::move(*static_cast<T*>(other))); });
            return memory::move(*this);
        }

        /// @brief Sets the default, copy and move constructors of the type.
        /// @return Builder.
        Builder&& withBasicConstructors() &&
        {
            return memory::move(*this).withDefaultConstructor().withCopyConstructor().withMoveConstructor();
        }

    private:
        ConstructibleTrait mTrait;
    };

    template <typename T>
    ConstructibleTrait::Builder<T> ConstructibleTrait::typed()
    {
        return Builder<T>();
    }
} // namespace cubos::core::reflection
