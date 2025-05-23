/// @file
/// @brief Component @ref cubos::engine::VoxelCollisionShape.
/// @ingroup collisions-plugin

#pragma once

#include <glm/glm.hpp>

#include <cubos/core/geom/box.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>
#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/voxels/grid.hpp>

namespace cubos::engine
{
    /// @brief Component which adds a collision shape corresponding to a given voxel grid to an entity, used with a @ref
    /// Collider component.
    ///
    /// Assumes the VoxelGrid to be centered in the entity space.
    /// @ingroup collisions-plugin
    class CUBOS_ENGINE_API VoxelCollisionShape
    {
    public:
        CUBOS_REFLECT;

        /// @brief Struct which holds a sub-box of the voxel collision shape, and its shift from the center of the
        /// shape.
        /// @ingroup collisions-plugin
        struct BoxShiftPair
        {
            cubos::core::geom::Box box;
            glm::vec3 shift;
            uint32_t boxId;
        };

        /// @brief Entities voxel grid.
        Asset<VoxelGrid> grid;

        bool changed{true};

        /// @brief Constructs voxel shape with no grid.
        VoxelCollisionShape() = default;

        /// @brief Constructs voxel shape with voxel grid.
        /// @param grid VoxelGrid given in constructor.
        VoxelCollisionShape(Asset<VoxelGrid> grid)
        {
            setGrid(grid);
        }

        /// @brief Default destructor.
        ~VoxelCollisionShape() = default;

        /// @brief Move constructor.
        /// @param other VoxelCollisionShape to move.
        VoxelCollisionShape(VoxelCollisionShape&& other) noexcept
        {
            this->grid = std::move(other.grid);
            this->mBoxes = std::move(other.mBoxes);
        }

        /// @brief Copy constructor.
        /// @param shape VoxelCollisionSHape to copy.
        VoxelCollisionShape(const VoxelCollisionShape& shape)
        {
            this->grid = shape.grid;
            this->mBoxes = shape.mBoxes;
        }

        /// @brief Sets the grid.
        /// @param grid to set.
        void setGrid(Asset<VoxelGrid>& grid)
        {
            this->grid = grid;
        }

        /// @brief Inserts a new @ref BoxShiftPair to the list of the class.
        /// @param box Box to insert.
        /// @param shift Shift vector of the box.
        void insertBox(const cubos::core::geom::Box& box, const glm::vec3& shift, uint32_t boxId)
        {
            BoxShiftPair pair;
            pair.box = box;
            pair.shift = shift;
            pair.boxId = boxId;
            this->mBoxes.push_back(pair);
        }

        /// @brief Getter for the list of @ref BoxShiftPair of the class.
        const std::vector<BoxShiftPair>& getBoxes() const
        {
            return this->mBoxes;
        }

    private:
        /// @brief List of pairs composing the shape.
        std::vector<BoxShiftPair> mBoxes; ///< List of boxes.
    };
} // namespace cubos::engine
