#version 330 core
#extension GL_ARB_explicit_uniform_location : enable

in vec3 vPos;

layout(location = 0) uniform mat4 inTr;
layout(location = 4) uniform mat4 exTr;
layout(location = 9) uniform float viewHeight;

out vec2 fPos;

void main(){
	vec4 pos4 = exTr * vec4(vPos, 1);
	gl_Position = inTr * pos4;
	fPos.x = sqrt(pow(pos4.x, 2) + pow(pos4.z, 2));
	fPos.y = - pos4.y;
}