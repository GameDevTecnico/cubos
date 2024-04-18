#version 330 core

in vec2 fragUv;

uniform sampler2D hdrTexture;
uniform float gamma;
uniform float exposure;

layout(location = 0) out vec4 color;

void main()
{
    vec3 hdrColor = texture(hdrTexture, fragUv).rgb;
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    mapped = pow(mapped, vec3(1.0 / gamma));
    color = vec4(mapped, 1.0);
}
