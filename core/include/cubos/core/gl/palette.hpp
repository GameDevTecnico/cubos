/// @file
/// @brief Class @ref cubos::core::gl::Palette.
/// @ingroup core-gl

#pragma once

#include <vector>

#include <cubos/core/gl/material.hpp>

namespace cubos::core::gl
{
    class Palette;
}

namespace cubos::core::data
{
    void serialize(Serializer& serializer, const gl::Palette& palette, const char* name);
    void deserialize(Deserializer& deserializer, gl::Palette& palette);
} // namespace cubos::core::data

namespace cubos::core::gl
{
    /// @brief Holds a palette of materials. Supports up to 65535 materials.
    /// @ingroup core-gl
    class Palette final
    {
    public:
        ~Palette() = default;

        /// @brief Constructs a palette with the given materials.
        /// @param materials Materials to add to the palette.
        Palette(std::vector<Material>&& materials);

        /// @brief Constructs an empty palette.
        Palette() = default;

        /// @brief Gets a pointer to the array of materials on the palette.
        /// @note The first element in the array corresponds to index 1, as index 0 is reserved.
        /// @return Pointer to the array of materials on the palette.
        const Material* data() const;

        /// @brief Gets the number of materials in the palette, excluding the empty material.
        /// @return Number of materials in the palette.
        uint16_t size() const;

        /// @brief Gets the material with the given index.
        /// @param index Index of the material to get (1-based, 0 is empty).
        /// @return Material at the given index.
        const Material& get(uint16_t index) const;

        /// @brief Sets the material at the given index.
        /// @param index Index of the material to set (1-based, 0 is empty).
        /// @param material Material to set.
        void set(uint16_t index, const Material& material);

        /// @brief Searches for the index of the material most similar with the given material.
        /// @param material Material to compare with.
        /// @return Index of the material.
        uint16_t find(const Material& material) const;

        /// @brief Adds a material to the palette, if one not similar enough already exists.
        /// @note Materials equal to Material::Empty will be considered empty and may be replaced
        /// by the new material.
        /// @param material Material to add.
        /// @param similarity Minimum similarity for a material to be considered similar enough.
        /// @return Index of the material in the palette (1-based, 0 is empty).
        uint16_t add(const Material& material, float similarity = 1.0F);

        /// @brief Merges another palette into this one.
        /// @note All materials equal to Material::Empty will be considered empty and may be
        /// overwritten.
        /// @param palette Palette to merge.
        /// @param similarity Minimum similarity for two materials to be merged.
        void merge(const Palette& palette, float similarity = 1.0F);

    private:
        friend void data::serialize(data::Serializer& /*serializer*/, const Palette& /*palette*/, const char* /*name*/);
        friend void data::deserialize(data::Deserializer& /*deserializer*/, Palette& /*palette*/);

        std::vector<Material> mMaterials; ///< Materials in the palette.
    };
} // namespace cubos::core::gl
