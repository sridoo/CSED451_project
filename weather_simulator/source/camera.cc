#include "GL/glew.h"
#include "GL/freeglut.h"
#include "glm/gtc/matrix_transform.hpp"

#include<iostream>

#include "camera.hpp"
#include "globalParams.hpp"
#include "inputRecorder.hpp"

using namespace glm;

static constexpr float defaultAspect = 16.0 / 9.0;
static constexpr float defaultFov = 87;
static constexpr float camMoveSpeed = 0.01;

static const mat4 intrinsicMat = perspective(defaultFov, defaultAspect, 1.0f, 2.f * mapWidth);
static float yRotVal = 0;
static float xRotVal = 0;
vec3 camPos = vec3{ 0.f, 3.f, 0.f };

static constexpr float roofHeight = 100.f;
static constexpr float floorHeight = 1.f;

mat4 getCamMat() {
	return rotate(rotate(intrinsicMat, xRotVal, { 1.f, 0.f, 0.f }), yRotVal, { 0.f, 1.f, 0.f })
		* lookAt(camPos, camPos + vec3{ 0.f, 0.f, 1.f }, { 0.f, -1.f, 0.f });
}

void camIdle() {
	static int lastTime = glutGet(GLUT_ELAPSED_TIME);
	int curTime = glutGet(GLUT_ELAPSED_TIME);
	if (aPressed())
		yRotVal += static_cast<float>(curTime - lastTime) * camMoveSpeed;
	if(dPressed())
		yRotVal -= static_cast<float>(curTime - lastTime) * camMoveSpeed;
	if (wPressed())
		xRotVal += static_cast<float>(curTime - lastTime) * camMoveSpeed;
	if (sPressed())
		xRotVal -= static_cast<float>(curTime - lastTime) * camMoveSpeed;
	if (upPressed()) {
		if (camPos.y < roofHeight)
			camPos.y += static_cast<float>(curTime - lastTime) * camMoveSpeed;
	}
	if (downPressed()) {
		if (camPos.y > floorHeight)
			camPos.y -= static_cast<float>(curTime - lastTime) * camMoveSpeed;
	}

	lastTime = curTime;
	glutPostRedisplay();
}
