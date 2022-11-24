#version 460 core
in vec2 UV;
out vec4 Color;

void main()
{
    Color = vec4(UV,0.0,1.0);
}