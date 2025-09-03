in vec3 position;
in vec3 normal;

// Atributos de instância (dados da partícula)
in vec3 inPosition;
in vec3 inVelocity;
in float inLifetime;
in float inAge;

out float ageFraction;
out vec3 fragNormal;
out vec3 fragWorldPos;

uniform Camera {
    mat4 uViewMatrix;
    mat4 uProjectionMatrix;
};

void main() {
    // Calcular posição atual da partícula
    vec3 particlePosition = inPosition + inVelocity * inAge;

    if (inLifetime > 0.0) {
        ageFraction = inAge / inLifetime;
    } else {
        ageFraction = 0.0;
    }

    // Escalar baseado na idade (cubos menores conforme envelhecem)
    //float scale = mix(0.1, 0.02, ageFraction);
    float scale = 1.0;

    // Aplicar escala à posição do vértice
    vec3 scaledPosition = position * scale;
    
    // Posição final do vértice no mundo
    vec3 worldPosition = particlePosition + scaledPosition;
    
    // Transformar para clip space
    vec4 viewPosition = uViewMatrix * vec4(worldPosition, 1.0);
    gl_Position = uProjectionMatrix * viewPosition;

    // Passar dados para fragment shader
    fragNormal = normal;
    fragWorldPos = worldPosition;
}