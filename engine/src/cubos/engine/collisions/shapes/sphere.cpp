#include <cubos/engine/collisions/shapes/sphere.hpp>

using cubos::engine::Manifold;

bool cubos::engine::checkCollision(const SphereShape& sphere1, const SphereShape& sphere2, Manifold& manifold)
{
    // Calculate distance between sphere centers
    auto offset = sphere1.position - sphere2.position;
    auto distSq = glm::dot(offset, offset);

    // Check if spheres are colliding
    auto radiiSum = sphere1.radius + sphere2.radius;
    if (distSq <= radiiSum * radiiSum)
    {
        // Calculate penetration depth and collision normal
        auto dist = glm::sqrt(distSq);
        manifold.normal = offset / dist;
        manifold.depth = radiiSum - dist;
        return true;
    }

    return false;
}
