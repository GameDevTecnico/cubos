#pragma once

#include <vector>

#include <glm/glm.hpp>

#include <cubos/core/data/deserializer.hpp>
#include <cubos/core/data/serializer.hpp>

namespace cubos::core::gl
{
    class Palette;
    class Grid;
} // namespace cubos::core::gl

namespace cubos::core::old::data
{
    /// Serializes a voxel grid.
    /// @param serializer The serializer to use.
    /// @param grid The voxel grid to serialize.
    /// @param name The name of the voxel grid.
    void serialize(Serializer& serializer, const gl::Grid& grid, const char* name);

    /// Deserializes a voxel grid.
    /// @param deserializer The deserializer to use.
    /// @param grid The voxel grid to deserialize.
    void deserialize(Deserializer& deserializer, gl::Grid& grid);
} // namespace cubos::core::old::data

namespace cubos::core::gl
{
    /// Represents a grid of voxels.
    /// Voxel indices are determined by the following formula: x + y * size.x + z * size.x * size.y
    class Grid final
    {
    public:
        // Default constructor.
        Grid();

        /// @param size The size of the grid.
        /// @param width The material index width to use.
        Grid(const glm::uvec3& size);

        /// @param size The size of the grid.
        /// @param indices The material indices of the voxels.
        Grid(const glm::uvec3& size, const std::vector<uint16_t>& indices);

        Grid(Grid&& /*other*/) noexcept;
        ~Grid() = default;

        Grid& operator=(const Grid& rhs);

        /// Resizes the grid.
        /// @param size The new size of the grid.
        void setSize(const glm::uvec3& size);

        /// @return The size of the grid.
        const glm::uvec3& size() const;

        /// Clears the grid.
        void clear();

        /// @param position The position of the voxel.
        /// @param mat The material index to set.
        void set(const glm::ivec3& position, uint16_t mat);

        /// @param position The position of the voxel.
        /// @return The material index at a given position.
        uint16_t get(const glm::ivec3& position) const;

        /// Converts the material indices of this grid from one palette to another.
        /// For each index, it will search for another index in the second palette which matches the first palette.
        /// The conversion fails if no matching index is found.
        /// @param src The original palette.
        /// @param dst The new palette.
        /// @param minSimilarity The minimum similarity between two materials to consider them the same.
        /// @return Whether the conversion was successful.
        bool convert(const Palette& src, const Palette& dst, float minSimilarity);

    private:
        friend void old::data::serialize(old::data::Serializer& /*serializer*/, const Grid& /*grid*/, const char* /*name*/);
        friend void old::data::deserialize(old::data::Deserializer& /*deserializer*/, Grid& /*grid*/);

        glm::uvec3 mSize;               ///< The size of the grid.
        std::vector<uint16_t> mIndices; ///< The indices of the grid.
    };
} // namespace cubos::core::gl
