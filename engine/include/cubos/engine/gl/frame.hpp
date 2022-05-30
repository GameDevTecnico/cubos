#ifndef CUBOS_ENGINE_GL_FRAME_HPP
#define CUBOS_ENGINE_GL_FRAME_HPP

namespace cubos::engine::gl
{
    /// Contains all data needed to draw a frame.
    class Frame
    {
    public:
        void drawGrid(GridID gridID, glm::mat4 modelMat);
        void addLight(const core::gl::SpotLight& light);
        void addLight(const core::gl::DirectionalLight& light);
        void addLight(const core::gl::PointLight& light);
        void clear();

    private:
        friend Renderer;
        // TODO
    };
} // namespace cubos::engine::gl

#endif // CUBOS_ENGINE_GL_FRAME_HPP
