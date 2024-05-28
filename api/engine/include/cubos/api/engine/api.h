/// @file
/// @brief Macro @ref CUBOS_ENGINE_C_API.
/// @ingroup engine

#pragma once

/// @brief Macro used to export and import symbols from the library.
/// @addtogroup engine
/// @{

#if defined(_WIN32) || defined(_WIN64)
#if defined(CUBOS_ENGINE_C_EXPORT)
#define CUBOS_ENGINE_C_API __declspec(dllexport)
#elif defined(CUBOS_ENGINE_C_IMPORT)
#define CUBOS_ENGINE_C_API __declspec(dllimport)
#else
#define CUBOS_ENGINE_C_API
#endif
#else
#define CUBOS_ENGINE_C_API
#endif

/// @}
