/// @file
/// @brief Class @ref cubos::engine::Palette.
/// @ingroup voxels-plugin

#pragma once

#include <vector>

#include <cubos/engine/voxels/material.hpp>

namespace cubos::engine
{
    class Palette;
}

namespace cubos::core::data
{
    void serialize(Serializer& serializer, const engine::Palette& palette, const char* name);
    void deserialize(Deserializer& deserializer, engine::Palette& palette);
} // namespace cubos::core::data

namespace cubos::engine
{
    /// @brief Holds a palette of materials. Supports up to 65535 materials.
    ///
    /// Instead of storing the materials themselves in voxel data, @b CUBOS.
    /// uses palettes, and stores the index of the material in the palette
    /// instead.
    ///
    /// This allows for more efficient storage of voxel data, since now instead
    /// of storing the whole material per each voxel, we just store a 16-bit
    /// integer.
    ///
    /// @ingroup voxels-plugin
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
        friend void core::data::serialize(core::data::Serializer& /*serializer*/, const Palette& /*palette*/,
                                          const char* /*name*/);
        friend void core::data::deserialize(core::data::Deserializer& /*deserializer*/, Palette& /*palette*/);

        std::vector<Material> mMaterials; ///< Materials in the palette.
    };
} // namespace cubos::engine
