#include "GL/glew.h"

enum class UserCommandType {
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
	Terminal& instance();
};