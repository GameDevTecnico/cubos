#define MAX_KERNEL_SIZE 64

in vec2 fragUv;

uniform sampler2D positionTexture;
uniform sampler2D normalTexture;
uniform sampler2D noiseTexture;

uniform vec2 viewportOffset;
uniform vec2 viewportSize;

layout(std140) uniform PerScene
{
    mat4 view;
    mat4 projection;
    vec4 samples[MAX_KERNEL_SIZE];
    int kernelSize;
    float radius;
    float bias;
};

layout (location = 0) out float color;

vec3 getViewPosition(vec2 fragUv)
{
    vec2 uv = fragUv * viewportSize + viewportOffset;
    vec3 worldPosition = texture(positionTexture, uv).xyz;
    return (view * vec4(worldPosition, 1.0)).xyz;
}

vec3 getViewNormal(vec2 fragUv)
{
    vec2 uv = fragUv * viewportSize + viewportOffset;
    vec3 worldNormal = texture(normalTexture, uv).xyz;
    return normalize(view * vec4(worldNormal, 0.0)).xyz;
}

void main(void)
{
    // Sample view-space position and normal of the current fragment.
    vec3 position = getViewPosition(fragUv);
    vec3 normal = getViewNormal(fragUv);

    // Sample a noise vector.
    vec2 noiseScale = vec2(textureSize(positionTexture, 0)) / 4.0;
    vec3 randomVector = normalize(texture(noiseTexture, fragUv * noiseScale).xyz);

    // Calculate the TBN matrix from the fragment normal and the random vector.
    vec3 tangent = normalize(randomVector - normal * dot(randomVector, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    // Calculate the occlusion factor.
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; i++)
    {
        // Calculate the sample position in view-space.
        vec3 samplePosition = TBN * samples[i].xyz;
        samplePosition = position + samplePosition * radius;

        // Calculate the sample position in clip-space.
        vec4 sampleOffset = vec4(samplePosition, 1.0);
        sampleOffset = projection * sampleOffset;
        sampleOffset.xyz /= sampleOffset.w;
        sampleOffset.xyz = sampleOffset.xyz * 0.5 + 0.5;

        // Sample the real depth value of the sample's fragment.
        float sampleDepth = getViewPosition(sampleOffset.xy).z;

        // Calculate the occlusion factor.
        // If the sample is farther away than the real depth value, then it is occluded.
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(position.z - sampleDepth));
        occlusion += (sampleDepth >= samplePosition.z + bias ? 1.0 : 0.0) * rangeCheck;
    }

    // We substract the occlusion factor from 1.0 to get the ambient occlusion color.
    // Then in further processing we can simply multiply this value with the color of the fragment.
    color = 1.0 - (occlusion / float(kernelSize));
}
