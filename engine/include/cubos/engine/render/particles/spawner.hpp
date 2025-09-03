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

        /// @brief Constant buffer for camera matrices.
        core::gl::ConstantBuffer cameraUniformBuffer{nullptr};

        /// @brief Texture for particle rendering.
        core::gl::Texture2D particleTexture{nullptr};

        /// @brief List of active particles.
        std::vector<Particle> particles = {};

        void setEmissionRate(float rate);
        void setLifetime(float minLifetime, float maxLifetime);
        void setEmissionPoint(const glm::vec3& point);
        void setParticleTexture(core::gl::Texture2D textureID);
        virtual void updateUniforms(core::gl::ShaderPipeline& shaderPipeline, const glm::mat4& viewMatrix,
                                    const glm::mat4& projectionMatrix);
        virtual void draw(core::gl::RenderDevice& renderDevice, core::gl::ShaderPipeline shaderPipeline,
                          const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
        void setColors(const glm::vec3& firstColor, const glm::vec3& secondColor);
        void setVelocity(const glm::vec3& velocity);
        void emitParticle();
        void uploadParticlesToGPU();
        void setupBuffers(core::gl::RenderDevice& renderDevice, core::gl::ShaderPipeline shaderPipeline);
        void setRadius(float radius);
    };
} // namespace cubos::engine