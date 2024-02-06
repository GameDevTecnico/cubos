/// @file
/// @brief Function @ref cubos::core::reflection::compare.
/// @ingroup core-reflection
#include <cubos/core/reflection/type.hpp>

namespace cubos::core::reflection
{
    /// @brief Compares two instances of the same type.
    /// @param type The type of the data being compared
    /// @param a One of the instances.
    /// @param b The other instance.
    /// @note If the type has no comparable trait, this function will abort.
    /// @return True if the values are equal, false otherwise.
    bool compare(const Type& type, void* a, void* b);
} // namespace cubos::core::reflection
