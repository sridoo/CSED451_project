#include <iostream>

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "glm/gtc/matrix_transform.hpp"

#include "loadProgram.hpp"
#include "globalParams.hpp"
#include "camera.hpp"
#include "inputRecorder.hpp"
#include "environment.hpp"
#include "transmit.hpp"
#include "Sun.hpp"

using namespace std;
using namespace glm;

void idle() {
	camIdle();
	initInRec();
	moveTime();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mat4 camMat = getCamMat();
	drawEnvIter(camMat, glm::mat4(1));
	Sun::instance().drawSunIter(camMat, glm::mat4(1));
	if (GLuint errVal = glGetError())
		cerr << errVal << endl;
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
	glEnable(GL_DEPTH_TEST);

	loadPrograms();
	cout << "Loading shaders done. Now loading transmit table..." << std::endl;
	initTransmitTableTexID();
	cout << "Loading transmit table done. Start running program..." << std::endl;
	glutIdleFunc(idle);
	glutKeyboardFunc(inRecKeyboardFunc);
	glutDisplayFunc(display);

	glutMainLoop();
}