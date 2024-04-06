/// @file
/// @brief Tree structure that breaks down an asset into its composing boxes.
/// @ingroup collisions-plugin

#pragma once

#include <cubos/core/geom/box.hpp>

namespace cubos::engine
{
    /// @ingroup collisions-plugin
    struct OctreeNode
    {
        cubos::core::geom::Box box; ///< Box shape.
        glm::vec3 shift;            ///< Position relative to the main Boxe's localToWorld
        OctreeNode* topRight;
        OctreeNode* topLeft;
        OctreeNode* bottomRight;
        OctreeNode* bottomLeft;

        OctreeNode(const cubos::core::geom::Box& box, const glm::vec3& shift)
            : box(box)
            , shift(shift)
        {
        }
    };

    inline void divideIntoOctree(OctreeNode& node)
    {
        if (node.box.halfSize[0] <= 1.5F || node.box.halfSize[1] <= 1.5F || node.box.halfSize[2] <= 1.5)
        {
            node.topRight = nullptr;
            node.topLeft = nullptr;
            node.bottomRight = nullptr;
            node.bottomLeft = nullptr;
            return;
        }

        cubos::core::geom::Box quarterBox;
        quarterBox.halfSize = node.box.halfSize / 2.0F;

        node.topRight =
            new OctreeNode(quarterBox, {quarterBox.halfSize[0], quarterBox.halfSize[1], quarterBox.halfSize[2]});
        node.topLeft =
            new OctreeNode(quarterBox, {quarterBox.halfSize[0], -quarterBox.halfSize[1], quarterBox.halfSize[2]});
        node.bottomRight =
            new OctreeNode(quarterBox, {quarterBox.halfSize[0], quarterBox.halfSize[1], -quarterBox.halfSize[2]});
        node.bottomLeft =
            new OctreeNode(quarterBox, {quarterBox.halfSize[0], -quarterBox.halfSize[1], -quarterBox.halfSize[2]});

        divideIntoOctree(*node.topRight);
        divideIntoOctree(*node.topLeft);
        divideIntoOctree(*node.bottomRight);
        divideIntoOctree(*node.bottomLeft);
    }

    inline void deleteNode(OctreeNode& node)
    {
        if (node.box.halfSize[0] <= 1.5F || node.box.halfSize[1] <= 1.5F || node.box.halfSize[2] <= 1.5)
        {
            delete (&node);
            return;
        }
        deleteNode(*node.topRight);
        deleteNode(*node.topLeft);
        deleteNode(*node.bottomRight);
        deleteNode(*node.bottomLeft);
        delete (&node);
    }

} // namespace cubos::engine