#pragma once

#include <vector>

#include <cubos/core/gl/material.hpp>

namespace cubos::core::gl
{
    class Palette;
}

namespace cubos::core::old::data
{
    /// Serializes a material palette.
    /// @param serializer The serializer to use.
    /// @param palette The palette to serialize.
    /// @param name The name of the palette.
    void serialize(Serializer& serializer, const gl::Palette& palette, const char* name);

    /// Deserializes a material palette.
    /// @param deserializer The deserializer to use.
    /// @param palette The palette to deserialize.
    void deserialize(Deserializer& deserializer, gl::Palette& palette);
} // namespace cubos::core::old::data

namespace cubos::core::gl
{
    /// Represents a palette of materials. Supports up to 65535 materials.
    class Palette final
    {
    public:
        /// @param materials The materials to add to the palette.
        Palette(std::vector<Material>&& materials);

        Palette() = default;
        ~Palette() = default;

        /// @return Pointer to the array of materials on the palette.
        const Material* data() const;

        /// @return The number of materials in the palette.
        uint16_t size() const;

        /// @param index The index of the material to get (1-based, 0 is empty).
        /// @return The material at the given index.
        const Material& get(uint16_t index) const;

        /// @param index The index of the material to set (1-based, 0 is empty).
        /// @param material The material to set.
        void set(uint16_t index, const Material& material);

        /// Searches for the index of the material most similar to the given material.
        /// @param material The material to search for.
        /// @return The index of the material.
        uint16_t find(const Material& material) const;

        /// Adds a material to the palette, if one not similar enough already exists.
        /// Materials equal to Material::Empty will be considered empty and may be replaced by the new material.
        /// @param material The material to add.
        /// @param similarity The minimum similarity for a material to be considered similar enough.
        /// @return The index of the material in the palette.
        uint16_t add(const Material& material, float similarity = 1.0F);

        /// Merges another palette into this one. All materials equal to Material::Empty will be considered empty and
        /// may be overwritten.
        /// @param palette The palette to merge.
        /// @param similarity The minimum similarity for two materials to be merged.
        void merge(const Palette& palette, float similarity = 1.0F);

    private:
        friend void old::data::serialize(old::data::Serializer& /*serializer*/, const Palette& /*palette*/, const char* /*name*/);
        friend void old::data::deserialize(old::data::Deserializer& /*deserializer*/, Palette& /*palette*/);

        std::vector<Material> mMaterials; ///< The materials in the palette.
    };
} // namespace cubos::core::gl
