in float ageFraction;
in vec3 fragNormal;
in vec3 fragWorldPos;
out vec4 FragColor;

uniform vec3 firstColor;
uniform vec3 secondColor;

void main() {
    // Interpolação de cores com base na idade
    vec3 color = mix(firstColor, secondColor, ageFraction);
    
    // Iluminação simples com direção fixa
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    vec3 normal = normalize(fragNormal);
    
    // Luz ambiente
    float ambient = 0.3;
    
    // Luz difusa
    float diffuse = max(dot(normal, lightDir), 0.0);
    
    // Combinação final
    float lighting = ambient + diffuse * 0.7;
    
    // Aplicar iluminação à cor
    vec3 finalColor = color * lighting;
    
    // Opacidade baseada na idade (não usar alpha blending)
    FragColor = vec4(finalColor, 1.0);
}