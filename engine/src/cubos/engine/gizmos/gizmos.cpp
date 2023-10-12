#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/transform.hpp>

#include <cubos/engine/gizmos/gizmos.hpp>

using cubos::core::gl::IndexBuffer;
using cubos::core::gl::IndexFormat;
using cubos::core::gl::RasterMode;
using cubos::core::gl::RasterStateDesc;
using cubos::core::gl::ShaderBindingPoint;
using cubos::core::gl::Stage;
using cubos::core::gl::Type;
using cubos::core::gl::Usage;
using cubos::core::gl::VertexArray;
using cubos::core::gl::VertexArrayDesc;
using cubos::engine::DeltaTime;
using cubos::engine::Gizmos;

/// @brief A gizmo to be drawn
class Gizmos::GizmoBase
{
protected:
    const std::string& mId;
    glm::vec3 mColor;
    float mLifespan;
    VertexArray mVa;
    IndexBuffer mIb;
    VertexArrayDesc mVaDesc;

public:
    GizmoBase(const std::string& id, glm::vec3 color, float lifespan)
        : mId(id)
        , mColor(color)
        , mLifespan(lifespan)
    {
    }

    virtual void draw(RenderDevice& renderDevice, const ShaderPipeline& shaderPipeline) = 0;

    bool decreaseLifespan(float delta)
    {
        mLifespan -= delta;
        return mLifespan <= 0;
    }

    virtual ~GizmoBase() = default;
};

/// @brief A line gizmo to be drawn
class Gizmos::LineGizmo : public Gizmos::GizmoBase
{
public:
    LineGizmo(const std::string& id, glm::vec3 from, glm::vec3 to, const glm::vec3& color, float lifespan,
              RenderDevice& renderDevice, const ShaderPipeline& shaderPipeline)
        : Gizmos::GizmoBase(id, color, lifespan)
        , mPointA(from)
        , mPointB(to)
    {
        mVaDesc.elementCount = 1;
        mVaDesc.elements[0].name = "position";
        mVaDesc.elements[0].type = Type::Float;
        mVaDesc.elements[0].size = 3;
        mVaDesc.elements[0].buffer.index = 0;
        mVaDesc.elements[0].buffer.offset = 0;
        mVaDesc.elements[0].buffer.stride = 3 * sizeof(float);
        mVaDesc.shaderPipeline = shaderPipeline;

        float verts[] = {mPointA[0], mPointA[1], mPointA[2], mPointB[0], mPointB[1], mPointB[2]};
        renderDevice.setShaderPipeline(shaderPipeline);
        mVaDesc.buffers[0] = renderDevice.createVertexBuffer(sizeof(verts), verts, Usage::Static);
        mVa = renderDevice.createVertexArray(mVaDesc);
    }
    void draw(RenderDevice& renderDevice, const ShaderPipeline& shaderPipeline) override;

private:
    glm::vec3 mPointA;
    glm::vec3 mPointB;
};

/// @brief A box gizmo to be drawn
class Gizmos::BoxGizmo : public Gizmos::GizmoBase
{
public:
    BoxGizmo(const std::string& id, glm::vec3 corner, glm::vec3 oppositeCorner, const glm::vec3& color, float lifespan,
             RenderDevice& renderDevice, const ShaderPipeline& shaderPipeline)
        : Gizmos::GizmoBase(id, color, lifespan)
        , mPointA(corner)
        , mPointB(oppositeCorner)
    {
        mVaDesc.elementCount = 1;
        mVaDesc.elements[0].name = "position";
        mVaDesc.elements[0].type = Type::Float;
        mVaDesc.elements[0].size = 3;
        mVaDesc.elements[0].buffer.index = 0;
        mVaDesc.elements[0].buffer.offset = 0;
        mVaDesc.elements[0].buffer.stride = 3 * sizeof(float);
        mVaDesc.shaderPipeline = shaderPipeline;

        float verts[] = {mPointA[0], mPointA[1], mPointA[2], mPointB[0], mPointA[1], mPointA[2],
                         mPointB[0], mPointA[1], mPointB[2], mPointA[0], mPointA[1], mPointB[2],
                         mPointA[0], mPointB[1], mPointB[2], mPointA[0], mPointB[1], mPointA[2],
                         mPointB[0], mPointB[1], mPointA[2], mPointB[0], mPointB[1], mPointB[2]};
        renderDevice.setShaderPipeline(shaderPipeline);
        mVaDesc.buffers[0] = renderDevice.createVertexBuffer(sizeof(verts), verts, Usage::Static);
        mVa = renderDevice.createVertexArray(mVaDesc);

        unsigned int indices[] = {// front
                                  0, 6, 1, 0, 5, 6,
                                  // right
                                  1, 7, 2, 1, 6, 7,
                                  // back
                                  2, 7, 3, 3, 7, 4,
                                  // left
                                  3, 5, 0, 3, 4, 5,
                                  // bottom
                                  3, 1, 2, 3, 0, 1,
                                  // top
                                  5, 7, 6, 5, 4, 7};
        mIb = renderDevice.createIndexBuffer(sizeof(indices), indices, IndexFormat::UInt, Usage::Static);
    }
    void draw(RenderDevice& renderDevice, const ShaderPipeline& shaderPipeline) override;

private:
    glm::vec3 mPointA;
    glm::vec3 mPointB;
};

