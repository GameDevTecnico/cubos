#version 330 core

in uvec3 position;

uniform PerMesh
{
    mat4 model;
};

void main(void)
{
    gl_Position = model * vec4(position, 1.0);
}
