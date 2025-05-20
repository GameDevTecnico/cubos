/// @file
/// @brief Class @ref cubos::core::reflection::ConstructibleTrait.
/// @ingroup core-reflection

#pragma once

#include <cstddef>
#include <utility>
#include <vector>

#include <cubos/core/api.hpp>
#include <cubos/core/memory/function.hpp>
#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::reflection
{
    /// @brief Describes how a reflected type may be constructed and destructed.
    /// @see See @ref examples-core-reflection-traits-constructible for an example of using this
    /// trait.
    /// @ingroup core-reflection
    class CUBOS_CORE_API ConstructibleTrait
    {
    public:
        CUBOS_REFLECT;

        /// @brief Destructor of a type.
        using Destructor = memory::Function<void(void* instance) const>;

        /// @brief Default constructor of a type.
        using DefaultConstructor = memory::Function<void(void* instance) const>;

        /// @brief Copy constructor of a type.
        using CopyConstructor = memory::Function<void(void* instance, const void* other) const>;

        /// @brief Move constructor of a type.
        using MoveConstructor = memory::Function<void(void* instance, void* other) const>;

        /// @brief Custom constructor of a type.
        class CustomConstructor
        {
        public:
            /// @brief Constructs a custom constructor.
            /// @param argTypes Types of the arguments of the constructor.
            /// @param argNames Names of the arguments of the constructor.
            /// @param functor Functor taking the array of arguments to be passed.
            CustomConstructor(std::vector<const Type*> argTypes, std::vector<std::string> argNames,
                              memory::Function<void(void* instance, void** args) const> functor);

            /// @brief Move constructs.
            CustomConstructor(CustomConstructor&& other) = default;

            /// @brief Copy constructs.
            CustomConstructor(const CustomConstructor& other) = default;

            /// @brief Copy assignment.
            CustomConstructor& operator=(CustomConstructor&& other) = default;

            /// @brief Gets the number of arguments to the constructor.
            /// @return Number of arguments to the constructor.
            std::size_t argCount() const;

            /// @brief Gets the name of an argument of the constructor.
            /// @param index Index of the argument.
            /// @return Name of the argument.
            const std::string& argName(std::size_t index) const;

            /// @brief Gets the type of an argument of the constructor.
            /// @param index Index of the argument.
            /// @return Type of the argument.
            const Type& argType(std::size_t index) const;

            /// @brief Calls the constructor with the given arguments.
            /// @param instance Pointer to the instance to construct.
            /// @param args Array of pointers to the arguments to pass to the constructor.
            void call(void* instance, void** args) const;

        private:
            /// @brief Types of the arguments of the constructor.
            std::vector<const Type*> mArgTypes;

            /// @brief Names of the arguments of the constructor.
            std::vector<std::string> mArgNames;

            /// @brief Functor taking the array of arguments to be passed.
            memory::Function<void(void* instance, void** args) const> mFunctor;
        };

        /// @brief Builder for @ref ConstructibleTrait.
        template <typename T>
        class Builder;

        /// @brief Constructs.
        /// @param size Size of the type in bytes.
        /// @param alignment Alignment of the type in bytes (must be a power of two).
        /// @param destructor Destructor of the type.
        ConstructibleTrait(std::size_t size, std::size_t alignment, Destructor destructor);

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

        /// @brief Adds a custom constructor to the type.
        /// @param customConstructor Custom constructor to add.
        /// @return Trait.
        ConstructibleTrait&& withCustomConstructor(CustomConstructor customConstructor) &&;

        /// @brief Adds a custom constructor to the type.
        /// @param customConstructor Custom constructor to add.
        void addCustomConstructor(CustomConstructor customConstructor);

        /// @brief Adds a custom constructor to the type.
        /// @tparam Args Argument types of the constructor.
        /// @param argNames Names of the arguments to the constructor.
        template <typename T, typename... Args>
        void addCustomConstructor(std::initializer_list<std::string> argNames)
        {
            addCustomConstructor(CustomConstructor({&reflect<Args>()...}, argNames, [](void* instance, void** args) {
                auto callConstructor = [&]<std::size_t... Is>(std::index_sequence<Is...>) {
                    new (instance) T(*static_cast<Args*>(args[Is])...);
                };
                callConstructor(std::index_sequence_for<Args...>());
            }));
        }

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

        /// @brief Returns the number of custom constructors.
        /// @return Number of custom constructors.
        std::size_t customConstructorCount() const;

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

        /// @brief Calls a custom constructor.
        /// @note Aborts if the index is out of bounds.
        /// @param index Index of the custom constructor to call.
        /// @param instance Pointer to the location to construct the instance at.
        /// @param args Array of pointers to the arguments to pass to the constructor.
        void customConstruct(std::size_t index, void* instance, void** args) const;

        /// @brief Gets the destructor of the type.
        /// @return Destructor of the type.
        Destructor destructor() const;

        /// @brief Gets the default constructor of the type.
        /// @return Default constructor of the type.
        DefaultConstructor defaultConstructor() const;

        /// @brief Gets the copy constructor of the type.
        /// @return Copy constructor of the type.
        CopyConstructor copyConstructor() const;

        /// @brief Gets the move constructor of the type.
        /// @return Move constructor of the type.
        MoveConstructor moveConstructor() const;

        /// @brief Gets the custom constructor of the type.
        /// @param index Index of the custom constructor.
        /// @return Custom constructor of the type.
        const CustomConstructor& customConstructor(std::size_t index) const;

    private:
        std::size_t mSize;
        std::size_t mAlignment;
        Destructor mDestructor;
        DefaultConstructor mDefaultConstructor{};
        CopyConstructor mCopyConstructor{};
        MoveConstructor mMoveConstructor{};
        std::vector<CustomConstructor> mCustomConstructors{};
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

        /// @brief Constructs.
        Builder(ConstructibleTrait trait)
            : mTrait(memory::move(trait))
        {
        }

        /// @brief Returns the constructed trait.
        /// @return Constructed trait.
        ConstructibleTrait build() &&
        {
            return memory::move(mTrait);
        }

        /// @brief Sets the default constructor of the type.
        /// @return Builder.
        Builder withDefaultConstructor() &&
        {
            return {memory::move(mTrait).withDefaultConstructor([](void* instance) { new (instance) T(); })};
        }

        /// @brief Sets the copy constructor of the type.
        /// @return Builder.
        Builder withCopyConstructor() &&
        {
            return {memory::move(mTrait).withCopyConstructor(
                [](void* instance, const void* other) { new (instance) T(*static_cast<const T*>(other)); })};
        }

        /// @brief Sets the move constructor of the type.
        /// @return Builder.
        Builder withMoveConstructor() &&
        {
            return {memory::move(mTrait).withMoveConstructor(
                [](void* instance, void* other) { new (instance) T(memory::move(*static_cast<T*>(other))); })};
        }

        /// @brief Sets the default, copy and move constructors of the type.
        /// @return Builder.
        Builder withBasicConstructors() &&
        {
            return memory::move(*this).withDefaultConstructor().withCopyConstructor().withMoveConstructor();
        }

        /// @brief Adds a custom constructor to the type.
        /// @tparam Argument types of the arguments to the constructor.
        /// @param argNames Names of the arguments to the constructor.
        /// @return Builder.
        template <typename... Args>
        Builder withCustomConstructor(std::initializer_list<std::string> argNames) &&
        {
            mTrait.addCustomConstructor<T, Args...>(argNames);
            return memory::move(*this);
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
