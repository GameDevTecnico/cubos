#ifndef CUBOS_RENDERING_RENDERER_HPP
#define CUBOS_RENDERING_RENDERER_HPP

#include <vector>

#include <cubos/gl/render_device.hpp>

class Renderer
{
protected:
    struct RendererModel
    {
        cubos::gl::VertexArray va;
        cubos::gl::IndexBuffer ib;
        size_t numTris;
        glm::mat4 modelMat;
        bool enabled = true;
    };

    std::vector<RendererModel> models;

public:
    using ID = size_t;

    virtual ID registerModel(cubos::gl::VertexArray va, cubos::gl::IndexBuffer ib, size_t numTris,
                             glm::mat4 initModelMat = glm::mat4());

    virtual void setModelMat(ID modelID, glm::mat4 modelMat);
    virtual void render() = 0;
};

#endif // CUBOS_RENDERING_RENDERER_HPP