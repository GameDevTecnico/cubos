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
        /// Used to specify the material index width of the grid.
        enum class IndexWidth
        {
            U8,  ///< 1-byte material index, works with up to 255 materials.
            U16, ///<  2-byte material indices, works with up to 65535 materials.
        };

        /// @param size The size of the grid.
        /// @param width The material index width to use.
        Grid(const glm::ivec3& size, IndexWidth width);

        /// @param size The size of the grid.
        /// @param indices The material indices of the voxels.
        Grid(const glm::ivec3& size, const std::vector<uint8_t>& indices);

        /// @param size The size of the grid.
        /// @param indices The material indices of the voxels.
        Grid(const glm::ivec3& size, const std::vector<uint16_t>& indices);

        Grid();
        ~Grid();

        /// Changes the material index width to use. This will clear the grid if the width is changed.
        /// @param width The new material index width to use.
        void setIndexWidth(IndexWidth width);

        /// @return The material index width used.
        IndexWidth getIndexWidth() const;

        /// Resizes the grid.
        /// @param size The new size of the grid.
        void setSize(const glm::ivec3& size);

        /// @return The size of the grid.
        const glm::uvec3& getSize() const;

        /// @param position The position of the voxel.
        /// @param mat The material index to set.
        void set(const glm::ivec3& position, size_t mat);

        /// @param position The position of the voxel.
        /// @return The material index at a given position.
        size_t get(const glm::ivec3& position) const;

        /// Serializes the grid.
        /// @param serializer The serializer to use.
        void serialize(memory::Serializer& serializer) const;

        /// Deserializes the grid.
        /// @param deserializer The deserializer to use.
        void deserialize(memory::Deserializer& deserializer);

    private:
        IndexWidth width; ///< The index width to use.
        glm::ivec3 size;  ///< The size of the grid.

        union {
            std::vector<uint8_t> shortIndices; ///< The indices of the grid.
            std::vector<uint16_t> longIndices; ///< The indices of the grid.
        };
    };
} // namespace cubos::gl

#endif // CUBOS_GL_GRID_HPP
