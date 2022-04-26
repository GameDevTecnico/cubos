#ifndef CUBOS_ENGINE_DATA_META_HPP
#define CUBOS_ENGINE_DATA_META_HPP

#include <cubos/core/memory/serializer.hpp>
#include <cubos/core/memory/deserializer.hpp>

#include <string>
#include <vector>
#include <unordered_map>

namespace cubos::engine::data
{
    /// Describes how an asset should be loaded.
    enum class Usage
    {
        Static,  ///< The asset is loaded once and never unloaded.
        Dynamic, ///< The asset is loaded and unloaded through reference counting.
    };

    /// Class that stores information about an asset.
    /// This data is loaded from the asset's meta file.
    class Meta final
    {
    public:
        Meta() = default;
        Meta(Meta&& rhs);
        ~Meta() = default;

        /// Gets the asset's id.
        /// @return The asset's id.
        const std::string& getId() const;

        /// Gets the asset's type name.
        /// @return The asset's type name.
        const std::string& getType() const;

        /// Gets the asset's usage.
        /// @return The asset's usage.
        Usage getUsage() const;

        /// Gets the parameters of the asset.
        /// @return The parameters of the asset.
        const std::unordered_map<std::string, std::string>& getParameters() const;

        /// Serializes the meta data.
        /// @param serializer The serializer to use.
        void serialize(core::memory::Serializer& serializer) const;

        /// Deserializes the meta data.
        /// @param deserializer The deserializer to use.
        void deserialize(core::memory::Deserializer& deserializer);

    private:
        std::string id;                                          ///< The asset's ID.
        std::string type;                                        ///< The asset's type name.
        Usage usage;                                             ///< The asset's usage.
        std::unordered_map<std::string, std::string> parameters; ///< List of parameters that this asset has.
    };
} // namespace cubos::engine::data

#endif // CUBOS_ENGINE_DATA_META_HPP