void Gizmos::init(RenderDevice& renderDevice)
{
    auto vs = renderDevice.createShaderStage(Stage::Vertex, R"(
            #version 330 core

            in vec3 position;

            uniform MVP
            {
                mat4 mvp;
            };

            void main()
            {
                gl_Position = mvp * vec4(position, 1.0f);
            }
        )");

    auto ps = renderDevice.createShaderStage(Stage::Pixel, R"(
            #version 330 core

            out vec4 color;

            uniform vec3 objColor;

            void main()
            {
                color = vec4(objColor, 1.0f);
            }
        )");

    mRenderDevice = &renderDevice;
    mPipeline = renderDevice.createShaderPipeline(vs, ps);
}

void Gizmos::color(const glm::vec3& color)
{
    mColor = color;
}

void Gizmos::drawLine(const std::string& id, glm::vec3 from, glm::vec3 to, float lifespan)
{
    mGizmosVector.push_back(std::make_shared<LineGizmo>(id, from, to, mColor, lifespan, *mRenderDevice, mPipeline));
}

void Gizmos::drawBox(const std::string& id, glm::vec3 corner, glm::vec3 oppositeCorner, float lifespan)
{
    mGizmosVector.push_back(
        std::make_shared<BoxGizmo>(id, corner, oppositeCorner, mColor, lifespan, *mRenderDevice, mPipeline));
}

