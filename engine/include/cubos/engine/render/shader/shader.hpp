/// @file
/// @brief Class @ref cubos::engine::Shader.
/// @ingroup render-shader-plugin

#pragma once

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Contains a shader stage created from GLSL code.
    /// @ingroup render-shader-plugin
    class CUBOS_ENGINE_API Shader final
    {
    public:
        CUBOS_REFLECT;

        ~Shader() = default;

        /// @brief Constructs a shader from code.
        /// @param shaderStage Shader stage created from GLSL code.
        Shader(cubos::core::gl::ShaderStage shaderStage)
            : mShaderStage(std::move(shaderStage)){};

        /// @brief Returns the asset's shader stage.
        /// @return Shader stage.
        cubos::core::gl::ShaderStage shaderStage() const;

    private:
        cubos::core::gl::ShaderStage mShaderStage;
    };
} // namespace cubos::engine
