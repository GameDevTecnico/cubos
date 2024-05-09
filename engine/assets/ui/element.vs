#version 330 core

in vec3 position;

uniform PerElement
{
    vec2 xRange;
    vec2 yRange;
    int depth;
};

uniform MVP
{
    mat4 mvp;
};

void main()
{
    gl_Position = mvp * vec4((1 - position.x) * xRange.x + position.x * xRange.y,
                             (1 - position.y) * yRange.x + position.y * yRange.y, depth, 1);
}
