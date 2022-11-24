#version 460 core

out vec2 UV;
void main() 
{
    float X = float(((uint(gl_VertexID) + 2u) / 3u)%2u); 
    float Y = float(((uint(gl_VertexID) + 1u) / 3u)%2u); 

    gl_Position = vec4(-1.0f + X*2.0f, -1.0f+Y*2.0f, 0.0f, 1.0f);
    UV = vec2(X, Y);
}