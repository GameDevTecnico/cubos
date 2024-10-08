/// @file
/// @brief Class @ref cubos::engine::VoxelPalette.
/// @ingroup voxels-plugin

#pragma once

#include <vector>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/assets/bridges/file.hpp>
#include <cubos/engine/voxels/material.hpp>

namespace cubos::engine
{
    class VoxelPalette;
}

namespace cubos::engine
{
    /// @brief Holds a palette of materials. Supports up to 65535 materials.
    ///
    /// Instead of storing the materials themselves in voxel data, @b Cubos
    /// uses palettes, and stores the index of the material in the palette
    /// instead.
    ///
    /// This allows for more efficient storage of voxel data, since now instead
    /// of storing the whole material per each voxel, we just store a 16-bit
    /// integer.
    ///
    /// @ingroup voxels-plugin
    class CUBOS_ENGINE_API VoxelPalette final
    {
    public:
        static constexpr uint32_t MaxMaterials = 65536;

        /// @brief Used to iterate over materials on the palette.
        class Iterator;

        CUBOS_REFLECT;

        ~VoxelPalette() = default;

        /// @brief Constructs a palette with the given materials.
        /// @param materials Materials to add to the palette.
        VoxelPalette(std::vector<VoxelMaterial>&& materials);

        /// @brief Constructs an empty palette.
        VoxelPalette() = default;

        /// @brief Copy constructs.
        VoxelPalette(const VoxelPalette&) = default;

        /// @brief Move constructs.
        VoxelPalette(VoxelPalette&&) noexcept = default;

        /// @brief Copy assigns.
        VoxelPalette& operator=(const VoxelPalette&) = default;

        /// @brief Move assigns.
        VoxelPalette& operator=(VoxelPalette&&) noexcept = default;

        /// @brief Gets a pointer to the array of materials on the palette.
        /// @note The first element in the array corresponds to index 1, as index 0 is reserved.
        /// @return Pointer to the array of materials on the palette.
        const VoxelMaterial* data() const;

        /// @brief Gets the number of materials in the palette, excluding the empty material.
        /// @return Number of materials in the palette.
        uint16_t size() const;

        /// @brief Gets the material with the given index.
        /// @param index Index of the material to get (1-based, 0 is empty).
        /// @return Material at the given index.
        const VoxelMaterial& get(uint16_t index) const;

        /// @brief Sets the material at the given index.
        /// @param index Index of the material to set (1-based, 0 is empty).
        /// @param material Material to set.
        void set(uint16_t index, const VoxelMaterial& material);

        /// @brief Searches for the index of the material most similar with the given material.
        /// @param material Material to compare with.
        /// @return Index of the material.
        uint16_t find(const VoxelMaterial& material) const;

        /// @brief Adds a material to the palette, if one not similar enough already exists.
        /// @note Materials equal to @ref VoxelMaterial::Empty will be considered empty and may be
        /// replaced by the new material.
        /// @param material Material to add.
        /// @param similarity Minimum similarity for a material to be considered similar enough.
        /// @return Index of the material in the palette (1-based, 0 is empty).
        uint16_t add(const VoxelMaterial& material, float similarity = 1.0F);

        /// @brief Pushes a material to the palette without checking for uniqueness.
        /// @note If the palette is already at its maximum capacity, this function will abort.
        /// @param material Material to push.
        /// @return Size of the palette.
        uint16_t push(const VoxelMaterial& material);

        /// @brief Merges another palette into this one.
        /// @note All materials equal to @ref VoxelMaterial::Empty will be considered empty and may be
        /// overwritten.
        /// @param palette Palette to merge.
        /// @param similarity Minimum similarity for two materials to be merged.
        void merge(const VoxelPalette& palette, float similarity = 1.0F);

        /// @brief Returns an iterator to the first material.
        /// @return Iterator.
        Iterator begin();

        /// @brief Returns an iterator to the last material.
        /// @return Iterator.
        Iterator end();

        /// @brief Loads the palette's data from the given stream.
        ///
        /// Assumes the data is stored in big-endian (network byte order).
        /// The first bytes correspond to an uint16_t, which represents the number of materials in the palette.
        /// The next bytes correspond to `numMaterials * 4` floats (each material is represented by 4 floats (r, g, b,
        /// a)), which represents the actual palette data.
        ///
        /// @param stream Stream to read from.
        /// @return Whether the stream contained valid data.
        bool loadFrom(core::memory::Stream& stream);

        /// @brief Writes the palette's data to the given stream.
        ///
        /// Writes in the format specified in @ref loadFrom.
        ///
        /// @param stream Stream to write to.
        /// @return Whether the write was successful.
        bool writeTo(core::memory::Stream& stream) const;

    private:
        std::vector<VoxelMaterial> mMaterials; ///< Materials in the palette.
    };

    /// @brief Used to iterate over materials on the palette.
    class VoxelPalette::Iterator
    {
    public:
        Iterator(std::vector<VoxelMaterial>::iterator it);
        Iterator& operator++();
        VoxelMaterial& operator*();
        bool operator==(const Iterator& other) const;

    private:
        std::vector<VoxelMaterial>::iterator mIter;
    };
} // namespace cubos::engine
