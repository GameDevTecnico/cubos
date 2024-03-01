/// @file
/// @brief Macro @ref CUBOS_CORE_API.
/// @ingroup core

#pragma once

/// @brief Macro used to export and import symbols from the library.
/// @addtogroup core
/// @{

#if defined(_WIN32) || defined(_WIN64)
#if defined(CUBOS_CORE_EXPORT)
#define CUBOS_CORE_API __declspec(dllexport)
#elif defined(CUBOS_CORE_IMPORT)
#define CUBOS_CORE_API __declspec(dllimport)
#else
#define CUBOS_CORE_API
#endif
#else
#define CUBOS_CORE_API
#endif

/// @}
