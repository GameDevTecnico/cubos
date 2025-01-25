/// @file
/// @brief Class @ref cubos::engine::ShaderBuilder.
/// @ingroup render-shader-plugin

#pragma once

#include <optional>
#include <string>
#include <utility>

#include <cubos/core/gl/render_device.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    class Shader;

    /// @brief Configures compile-time shader parameters using #define macros.
    /// @ingroup render-shader-plugin
    class CUBOS_ENGINE_API ShaderBuilder final
    {
    public:
        /// @brief Creates a shader builder.
        /// @param renderDevice Render device used to create the shader.
        /// @param stage Shader stage to create.
        /// @param contents Shader source code.
        ShaderBuilder(cubos::core::gl::RenderDevice& renderDevice, const cubos::core::gl::Stage stage,
                      std::string contents)
            : mRenderDevice(renderDevice)
            , mStage(stage)
            , mContents(std::move(contents)){};

        /// @brief Defines a parameter with no value.
        /// @param defineName Parameter name to define.
        /// @return This shader builder (for chaining calls).
        ShaderBuilder& with(const std::string& defineName);

        /// @brief Defines a parameter with a value.
        /// @param defineName Parameter name to define.
        /// @param defineValue Parameter value.
        /// @return This shader builder (for chaining calls).
        ShaderBuilder& with(const std::string& defineName, const std::string& defineValue);

        /// @brief Compiles a shader from the source code stored in the builder.
        /// @return Shader stage.
        cubos::core::gl::ShaderStage build() const;

    private:
        struct ShaderParameter
        {
            const std::string name;
            std::optional<const std::string> value;
        };

        cubos::core::gl::RenderDevice& mRenderDevice; ///< Render device used to create the shader.
        const cubos::core::gl::Stage mStage;          ///< Shader stage type to create.
        const std::string mContents;                  ///< Shader source code.
        std::vector<ShaderParameter> mParameters;
    };
} // namespace cubos::engine
