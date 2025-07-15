// Cube attributes
in vec3 position;
in vec3 normal;

// Particle attributes
in vec3 inPosition;
in vec3 inVelocity;
in float inLifetime;
in float inAge;

out float ageFraction;
out vec3 fragNormal;
out vec3 fragWorldPos;
out vec2 texCoord;

uniform Camera {
    mat4 uViewMatrix;
    mat4 uProjectionMatrix;
};

void main() {
    vec3 particlePosition = inPosition + inVelocity * inAge;

    if (inLifetime > 0.0) {
        ageFraction = inAge / inLifetime;
    } else {
        ageFraction = 0.0;
    }

    // Scale the particle based on its age
    float scale = mix(0.1, 0.02, ageFraction);

    // Apply scale to vertex position
    vec3 scaledPosition = position * scale;

    texCoord = position.xy + 0.5;

    // Final vertex position in world space
    vec3 worldPosition = particlePosition + scaledPosition;

    // Transform to clip space
    vec4 viewPosition = uViewMatrix * vec4(worldPosition, 1.0);
    gl_Position = uProjectionMatrix * viewPosition;

    fragNormal = normal;
    fragWorldPos = worldPosition;
}