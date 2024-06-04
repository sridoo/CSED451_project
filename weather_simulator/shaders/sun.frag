#version 330 core
#extension GL_ARB_explicit_uniform_location : enable

in vec2 fPos;
in float fViewHeight;

layout(location = 8) uniform sampler2D transmitTex;
layout(location = 9) uniform float viewHeight;

out vec4 FragColor;

void main(){
	float r = sqrt(pow(fPos.x, 2) + pow(fPos.y, 2));
	FragColor = texture(transmitTex, vec2(0.5 - fPos.y / r / 2.0, viewHeight));
}