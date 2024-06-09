#version 330 core
#extension GL_ARB_explicit_uniform_location : enable

layout(location = 0) in vec3 vPos;

layout(location = 1) uniform mat4 inTr;
layout(location = 9) uniform float viewHeight;

out vec3 fPos;

void main(){
	gl_Position = inTr * vec4(vPos, 1);
	fPos.x = vPos.x;
	fPos.y = vPos.y - viewHeight;
	fPos.z = vPos.z;
}