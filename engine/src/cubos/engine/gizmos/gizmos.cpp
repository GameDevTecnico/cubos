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

class Gizmos::GizmoBase
{
protected:
    const std::string& mId;
    glm::vec3 mColor;
    float mLifespan;
    VertexArray va;
    IndexBuffer ib;
    VertexArrayDesc vaDesc;

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

class Gizmos::LineGizmo : public Gizmos::GizmoBase
{
public:
    LineGizmo(const std::string& id, glm::vec3 from, glm::vec3 to, const glm::vec3& color, float lifespan,
              RenderDevice& renderDevice, const ShaderPipeline& shaderPipeline)
        : Gizmos::GizmoBase(id, color, lifespan)
        , mPointA(from)
        , mPointB(to)
    {
        vaDesc.elementCount = 1;
        vaDesc.elements[0].name = "position";
        vaDesc.elements[0].type = Type::Float;
        vaDesc.elements[0].size = 3;
        vaDesc.elements[0].buffer.index = 0;
        vaDesc.elements[0].buffer.offset = 0;
        vaDesc.elements[0].buffer.stride = 3 * sizeof(float);
        vaDesc.shaderPipeline = shaderPipeline;

        float verts[] = {mPointA[0], mPointA[1], mPointA[2], mPointB[0], mPointB[1], mPointB[2]};
        renderDevice.setShaderPipeline(shaderPipeline);
        vaDesc.buffers[0] = renderDevice.createVertexBuffer(sizeof(verts), verts, Usage::Static);
        va = renderDevice.createVertexArray(vaDesc);
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

void Gizmos::drawLine(std::string id, glm::vec3 from, glm::vec3 to, float lifespan)
{
    mGizmosVector.push_back(std::make_shared<LineGizmo>(id, from, to, mColor, lifespan, *mRenderDevice, mPipeline));
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

    renderDevice.setVertexArray(va);

    auto v = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, -1.0f, 0.0f));
    auto p = glm::ortho(-0.5f, 0.5f, -0.5f, 0.5f);
    auto vp = v * p;
    auto mvpBuffer = renderDevice.createConstantBuffer(sizeof(glm::mat4), &vp, Usage::Static);
    shaderPipeline->getBindingPoint("MVP")->bind(mvpBuffer);

    shaderPipeline->getBindingPoint("objColor")->setConstant(mColor);
    renderDevice.setRasterState(renderDevice.createRasterState(RasterStateDesc{}));
    renderDevice.drawLines(0, 2);
}