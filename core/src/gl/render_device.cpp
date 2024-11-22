#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/tel/logging.hpp>

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

void FramebufferDesc::FramebufferTarget::setTexture2DArrayTarget(const Texture2DArray& handle, uint32_t layer)
{
    mSet = true;
    mType = TargetType::Texture2DArray;
    mTarget = Texture2DArrayTarget{handle, layer};
}

bool FramebufferDesc::FramebufferTarget::isSet() const
{
    return mSet;
}

impl::PipelinedTimer::PipelinedTimer(RenderDevice& rd)
    : mRd(rd)
{
}

void impl::PipelinedTimer::begin()
{
    CUBOS_ASSERT(!mNeedsEnd, "The previous call to begin() wasn't matched with a call to end()");
    gl::Timer timer = mRd.createTimer();
    timer->begin();
    mTimers.push(timer);
    mNeedsEnd = true;
}

impl::PipelinedTimer::Result impl::PipelinedTimer::end()
{
    CUBOS_ASSERT(mNeedsEnd, "Call to end() wasn't matched with a previous call to begin()");

    mTimers.back()->end();

    while (!mTimers.empty() && mTimers.front()->done())
    {
        mResult = mTimers.front()->result();
        mTimers.pop();
    }

    mNeedsEnd = false;

    return {mResult, mTimers.size()};
}
