#ifndef CUBOS_GL_GRID_HPP
#define CUBOS_GL_GRID_HPP

#include <cubos/memory/serializer.hpp>
#include <cubos/memory/deserializer.hpp>

#include <glm/glm.hpp>
#include <vector>

namespace cubos::gl
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
        
        Grid(Grid&&);
        ~Grid() = default;

        /// Resizes the grid.
        /// @param size The new size of the grid.
        void setSize(const glm::uvec3& size);

        /// @return The size of the grid.
        const glm::uvec3& getSize() const;

        /// Clears the grid.
        void clear();

        /// @param position The position of the voxel.
        /// @param mat The material index to set.
        void set(const glm::ivec3& position, uint16_t mat);

        /// @param position The position of the voxel.
        /// @return The material index at a given position.
        uint16_t get(const glm::ivec3& position) const;

        /// Serializes the grid.
        /// @param serializer The serializer to use.
        void serialize(memory::Serializer& serializer) const;

        /// Deserializes the grid.
        /// @param deserializer The deserializer to use.
        void deserialize(memory::Deserializer& deserializer);

    private:
        glm::uvec3 size;               ///< The size of the grid.
        std::vector<uint16_t> indices; ///< The indices of the grid.
    };
} // namespace cubos::gl

#endif // CUBOS_GL_GRID_HPP
