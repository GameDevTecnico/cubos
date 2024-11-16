/// @file
/// @brief Metrics and profiling utilities and macros.
/// @ingroup core-tel

#pragma once

#include <chrono>
#include <string>

#include <cubos/core/api.hpp>
#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::tel
{
    /// @brief Singleton class that manages a collection of metrics.
    /// @ingroup core
    class CUBOS_CORE_API Metrics
    {
    public:
        /// @brief Deleted constructor.
        Metrics() = delete;

        /// @brief Add a metric in the pool.
        /// @param metric The metric to add.
        static void metric(const std::string& name, double metric);

        /// @brief Size of the metric pool.
        /// @return Metrics count.
        static std::size_t size();

        /// @brief Size of metrics associated with a given name.
        /// @param name Metric name.
        /// @return Metrics count.
        static std::size_t sizeByName(const std::string& name);

        /// @brief Clears the metric pool
        static void clear();

        /// @brief Sets maximum metric entries.
        /// @param n The new maximum value.
        static void setMaxEntries(std::size_t n);

        /// @brief Read a metric value by its name.
        /// @param name Metric name.
        /// @param[out] value Buffer to store metric value.
        /// @param offset Read count per metric.
        /// @return Whether the metric was found.
        static bool readValue(const std::string& name, double& value, size_t& offset);

        /// @brief Search for a new unique metric name.
        /// @param[out] name Buffer to store metric name.
        /// @param seenCount Seen metrics count.
        /// @return Whether a new metric was found.
        static bool readName(std::string& name, size_t& seenCount);
    };

} // namespace cubos::core::tel

/// @def CUBOS_METRIC
/// @ingroup core
/// @brief Macro to add a metric.
/// @param name The name of the metric.
/// @param val The value to set for the metric.

#ifdef CUBOS_CORE_PROFILING
#define CUBOS_METRIC(name, val) ::cubos::core::tel::Metrics::metric(name, val)
#else
#define CUBOS_METRIC(...)                                                                                              \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (false)
#endif
