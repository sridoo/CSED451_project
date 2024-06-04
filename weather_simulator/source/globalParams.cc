#include "GL/glew.h"
#include "GL/freeglut.h"

#include "globalParams.hpp"

float curTime = 15.f;
float timeFlow = 0.3f;

void moveTime(float timePassed) {
	curTime += timePassed;
	while (curTime < 0)
		curTime += 24.f;

	while (curTime >= 24.0)
		curTime -= 24.f;
}

void moveTime() {
	static int last = glutGet(GLUT_ELAPSED_TIME);
	int elapsed = glutGet(GLUT_ELAPSED_TIME) - last;

	curTime += static_cast<float>(elapsed) * 0.001f * timeFlow;
	last += elapsed;
}