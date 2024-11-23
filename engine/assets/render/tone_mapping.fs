#version 330 core

in vec2 fragUv;

uniform sampler2D hdrTexture;
uniform float gamma;
uniform float exposure;

uniform uvec2 screenSize;
uniform bool fxaaEnabled;

layout(std140) uniform FxaaConfig
{
    float edgeThresholdMin;
    float edgeThresholdMax;
    float subpixelQuality;
    int iterations;
};

layout(location = 0) out vec4 color;

// Convert RGB to luma using the formula: L = 0.299 * R + 0.587 * G + 0.114 * B
float rgb2luma(vec3 rgb){
    return sqrt(dot(rgb, vec3(0.299, 0.587, 0.114)));
}

float quality(int i) {
    return (i < 5) ? 1.0 : 1.5 + (i - 5) * 0.5; //increase progressively the quality
}

vec3 fxaa(sampler2D screenTexture, vec2 fragUv, vec2 inverseScreenSize){
    
    vec3 colorCenter = texture(screenTexture, fragUv).rgb;
    float lumaCenter = rgb2luma(colorCenter);
   
    // direct neighbours of the current fragment
    float lumaDown = rgb2luma(textureOffset(screenTexture, fragUv, ivec2(0,-1)).rgb);
    float lumaUp = rgb2luma(textureOffset(screenTexture, fragUv, ivec2(0,1)).rgb);
    float lumaLeft = rgb2luma(textureOffset(screenTexture, fragUv, ivec2(-1,0)).rgb);
    float lumaRight = rgb2luma(textureOffset(screenTexture, fragUv, ivec2(1,0)).rgb);

    float lumaMin = min(lumaCenter,min(min(lumaDown,lumaUp),min(lumaLeft,lumaRight)));
    float lumaMax = max(lumaCenter,max(max(lumaDown,lumaUp),max(lumaLeft,lumaRight)));
    float lumaRange = lumaMax - lumaMin;
    
    // when luma variation is lower that a threshold (or if we are in a dark area), we aren't on an edge, so don't do AA
    if(lumaRange < max(edgeThresholdMin,lumaMax*edgeThresholdMax)){
        return colorCenter;
    }

    // corners of the current fragment
    float lumaDownLeft = rgb2luma(textureOffset(screenTexture, fragUv, ivec2(-1,-1)).rgb);
    float lumaUpRight = rgb2luma(textureOffset(screenTexture, fragUv, ivec2(1,1)).rgb);
    float lumaUpLeft = rgb2luma(textureOffset(screenTexture, fragUv, ivec2(-1,1)).rgb);
    float lumaDownRight = rgb2luma(textureOffset(screenTexture, fragUv, ivec2(1,-1)).rgb);
    
    float lumaDownUp = lumaDown + lumaUp;
    float lumaLeftRight = lumaLeft + lumaRight;
    float lumaLeftCorners = lumaDownLeft + lumaUpLeft;
    float lumaDownCorners = lumaDownLeft + lumaDownRight;
    float lumaRightCorners = lumaDownRight + lumaUpRight;
    float lumaUpCorners = lumaUpRight + lumaUpLeft;
    
    // gradient for horizontal and vertical axis
    float edgeHorizontal = abs(-2.0 * lumaLeft + lumaLeftCorners) + abs(-2.0 * lumaCenter + lumaDownUp ) * 2.0 + abs(-2.0 * lumaRight + lumaRightCorners);
    float edgeVertical = abs(-2.0 * lumaUp + lumaUpCorners) + abs(-2.0 * lumaCenter + lumaLeftRight) * 2.0 + abs(-2.0 * lumaDown + lumaDownCorners);
    bool isLocalEdgeHorizontal = (edgeHorizontal >= edgeVertical);

    // pick the 2 neighboring texels lumas in the opposite direction to the local edge
    float luma1 = isLocalEdgeHorizontal ? lumaDown : lumaLeft;
    float luma2 = isLocalEdgeHorizontal ? lumaUp : lumaRight;
    
    float gradient1 = luma1 - lumaCenter;
    float gradient2 = luma2 - lumaCenter;

    bool isGradient1Steepest = abs(gradient1) >= abs(gradient2); // steepness direction
    float gradientScaled = 0.25*max(abs(gradient1),abs(gradient2));
    float stepLength = isLocalEdgeHorizontal ? inverseScreenSize.y : inverseScreenSize.x; // step size (1 pixel) according to the edge direction
   
    float lumaLocalAverage = 0.0;
    if(isGradient1Steepest){
        stepLength = - stepLength;
        lumaLocalAverage = 0.5*(luma1 + lumaCenter);
    } else {
        lumaLocalAverage = 0.5*(luma2 + lumaCenter);
    }
    vec2 currentUv = fragUv;
    if(isLocalEdgeHorizontal){
        currentUv.y += stepLength * 0.5; // shift UV by half a pixel in the edge direction
    } else {
        currentUv.x += stepLength * 0.5;
    }

    vec2 offset = isLocalEdgeHorizontal ? vec2(inverseScreenSize.x, 0.0) : vec2(0.0, inverseScreenSize.y);
    vec2 uv1 = currentUv - offset; // UV to explore sides of the edge
    vec2 uv2 = currentUv + offset;
    
    float lumaEnd1 = rgb2luma(texture(screenTexture,uv1).rgb);
    float lumaEnd2 = rgb2luma(texture(screenTexture,uv2).rgb);
    lumaEnd1 -= lumaLocalAverage;
    lumaEnd2 -= lumaLocalAverage;
    
    bool reachedEdge1 = abs(lumaEnd1) >= gradientScaled;
    bool reachedEdge2 = abs(lumaEnd2) >= gradientScaled;
    bool reachedBoth = reachedEdge1 && reachedEdge2;
    if(!reachedEdge1){
        uv1 -= offset;
    }
    if(!reachedEdge2){
        uv2 += offset;
    }   
    if(!reachedBoth){
        for(int i = 2; i < iterations; i++){ // explores until reach both sides
            if(!reachedEdge1){
                lumaEnd1 = rgb2luma(texture(screenTexture, uv1).rgb);
                lumaEnd1 = lumaEnd1 - lumaLocalAverage;
            }
            if(!reachedEdge2){
                lumaEnd2 = rgb2luma(texture(screenTexture, uv2).rgb);
                lumaEnd2 = lumaEnd2 - lumaLocalAverage;
            }
            reachedEdge1 = abs(lumaEnd1) >= gradientScaled;
            reachedEdge2 = abs(lumaEnd2) >= gradientScaled;
            reachedBoth = reachedEdge1 && reachedEdge2;
            if(!reachedEdge1){
                uv1 -= offset * quality(i);
            }
            if(!reachedEdge2){
                uv2 += offset * quality(i);
            }
            if(reachedBoth){ 
                break;
            }
        }
    }

    float distanceToEdge1 = isLocalEdgeHorizontal ? (fragUv.x - uv1.x) : (fragUv.y - uv1.y);
    float distanceToEdge2 = isLocalEdgeHorizontal ? (uv2.x - fragUv.x) : (uv2.y - fragUv.y);
    bool isDirection1Closer = distanceToEdge1 < distanceToEdge2;
    float distanceFinal = min(distanceToEdge1, distanceToEdge2);
    float edgeLength = (distanceToEdge1 + distanceToEdge2);
    float pixelOffset = - distanceFinal / edgeLength + 0.5; // UV offset
    
    bool isLumaCenterSmaller = lumaCenter < lumaLocalAverage;
    // if the luma center is smaller, the delta at each end should be positive (same variation) in the direction of the closer side of the edge
    bool correctVariation = ((isDirection1Closer ? lumaEnd1 : lumaEnd2) < 0.0) != isLumaCenterSmaller;
    float finalOffset = correctVariation ? pixelOffset : 0.0;

    // sub-pixel shifting for thin lines, for this cases AA is computed over a 3x3 neighborhood
    float lumaAverage = (1.0/12.0) * (2.0 * (lumaDownUp + lumaLeftRight) + lumaLeftCorners + lumaRightCorners);
    float subPixelOffset1 = clamp(abs(lumaAverage - lumaCenter)/lumaRange,0.0,1.0);
    float subPixelOffset2 = (-2.0 * subPixelOffset1 + 3.0) * subPixelOffset1 * subPixelOffset1;
    float subPixelOffsetFinal = subPixelOffset2 * subPixelOffset2 * subpixelQuality;
    finalOffset = max(finalOffset,subPixelOffsetFinal);

    vec2 finalUv = fragUv;
    if(isLocalEdgeHorizontal){
        finalUv.y += finalOffset * stepLength;
    } else {
        finalUv.x += finalOffset * stepLength;
    }
    return texture(screenTexture,finalUv).rgb;
}

void main()
{
    vec2 inverseScreenSize = vec2(1.0 / screenSize.x, 1.0 / screenSize.y);
    vec3 hdrColor;
    if(fxaaEnabled){
        hdrColor = fxaa(hdrTexture, fragUv, inverseScreenSize);
    }
    else {
        hdrColor = texture(hdrTexture, fragUv).rgb;
    }
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    mapped = pow(mapped, vec3(1.0 / gamma));
    color = vec4(mapped, 1.0);
}
