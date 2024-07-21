/// @file
/// @brief Class @ref cubos::core::reflection::InheritsTrait.
/// @ingroup core-reflection

#include <cubos/core/reflection/type.hpp>

namespace cubos::core::reflection
{
    /// @brief Provides inheritance relationship between types.
    /// @see See @ref examples-core-reflection-traits-inherits for an example of using this trait.
    /// @ingroup core-reflection
    class CUBOS_CORE_API InheritsTrait
    {
    public:
        CUBOS_REFLECT;

        ~InheritsTrait() = default;

        /// @brief Constructs.
        InheritsTrait(const Type& type)
            : mType(&type)
        {
        }

        /// @brief Adds an inheritance relationship to the type.
        /// @tparam T The parent type from which this type inherits.
        /// @return Trait.
        template <typename T>
        static InheritsTrait from()
        {
            return InheritsTrait(reflect<T>());
        }

        /// @brief Checks if the type inherits from the specified parent type.
        /// @tparam T The parent type to check for inheritance.
        /// @return Whether type inherits from the specified parent type, false otherwise.
        template <typename T>
        bool inherits() const
        {
            return mType->is<T>() || (mType->has<InheritsTrait>() && mType->get<InheritsTrait>().inherits<T>());
        }

        /// @brief Gets the base type of the inherited type.
        /// @return Type.
        const Type& base();

    private:
        const Type* mType;
    };
} // namespace cubos::core::reflection