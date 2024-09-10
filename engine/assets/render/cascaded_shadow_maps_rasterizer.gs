#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 30) out; // max_vertices = 3*splits

layout(std140) uniform PerScene
{
    mat4 lightViewProj[10]; // max number of splits hardcoded here
    int numCascades;
};

void main()
{
    for (int j = 0; j < numCascades; j++)
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
