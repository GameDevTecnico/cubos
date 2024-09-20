/// @file
/// @brief Function @ref cubos::core::reflection::compare.
/// @ingroup core-reflection

#include <cubos/core/api.hpp>

namespace cubos::core::reflection
{
    class Type;

    /// @brief Compares two instances of the same type.
    /// @param type The type of the data being compared
    /// @param a One of the instances.
    /// @param b The other instance.
    /// @note If the type has no comparable trait, this function will abort.
    /// @return True if the values are equal, false otherwise.
    CUBOS_CORE_API bool compare(const Type& type, const void* a, const void* b);
} // namespace cubos::core::reflection
