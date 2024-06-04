#version 330 core
#extension GL_ARB_explicit_uniform_location : enable

in vec2 fPos;

layout(location = 0) uniform float viewHeight;
layout(location = 5) uniform float sunCos;
layout(location = 6) uniform sampler3D intensityTex;

out vec4 FragColor;

void main(){
	float r = sqrt(pow(fPos.x, 2) + pow(fPos.y, 2));
	FragColor = texture(intensityTex, vec3(0.5 + fPos.y / r, viewHeight, sunCos));
}