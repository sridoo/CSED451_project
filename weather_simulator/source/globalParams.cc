#include<cmath>

#include "GL/glew.h"
#include "GL/freeglut.h"

#include "globalParams.hpp"

using namespace glm;

static const float PI = std::acosf(-1.f);
float curTime = 12.f;
float timeFlow = 0.1f;
float sunCos = 1.f;
vec3 sunLightDir;

void moveTime(float timePassed) {
	curTime += timePassed;
	while (curTime < 0)
		curTime += 24.f;

	while (curTime >= 24.0)
		curTime -= 24.f;
}

static void moveTime() {
	static int last = glutGet(GLUT_ELAPSED_TIME);
	int elapsed = glutGet(GLUT_ELAPSED_TIME) - last;

	moveTime(static_cast<float>(elapsed) * 0.001f * timeFlow);
	last += elapsed;
}

static void setSunLightDir() {
	float angle = curTime / 12.f * PI;
	sunLightDir = { std::sinf(angle), -std::cosf(angle), 0 };
	sunCos = 0.5 - sunLightDir.y / sqrt(pow(sunLightDir.x, 2.f) + pow(sunLightDir.y, 2.f) + pow(sunLightDir.z, 2.f)) / 2.f;
}

void globalParamsIdle() {
	moveTime();
	setSunLightDir();
}