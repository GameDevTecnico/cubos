in uvec3 position;

layout(std140) uniform PerScene
{
    mat4 lightViewProj;
};

layout(std140) uniform PerMesh
{
    mat4 model;
};

void main(void)
{
    gl_Position = lightViewProj * model * vec4(position, 1.0);
}
