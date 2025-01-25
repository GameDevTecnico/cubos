#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/render/shader/shader.hpp>
#include <cubos/engine/render/shader/shader_builder.hpp>

using namespace cubos::engine;

ShaderBuilder& ShaderBuilder::with(const std::string& defineName)
{
    mParameters.push_back({defineName, {}});
    return *this;
}

ShaderBuilder& ShaderBuilder::with(const std::string& defineName, const std::string& defineValue)
{
    mParameters.push_back({defineName, defineValue});
    return *this;
}

cubos::core::gl::ShaderStage ShaderBuilder::build() const
{
    std::string contents;
    for (const auto& parameter : mParameters)
    {
        if (parameter.value.has_value())
        {
            contents += "#define " + parameter.name + " " + parameter.value.value() + "\n";
        }
        else
        {
            contents += "#define " + parameter.name + "\n";
        }
    }
    contents += mContents;

    cubos::core::gl::ShaderStage shaderStage = mRenderDevice.createShaderStage(mStage, contents.c_str());
    if (shaderStage == nullptr)
    {
        CUBOS_ERROR("Shader asset stage creation failed");
        return nullptr;
    }
    return shaderStage;
}
