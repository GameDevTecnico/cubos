#include <cubos/gl/render_device.hpp>

using namespace cubos::core::gl;

const FramebufferDesc::CubeMapTarget& FramebufferDesc::FramebufferTarget::getCubeMapTarget() const
{
    return std::get<CubeMapTarget>(target);
}

const FramebufferDesc::Texture2DTarget& FramebufferDesc::FramebufferTarget::getTexture2DTarget() const
{
    return std::get<Texture2DTarget>(target);
}

void FramebufferDesc::FramebufferTarget::setCubeMapTarget(const CubeMap& handle, CubeFace face)
{
    isCubeMap = true;
    target = CubeMapTarget{handle, face};
}
void FramebufferDesc::FramebufferTarget::setTexture2DTarget(const Texture2D& handle)
{
    isCubeMap = false;
    target = Texture2DTarget{handle};
}
