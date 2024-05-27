/*
terminal mode:
	it makes small terminal window in the bottom of the screen.
*/

#include <string>
#include <vector>
#include <cctype>
#include <algorithm>
#include <sstream>

#include "GL/glut.h"

using namespace std;

static string terminalInput;
static bool terminalInputDone;
static bool terminalExitFlag;
static size_t terminalCursorPos;

static void tm_display() {
	//display invironment
	//display terminal window
	//display letters on terminal window
}

void dispatchTerminalMode() {

}

static inline bool tm_inputNotReadable() {
	return terminalInputDone || terminalCursorPos;
}

static void tm_keyboardFun(unsigned char key, int i, int y) {

	//ignore input if you are still processing last input.
	if (tm_inputNotReadable())
		return;

	//tell input is finished when enter is pressed.
	if (key == '\n') {
		terminalInputDone = true;
		terminalCursorPos = 0;
	}
	//input is letter or dot used for decimal
	else if (isalnum(key) || key == '.' || key == ' ') {
		terminalInput.insert(terminalCursorPos, 1, key);
		terminalCursorPos++;
	}
	//input is backspace
	else if (key == '\b' && terminalCursorPos > 0) {
		terminalCursorPos--;
		terminalInput.erase(terminalCursorPos, 1);
	}
	else if (key == 27)
		terminalExitFlag = true;
	//ignore other inputs
}


static void tm_SpecialFun(int key, int x, int y) {
	//ignore input if you are still processing last input.
	if (tm_inputNotReadable())
		return;

	if (key == GLUT_KEY_LEFT && terminalCursorPos > 0)
		terminalCursorPos--;
	else if (key == GLUT_KEY_RIGHT && terminalCursorPos < terminalInput.size())
		terminalCursorPos++;
}

static inline void trimString(string& str) {
	if (size_t idx = str.find_first_not_of(' '); idx != string::npos)
		str.erase(0, idx);
}

static bool interpreteTerminalInput(string&& input) {
	trimString(input);

	string token;
	stringstream line(std::move(input));
	if (!(line >> token))
		return false;

	if (token == "cloudy");

	else if (token == "rainy")
		;
	else if (token == "snowy")
		;
	else if (token == "hour")
		;
	
	if (!(line >> token))
		return false; //return error type

	try {
		float parm = stof(token);
	}
	catch (invalid_argument e) {
		/*set error mesage in the return type*/
		return false;
	}
	catch (out_of_range e) {
		/*set error mesage in the return type*/
		return false;
	}

}

void tm_idel() {
	if(terminalInputDone)
		
}