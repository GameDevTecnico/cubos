#ifndef CUBOS_ENGINE_DATA_META_HPP
#define CUBOS_ENGINE_DATA_META_HPP

#include <cubos/core/data/serializer.hpp>
#include <cubos/core/data/deserializer.hpp>

#include <string>
#include <vector>
#include <unordered_map>

namespace cubos::engine::data
{
    class Meta;
}

namespace cubos::core::data
{
    /// Serializes asset meta data.
    /// @param serializer The serializer to use.
    /// @param meta The meta data to serialize.
    /// @param name The name of the meta data.
    void serialize(Serializer& serializer, const engine::data::Meta& meta, const char* name);

    /// Deserializes asset meta data.
    /// @param deserializer The deserializer to use.
    /// @param meta The meta data to deserialize.
    void deserialize(Deserializer& deserializer, engine::data::Meta& meta);
} // namespace cubos::core::data

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
        Meta(const std::string& id, const std::string& type, Usage usage);
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

        /// Checks if the asset was stored instead of being loaded.
        /// @return True if the asset was stored, false otherwise.
        bool isStored() const;

        /// Gets the parameters of the asset.
        /// @return The parameters of the asset.
        const std::unordered_map<std::string, std::string>& getParameters() const;

    private:
        friend void core::data::serialize(core::data::Serializer&, const Meta&, const char*);
        friend void core::data::deserialize(core::data::Deserializer&, Meta&);

        std::string id;                                          ///< The asset's ID.
        std::string type;                                        ///< The asset's type name.
        Usage usage;                                             ///< The asset's usage.
        bool stored;                                             ///< True if the asset was stored, false otherwise.
        std::unordered_map<std::string, std::string> parameters; ///< List of parameters that this asset has.
    };
} // namespace cubos::engine::data

#endif // CUBOS_ENGINE_DATA_META_HPP
