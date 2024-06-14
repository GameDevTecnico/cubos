in vec3 position;

layout(std140) uniform PerElement
{
    vec2 xRange;
    vec2 yRange;
    vec4 color;
    int depth;
};

out vec2 texCoord;

uniform MVP
{
    mat4 mvp;
};

void main()
{
    gl_Position = mvp * vec4((1 - position.x) * xRange.x + position.x * xRange.y,
                             (1 - position.y) * yRange.x + position.y * yRange.y, depth, 1);
    texCoord = position.xy;
}
