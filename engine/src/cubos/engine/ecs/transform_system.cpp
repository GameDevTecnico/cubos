#include <cubos/engine/ecs/transform_system.hpp>

#include <glm/gtc/matrix_transform.hpp>

void cubos::engine::ecs::transformSystem(
    core::ecs::Query<LocalToWorld&, const Position*, const Rotation*, const Scale*> query)
{
    for (auto [entity, localToWorld, position, rotation, scale] : query)
    {
        localToWorld.mat = glm::mat4(1.0f);
        if (position != nullptr)
            localToWorld.mat = glm::translate(localToWorld.mat, position->vec);
        if (rotation != nullptr)
            localToWorld.mat *= glm::toMat4(rotation->quat);
        if (scale != nullptr)
            localToWorld.mat = glm::scale(localToWorld.mat, glm::vec3(scale->factor));
        printf("%p %p %p\n", position, rotation, scale);
    }
}
