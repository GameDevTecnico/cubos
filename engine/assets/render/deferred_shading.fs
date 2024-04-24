#version 330 core

in vec2 fragUv;

uniform sampler2D positionTexture;
uniform sampler2D normalTexture;
uniform sampler2D albedoTexture;

struct DirectionalLight
{
    vec4 direction;
    vec4 color;
    float intensity;
};

struct PointLight
{
    vec4 position;
    vec4 color;
    float intensity;
    float range;
};

struct SpotLight
{
    vec4 position;
    vec4 direction;
    vec4 color;
    float intensity;
    float range;
    float spotCutoff;
    float innerSpotCutoff;
};

layout(std140) uniform PerScene
{
    // Camera matrices.
    mat4 inverseView;
    mat4 inverseProjection;

    // Environment parameters.
    vec4 skyGradient[2];
    vec4 ambientLight;

    // Lights data.
    uint numDirectionalLights;
    DirectionalLight directionalLights[16];

    uint numPointLights;
    PointLight pointLights[128];

    uint numSpotLights;
    SpotLight spotLights[128];
};

layout(location = 0) out vec3 color;

float remap(float value, float min1, float max1, float min2, float max2)
{
    return max2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

vec3 spotLightCalc(vec3 fragPos, vec3 fragNormal, SpotLight light)
{
    vec3 toLight = vec3(light.position) - fragPos;
    float r = length(toLight) / light.range;
    if (r < 1)
    {
        vec3 toLightNormalized = normalize(toLight);
        float a = dot(toLightNormalized, -light.direction.xyz);
        if (a > light.spotCutoff)
        {
            float angleValue = clamp(remap(a, light.innerSpotCutoff, light.spotCutoff, 1, 0), 0, 1);
            float attenuation = clamp(1.0 / (1.0 + 25.0 * r * r) * clamp((1 - r) * 5.0, 0, 1), 0, 1);
            float diffuse = max(dot(fragNormal, toLightNormalized), 0);
            return angleValue * attenuation * diffuse * light.intensity * vec3(light.color);
        }
    }
    return vec3(0);
}

vec3 directionalLightCalc(vec3 fragNormal, DirectionalLight light)
{
    return max(dot(fragNormal, -light.direction.xyz), 0) * light.intensity * vec3(light.color);
}

vec3 pointLightCalc(vec3 fragPos, vec3 fragNormal, PointLight light)
{
    vec3 toLight = vec3(light.position) - fragPos;
    float r = length(toLight) / light.range;
    if (r < 1)
    {
        float attenuation = clamp(1.0 / (1.0 + 25.0 * r * r) * clamp((1 - r) * 5.0, 0, 1), 0, 1);
        float diffuse = max(dot(fragNormal, vec3(normalize(toLight))), 0);
        return attenuation * diffuse * light.intensity * vec3(light.color);
    }
    return vec3(0);
}

vec3 rayDir(vec2 uv)
{
    // Convert fragment coords to normalized device space.
    vec4 pointNDS = vec4(uv * 2.0 - 1.0, -1.0, 1.0);
    // Get the point in view space.
    vec3 pointView = (inverseProjection * pointNDS).xyz;
    // Get the direction in world space.
    vec3 dir = (inverseView * vec4(pointView, 0.0)).xyz;
    return normalize(dir);
}

void main()
{
    vec3 normal = texture(normalTexture, fragUv).xyz;
    if (normal == vec3(0.0))
    {
        // If the normal is zero, then we are rendering the sky.
        vec3 dir = rayDir(fragUv);
        color = mix(skyGradient[0].rgb, skyGradient[1].rgb, clamp(dir.y * 0.5 + 0.5, 0.0, 1.0));
    }
    else
    {
        vec3 albedo = texture(albedoTexture, fragUv).rgb;
        vec3 position = texture(positionTexture, fragUv).xyz;

        // Calculate lighting from each light source.
        vec3 lighting = ambientLight.rgb;
        for (uint i = 0u; i < numSpotLights; i++)
        {
            lighting += spotLightCalc(position, normal, spotLights[i]);
        }
        for (uint i = 0u; i < numDirectionalLights; i++)
        {
            lighting += directionalLightCalc(normal, directionalLights[i]);
        }
        for (uint i = 0u; i < numPointLights; i++)
        {
            lighting += pointLightCalc(position, normal, pointLights[i]);
        }
        color = min(albedo * lighting, 1.0);
    }
}
