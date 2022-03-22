#ifndef CUBOS_RENDERING_SHADOW_MAPPING_CSM_SHADOW_MAPPER_HPP
#define CUBOS_RENDERING_SHADOW_MAPPING_CSM_SHADOW_MAPPER_HPP

#include <cubos/rendering/shadow_mapping/shadow_mapper.hpp>

#define CUBOS_CSM_MAX_DIRECTIONAL_SHADOW_COUNT 4

namespace cubos::rendering
{
    class CSMShadowMapper : public ShadowMapper
    {
    private:
        gl::SpotLightData spotLights[CUBOS_MAX_SPOT_SHADOW_MAPS]{};
        glm::mat4 spotMatrices[CUBOS_MAX_SPOT_SHADOW_MAPS];
        size_t numSpotLights = 0;
        gl::DirectionalLightData directionalLights[CUBOS_CSM_MAX_DIRECTIONAL_SHADOW_COUNT]{};
        glm::mat4 directionalMatrices[CUBOS_MAX_DIRECTIONAL_SHADOW_MAPS];
        size_t numDirectionalLights = 0;
        gl::PointLightData pointLights[CUBOS_MAX_POINT_SHADOW_MAPS]{};
        size_t numPointLights = 0;

        size_t numCascades;
        float cascades[CUBOS_MAX_DIRECTIONAL_SHADOW_MAP_STRIDE - 1];

        float zMultiplier = 1.0f;

        // region Spot Pipeline
        gl::Texture2DArray spotAtlas;

        gl::Framebuffer spotFramebuffer;

        size_t spotResolution;

        gl::ShaderPipeline spotPipeline;

        gl::ShaderBindingPoint spotModelMatrixBP;
        gl::ConstantBuffer spotModelMatrixBuffer;

        gl::ConstantBuffer spotLightBuffer;
        gl::ShaderBindingPoint spotLightBP;

        gl::ShaderBindingPoint spotAtlasOffsetBP;
        // endregion

        // region Directional Pipeline
        gl::Texture2DArray directionalAtlas;

        gl::Framebuffer directionalFramebuffer;

        size_t directionalResolution;

        gl::ShaderPipeline directionalPipeline;

        gl::ShaderBindingPoint directionalModelMatrixBP;
        gl::ConstantBuffer directionalModelMatrixBuffer;

        gl::ConstantBuffer directionalLightMatricesBuffer;
        gl::ShaderBindingPoint directionalLightMatricesBP;

        gl::ShaderBindingPoint directionalAtlasOffsetBP;
        // endregion

        // region Point Pipeline
        gl::CubeMapArray pointAtlas;

        gl::Framebuffer pointFramebuffer;

        size_t pointResolution;

        gl::ShaderPipeline pointPipeline;

        gl::ShaderBindingPoint pointModelMatrixBP;
        gl::ConstantBuffer pointModelMatrixBuffer;

        gl::ConstantBuffer pointLightMatricesBuffer;
        gl::ShaderBindingPoint pointLightMatricesBP;

        gl::ShaderBindingPoint pointAtlasOffsetBP;

        gl::ShaderBindingPoint pointLightPosBP;
        gl::ShaderBindingPoint pointFarPlaneBP;
        // endregion

        gl::RasterState rasterState;
        gl::BlendState blendState;
        gl::DepthStencilState depthStencilState;

    private:
        inline void setupFramebuffers();
        inline void setupPipelines();

        inline void createRenderDeviceStates();

        inline void renderSpotLights();
        inline void renderDirectionalLights(const gl::CameraData& camera);
        inline void renderPointLights();

    public:
        CSMShadowMapper(gl::RenderDevice& renderDevice, size_t spotResolution, size_t directionalResolution,
                        size_t pointResolution, size_t numCascades);
        virtual void drawModel(const Renderer::DrawRequest& model) override;
        virtual void render(const gl::CameraData& camera) override;
        virtual void flush() override;
        virtual void addLight(const gl::SpotLightData& light) override;
        virtual void addLight(const gl::DirectionalLightData& light) override;
        virtual void addLight(const gl::PointLightData& light) override;

        virtual SpotOutput getSpotOutput() const override;
        virtual DirectionalOutput getDirectionalOutput() const override;
        virtual PointOutput getPointOutput() const override;
        void setCascadeDistances(const std::vector<float>& distances);
    };
} // namespace cubos::rendering

#endif // CUBOS_RENDERING_SHADOW_MAPPING_CSM_SHADOW_MAPPER_HPP
