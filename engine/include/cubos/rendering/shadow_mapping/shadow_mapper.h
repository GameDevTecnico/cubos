#ifndef CUBOS_RENDERING_SHADOW_MAPPING_SHADOW_MAPPER_H
#define CUBOS_RENDERING_SHADOW_MAPPING_SHADOW_MAPPER_H

#include <cubos/gl/render_device.hpp>

#include <vector>

namespace cubos::rendering
{
    class ShadowMapper
    {
    public:
        virtual void bind() = 0;
        virtual void unbind() = 0;
        virtual void clear() = 0;
        virtual size_t getDirectionalOutput(std::vector<gl::Texture2D>& maps, std::vector<glm::mat4>& matrices);
        virtual size_t getDirectionalOutput(std::vector<gl::Texture2DArray>& maps, std::vector<glm::mat4>& matrices);
        virtual size_t getSpotOutput(gl::Texture2DArray& mapAtlas, std::vector<glm::mat4>& matrices);

        // TODO: Need CubeMapArrays for Point Light shadows, otherwise not enough texture units
        // virtual size_t getPointOutput(gl::CubeMapArray& mapAtlas, std::vector<glm::mat4>& matrices) = 0;
    };
} // namespace cubos::rendering

#endif // CUBOS_RENDERING_SHADOW_MAPPING_SHADOW_MAPPER_H
