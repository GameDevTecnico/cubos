#ifndef CUBOS_RENDERING_DEFERRED_DEFERRED_RENDERER_HPP
#define CUBOS_RENDERING_DEFERRED_DEFERRED_RENDERER_HPP

#include <vector>

#include <cubos/gl/render_device.hpp>
#include <cubos/rendering/renderer.hpp>

namespace cubos::rendering
{
    class DeferredRenderer : public Renderer
    {
    private:
        gl::ShaderPipeline shaderPipeline;

        // Framebuffers
        gl::Framebuffer gBuffer;

        // Textures
        gl::Texture2D positionTex;
        gl::Texture2D normalTex;
        gl::Texture2D materialTex;
        gl::Texture2D depthTex;

    private:
        void createPipeline();
        void setupFrameBuffers();

    public:
        explicit DeferredRenderer(io::Window& window);
        ID registerModel(const std::vector<glm::uvec3>& vertices, const std::vector<glm::vec3>& normals,
                         const std::vector<uint32_t>& materials, std::vector<uint32_t>& indices) override;
        void drawModel(ID modelID, glm::mat4 modelMat) override;

        void render() override;
    };
} // namespace cubos::rendering

#endif // CUBOS_RENDERING_DEFERRED_DEFERRED_RENDERER_HPP