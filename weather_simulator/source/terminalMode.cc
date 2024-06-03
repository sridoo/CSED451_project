#include <string>
#include <vector>
#include <cctype>
#include <algorithm>
#include <sstream>
#include <array>

#include "GL/glut.h"
#include "glm/vec3.hpp"
#include "terminalMode.hpp"
#include "shaderParams.hpp"

using namespace std;
using namespace glm;

static string terminalInput;
static bool terminalInputDone;
static bool terminalExitFlag;
static string msg;
static bool interpretingLast = false;
static constexpr size_t maxStrLen = 30;


static inline void trimString(string& str) {
	if (size_t idx = str.find_first_not_of(' '); idx != string::npos)
		str.erase(0, idx);
}

Terminal::Terminal() {
	array<vec3, 4> verts = {
		vec3{-1.f, -1.f, 0.f}, {1.f, -1.f, 0.f}, {-1.f, -0.95f, 0.f}, {1.f, -0.95f, 0.f}
	};

	baoLen_ = verts.size();

	glCreateVertexArrays(1, &vaoID_);
	glBindVertexArray(vaoID_);

	GLuint baoID;
	glCreateBuffers(1, &baoID);
	glBindBuffer(GL_ARRAY_BUFFER, baoID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(TerminalProgramParam::vPos, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
	for (size_t i = 0; i < TerminalProgramParam::vEnd; ++i)
		glEnableVertexArrayAttrib(vaoID_, i);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &baoID);
}

void Terminal::draw() {
	glUseProgram(terminalProgram);
	glBindVertexArray(vaoID_);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, baoLen_);
	glBindVertexArray(0);
}

UserCommand wrongInputFun() {
	msg = "Wrong input: ";
	terminalInput.clear();
	terminalInputDone = false;
	return { .command = UserCommandType::none };
}

UserCommand Terminal::read() {
	if (terminalExitFlag)
		return { .command = UserCommandType::exitTerminal };

	UserCommand val = { .command = UserCommandType::none };

	if (!terminalInputDone)
		return val;

	trimString(terminalInput);

	string token;
	stringstream line(move(terminalInput));
	if (!(line >> token))
		return wrongInputFun();

	if (token == "cloudy")
		;
	else if (token == "time")
		val.command = UserCommandType::timeSet;
	else if (token == "timeSpeed")
		val.command = UserCommandType::timeFlowSet;

	if (!(line >> token))
		wrongInputFun();
	else try {
		val.val = stof(token);
	}
	catch (...) {
		return wrongInputFun();
	}

	if(!line.eof())
		return wrongInputFun();

	msg = "Input command: ";
	terminalInput.clear();
	return val;
}

void Terminal::dispatchTerminalMode() {
	
}

static void tm_keyboardFun(unsigned char key, int i, int y) {
	//ignore input if you are still processing last input.
	if (interpretingLast)
		return;

	//tell input is finished when enter is pressed.
	if (key == '\n') {
		terminalInputDone = true;
	}
	//input is letter or dot used for decimal
	else if (isalnum(key) || key == '.' || key == ' ') {
		if(terminalInput.size() < maxStrLen)
			terminalInput.push_back(key);
	}
	//input is backspace
	else if (key == '\b') {
		if(!terminalInput.empty())
			terminalInput.pop_back();
	}
	else if (key == 27)
		terminalExitFlag = true;
	//ignore other inputs

	interpretingLast = false;
}