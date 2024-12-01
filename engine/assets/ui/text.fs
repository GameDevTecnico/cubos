in vec2 texCoord;
out vec4 out_color;
uniform sampler2D fontAtlas;

const float pxRange = 4.0;

layout(std140) uniform PerElement
{
    vec2 xRange;
    vec2 yRange;
    vec4 color;
    int depth;
};

float screenPxRange()
{
    vec2 unitRange = vec2(pxRange) / vec2(textureSize(fontAtlas, 0));
    vec2 screenTexSize = vec2(1.0) / fwidth(texCoord);
    return max(0.5 * dot(unitRange, screenTexSize), 1.0);
}

float median(float r, float g, float b)
{
    return max(min(r, g), min(max(r, g), b));
}

void main()
{
    vec3 msd = texture(fontAtlas, texCoord).rgb;
    float sd = median(msd.r, msd.g, msd.b);
    float screenPxDistance = screenPxRange() * (sd - 0.5);
    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
    out_color = mix(vec4(0.0), color, opacity);
}
