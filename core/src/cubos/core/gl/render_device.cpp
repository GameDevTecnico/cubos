#include <cubos/core/gl/render_device.hpp>

using namespace cubos::core::gl;

FramebufferDesc::TargetType FramebufferDesc::FramebufferTarget::getTargetType() const
{
    return mType;
}

const FramebufferDesc::CubeMapTarget& FramebufferDesc::FramebufferTarget::getCubeMapTarget() const
{
    return std::get<CubeMapTarget>(mTarget);
}

const FramebufferDesc::Texture2DTarget& FramebufferDesc::FramebufferTarget::getTexture2DTarget() const
{
    return std::get<Texture2DTarget>(mTarget);
}

const FramebufferDesc::CubeMapArrayTarget& FramebufferDesc::FramebufferTarget::getCubeMapArrayTarget() const
{
    return std::get<CubeMapArrayTarget>(mTarget);
}

const FramebufferDesc::Texture2DArrayTarget& FramebufferDesc::FramebufferTarget::getTexture2DArrayTarget() const
{
    return std::get<Texture2DArrayTarget>(mTarget);
}

void FramebufferDesc::FramebufferTarget::setCubeMapTarget(const CubeMap& handle, CubeFace face)
{
    mSet = true;
    mType = TargetType::CubeMap;
    mTarget = CubeMapTarget{handle, face};
}

void FramebufferDesc::FramebufferTarget::setTexture2DTarget(const Texture2D& handle)
{
    mSet = true;
    mType = TargetType::Texture2D;
    mTarget = Texture2DTarget{handle};
}

void FramebufferDesc::FramebufferTarget::setCubeMapArrayTarget(const CubeMapArray& handle)
{
    mSet = true;
    mType = TargetType::CubeMapArray;
    mTarget = CubeMapArrayTarget{handle};
}

void FramebufferDesc::FramebufferTarget::setTexture2DArrayTarget(const Texture2DArray& handle)
{
    mSet = true;
    mType = TargetType::Texture2DArray;
    mTarget = Texture2DArrayTarget{handle};
}

bool FramebufferDesc::FramebufferTarget::isSet() const
{
    return mSet;
}
