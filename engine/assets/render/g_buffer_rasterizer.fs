in vec3 fragPosition;
in vec3 fragNormal;
in vec3 fragAlbedo;
#ifdef RENDER_PICKER
flat in uvec2 fragPicker;
#endif

layout(location = 0) out vec3 position;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec3 albedo;
#ifdef RENDER_PICKER
layout(location = 3) out uvec2 picker;
#endif

void main()
{
    position = fragPosition;
    normal = fragNormal;
    albedo = fragAlbedo;
    #ifdef RENDER_PICKER
    picker = fragPicker;
    #endif
}
