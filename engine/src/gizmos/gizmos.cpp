#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/transform.hpp>

#include <cubos/engine/gizmos/gizmos.hpp>

#include "renderer.hpp"
#include "types/box.hpp"
#include "types/cut_cone.hpp"
#include "types/line.hpp"
#include "types/ring.hpp"

using cubos::engine::DeltaTime;
using cubos::engine::Gizmos;
using cubos::engine::GizmosRenderer;

using namespace cubos::core::gl;

Gizmos::Gizmo::Gizmo(uint32_t id, glm::vec3 color, float lifespan)
    : id(id)
    , mColor(color)
    , mLifespan(lifespan)
{
}

void Gizmos::handleInput(uint32_t hovered, bool pressed)
{
    mIdInteraction = hovered;
    mPressed = pressed;
}

void Gizmos::setLocked(uint32_t locked)
{
    mLocked = true;
    mIdInteractionLocked = locked;
}

void Gizmos::releaseLocked()
{
    mLocked = false;
}

bool Gizmos::locked(const std::string& id) const
{
    return mLocked && mHasher(id) == mIdInteractionLocked;
}

bool Gizmos::pressed(const std::string& id) const
{
    return mPressed && mHasher(id) == mIdInteraction && mIdInteraction != 0;
}

bool Gizmos::hovered(const std::string& id) const
{
    return !mPressed && mHasher(id) == mIdInteraction && mIdInteraction != 0;
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

uint32_t Gizmos::mHasher(const std::string& id)
{
    uint32_t hash = static_cast<uint32_t>(std::hash<std::string>{}(id));
    // Set 32nd bit to distinguish from entities
    hash |= static_cast<uint32_t>(1 << 31);
    return hash;
}

void Gizmos::drawLine(const std::string& id, glm::vec3 from, glm::vec3 to, float lifespan, Space space)
{
    push(std::make_shared<LineGizmo>((uint32_t)mHasher(id), from, to, mColor, lifespan), space);
}

void Gizmos::drawBox(const std::string& id, glm::vec3 corner, glm::vec3 oppositeCorner, float lifespan, Space space)
{
    push(std::make_shared<BoxGizmo>((uint32_t)mHasher(id), corner, oppositeCorner, mColor, lifespan), space);
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
    push(std::make_shared<CutConeGizmo>((uint32_t)mHasher(id), firstBaseCenter, firstBaseRadius, secondBaseCenter,
                                        secondBaseRadius, mColor, lifespan),
         space);
}

void Gizmos::drawRing(const std::string& id, glm::vec3 firstBasePosition, glm::vec3 secondBasePosition,
                      float outerRadius, float innerRadius, float lifespan, Space space)
{
    push(std::make_shared<RingGizmo>((uint32_t)mHasher(id), firstBasePosition, secondBasePosition, outerRadius,
                                     innerRadius, mColor, lifespan),
         space);
}

void Gizmos::drawArrow(const std::string& id, glm::vec3 origin, glm::vec3 direction, float girth, float width,
                       float ratio, float lifespan, Space space)
{
    auto p = direction * ratio;
    drawCutCone(id, origin, girth, origin + p, girth, lifespan, space);
    drawCutCone(id, origin + p, width, origin + direction, 0.0F, lifespan, space);
}
