/// @file
/// @brief Class @ref cubos::engine::Shader.
/// @ingroup render-shader-plugin

#pragma once

#include <utility>

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>
#include <cubos/engine/render/shader/shader_builder.hpp>

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
        /// @param stage Shader stage to create.
        /// @param renderDevice Render device used to create the shader.
        /// @param contents Shader source code.
        Shader(const cubos::core::gl::Stage stage, cubos::core::gl::RenderDevice& renderDevice, std::string contents)
            : mStage(stage)
            , mRenderDevice(renderDevice)
            , mContents(std::move(contents)){};

        /// @brief Returns the asset's shader stage.
        /// @return Shader stage.
        cubos::core::gl::ShaderStage shaderStage() const;

        /// @brief Returns a shader builder to configure compile-time shader parameters.
        /// @return Shader builder.
        ShaderBuilder builder() const;

    private:
        const cubos::core::gl::Stage mStage;          ///< Shader stage type to create.
        cubos::core::gl::RenderDevice& mRenderDevice; ///< Render device used to create the shader.
        const std::string mContents;                  ///< Shader source code.
    };
} // namespace cubos::engine
