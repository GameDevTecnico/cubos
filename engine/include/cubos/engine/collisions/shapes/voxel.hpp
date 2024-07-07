/// @file
/// @brief Component @ref cubos::engine::VoxelCollisionShape.
/// @ingroup collisions-plugin

#pragma once

#include <glm/glm.hpp>

#include <cubos/core/geom/box.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/voxels/grid.hpp>

namespace cubos::engine
{
    /// @brief Component which adds a voxel collision shape to an entity, used with a @ref Collider component.
    /// @ingroup collisions-plugin
    class VoxelCollisionShape
    {

    public:
        CUBOS_REFLECT;

        struct BoxShiftPair
        {
            cubos::core::geom::Box box;
            glm::vec3 shift;
        };

        Asset<VoxelGrid> grid; ///< Entities voxel shape.

        VoxelCollisionShape() = default;

        VoxelCollisionShape(Asset<VoxelGrid> grid)
        {
            setGrid(grid);
        }

        ~VoxelCollisionShape() = default;

        VoxelCollisionShape(VoxelCollisionShape&& other) noexcept
        {
            this->grid = std::move(other.grid);
            this->boxes = std::move(other.boxes);
        }

        VoxelCollisionShape(const VoxelCollisionShape& shape)
        {
            this->grid = shape.grid;
            this->boxes = shape.boxes;
        }

        void setGrid(Asset<VoxelGrid>& grid)
        {
            this->grid = grid;
        }

        void insertBox(const cubos::core::geom::Box& box, const glm::vec3& shift)
        {
            BoxShiftPair pair;
            pair.box = box;
            pair.shift = shift;
            this->boxes.push_back(pair);
        }

        std::vector<BoxShiftPair> getBoxes() const
        {
            return this->boxes;
        }

    private:
        std::vector<BoxShiftPair> boxes; ///< List of boxes.
    };
} // namespace cubos::engine
