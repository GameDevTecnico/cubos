#ifndef CUBOS_ENGINE_RENDERING_SHADOW_MAPPING_CSM_SHADOW_MAPPER_HPP
#define CUBOS_ENGINE_RENDERING_SHADOW_MAPPING_CSM_SHADOW_MAPPER_HPP

#include <cubos/engine/rendering/shadow_mapping/shadow_mapper.hpp>

#define CUBOS_CSM_MAX_DIRECTIONAL_SHADOW_COUNT 4

namespace cubos::engine::rendering
{
    class CSMShadowMapper : public ShadowMapper
    {
    private:
        core::gl::SpotLightData spotLights[CUBOS_MAX_SPOT_SHADOW_MAPS]{};
        glm::mat4 spotMatrices[CUBOS_MAX_SPOT_SHADOW_MAPS];
        size_t numSpotLights = 0;
        core::gl::DirectionalLightData directionalLights[CUBOS_CSM_MAX_DIRECTIONAL_SHADOW_COUNT]{};
        glm::mat4 directionalMatrices[CUBOS_MAX_DIRECTIONAL_SHADOW_MAPS];
        size_t numDirectionalLights = 0;
        core::gl::PointLightData pointLights[CUBOS_MAX_POINT_SHADOW_MAPS]{};
        size_t numPointLights = 0;

        size_t numCascades;
        float cascades[CUBOS_MAX_DIRECTIONAL_SHADOW_MAP_STRIDE - 1];

        float zMultiplier = 1.0f;

        // region Spot Pipeline
        core::gl::Texture2DArray spotAtlas;

        core::gl::Framebuffer spotFramebuffer;

        size_t spotResolution;

        core::gl::ShaderPipeline spotPipeline;

        core::gl::ShaderBindingPoint spotModelMatrixBP;
        core::gl::ConstantBuffer spotModelMatrixBuffer;

        core::gl::ConstantBuffer spotLightBuffer;
        core::gl::ShaderBindingPoint spotLightBP;

        core::gl::ShaderBindingPoint spotAtlasOffsetBP;
        // endregion

        // region Directional Pipeline
        core::gl::Texture2DArray directionalAtlas;

        core::gl::Framebuffer directionalFramebuffer;

        size_t directionalResolution;

        core::gl::ShaderPipeline directionalPipeline;

        core::gl::ShaderBindingPoint directionalModelMatrixBP;
        core::gl::ConstantBuffer directionalModelMatrixBuffer;

        core::gl::ConstantBuffer directionalLightMatricesBuffer;
        core::gl::ShaderBindingPoint directionalLightMatricesBP;

        core::gl::ShaderBindingPoint directionalAtlasOffsetBP;
        // endregion

        // region Point Pipeline
        core::gl::CubeMapArray pointAtlas;

        core::gl::Framebuffer pointFramebuffer;

        size_t pointResolution;

        core::gl::ShaderPipeline pointPipeline;

        core::gl::ShaderBindingPoint pointModelMatrixBP;
        core::gl::ConstantBuffer pointModelMatrixBuffer;

        core::gl::ConstantBuffer pointLightMatricesBuffer;
        core::gl::ShaderBindingPoint pointLightMatricesBP;

        core::gl::ShaderBindingPoint pointAtlasOffsetBP;

        core::gl::ShaderBindingPoint pointLightPosBP;
        core::gl::ShaderBindingPoint pointFarPlaneBP;
        // endregion

        core::gl::RasterState rasterState;
        core::gl::BlendState blendState;
        core::gl::DepthStencilState depthStencilState;

    private:
        inline void setupFramebuffers();
        inline void setupPipelines();

        inline void createRenderDeviceStates();

        inline void renderSpotLights();
        inline void renderDirectionalLights(const core::gl::CameraData& camera);
        inline void renderPointLights();

    public:
        CSMShadowMapper(core::gl::RenderDevice& renderDevice, size_t spotResolution, size_t directionalResolution,
                        size_t pointResolution, size_t numCascades);
        virtual void drawModel(const Renderer::DrawRequest& model) override;
        virtual void render(const core::gl::CameraData& camera) override;
        virtual void flush() override;
        virtual void addLight(const core::gl::SpotLightData& light) override;
        virtual void addLight(const core::gl::DirectionalLightData& light) override;
        virtual void addLight(const core::gl::PointLightData& light) override;

        virtual SpotOutput getSpotOutput() const override;
        virtual DirectionalOutput getDirectionalOutput() const override;
        virtual PointOutput getPointOutput() const override;
        void setCascadeDistances(const std::vector<float>& distances);
    };
} // namespace cubos::engine::rendering

#endif // CUBOS_ENGINE_RENDERING_SHADOW_MAPPING_CSM_SHADOW_MAPPER_HPP
