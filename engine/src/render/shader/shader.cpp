#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/render/shader/shader.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(Shader)
{
    using namespace cubos::core::reflection;

    return Type::create("cubos::engine::Shader")
        .with(ConstructibleTrait::typed<Shader>().withMoveConstructor().build());
}

cubos::core::gl::ShaderStage Shader::shaderStage() const
{
    return builder().build();
}

ShaderBuilder Shader::builder() const
{
    return {mRenderDevice, mStage, mContents};
}
