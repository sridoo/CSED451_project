#include "GL/glew.h"

enum class UserCommandType : unsigned char {
	none, timeSet, timeFlowSet, exitTerminal
};

struct UserCommand {
	UserCommandType command;
	float val;
};

class Terminal final {
	Terminal();
	GLuint vaoID_, baoLen_;

public:
	void draw();
	UserCommand read();
	void dispatchTerminalMode();
	static Terminal& instance();
};