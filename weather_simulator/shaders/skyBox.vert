#version 330 core
#extension GL_ARB_explicit_uniform_location : enable

layout(location = 0) in vec3 vPos;

layout(location = 0) uniform vec3 camPos;
layout(location = 1) uniform mat4 tr;

void main(){
	gl_Position = tr * vec4(vPos, 1);
}