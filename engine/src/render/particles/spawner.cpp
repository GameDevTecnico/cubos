#include <algorithm>
#include <cstddef>

#include <glm/ext/scalar_constants.hpp>
#include <glm/gtc/random.hpp>

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/render/particles/spawner.hpp>
using namespace cubos;
CUBOS_REFLECT_IMPL(cubos::engine::ParticleSpawner)
{
    return core::ecs::TypeBuilder<ParticleSpawner>("cubos::engine::ParticleSpawner")
        .withField("maxParticles", &ParticleSpawner::maxParticles)
        .withField("emissionRate", &ParticleSpawner::emissionRate)
        .withField("minLifetime", &ParticleSpawner::minLifetime)
        .withField("maxLifetime", &ParticleSpawner::maxLifetime)
        .withField("timeSinceLastEmission", &ParticleSpawner::timeSinceLastEmission)
        .withField("emissionPoint", &ParticleSpawner::emissionPoint)
        .withField("emissionRadius", &ParticleSpawner::emissionRadius)
        .withField("startColor", &ParticleSpawner::startColor)
        .withField("endColor", &ParticleSpawner::endColor)
        .withField("velocity", &ParticleSpawner::velocity)
        .build();
}

void cubos::engine::ParticleSpawner::emitParticle()
{
    if (this->particles.size() >= this->maxParticles)
    {
        return; // Cannot emit more particles.
    }

    float angle = glm::linearRand(0.0f, 2.0f * glm::pi<float>());
    float radius = glm::linearRand(0.0f, this->emissionRadius);

    Particle particle;
    particle.position = this->emissionPoint + glm::vec3(radius * cos(angle), 0.0f, radius * sin(angle));
    particle.velocity = this->velocity; // Default velocity, can be set later.
    particle.lifetime = glm::linearRand(this->minLifetime, this->maxLifetime);
    particle.age = 0.0f;

    this->particles.push_back(particle);
}

void cubos::engine::ParticleSpawner::setVelocity(const glm::vec3& velocity)
{
    this->velocity = velocity;
}

void cubos::engine::ParticleSpawner::setColors(const glm::vec3& startColor, const glm::vec3& endColor)
{
    this->startColor = startColor;
    this->endColor = endColor;
}

void cubos::engine::ParticleSpawner::setEmissionPoint(const glm::vec3& point)
{
    this->emissionPoint = point;
}

void cubos::engine::ParticleSpawner::setEmissionRadius(float radius)
{
    this->emissionRadius = radius;
}

void cubos::engine::ParticleSpawner::setEmissionRate(float rate)
{
    this->emissionRate = rate;
}

void cubos::engine::ParticleSpawner::setLifetime(float minLifetime, float maxLifetime)
{
    this->minLifetime = minLifetime;
    this->maxLifetime = maxLifetime;
}

void cubos::engine::ParticleSpawner::setParticleTexture(core::gl::Texture2D texture)
{
    this->particleTexture = texture;
}

void cubos::engine::ParticleSpawner::setParticleTexture(core::gl::Texture2D texture, core::gl::Sampler sampler)
{
    this->particleTexture = texture;
    this->particleSampler = sampler;
}

void cubos::engine::ParticleSpawner::setupBuffers(core::gl::RenderDevice& renderDevice,
                                                  core::gl::ShaderPipeline shaderPipeline)
{
    this->particles.reserve(maxParticles);
    this->velocity = glm::vec3(0.0f, 1.0f, 0.0f) * glm::linearRand(0.1f, 0.2f);

    // 3D Cube Vertices (Position + Normals)
    float cubeVertices[] = {// Front
                            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.5f, 0.5f, 0.5f,
                            0.0f, 0.0f, 1.0f, -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
                            // Back
                            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.5f, 0.5f,
                            -0.5f, 0.0f, 0.0f, -1.0f, -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
                            // Left
                            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, -0.5f, 0.5f,
                            0.5f, -1.0f, 0.0f, 0.0f, -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
                            // Right
                            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f,
                            1.0f, 0.0f, 0.0f, 0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
                            // Down
                            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.5f, -0.5f,
                            0.5f, 0.0f, -1.0f, 0.0f, -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
                            // Up
                            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f,
                            0.0f, 1.0f, 0.0f, -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f};

    unsigned int cubeIndices[] = {// Front
                                  0, 1, 2, 2, 3, 0,
                                  // Back
                                  4, 6, 5, 6, 4, 7,
                                  // Left
                                  8, 9, 10, 10, 11, 8,
                                  // Right
                                  12, 14, 13, 14, 12, 15,
                                  // Down
                                  16, 17, 18, 18, 19, 16,
                                  // Up
                                  20, 22, 21, 22, 20, 23};

    // Create vertex buffer for cube vertices
    this->cubeVBO = renderDevice.createVertexBuffer(sizeof(cubeVertices), cubeVertices, core::gl::Usage::Static);

    // Create index buffer for cube indices
    this->cubeEBO = renderDevice.createIndexBuffer(sizeof(cubeIndices), cubeIndices, core::gl::IndexFormat::UInt,
                                                   core::gl::Usage::Static);

    // Create instance buffer for particle data
    this->instanceVBO =
        renderDevice.createVertexBuffer(maxParticles * sizeof(Particle), nullptr, core::gl::Usage::Dynamic);

    // Create constant buffer for camera uniforms
    struct CameraUniforms
    {
        glm::mat4 uViewMatrix;
        glm::mat4 uProjectionMatrix;
    };
    this->cameraUniformBuffer =
        renderDevice.createConstantBuffer(sizeof(CameraUniforms), nullptr, core::gl::Usage::Dynamic);

    // Create default sampler for particle texture if not set
    if (this->particleSampler == nullptr)
    {
        core::gl::SamplerDesc samplerDesc{};
        samplerDesc.minFilter = core::gl::TextureFilter::Linear;
        samplerDesc.magFilter = core::gl::TextureFilter::Linear;
        samplerDesc.addressU = core::gl::AddressMode::Clamp;
        samplerDesc.addressV = core::gl::AddressMode::Clamp;
        this->particleSampler = renderDevice.createSampler(samplerDesc);
    }

    // Always create the VAO if we have a shader pipeline
    if (shaderPipeline != nullptr)
    {
        this->createVertexArray(renderDevice, shaderPipeline);
    }
}

