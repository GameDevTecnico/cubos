/// @file
/// @brief Macro @ref CUBOS_CORE_C_API.
/// @ingroup core-api

#pragma once

/// @brief Macro used to export and import symbols from the library.
/// @addtogroup core-api
/// @{

#if defined(_WIN32) || defined(_WIN64)
#if defined(CUBOS_CORE_C_EXPORT)
#define CUBOS_CORE_C_API __declspec(dllexport)
#elif defined(CUBOS_CORE_C_IMPORT)
#define CUBOS_CORE_C_API __declspec(dllimport)
#else
#define CUBOS_CORE_C_API
#endif
#else
#define CUBOS_CORE_C_API
#endif

/// @}
