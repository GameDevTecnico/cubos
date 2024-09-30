/// @file
/// @brief Class @ref cubos::engine::AssetMeta.
/// @ingroup assets-plugin

#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <uuid.h>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Stores metadata about an asset - the data stored in .meta files.
    /// Each asset has a corresponding meta object, which contains load or import parameters.
    ///
    /// Serialization:
    /// - can be serialized and deserialized without context.
    /// - when serialized with the type @ref AssetMeta::Exclude in the context, the specified keys are
    ///   excluded from the serialization.
    ///
    /// @ingroup assets-plugin
    class CUBOS_ENGINE_API AssetMeta final
    {
    public:
        /// @brief Used as context to exclude parameters from being serialized.
        struct Exclude
        {
            /// @brief Keys of the parameters to exclude when serializing.
            std::vector<std::string> keys;
        };

        ~AssetMeta() = default;
        AssetMeta() = default;
        AssetMeta(const AssetMeta&) = default;
        AssetMeta(AssetMeta&&) = default;
        AssetMeta& operator=(const AssetMeta&) = default;
        AssetMeta& operator=(AssetMeta&&) = default;

        /// @brief Gets the value of a parameter on the asset's metadata.
        /// @param key Key of the parameter.
        /// @return The value of the parameter, if the parameter exists.
        std::optional<std::string> get(std::string_view key) const;

        /// @brief Gets the ID of the asset.
        /// @return ID of the asset.
        uuids::uuid getId() const;

        /// @brief Gets the path of the asset.
        /// @return Path of the asset.
        std::string getPath() const;

        /// @brief Sets a parameter on the asset's metadata.
        /// @param key Key of the parameter.
        /// @param value Value of the parameter.
        void set(std::string_view key, std::string_view value);

        /// @brief Removes a parameter from the asset's metadata.
        /// @param key Key of the parameter.
        void remove(std::string_view key);

        /// @brief Gets the parameters of the asset's metadata.
        /// @return Parameters of the asset.
        inline const std::unordered_map<std::string, std::string>& params() const
        {
            return mParams;
        }

        /// @brief Gets the parameters of the asset's metadata.
        /// @return Parameters of the asset.
        inline std::unordered_map<std::string, std::string>& params()
        {
            return mParams;
        }

    private:
        std::unordered_map<std::string, std::string> mParams; ///< Parameters of the asset.
    };
} // namespace cubos::engine
