out vec4 out_color;

uniform sampler2D image;
in vec2 texCoord;

void main()
{
    out_color = texture(image, texCoord);
}