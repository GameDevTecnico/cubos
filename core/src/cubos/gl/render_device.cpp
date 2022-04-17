#include <cubos/gl/render_device.hpp>

using namespace cubos::core::gl;

FramebufferDesc::TargetType FramebufferDesc::FramebufferTarget::getTargetType() const
{
    return type;
}

const FramebufferDesc::CubeMapTarget& FramebufferDesc::FramebufferTarget::getCubeMapTarget() const
{
    return std::get<CubeMapTarget>(target);
}

const FramebufferDesc::Texture2DTarget& FramebufferDesc::FramebufferTarget::getTexture2DTarget() const
{
    return std::get<Texture2DTarget>(target);
}

const FramebufferDesc::CubeMapArrayTarget& FramebufferDesc::FramebufferTarget::getCubeMapArrayTarget() const
{
    return std::get<CubeMapArrayTarget>(target);
}

const FramebufferDesc::Texture2DArrayTarget& FramebufferDesc::FramebufferTarget::getTexture2DArrayTarget() const
{
    return std::get<Texture2DArrayTarget>(target);
}

void FramebufferDesc::FramebufferTarget::setCubeMapTarget(const CubeMap& handle, CubeFace face)
{
    set = true;
    type = TargetType::CubeMap;
    target = CubeMapTarget{handle, face};
}
void FramebufferDesc::FramebufferTarget::setTexture2DTarget(const Texture2D& handle)
{
    set = true;
    type = TargetType::Texture2D;
    target = Texture2DTarget{handle};
}

void FramebufferDesc::FramebufferTarget::setCubeMapArrayTarget(const CubeMapArray& handle)
{
    set = true;
    type = TargetType::CubeMapArray;
    target = CubeMapArrayTarget{handle};
}

void FramebufferDesc::FramebufferTarget::setTexture2DArrayTarget(const Texture2DArray& handle)
{
    set = true;
    type = TargetType::Texture2DArray;
    target = Texture2DArrayTarget{handle};
}

bool FramebufferDesc::FramebufferTarget::isSet() const
{
    return set;
}
