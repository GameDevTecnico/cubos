#include <cubos/rendering/renderer.hpp>

Renderer::ID Renderer::registerModel(cubos::gl::VertexArray va, cubos::gl::IndexBuffer ib, size_t numTris,
                                     glm::mat4 initModelMat)
{
    models.push_back({va, ib, numTris, initModelMat, true});
    return models.size() - 1;
}

void Renderer::setModelMat(Renderer::ID modelID, glm::mat4 modelMat)
{
    models[modelID].modelMat = modelMat;
}