void cubos::engine::ParticleSpawner::createVertexArray(core::gl::RenderDevice& renderDevice,
                                                       core::gl::ShaderPipeline shaderPipeline)
{
    // Configure vertex array descriptor
    core::gl::VertexArrayDesc vaDesc{};
    vaDesc.elementCount = 6;
    vaDesc.shaderPipeline = shaderPipeline;

    // Configure cube vertex attributes (position and normals)
    vaDesc.elements[0].name = "position";
    vaDesc.elements[0].type = core::gl::Type::Float;
    vaDesc.elements[0].size = 3;
    vaDesc.elements[0].buffer.index = 0;
    vaDesc.elements[0].buffer.offset = 0;
    vaDesc.elements[0].buffer.stride = 6 * sizeof(float);

    vaDesc.elements[1].name = "normal";
    vaDesc.elements[1].type = core::gl::Type::Float;
    vaDesc.elements[1].size = 3;
    vaDesc.elements[1].buffer.index = 0;
    vaDesc.elements[1].buffer.offset = 3 * sizeof(float);
    vaDesc.elements[1].buffer.stride = 6 * sizeof(float);

    // Configure instance attributes (particle data)
    vaDesc.elements[2].name = "inPosition";
    vaDesc.elements[2].type = core::gl::Type::Float;
    vaDesc.elements[2].size = 3;
    vaDesc.elements[2].buffer.index = 1;
    vaDesc.elements[2].buffer.offset = offsetof(Particle, position);
    vaDesc.elements[2].buffer.stride = sizeof(Particle);

    vaDesc.elements[3].name = "inVelocity";
    vaDesc.elements[3].type = core::gl::Type::Float;
    vaDesc.elements[3].size = 3;
    vaDesc.elements[3].buffer.index = 1;
    vaDesc.elements[3].buffer.offset = offsetof(Particle, velocity);
    vaDesc.elements[3].buffer.stride = sizeof(Particle);

    vaDesc.elements[4].name = "inLifetime";
    vaDesc.elements[4].type = core::gl::Type::Float;
    vaDesc.elements[4].size = 1;
    vaDesc.elements[4].buffer.index = 1;
    vaDesc.elements[4].buffer.offset = offsetof(Particle, lifetime);
    vaDesc.elements[4].buffer.stride = sizeof(Particle);

    vaDesc.elements[5].name = "inAge";
    vaDesc.elements[5].type = core::gl::Type::Float;
    vaDesc.elements[5].size = 1;
    vaDesc.elements[5].buffer.index = 1;
    vaDesc.elements[5].buffer.offset = offsetof(Particle, age);
    vaDesc.elements[5].buffer.stride = sizeof(Particle);

    // Configure buffers in descriptor
    vaDesc.buffers[0] = this->cubeVBO;
    vaDesc.buffers[1] = this->instanceVBO;

    // Create the Vertex Array
    this->particleVAO = renderDevice.createVertexArray(vaDesc);
}

void cubos::engine::ParticleSpawner::updateParticles(float deltaTime)
{
    // Update time since last emission
    this->timeSinceLastEmission += deltaTime;

    // Calculate how many particles to emit based on the emission rate
    float particlesToEmit = this->emissionRate * this->timeSinceLastEmission;
    while (particlesToEmit >= 1.0f)
    {
        this->emitParticle();
        particlesToEmit -= 1.0f;
        this->timeSinceLastEmission -= 1.0f / this->emissionRate;
    }

    // Update age of all particles
    for (auto& particle : this->particles)
    {
        particle.age += deltaTime;
    }

    // Remove particles that exceeded their lifetime
    this->particles.erase(std::remove_if(this->particles.begin(), this->particles.end(),
                                         [](const Particle& p) { return p.age >= p.lifetime; }),
                          this->particles.end());

    // Update data on GPU
    this->uploadParticlesToGPU();
}

