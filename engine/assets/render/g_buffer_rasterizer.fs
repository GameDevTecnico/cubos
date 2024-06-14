in vec3 fragPosition;
in vec3 fragNormal;
in vec3 fragAlbedo;
flat in uvec2 fragPicker;

layout(location = 0) out vec3 position;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec3 albedo;
layout(location = 3) out uvec2 picker;

void main()
{
    position = fragPosition;
    normal = fragNormal;
    albedo = fragAlbedo;
    picker = fragPicker;
}
