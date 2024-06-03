#version 330 core
#extension GL_ARB_explicit_uniform_location : enable

in vec2 fPos;
in float fViewHeight;

layout(location = 8) uniform sampler2D transmitTex;

out vec4 FragColor;

void main(){
	float r = sqrt(pow(fPos.x, 2) + pow(fPos.y, 2));
	FragColor = vec4(1.0, 1.0, 1.0, 1.0) * texture(transmitTex, vec2(fViewHeight, -fPos.x / r));
}
