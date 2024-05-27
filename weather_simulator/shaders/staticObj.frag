#version 330 core
#extension GL_ARB_explicit_uniform_location : enable

in vec2 ftexCoord;
out vec4 FragColor;

layout(location = 4) uniform sampler2D tex;

void main()
{
    FragColor = texture(tex, ftexCoord);
}
