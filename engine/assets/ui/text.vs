in vec2 in_position;
in vec2 in_texCoord;

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
    gl_Position = mvp * (vec4(xRange.x, yRange.x, 0, 0) + vec4(in_position, depth, 1));
    texCoord = in_texCoord;
}
