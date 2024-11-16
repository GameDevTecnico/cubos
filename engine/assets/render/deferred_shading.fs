#version 330 core

in vec2 fragUv;

uniform sampler2D positionTexture;
uniform sampler2D normalTexture;
uniform sampler2D albedoTexture;
uniform sampler2D ssaoTexture;
uniform sampler2D spotShadowAtlasTexture;
uniform sampler2DArray pointShadowAtlasTexture;

uniform sampler2DArray directionalShadowMap; // only one directional light with shadows is supported, for now

uniform vec2 viewportOffset;
uniform vec2 viewportSize;

struct DirectionalLight
{
    vec4 direction;
    vec4 color;
    mat4 matrices[10]; // hardcoded max splits
    float intensity;
    float shadowBias;
    float shadowBlurRadius;
    float padding;
    vec4 shadowFarSplitDistances[3]; // intended to be a float array, but std140 layout aligns array elements
                                     // to vec4 size; number of vec4s = ceiling(MaxCascades / 4 components)
    int numCascades;
    float normalOffsetScale;
};

struct PointLight
{
    vec4 position;
    vec4 color;
    mat4 matrices[6];
    float intensity;
    float range;
    float shadowBias;
    float shadowBlurRadius;
    vec2 shadowMapOffset;
    vec2 shadowMapSize;
    float normalOffsetScale;
};

struct SpotLight
{
    vec4 position;
    vec4 direction;
    mat4 matrix;
    vec4 color;
    float intensity;
    float range;
    float spotCutoff;
    float innerSpotCutoff;
    vec2 shadowMapOffset;
    vec2 shadowMapSize;
    float shadowBias;
    float shadowBlurRadius;
    float normalOffsetScale;
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
    DirectionalLight directionalLights[16];
    PointLight pointLights[128];
    SpotLight spotLights[128];

    uint numDirectionalLights;
    uint numPointLights;
    uint numSpotLights;
    
    int directionalLightWithShadowsId; // index of directional light that casts shadows, or -1 if none
};

layout(location = 0) out vec3 color;

