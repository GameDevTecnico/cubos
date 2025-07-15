/// @file
/// @brief Component @ref cubos::engine::ParticleSpawner.
/// @ingroup render-particles-plugin
#pragma once
#include <glm/vec3.hpp>

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>
#include <cubos/engine/render/particles/particle.hpp>

namespace cubos::engine
{
    /// @brief Represents a particle spawner in the particle system.
    /// @ingroup render-particles-plugin
    struct CUBOS_ENGINE_API ParticleSpawner
    {
        CUBOS_REFLECT;

        /// @brief Maximum number of particles that can be active at once.
        unsigned int maxParticles = 1000;

        /// @brief Number of particles to emit per second.
        float emissionRate = 10.0f;

        /// @brief Minimum lifetime for spawned particles.
        float minLifetime = 1.0f;

        /// @brief Maximum lifetime for spawned particles.
        float maxLifetime = 5.0f;

        /// @brief Time since the last particle was emitted.
        float timeSinceLastEmission = 0.0f;

        /// @brief Position where particles are spawned.
        glm::vec3 emissionPoint = {0.0f, 0.0f, 0.0f};

        /// @brief Radius of the emission area.
        float emissionRadius = 0.1f;

        /// @brief Color at the start of particle lifetime.
        glm::vec3 startColor = {1.0f, 1.0f, 1.0f};

        /// @brief Color at the end of particle lifetime.
        glm::vec3 endColor = {1.0f, 1.0f, 1.0f};

        /// @brief Velocity of the particles.
        glm::vec3 velocity = {0.0f, 0.0f, 0.0f};

        /// @brief VBO for particle data.
        core::gl::VertexBuffer particleBuffer{nullptr};

        /// @brief VAO for rendering particles.
        core::gl::VertexArray particleVAO{nullptr};

        /// @brief VBO for cube vertices.
        core::gl::VertexBuffer cubeVBO{nullptr};

        /// @brief EBO for cube indices.
        core::gl::IndexBuffer cubeEBO{nullptr};

        /// @brief VBO for instance data.
        core::gl::VertexBuffer instanceVBO{nullptr};

        /// @brief Constant buffer for camera uniforms.
        core::gl::ConstantBuffer cameraUniformBuffer{nullptr};

        /// @brief Texture for particle rendering.
        core::gl::Texture2D particleTexture{nullptr};

        /// @brief Sampler for particle texture.
        core::gl::Sampler particleSampler{nullptr};

        /// @brief List of active particles.
        std::vector<Particle> particles = {};

        /// @brief Emits a new particle.
        void emitParticle();

        /// @brief Uploads the current particles to the GPU.
        void uploadParticlesToGPU();

        /// @brief Sets up all buffers and vertex array for particle rendering.
        /// @param renderDevice The render device to create resources with.
        /// @param shaderPipeline Optional shader pipeline. If provided, creates the vertex array object immediately.
        void setupBuffers(core::gl::RenderDevice& renderDevice, core::gl::ShaderPipeline shaderPipeline = nullptr);

        /// @brief Creates the vertex array object with the specified shader pipeline.
        /// @param renderDevice The render device to create the VAO with.
        /// @param shaderPipeline The shader pipeline to associate with the VAO.
        void createVertexArray(core::gl::RenderDevice& renderDevice, core::gl::ShaderPipeline shaderPipeline);

        /// @brief Sets the emission rate for particles.
        /// @param rate The emission rate (particles per second).
        void setEmissionRate(float rate);

        /// @brief Sets the lifetime for particles.
        /// @param minLifetime Minimum lifetime for particles.
        /// @param maxLifetime Maximum lifetime for particles.
        void setLifetime(float minLifetime, float maxLifetime);

        /// @brief Sets the emission point for particles.
        /// @param point The point in world space where particles will be emitted from.
        void setEmissionPoint(const glm::vec3& point);

        /// @brief Sets the particle texture.
        /// @param texture The texture to use for particles.
        /// @note If no custom sampler is set, a default sampler will be created automatically.
        void setParticleTexture(core::gl::Texture2D textureID);

        /// @brief Sets the particle texture with custom sampler.
        /// @param texture The texture to use for particles.
        /// @param sampler The sampler to use for texture filtering.
        void setParticleTexture(core::gl::Texture2D texture, core::gl::Sampler sampler);

        /// @brief Updates particle simulation logic (spawning, aging, cleanup).
        /// Call this every frame to update particle lifecycle.
        /// @param deltaTime Time elapsed since last frame.
        void updateParticles(float deltaTime);

        /// @brief Updates shader uniforms for rendering
        /// @param shaderPipeline The shader pipeline to set uniforms on.
        /// @param viewMatrix Camera view matrix.
        /// @param projectionMatrix Camera projection matrix.
        /// @param eyePos Camera world position.
        /// @param time Current time.
        void updateUniforms(core::gl::ShaderPipeline& shaderPipeline, const glm::mat4& viewMatrix,
                            const glm::mat4& projectionMatrix, const glm::vec3& eyePos, float time);

        /// @brief Draws the particle geometry
        /// @param renderDevice The render device to use.
        void drawGeometry(core::gl::RenderDevice& renderDevice);

        /// @brief Draws the particles using the specified shader pipeline.
        /// @param renderDevice The render device to use.
        /// @param shaderPipeline The shader pipeline to use.
        /// @param viewMatrix Camera view matrix.
        /// @param projectionMatrix Camera projection matrix.
        /// @param eyePos Camera world position.
        /// @param time Current time.
        void draw(core::gl::RenderDevice& renderDevice, core::gl::ShaderPipeline shaderPipeline,
                  const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::vec3& eyePos, float time);

        /// @brief Sets the emission radius for particles.
        /// @param radius The emission radius (distance from the emission point).
        void setEmissionRadius(float radius);

        /// @brief Sets the colors for particles.
        /// @param firstColor The color of the particle at birth.
        /// @param secondColor The color of the particle at death.
        void setColors(const glm::vec3& firstColor, const glm::vec3& secondColor);

        /// @brief Sets the particle velocity.
        /// @param velocity The velocity vector to set for particles.
        void setVelocity(const glm::vec3& velocity);

        /// @brief Creates a default sampler for particle textures.
        /// @param renderDevice The render device to create the sampler with.
        /// @return A sampler suitable for particle textures.
        static core::gl::Sampler createDefaultTextureSampler(core::gl::RenderDevice& renderDevice);
    };
} // namespace cubos::engine