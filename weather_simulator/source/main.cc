#include <vector>
#include <string>
#include <iostream>

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "glm/gtc/matrix_transform.hpp"

#include "loadProgram.hpp"
#include "camera.hpp"
#include "inputRecorder.hpp"
#include "environment.hpp"
#include "sky.hpp"

using namespace std;

void idle() {
	camIdle();
	initInRec();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawEnvIter(getCamMat(), glm::mat4(1));
    glutSwapBuffers();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(-1, -1);
	glutInitWindowSize(1600, 900);
	glutCreateWindow("SunLee's Crossing Road");
	glewInit();
    glClearColor(0, 0, 0, 1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glUseProgram(loadProgram(".\\shaders\\staticObj.vert", ".\\shaders\\staticObj.frag"));
	// cookIterAtmTables();
	loadCookedIterAtmTables();
	glutIdleFunc(idle);
	glutKeyboardFunc(inRecKeyboardFunc);
	glutDisplayFunc(display);

	glutMainLoop();
}