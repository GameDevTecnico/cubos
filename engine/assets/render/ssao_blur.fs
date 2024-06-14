in vec2 fragUv;

uniform sampler2D ssaoTexture;

uniform vec2 viewportOffset;
uniform vec2 viewportSize;

layout (location = 0) out float color;

void main() {
    vec2 uv = fragUv * viewportSize + viewportOffset;
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoTexture, 0));
    float result = 0.0;
    for (int x = -2; x < 2; ++x)
    {
        for (int y = -2; y < 2; ++y)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoTexture, uv + offset).r;
        }
    }
    color = result / (4.0 * 4.0);
}
