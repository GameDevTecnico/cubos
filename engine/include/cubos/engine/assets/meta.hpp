#pragma once

#include <unordered_map>
#include <optional>
#include <string>
#include <vector>

namespace cubos::engine
{
    /// Stores metadata about an asset - the data stored in .meta files.
    /// Each asset has a corresponding meta object, which contains load or import parameters.
    ///
    /// @details Serialization:
    /// - can be serialized and deserialized without context.
    /// - when serialized with the type `AssetMeta::Exclude` in the context, the specified keys are
    ///   excluded from the serialization.
    class AssetMeta final
    {
    public:
        /// Used as context to exclude parameters from being serialized.
        struct Exclude
        {
            /// Keys of the parameters to exclude when serializing.
            std::vector<std::string> keys;
        };

        ~AssetMeta() = default;
        AssetMeta() = default;
        AssetMeta(const AssetMeta&) = default;
        AssetMeta(AssetMeta&&) = default;

        /// @param key Key of the parameter.
        /// @returns The value of the parameter, if the parameter exists.
        std::optional<std::string> get(const std::string& key) const;

        /// Sets a parameter for the asset's metadata.
        /// @param key Key of the parameter.
        /// @param value Value of the parameter.
        void set(const std::string& key, const std::string& value);

        /// Removes a parameter from the asset's metadata.
        /// @param key Key of the parameter.
        void remove(const std::string& key);

        /// @returns Parameters of the asset.
        inline const std::unordered_map<std::string, std::string>& params() const
        {
            return this->m_params;
        }

        /// @returns Parameters of the asset.
        inline std::unordered_map<std::string, std::string>& params()
        {
            return this->m_params;
        }

    private:
        std::unordered_map<std::string, std::string> m_params; ///< Parameters of the asset.
    };
} // namespace cubos::engine
