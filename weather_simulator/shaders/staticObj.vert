#version 330 core
#extension GL_ARB_explicit_uniform_location : enable

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vtexCoord;

layout(location = 0) uniform mat4 tr;
out vec2 ftexCoord;

void main(){
	gl_Position = tr * vec4(vPos, 1);
	ftexCoord = vtexCoord;
}