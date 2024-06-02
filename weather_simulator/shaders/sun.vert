#version 330 core
#extension GL_ARB_explicit_uniform_location : enable

in vec3 vPos;
in float vViewHeight;

layout(location = 0) uniform mat4 inTr;
layout(location = 4) uniform mat4 exTr;

out vec2 fPos;
out float fViewHeight;

void main(){
	vec4 pos4 = exTr * vec4(vPos, 1);
	gl_Position = inTr * pos4;
	fPos = vec2(sqrt(pow(pos4.x, 2) + pow(pos4.z, 2)), pos4.y - vViewHeight);
	fViewHeight = vViewHeight / 8000.0;
}