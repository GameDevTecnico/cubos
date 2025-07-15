in float ageFraction;
in vec3 fragNormal;
in vec3 fragWorldPos;
in vec2 texCoord;
out vec4 FragColor;

uniform vec3 firstColor;
uniform vec3 secondColor;
uniform sampler2D particleTexture;
uniform int hasTexture;

void main() {
    // Color interpolation between two colors based on ageFraction
    vec3 color = mix(firstColor, secondColor, ageFraction);
    
    if (hasTexture == 1) {
        vec4 texColor = texture(particleTexture, texCoord);
        color *= texColor.rgb;
    }
    
    // Simple lighting with fixed direction
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    vec3 normal = normalize(fragNormal);
    
    float ambient = 0.3;
    
    float diffuse = max(dot(normal, lightDir), 0.0);
    
    float lighting = ambient + diffuse * 0.7;
    
    vec3 finalColor = color * lighting;
    
    FragColor = vec4(finalColor, 1.0);
}