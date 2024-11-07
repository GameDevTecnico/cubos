#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out; // max_vertices = 3*6 (6 faces in a cube)

layout(std140) uniform PerScene
{
    mat4 lightViewProj[6]; // 6 faces in a cube
};

void main()
{
    for (int j = 0; j < 6; j++)
    {
        for (int i = 0; i < 3; i++) // triangles have 3 vertices
        {
            gl_Position = lightViewProj[j] * gl_in[i].gl_Position;
            gl_Layer = j;
            EmitVertex();
        }
        EndPrimitive();
    }
}
