#version 330 core

layout(std140) uniform PerElement
{
    vec2 xRange;
    vec2 yRange;
    vec4 color;
    int depth;
};

out vec4 out_color;

void main()
{
    out_color = color;
}
