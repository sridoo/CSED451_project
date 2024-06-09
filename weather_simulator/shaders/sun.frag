#version 330 core
#extension GL_ARB_explicit_uniform_location : enable

in vec2 fPos;

layout(location = 8) uniform sampler2D transmitTex;
layout(location = 10) uniform sampler3D scatterTex;
layout(location = 9) uniform float viewHeightN;
layout(location = 11) uniform float sunCos;

out vec4 FragColor;

void main(){
	float r = sqrt(pow(fPos.x, 2) + pow(fPos.y, 2));
	FragColor = texture(scatterTex, vec3(viewHeightN, sunCos, 0.5 + fPos.y / r / 2)) * 0.5 + texture(transmitTex, vec2(0.5 + fPos.y / r / 2.0, viewHeightN));
}