float remap(float value, float min1, float max1, float min2, float max2)
{
    return max2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

vec3 applyNormalOffset(vec3 normal, vec3 lightDir, vec3 position, float normalOffsetScale) {

    float prod = dot(normal, lightDir);
    prod = max(0.0, prod); 
    
    float slopeScale = 0.02;
    float offset = slopeScale * prod + normalOffsetScale;
    
    vec3 offsetPosition = position + normal * offset;
    return offsetPosition;
}

vec3 spotLightCalc(vec3 fragPos, vec3 fragNormal, uint lightI)
{
    // Shadows
    float shadow = 0.0;
    if (spotLights[lightI].shadowMapSize.x > 0.0)
    {
        float normalOffsetScale = spotLights[lightI].normalOffsetScale;
        vec3 offsetFragPos = normalOffsetScale > 0.0 ? applyNormalOffset(fragNormal, spotLights[lightI].direction.xyz, fragPos, normalOffsetScale) : fragPos;
        vec4 positionLightSpace = spotLights[lightI].matrix * vec4(offsetFragPos, 1.0);
        vec3 projCoords = positionLightSpace.xyz / positionLightSpace.w;
        projCoords = projCoords * 0.5 + 0.5;
        vec2 uv = projCoords.xy * spotLights[lightI].shadowMapSize + spotLights[lightI].shadowMapOffset;
        float currentDepth = projCoords.z;
        float bias = spotLights[lightI].shadowBias / positionLightSpace.w; // make the bias not depend on near/far planes
        // PCF
        if (spotLights[lightI].shadowBlurRadius <= 0.001f)
        {
            float pcfDepth = texture(spotShadowAtlasTexture, uv).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
        else
        {
            vec2 texelSize = vec2(1.0 / 1024.0); // largely arbitrary value, affects blur size
            for(int xi = -1; xi <= 1; xi++)
            {
                for(int yi = -1; yi <= 1; yi++)
                {
                    float x = spotLights[lightI].shadowBlurRadius*float(xi);
                    float y = spotLights[lightI].shadowBlurRadius*float(yi);
                    float pcfDepth = texture(spotShadowAtlasTexture, uv + vec2(x, y) * texelSize).r;
                    shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
                }
            }
            shadow /= 9.0;
        }
    }

    // Lighting
    vec3 toLight = vec3(spotLights[lightI].position) - fragPos;
    float r = length(toLight) / spotLights[lightI].range;
    if (r < 1)
    {
        vec3 toLightNormalized = normalize(toLight);
        float a = dot(toLightNormalized, -spotLights[lightI].direction.xyz);
        if (a > spotLights[lightI].spotCutoff)
        {
            float angleValue = clamp(remap(a, spotLights[lightI].innerSpotCutoff, spotLights[lightI].spotCutoff, 1, 0), 0, 1);
            float attenuation = clamp(1.0 / (1.0 + 25.0 * r * r) * clamp((1 - r) * 5.0, 0, 1), 0, 1);
            float diffuse = max(dot(fragNormal, toLightNormalized), 0);
            return angleValue * attenuation * diffuse * (1.0 - shadow) * spotLights[lightI].intensity
                * vec3(spotLights[lightI].color);
        }
    }
    return vec3(0);
}

vec3 directionalLightCalc(vec3 fragPos, vec3 fragNormal, uint lightI, bool drawShadows)
{
    float normalOffsetScale = directionalLights[lightI].normalOffsetScale;
    vec3 offsetFragPos = normalOffsetScale > 0.0 ? applyNormalOffset(fragNormal, directionalLights[lightI].direction.xyz, fragPos, normalOffsetScale) : fragPos;
    // Shadows
    float shadow = 0.0;
    if (drawShadows)
    {
        // Select split
        vec4 positionCameraSpace = inverse(inverseView) * vec4(offsetFragPos, 1.0);
        float depthCameraSpace = abs(positionCameraSpace.z);
        int split = directionalLights[lightI].numCascades - 1;
        for (int i = 0; i < directionalLights[lightI].numCascades; i++)
        {
            float far = directionalLights[lightI].shadowFarSplitDistances[i / 4][i % 4];
            if (depthCameraSpace < far)
            {
                split = i;
                break;
            }
        }

        // Sample shadow map
        vec4 positionLightSpace = directionalLights[lightI].matrices[split] * vec4(offsetFragPos, 1.0);
        vec3 projCoords = positionLightSpace.xyz / positionLightSpace.w;
        projCoords = projCoords * 0.5 + 0.5;
        if (projCoords.z < 1.0)
        {
            vec2 uv = projCoords.xy;
            float currentDepth = projCoords.z;
            float bias = directionalLights[lightI].shadowBias / positionLightSpace.w; // make the bias not depend on near/far planes
            // PCF
            if (directionalLights[lightI].shadowBlurRadius <= 0.001f)
            {
                float pcfDepth = texture(directionalShadowMap, vec3(uv.xy, split)).r;
                shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
            }
            else
            {
                vec2 texelSize = vec2(1.0 / 1024.0); // largely arbitrary value, affects blur size
                for(int xi = -1; xi <= 1; xi++)
                {
                    for(int yi = -1; yi <= 1; yi++)
                    {
                        float x = directionalLights[lightI].shadowBlurRadius*float(xi);
                        float y = directionalLights[lightI].shadowBlurRadius*float(yi);
                        vec2 newUv = uv + vec2(x, y) * texelSize;
                        float pcfDepth = texture(directionalShadowMap, vec3(newUv.xy, split)).r;
                        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
                    }
                }
                shadow /= 9.0;
            }
        }
    }
    return max(dot(fragNormal, -directionalLights[lightI].direction.xyz), 0)
        * (1.0 - shadow) * directionalLights[lightI].intensity * vec3(directionalLights[lightI].color);
}

vec3 pointLightCalc(vec3 fragPos, vec3 fragNormal, uint lightI)
{
    vec3 toLight = vec3(pointLights[lightI].position) - fragPos;
    // Shadows
    float shadow = 0.0;
    if (pointLights[lightI].shadowMapSize.x > 0.0)
    {
        // Select cube face
        int face = 0;
        float rx = -toLight.x;
        float ry = -toLight.y;
        float rz = -toLight.z;
        if (abs(rz) >= abs(rx) && abs(rz) >= abs(ry))
        {
            if (rz <= 0) // z-
                face = 0;
            else // z+
                face = 1;
        }
        else if (abs(rx) >= abs(ry) && abs(rx) >= abs(rz))
        {
            if (rx <= 0) // x-
                face = 2;
            else // x+
                face = 3;
        }
        else if (abs(ry) >= abs(rx) && abs(ry) >= abs(rz))
        {
            if (ry <= 0) // y-
                face = 5;
            else // y+
                face = 4;
        }

        float normalOffsetScale = pointLights[lightI].normalOffsetScale;
        vec3 offsetFragPos = normalOffsetScale > 0.0 ? applyNormalOffset(fragNormal, toLight, fragPos, normalOffsetScale) : fragPos;
        vec4 positionLightSpace = pointLights[lightI].matrices[face] * vec4(offsetFragPos, 1.0);
        vec3 projCoords = positionLightSpace.xyz / positionLightSpace.w;
        projCoords = projCoords * 0.5 + 0.5;
        vec2 uv = projCoords.xy * pointLights[lightI].shadowMapSize + pointLights[lightI].shadowMapOffset;
        float currentDepth = projCoords.z;
        float bias = pointLights[lightI].shadowBias / positionLightSpace.w; // make the bias not depend on near/far planes
        // PCF
        if (pointLights[lightI].shadowBlurRadius <= 0.001f)
        {
            float pcfDepth = texture(pointShadowAtlasTexture, vec3(uv.xy, face)).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
        else
        {
            vec2 texelSize = vec2(1.0 / 1024.0); // largely arbitrary value, affects blur size
            for(int xi = -1; xi <= 1; xi++)
            {
                for(int yi = -1; yi <= 1; yi++)
                {
                    float x = pointLights[lightI].shadowBlurRadius*float(xi);
                    float y = pointLights[lightI].shadowBlurRadius*float(yi);
                    vec2 newUv = uv + vec2(x, y) * texelSize;
                    float pcfDepth = texture(pointShadowAtlasTexture, vec3(newUv.xy, face)).r;
                    shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
                }
            }
            shadow /= 9.0;
        }
    }

    // Lighting
    float r = length(toLight) / pointLights[lightI].range;
    if (r < 1)
    {
        float attenuation = clamp(1.0 / (1.0 + 25.0 * r * r) * clamp((1 - r) * 5.0, 0, 1), 0, 1);
        float diffuse = max(dot(fragNormal, vec3(normalize(toLight))), 0);
        return attenuation * diffuse * (1.0 - shadow) * pointLights[lightI].intensity
            * vec3(pointLights[lightI].color);
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
    vec2 uv = fragUv * viewportSize + viewportOffset;

    vec3 normal = texture(normalTexture, uv).xyz;
    if (normal == vec3(0.0))
    {
        // If the normal is zero, then we are rendering the sky.
        vec3 dir = rayDir(fragUv);
        color = mix(skyGradient[0].rgb, skyGradient[1].rgb, clamp(dir.y * 0.5 + 0.5, 0.0, 1.0));
    }
    else
    {
        vec3 albedo = texture(albedoTexture, uv).rgb;
        vec3 position = texture(positionTexture, uv).xyz;
        float ssao = texture(ssaoTexture, uv).r;

        // Calculate lighting from each light source.
        vec3 lighting = ambientLight.rgb * ssao;
        for (uint i = 0u; i < numSpotLights; i++)
        {
            lighting += spotLightCalc(position, normal, i);
        }
        for (uint i = 0u; i < numDirectionalLights; i++)
        {
            lighting += directionalLightCalc(position, normal, i, int(i) == directionalLightWithShadowsId);
        }
        for (uint i = 0u; i < numPointLights; i++)
        {
            lighting += pointLightCalc(position, normal, i);
        }
        color = albedo * lighting;
    }
}
