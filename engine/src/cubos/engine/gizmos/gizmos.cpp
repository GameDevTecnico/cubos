#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/transform.hpp>

#include <cubos/engine/gizmos/gizmos.hpp>

#include "renderer.hpp"
#include "types/box.hpp"
#include "types/cut_cone.hpp"
#include "types/line.hpp"

using cubos::engine::DeltaTime;
using cubos::engine::Gizmos;
using cubos::engine::GizmosRenderer;

using namespace cubos::core::gl;

Gizmos::Gizmo::Gizmo(const std::string& id, glm::vec3 color, float lifespan)
    : mId(id)
    , mColor(color)
    , mLifespan(lifespan)
{
}

bool Gizmos::Gizmo::decreaseLifespan(float delta)
{
    mLifespan -= delta;
    return mLifespan <= 0;
}

void Gizmos::color(const glm::vec3& color)
{
    mColor = color;
}

void Gizmos::push(const std::shared_ptr<Gizmo>& gizmo, const Space& space)
{
    if (space == Space::World)
    {
        worldGizmos.push_back(gizmo);
    }
    else if (space == Space::View)
    {
        viewGizmos.push_back(gizmo);
    }
    else if (space == Space::Screen)
    {
        screenGizmos.push_back(gizmo);
    }
}

void Gizmos::drawLine(const std::string& id, glm::vec3 from, glm::vec3 to, float lifespan, Space space)
{
    push(std::make_shared<LineGizmo>(id, from, to, mColor, lifespan), space);
}

void Gizmos::drawBox(const std::string& id, glm::vec3 corner, glm::vec3 oppositeCorner, float lifespan, Space space)
{
    push(std::make_shared<BoxGizmo>(id, corner, oppositeCorner, mColor, lifespan), space);
}

void Gizmos::drawWireBox(const std::string& id, glm::vec3 corner, glm::vec3 oppositeCorner, float lifespan, Space space)
{
    // Front
    drawLine(id, corner, glm::vec3{oppositeCorner[0], corner[1], corner[2]}, lifespan, space);
    drawLine(id, corner, glm::vec3{corner[0], oppositeCorner[1], corner[2]}, lifespan, space);
    drawLine(id, glm::vec3{oppositeCorner[0], oppositeCorner[1], corner[2]},
             glm::vec3{corner[0], oppositeCorner[1], corner[2]}, lifespan, space);
    drawLine(id, glm::vec3{oppositeCorner[0], oppositeCorner[1], corner[2]},
             glm::vec3{oppositeCorner[0], corner[1], corner[2]}, lifespan, space);

    // Back
    drawLine(id, oppositeCorner, glm::vec3{corner[0], oppositeCorner[1], oppositeCorner[2]}, lifespan, space);
    drawLine(id, oppositeCorner, glm::vec3{oppositeCorner[0], corner[1], oppositeCorner[2]}, lifespan, space);
    drawLine(id, glm::vec3{corner[0], corner[1], oppositeCorner[2]},
             glm::vec3{corner[0], oppositeCorner[1], oppositeCorner[2]}, lifespan, space);
    drawLine(id, glm::vec3{corner[0], corner[1], oppositeCorner[2]},
             glm::vec3{oppositeCorner[0], corner[1], oppositeCorner[2]}, lifespan, space);

    // Sides
    drawLine(id, corner, glm::vec3{corner[0], corner[1], oppositeCorner[2]}, lifespan, space);
    drawLine(id, glm::vec3{corner[0], oppositeCorner[1], corner[2]},
             glm::vec3{corner[0], oppositeCorner[1], oppositeCorner[2]}, lifespan, space);
    drawLine(id, glm::vec3{oppositeCorner[0], corner[1], corner[2]},
             glm::vec3{oppositeCorner[0], corner[1], oppositeCorner[2]}, lifespan, space);
    drawLine(id, glm::vec3{oppositeCorner[0], oppositeCorner[1], corner[2]}, oppositeCorner, lifespan, space);
}

void Gizmos::drawCutCone(const std::string& id, glm::vec3 firstBaseCenter, float firstBaseRadius,
                         glm::vec3 secondBaseCenter, float secondBaseRadius, float lifespan, Space space)
{
    push(std::make_shared<CutConeGizmo>(id, firstBaseCenter, firstBaseRadius, secondBaseCenter, secondBaseRadius,
                                        mColor, lifespan),
         space);
}
