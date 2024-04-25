#version 330 core

uniform sampler2D inputTexture;

uniform vec3 luminance;
uniform vec4 thresholdFilter;

in vec2 fragUv;

out vec4 color;

void main()
{
    vec4 texel = texture(inputTexture, fragUv);
    float pixelIntensity = dot(texel.rgb, luminance);
    float soft = pixelIntensity - thresholdFilter.y;
    soft = clamp(soft, 0.0, thresholdFilter.z);
    soft = soft * soft * thresholdFilter.w;
    float contrib = max(soft, pixelIntensity - thresholdFilter.x) / max(pixelIntensity, 0.00001);
    color = vec4(texel.rgb * contrib, texel.a);
}