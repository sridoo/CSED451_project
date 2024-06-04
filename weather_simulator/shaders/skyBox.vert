#version 330 core
#extension GL_ARB_explicit_uniform_location : enable

layout(location = 0) in vec3 vPos;

layout(location = 1) uniform mat4 inTr;

out vec2 fPos;

void main(){
	gl_Position = inTr * vec4(vPos, 1);
	fPos.x = sqrt(pow(vPos.x, 2) + pow(vPos.z, 2));
	fPos.y = -vPos.y;
}