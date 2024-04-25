#version 330 core

#define DOWNSCALE_PASS 0
#define UPSCALE_PASS 1
#define COMBINE_PASS 2

uniform sampler2D inputTexture;
uniform sampler2D sourceTexture;
uniform float scaling;
uniform float intensity;
uniform int pass;

in vec2 fragUv;

out vec4 fragColor;

vec3 sample(vec2 uv)
{
    return texture(inputTexture, uv).rgb;
}

vec4 sampleBox(vec2 texelSize, vec2 uv, float delta)
{
    vec4 offset = texelSize.xyxy * vec4(-delta, -delta, delta, delta);
    vec3 color = sample(uv + offset.xy) + sample(uv + offset.zy) +
                 sample(uv + offset.xw) + sample(uv + offset.zw);
    return vec4(color * 0.25, 1.0);
}

void main(void)
{
    vec2 texelSize = 1.0 / textureSize(inputTexture, 0);
    switch(pass)
    {
        case DOWNSCALE_PASS:
            fragColor = sampleBox(texelSize, fragUv * scaling, 1.0);
            break;
        case UPSCALE_PASS:
            fragColor = sampleBox(texelSize, fragUv * scaling, 0.5);
            break;
        case COMBINE_PASS:
            fragColor = texture(sourceTexture, fragUv);
            fragColor += intensity * sampleBox(texelSize, fragUv * scaling, 0.5);
            fragColor.a = 1.0;
            break;
    }
}