void Gizmos::drawWireBox(const std::string& id, glm::vec3 corner, glm::vec3 oppositeCorner, float lifespan)
{
    // Front
    mGizmosVector.push_back(std::make_shared<LineGizmo>(id, corner, glm::vec3{oppositeCorner[0], corner[1], corner[2]},
                                                        mColor, lifespan, *mRenderDevice, mPipeline));
    mGizmosVector.push_back(std::make_shared<LineGizmo>(id, corner, glm::vec3{corner[0], oppositeCorner[1], corner[2]},
                                                        mColor, lifespan, *mRenderDevice, mPipeline));
    mGizmosVector.push_back(std::make_shared<LineGizmo>(id, glm::vec3{oppositeCorner[0], oppositeCorner[1], corner[2]},
                                                        glm::vec3{corner[0], oppositeCorner[1], corner[2]}, mColor,
                                                        lifespan, *mRenderDevice, mPipeline));
    mGizmosVector.push_back(std::make_shared<LineGizmo>(id, glm::vec3{oppositeCorner[0], oppositeCorner[1], corner[2]},
                                                        glm::vec3{oppositeCorner[0], corner[1], corner[2]}, mColor,
                                                        lifespan, *mRenderDevice, mPipeline));

    // Back
    mGizmosVector.push_back(std::make_shared<LineGizmo>(id, oppositeCorner,
                                                        glm::vec3{corner[0], oppositeCorner[1], oppositeCorner[2]},
                                                        mColor, lifespan, *mRenderDevice, mPipeline));
    mGizmosVector.push_back(std::make_shared<LineGizmo>(id, oppositeCorner,
                                                        glm::vec3{oppositeCorner[0], corner[1], oppositeCorner[2]},
                                                        mColor, lifespan, *mRenderDevice, mPipeline));
    mGizmosVector.push_back(std::make_shared<LineGizmo>(id, glm::vec3{corner[0], corner[1], oppositeCorner[2]},
                                                        glm::vec3{corner[0], oppositeCorner[1], oppositeCorner[2]},
                                                        mColor, lifespan, *mRenderDevice, mPipeline));
    mGizmosVector.push_back(std::make_shared<LineGizmo>(id, glm::vec3{corner[0], corner[1], oppositeCorner[2]},
                                                        glm::vec3{oppositeCorner[0], corner[1], oppositeCorner[2]},
                                                        mColor, lifespan, *mRenderDevice, mPipeline));

    // Sides
    mGizmosVector.push_back(std::make_shared<LineGizmo>(id, corner, glm::vec3{corner[0], corner[1], oppositeCorner[2]},
                                                        mColor, lifespan, *mRenderDevice, mPipeline));
    mGizmosVector.push_back(std::make_shared<LineGizmo>(id, glm::vec3{corner[0], oppositeCorner[1], corner[2]},
                                                        glm::vec3{corner[0], oppositeCorner[1], oppositeCorner[2]},
                                                        mColor, lifespan, *mRenderDevice, mPipeline));
    mGizmosVector.push_back(std::make_shared<LineGizmo>(id, glm::vec3{oppositeCorner[0], corner[1], corner[2]},
                                                        glm::vec3{oppositeCorner[0], corner[1], oppositeCorner[2]},
                                                        mColor, lifespan, *mRenderDevice, mPipeline));
    mGizmosVector.push_back(std::make_shared<LineGizmo>(id, glm::vec3{oppositeCorner[0], oppositeCorner[1], corner[2]},
                                                        oppositeCorner, mColor, lifespan, *mRenderDevice, mPipeline));
}

void Gizmos::drawQueuedGizmos(DeltaTime deltaTime)
{
    std::vector<std::size_t> toRemove;
    for (std::size_t i = 0; i < mGizmosVector.size(); i++)
    {
        auto& gizmo = *mGizmosVector[i];
        gizmo.draw(*mRenderDevice, mPipeline);
        if (gizmo.decreaseLifespan(deltaTime.value))
        {
            toRemove.push_back(i);
        }
    }
    while (!toRemove.empty())
    {
        std::size_t i = toRemove.back();
        toRemove.pop_back();
        mGizmosVector[i] = mGizmosVector.back();
        mGizmosVector.pop_back();
    }
}

void Gizmos::LineGizmo::draw(RenderDevice& renderDevice, const ShaderPipeline& shaderPipeline)
{
    renderDevice.setVertexArray(mVa);

    auto v = glm::translate(glm::mat4(1.0F), glm::vec3(-1.0F, -1.0F, 0.0F));
    auto p = glm::ortho(-0.5F, 0.5F, -0.5F, 0.5F);
    auto vp = v * p;
    auto mvpBuffer = renderDevice.createConstantBuffer(sizeof(glm::mat4), &vp, Usage::Static);
    shaderPipeline->getBindingPoint("MVP")->bind(mvpBuffer);

    shaderPipeline->getBindingPoint("objColor")->setConstant(mColor);
    renderDevice.setRasterState(renderDevice.createRasterState(RasterStateDesc{}));
    renderDevice.drawLines(0, 2);
}

void Gizmos::BoxGizmo::draw(RenderDevice& renderDevice, const ShaderPipeline& shaderPipeline)
{
    renderDevice.setVertexArray(mVa);
    renderDevice.setIndexBuffer(mIb);

    auto v = glm::translate(glm::mat4(1.0F), glm::vec3(-1.0F, -1.0F, 0.0F));
    auto p = glm::ortho(-0.5F, 0.5F, -0.5F, 0.5F);
    auto vp = v * p;
    auto mvpBuffer = renderDevice.createConstantBuffer(sizeof(glm::mat4), &vp, Usage::Static);
    shaderPipeline->getBindingPoint("MVP")->bind(mvpBuffer);

    shaderPipeline->getBindingPoint("objColor")->setConstant(mColor);
    renderDevice.setRasterState(renderDevice.createRasterState(RasterStateDesc{}));
    renderDevice.drawTrianglesIndexed(0, 36);
}