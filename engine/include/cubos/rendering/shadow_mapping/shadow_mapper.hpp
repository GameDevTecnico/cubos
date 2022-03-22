#ifndef CUBOS_RENDERING_SHADOW_MAPPING_SHADOW_MAPPER_HPP
#define CUBOS_RENDERING_SHADOW_MAPPING_SHADOW_MAPPER_HPP

#include <cubos/gl/render_device.hpp>
#include <cubos/gl/light.hpp>
#include <cubos/gl/camera_data.hpp>
#include <cubos/rendering/renderer.hpp>

#include <vector>

#define CUBOS_MAX_SPOT_SHADOW_MAPS 32
#define CUBOS_MAX_DIRECTIONAL_SHADOW_MAPS 16
#define CUBOS_MAX_POINT_SHADOW_MAPS 8
#define CUBOS_MAX_DIRECTIONAL_SHADOW_MAP_STRIDE 4

namespace cubos::rendering
{
    class ShadowMapper
    {
    protected:
        gl::RenderDevice& renderDevice;

        std::vector<Renderer::DrawRequest> drawRequests;

    public:
        struct SpotOutput
        {
            gl::Texture2DArray mapAtlas = nullptr;
            std::vector<glm::mat4> matrices = {};
            size_t numLights = 0;
        };

        struct DirectionalOutput
        {
            gl::Texture2DArray mapAtlas = nullptr;
            std::vector<glm::mat4> matrices = {};
            std::vector<float> planeDistances = {};
            size_t atlasStride = 0;
            size_t numLights = 0;
        };

        struct PointOutput
        {
            gl::CubeMapArray mapAtlas = nullptr;
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
        virtual SpotOutput getSpotOutput() const;
        virtual DirectionalOutput getDirectionalOutput() const;
        virtual PointOutput getPointOutput() const;
    };
} // namespace cubos::rendering

#endif // CUBOS_RENDERING_SHADOW_MAPPING_SHADOW_MAPPER_HPP