void cubos::engine::ParticleSpawner::updateUniforms(core::gl::ShaderPipeline& shaderPipeline,
                                                    const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix,
                                                    const glm::vec3& eyePos, float time)
{
    // Configure binding points for individual uniforms
    auto firstColorBP = shaderPipeline->getBindingPoint("firstColor");
    auto secondColorBP = shaderPipeline->getBindingPoint("secondColor");

    // Configure particle colors
    if (firstColorBP != nullptr)
    {
        firstColorBP->setConstant(this->startColor);
    }

    if (secondColorBP != nullptr)
    {
        secondColorBP->setConstant(this->endColor);
    }

    // Configure camera uniforms block
    if (this->cameraUniformBuffer != nullptr)
    {
        // Structure that corresponds to the Camera block in the shader
        struct CameraUniforms
        {
            glm::mat4 uViewMatrix;
            glm::mat4 uProjectionMatrix;
        };

        CameraUniforms cameraData;
        cameraData.uViewMatrix = viewMatrix;
        cameraData.uProjectionMatrix = projectionMatrix;

        // Update the constant buffer
        this->cameraUniformBuffer->fill(&cameraData, sizeof(cameraData));

        // Bind the constant buffer to the binding point
        auto cameraBP = shaderPipeline->getBindingPoint("Camera");
        if (cameraBP != nullptr)
        {
            cameraBP->bind(this->cameraUniformBuffer);
        }
    }

    // Configure other optional uniforms if they exist
    auto viewPosBP = shaderPipeline->getBindingPoint("viewPos");
    if (viewPosBP != nullptr)
    {
        viewPosBP->setConstant(eyePos);
    }

    auto timeBP = shaderPipeline->getBindingPoint("time");
    if (timeBP != nullptr)
    {
        timeBP->setConstant(time);
    }

    // Configure particle texture if available
    auto particleTextureBP = shaderPipeline->getBindingPoint("particleTexture");
    if (particleTextureBP != nullptr && this->particleTexture != nullptr)
    {
        particleTextureBP->bind(this->particleTexture);
        if (this->particleSampler != nullptr)
        {
            particleTextureBP->bind(this->particleSampler);
        }
    }

    // Configure texture availability flag for shader
    auto hasTextureBP = shaderPipeline->getBindingPoint("hasTexture");
    if (hasTextureBP != nullptr)
    {
        hasTextureBP->setConstant(this->particleTexture != nullptr ? 1 : 0);
    }
}

void cubos::engine::ParticleSpawner::drawGeometry(core::gl::RenderDevice& renderDevice)
{
    // Check if there are particles to render
    if (this->particles.empty() || this->particleVAO == nullptr)
    {
        return;
    }

    // Configure vertex array and draw
    renderDevice.setVertexArray(this->particleVAO);

    // Render instantiated cubes for each particle
    renderDevice.drawTrianglesIndexedInstanced(0, 36, static_cast<std::size_t>(this->particles.size()));
}

void cubos::engine::ParticleSpawner::draw(core::gl::RenderDevice& renderDevice, core::gl::ShaderPipeline shaderPipeline,
                                          const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix,
                                          const glm::vec3& eyePos, float time)
{
    if (this->particles.empty() || this->particleVAO == nullptr || shaderPipeline == nullptr)
    {
        return;
    }

    renderDevice.setShaderPipeline(shaderPipeline);

    this->updateUniforms(shaderPipeline, viewMatrix, projectionMatrix, eyePos, time);

    this->drawGeometry(renderDevice);
}

void cubos::engine::ParticleSpawner::uploadParticlesToGPU()
{
    if (this->instanceVBO != nullptr && !this->particles.empty())
    {
        this->instanceVBO->fill(this->particles.data(), this->particles.size() * sizeof(Particle));
    }
}

core::gl::Sampler engine::ParticleSpawner::createDefaultTextureSampler(core::gl::RenderDevice& renderDevice)
{
    core::gl::SamplerDesc samplerDesc{};
    samplerDesc.minFilter = core::gl::TextureFilter::Linear;
    samplerDesc.magFilter = core::gl::TextureFilter::Linear;
    samplerDesc.mipmapFilter = core::gl::TextureFilter::Linear;
    samplerDesc.addressU = core::gl::AddressMode::Clamp;
    samplerDesc.addressV = core::gl::AddressMode::Clamp;
    samplerDesc.maxAnisotropy = 4;

    return renderDevice.createSampler(samplerDesc);
}