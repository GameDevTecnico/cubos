#version 330 core

in uvec3 position;
in uint normal;
in uint material;

out vec3 fragPosition;
out vec3 fragNormal;
out vec3 fragAlbedo;
flat out uvec2 fragPicker;

uniform PerScene
{
    mat4 viewProj;
};

uniform PerMesh
{
    mat4 model;
    uint picker;
};

uniform sampler2D palette;

void main(void)
{
    vec4 worldPosition = model * vec4(position, 1.0);
    fragPosition = vec3(worldPosition);
    gl_Position = viewProj * worldPosition;

    vec3 localNormal = vec3(0, 0, 0);
    if (normal == 0u) localNormal.x = +1;
    if (normal == 1u) localNormal.x = -1;
    if (normal == 2u) localNormal.y = +1;
    if (normal == 3u) localNormal.y = -1;
    if (normal == 4u) localNormal.z = +1;
    if (normal == 5u) localNormal.z = -1;
    mat3 N = transpose(inverse(mat3(model)));
    fragNormal = N * localNormal;

    fragAlbedo = texelFetch(palette, ivec2(mod(material, 256u), material / 256u), 0).rgb;

    fragPicker.r = (picker >> 16U);
    fragPicker.g = (picker & 65535U);
}
