in uvec3 position;

uniform PerScene
{
    mat4 lightViewProj;
};

uniform PerMesh
{
    mat4 model;
};

void main(void)
{
    gl_Position = lightViewProj * model * vec4(position, 1.0);
}
