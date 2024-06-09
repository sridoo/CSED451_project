#version 330 core
#extension GL_ARB_explicit_uniform_location : enable

in vec3 fPos;

layout(location = 0) uniform float viewHeightN;
layout(location = 5) uniform float sunCos;
layout(location = 6) uniform sampler3D intensityAMtex;
layout(location = 7) uniform sampler3D intensityFMtex;
layout(location = 8) uniform bool isAM;

out vec4 FragColor;

void main(){
	float r = sqrt(pow(fPos.x, 2) + pow(fPos.y, 2) + pow(fPos.z, 2));
	vec4 c1 = texture(intensityAMtex, , vec3(viewHeightN, sunCos, 0.5 + fPos.y / r / 2));
	vec4 c2 
	FragColor = texture((isAM == (fPos.x > 0)? intensityAMtex: intensityFMtex), vec3(viewHeightN, sunCos, 0.5 + fPos.y / r / 2));
}