#ifndef CUBOS_RENDERING_SHADOW_MAPPING_SHADOW_MAPPER_HPP
#define CUBOS_RENDERING_SHADOW_MAPPING_SHADOW_MAPPER_HPP

#include <cubos/gl/render_device.hpp>
#include <cubos/gl/light.hpp>

#include <vector>

namespace cubos::rendering
{
    class ShadowMapper
    {
    protected:
        struct SpotLightData
        {
            glm::vec4 position;
            glm::mat4 rotation;
            float range;
            float spotAngle;
            float padding[2];

            SpotLightData(const gl::SpotLightData& light);
        };

        struct DirectionalLightData
        {
            glm::mat4 rotation;

            DirectionalLightData(const gl::DirectionalLightData& light);
        };

        struct PointLightData
        {
            glm::vec4 position;
            float range;
            float padding[3];

            PointLightData(const gl::PointLightData& light);
        };

        gl::RenderDevice& renderDevice;

    public:
    public:
        explicit ShadowMapper(gl::RenderDevice& renderDevice);

        virtual void setModelMatrix(glm::mat4 modelMat) = 0;
        virtual void bind() = 0;
        virtual void unbind() = 0;
        virtual void clear() = 0;
        virtual void addLight(const gl::SpotLightData& light) = 0;
        virtual void addLight(const gl::DirectionalLightData& light) = 0;
        virtual void addLight(const gl::PointLightData& light) = 0;
        virtual size_t getSpotOutput(gl::Texture2DArray& mapAtlas, std::vector<glm::mat4>& matrices);
        virtual size_t getDirectionalOutput(gl::Texture2DArray& mapAtlas, std::vector<glm::mat4>& matrices,
                                            size_t& atlasStride);

        // TODO: Need CubeMapArrays for Point Light shadows, otherwise not enough texture units
        // virtual size_t getPointOutput(gl::CubeMapArray& mapAtlas, std::vector<glm::mat4>& matrices) = 0;
    };
} // namespace cubos::rendering

#endif // CUBOS_RENDERING_SHADOW_MAPPING_SHADOW_MAPPER_HPP
