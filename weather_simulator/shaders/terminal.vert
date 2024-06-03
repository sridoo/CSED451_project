#version 330 core
#extension GL_ARB_explicit_uniform_location : enable

layout(location = 0) in vec3 vPos;

void main(){
	gl_Position = vec4(vPos, 1.0);
}