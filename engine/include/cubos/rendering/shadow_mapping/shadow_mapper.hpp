#ifndef CUBOS_RENDERING_SHADOW_MAPPING_SHADOW_MAPPER_HPP
#define CUBOS_RENDERING_SHADOW_MAPPING_SHADOW_MAPPER_HPP

#include <cubos/gl/render_device.hpp>
#include <cubos/gl/light.hpp>
#include <cubos/gl/camera_data.hpp>
#include <cubos/rendering/renderer.hpp>

#include <vector>

#define CUBOS_MAX_DIRECTIONAL_SHADOW_MAPS 64
#define CUBOS_MAX_DIRECTIONAL_SHADOW_MAP_STRIDE 8

namespace cubos::rendering
{
    class ShadowMapper
    {
    protected:
        gl::RenderDevice& renderDevice;

        std::vector<Renderer::DrawRequest> drawRequests;

    public:
        struct DirectionalOutput
        {
            gl::Texture2DArray mapAtlas = nullptr;
            std::vector<glm::mat4> matrices = {};
            std::vector<float> planeDistances = {};
            size_t atlasStride = 0;
            size_t numLights = 0;
        };

    public:
        explicit ShadowMapper(gl::RenderDevice& renderDevice);

        virtual void drawModel(const Renderer::DrawRequest& model);
        virtual void render(const gl::CameraData& camera) = 0;
        virtual void flush() = 0;
        virtual void addLight(const gl::SpotLightData& light) = 0;
        virtual void addLight(const gl::DirectionalLightData& light) = 0;
        virtual void addLight(const gl::PointLightData& light) = 0;
        virtual size_t getSpotOutput(gl::Texture2DArray& mapAtlas, std::vector<glm::mat4>& matrices);
        virtual DirectionalOutput getDirectionalOutput();

        // TODO: Need CubeMapArrays for Point Light shadows, otherwise not enough texture units
        // virtual size_t getPointOutput(gl::CubeMapArray& mapAtlas, std::vector<glm::mat4>& matrices) = 0;
    };
} // namespace cubos::rendering

#endif // CUBOS_RENDERING_SHADOW_MAPPING_SHADOW_MAPPER_HPP
